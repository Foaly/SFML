////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/ScreenImpl.hpp>
#include <SFML/Window/Win32/ScreenImpl.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/System/String.hpp>

#ifdef _WIN32_WINDOWS
    #undef _WIN32_WINDOWS
#endif
#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT   0x0501
#include <windows.h>
#ifndef EDD_GET_DEVICE_INTERFACE_NAME
    #define EDD_GET_DEVICE_INTERFACE_NAME 0x00000001
#endif

#include <algorithm>
#include <vector>
#include <cstring>
#include <utility>

namespace
{
    std::vector<std::pair<sf::Screen, sf::String> > screensWithDeviceName;


    ////////////////////////////////////////////////////////////
    bool screenSorter(const std::pair<sf::Screen, sf::String>& left, const std::pair<sf::Screen, sf::String>& right)
    {
        return (left.first.bounds.left < right.first.bounds.left);
    }


    ////////////////////////////////////////////////////////////
    void ensureScreenListInitialized()
    {
        static bool initialized = false;
        if (!initialized)
        {
            // Enumerate over all available screen
            DISPLAY_DEVICE displayDevice;
            std::memset(&displayDevice, 0, sizeof(displayDevice));
            displayDevice.cb = sizeof(displayDevice);
            for (int count = 0; EnumDisplayDevices(NULL, count, &displayDevice, 0); ++count)
            {
                // check if the screen is not mirrored and there is actually a screen attached
                if (!(displayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) &&
                      displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
                {
                    // try to get the screens current desktop video mode
                    // to access its position and dimension in the virtual screen space
                    DEVMODE win32desktopMode;
                    std::memset(&win32desktopMode, 0, sizeof(win32desktopMode));
                    win32desktopMode.dmSize = sizeof(win32desktopMode);
                    if (EnumDisplaySettingsEx(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &win32desktopMode, 0) == FALSE)
                    {
                        sf::err() << "Couldn't get desktop settings of screen: " << displayDevice.DeviceName << std::endl;
                        break;
                    }
                    const sf::VideoMode desktopMode(win32desktopMode.dmPelsWidth, win32desktopMode.dmPelsHeight, win32desktopMode.dmBitsPerPel);
                    unsigned int refreshRate = win32desktopMode.dmDisplayFrequency;

                    // get the screens real name
                    DISPLAY_DEVICE nameDD;
                    std::memset(&nameDD, 0, sizeof(nameDD));
                    nameDD.cb = sizeof(nameDD);
                    EnumDisplayDevices(displayDevice.DeviceName, 0, &nameDD, EDD_GET_DEVICE_INTERFACE_NAME);
                    const sf::String monitorName(nameDD.DeviceString);

                    // get the bounds and the working area
                    sf::IntRect bounds;
                    sf::IntRect workingArea;
                    POINT point;
                    point.x = win32desktopMode.dmPosition.x;
                    point.y = win32desktopMode.dmPosition.y;
                    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);

                    MONITORINFOEX info;
                    std::memset(&info, 0, sizeof(MONITORINFOEX));
                    info.cbSize = sizeof(MONITORINFOEX);
                    if (GetMonitorInfo(monitor, &info) == TRUE)
                    {
                        bounds = sf::IntRect(info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right - info.rcMonitor.left, info.rcMonitor.bottom - info.rcMonitor.top);
                        workingArea = sf::IntRect(info.rcWork.left, info.rcWork.top, info.rcWork.right - info.rcWork.left, info.rcWork.bottom - info.rcWork.top);
                    }
                    else
                    {
                        bounds = sf::IntRect(win32desktopMode.dmPosition.x, win32desktopMode.dmPosition.y, win32desktopMode.dmPelsWidth, win32desktopMode.dmPelsHeight);
                        workingArea = bounds;
                    }

                    // get the DPI
                    sf::Vector2u dpi;

                    // Since windows 8.1 you can get the DPI on a per monitor basis
                    HINSTANCE shCoreDll = LoadLibrary(L"Shcore.dll");
                    if (shCoreDll)
                    {
                        typedef HRESULT (WINAPI* GetDpiForMonitorFuncType)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
                        GetDpiForMonitorFuncType GetDpiForMonitorFunc = reinterpret_cast<GetDpiForMonitorFuncType>(GetProcAddress(shCoreDll, "GetDpiForMonitor"));

                        if (GetDpiForMonitorFunc)
                        {
                            UINT dpiX;
                            UINT dpiY;

                            if (GetDpiForMonitorFunc(monitor, 0, &dpiX, &dpiY) == S_OK)
                            {
                                dpi = sf::Vector2u(dpiX, dpiY);
                            }
                        }

                        FreeLibrary(shCoreDll);
                    }

                    HDC hdc = CreateDC(displayDevice.DeviceName, NULL, NULL, NULL);
                    if (hdc)
                    {
                        // If the library loading failed (library not available or windows version < 8.1)
                        // use the old way of getting the DPI (one value for all monitors)
                        if (dpi == sf::Vector2u())
                            dpi = sf::Vector2u(GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY));

                        // try to get the device refresh rate
                        const unsigned int deviceRefreshRate = GetDeviceCaps(hdc, VREFRESH);
                        if (deviceRefreshRate > 1)
                            // 0,1 means hardware default, so we keep using the desktop modes refresh rate
                            refreshRate = deviceRefreshRate;

                        DeleteDC(hdc);
                    }

                    // get the supported fullscreen modes
                    std::vector<sf::VideoMode> fullscreenModes;
                    DEVMODE win32Mode;
                    std::memset(&win32Mode, 0, sizeof(win32Mode));
                    win32Mode.dmSize = sizeof(win32Mode);
                    for (int fullScreenCount = 0; EnumDisplaySettings(displayDevice.DeviceName, fullScreenCount, &win32Mode); ++fullScreenCount)
                    {
                        // convert to VideoMode and save it
                        sf::VideoMode mode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, win32Mode.dmBitsPerPel);
                        fullscreenModes.push_back(mode);
                    }

                    // remove duplicate fullscreen modes
                    std::sort(fullscreenModes.begin(), fullscreenModes.end());
                    std::vector<sf::VideoMode>::iterator lastUnique = std::unique(fullscreenModes.begin(), fullscreenModes.end());
                    fullscreenModes.erase(lastUnique, fullscreenModes.end());

                    // check if this is the primary monitor
                    bool isPrimary = false;
                    if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                        isPrimary = true;

                    // construct a screen object (note the id is not set until later)
                    sf::Screen screen = sf::priv::ScreenAccess::construct();
                    screen.name = monitorName;
                    screen.bounds = bounds;
                    screen.workingArea = workingArea;
                    screen.refreshRate = refreshRate;
                    screen.dpi = dpi;
                    screen.isPrimary = isPrimary;
                    screen.fullscreenModes = fullscreenModes;
                    screen.desktopMode = desktopMode;

                    screensWithDeviceName.push_back(std::make_pair(screen, sf::String(displayDevice.DeviceName)));
                }
            }

