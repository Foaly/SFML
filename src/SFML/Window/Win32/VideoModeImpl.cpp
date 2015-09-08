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
#include <SFML/Window/VideoModeImpl.hpp>
#include <SFML/Window/Win32/VideoModeImpl.hpp>
#include <SFML/System/Err.hpp>

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

#include <iostream>

namespace
{
    std::vector<sf::Screen> screens;
    std::vector<std::wstring> displayDeviceNames;

    void ensureScreenListInitilized()
    {
        static bool initialized = false;
        if(!initialized)
        {
            sf::Screen primaryScreen;
            std::wstring primaryScreenDeviceName;
            // Enumerate all available screen
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
                    DEVMODE win32Mode;
                    std::memset(&win32Mode, 0, sizeof(win32Mode));
                    win32Mode.dmSize = sizeof(win32Mode);
                    if (EnumDisplaySettingsEx(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &win32Mode, 0) == FALSE)
                    {
                        std::wcout << "Couldn't get settings of screen: " << displayDevice.DeviceName << std::endl;
                        break;
                    }

                    // get the screens real name
                    DISPLAY_DEVICE nameDD;
                    std::memset(&nameDD, 0, sizeof(nameDD));
                    nameDD.cb = sizeof(nameDD);
                    EnumDisplayDevices(displayDevice.DeviceName, 0, &nameDD, EDD_GET_DEVICE_INTERFACE_NAME);

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
                            POINT point;
                            point.x = win32Mode.dmPosition.x;
                            point.y = win32Mode.dmPosition.y;

                            HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);

                            UINT dpiX;
                            UINT dpiY;

                            if (GetDpiForMonitorFunc(monitor, 0, &dpiX, &dpiY) == S_OK)
                            {
                                dpi = sf::Vector2u(dpiX, dpiY);
                            }
                        }

                        FreeLibrary(shCoreDll);
                    }

                    // If the library loading failed (library not available or windows version < 8.1)
                    // use the old way of getting the DPI (one value for all monitors)
                    if(dpi == sf::Vector2u())
                    {
                        HDC hdc = CreateDC(displayDevice.DeviceName, NULL, NULL, NULL);
                        if (hdc)
                        {
                            dpi = sf::Vector2u(GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY));
                        }
                    }


                    // save the data
                    sf::Screen screen;
                    screen.name      = nameDD.DeviceString;
                    screen.bounds  = sf::FloatRect(win32Mode.dmPosition.x, win32Mode.dmPosition.y, win32Mode.dmPelsWidth, win32Mode.dmPelsHeight);
                    screen.refreshRate = win32Mode.dmDisplayFrequency;
                    screen.dpi       = dpi;

                    // sort any additional screen is from left to right
                    if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                    {
                        primaryScreen = screen;
                        primaryScreenDeviceName = std::wstring(displayDevice.DeviceName);
                        continue;
                    }
                    else
                    {
                        unsigned int index = screens.size();
                        for (int i = 0; i < screens.size(); ++i)
                        {
                            if (screens[i].bounds.left > screen.bounds.left)
                            {
                                index = i;
                                break;
                            }
                        }
                        screens.insert(screens.begin() + index, screen);
                        displayDeviceNames.insert(displayDeviceNames.begin() + index, std::wstring(displayDevice.DeviceName));
                    }

                }
            }

            // the primary screen is always at index [0]
            screens.insert(screens.begin(), primaryScreen);
            displayDeviceNames.insert(displayDeviceNames.begin(), primaryScreenDeviceName);


            // display the gathered information
            for (int i = 0; i < screens.size(); ++i)
            {
                sf::Screen screen = screens[i];
                std::wcout << screen.name << " " <<  screen.refreshRate << "Hz " << screen.dpi.x << "x" << screen.dpi.y << "dpi " << displayDeviceNames[i] << std::endl;
                std::cout << "x: " << screen.bounds.left << " y: " << screen.bounds.top << " width: " << screen.bounds.width << " height: " << screen.bounds.height << std::endl;
                std::cout << std::endl;
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
std::vector<std::vector<VideoMode> > VideoModeImpl::getFullscreenModes()
{
    ensureScreenListInitilized();

    // a vector that hold a vector with all the available video modes for each screen
    std::vector<std::vector<VideoMode> > modesPerScreen;

    // Enumerate all available video modes for the all display adapter
    for (int id = 0; id < displayDeviceNames.size(); ++id)
    {
        std::vector<VideoMode> modes;
        DEVMODE win32Mode;
        win32Mode.dmSize = sizeof(win32Mode);
        for (int count = 0; EnumDisplaySettings(displayDeviceNames[id].c_str(), count, &win32Mode); ++count)
        {
            // Convert to sf::VideoMode
            VideoMode mode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, win32Mode.dmBitsPerPel, id);

            // Add it only if it is not already in the array
            if (std::find(modes.begin(), modes.end(), mode) == modes.end())
                modes.push_back(mode);
        }
        modesPerScreen.push_back(modes);
    }

    return modesPerScreen;
}


////////////////////////////////////////////////////////////
VideoMode VideoModeImpl::getDesktopMode(unsigned int screenId)
{

    DEVMODE win32Mode;
    win32Mode.dmSize = sizeof(win32Mode);
    EnumDisplaySettings(displayDeviceNames[screenId].c_str(), ENUM_CURRENT_SETTINGS, &win32Mode);

    return VideoMode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, win32Mode.dmBitsPerPel);
}


Screen VideoModeImpl::getScreenInfo(unsigned int id)
{
    ensureScreenListInitilized();

    if (id < screens.size())
        return screens[id];

    err() << "Screen with ID: " << id << " requested. No device with specified ID found." << std::endl;
    return Screen();
}


unsigned int VideoModeImpl::getScreenCount()
{
    ensureScreenListInitilized();

    return screens.size();
}


const std::wstring& getDisplayDeviceFromId(unsigned int screenId)
{
    ensureScreenListInitilized();

    if (screenId < displayDeviceNames.size())
        return displayDeviceNames[screenId];

    err() << "Diplay device with ID: " << screenId << " requested. No device with specified ID found." << std::endl;
    return std::wstring(); // returning reference to local variable
}


} // namespace priv

} // namespace sf