            // sort the screens from left to right
            std::sort(screensWithDeviceName.begin(), screensWithDeviceName.end(), screenSorter);

            // put the primary screen at index [0]
            for (unsigned int i = 0; i < screensWithDeviceName.size(); ++i)
            {
                if (screensWithDeviceName[i].first.isPrimary)
                {
                    std::pair<sf::Screen, sf::String> primaryScreen = screensWithDeviceName[i];
                    screensWithDeviceName.erase(screensWithDeviceName.begin() + i);
                    screensWithDeviceName.insert(screensWithDeviceName.begin(), primaryScreen);
                }
            }

            // update the video mode screen ids
            for (unsigned int index = 0; index < screensWithDeviceName.size(); ++index)
            {
                screensWithDeviceName[index].first.index = index;

                for (unsigned int mode = 0; mode < screensWithDeviceName[index].first.fullscreenModes.size(); ++mode)
                {
                    screensWithDeviceName[index].first.fullscreenModes[mode].screenIndex = index;
                }

                screensWithDeviceName[index].first.desktopMode.screenIndex = index;
            }

            initialized = true;
        }
    }
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
std::size_t ScreenImpl::count()
{
    ensureScreenListInitialized();

    return screensWithDeviceName.size();
}


////////////////////////////////////////////////////////////
const Screen& ScreenImpl::get(unsigned int index)
{
    ensureScreenListInitialized();

    if (index < screensWithDeviceName.size())
        return screensWithDeviceName[index].first;

    err() << "Screen with index: " << index << " requested. No device with specified index found. Returning default screen." << std::endl;
    return screensWithDeviceName[0].first;
}




////////////////////////////////////////////////////////////
String getDisplayDeviceFromIndex(unsigned int screenIndex)
{
    ensureScreenListInitialized();

    if (screenIndex < screensWithDeviceName.size())
        return screensWithDeviceName[screenIndex].second;

    err() << "Diplay device with index: " << screenIndex << " requested. No device with specified index found." << std::endl;
        return String();
}


} // namespace priv

} // namespace sf
