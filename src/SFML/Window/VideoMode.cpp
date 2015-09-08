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
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/VideoModeImpl.hpp>
#include <SFML/System/Err.hpp>
#include <algorithm>
#include <functional>


namespace sf
{
////////////////////////////////////////////////////////////
VideoMode::VideoMode() :
width       (0),
height      (0),
bitsPerPixel(0),
screenId    (0)
{

}


////////////////////////////////////////////////////////////
VideoMode::VideoMode(unsigned int modeWidth, unsigned int modeHeight, unsigned int modeBitsPerPixel, unsigned int screenID) :
width       (modeWidth),
height      (modeHeight),
bitsPerPixel(modeBitsPerPixel),
screenId    (screenID)
{

}


////////////////////////////////////////////////////////////
VideoMode VideoMode::getDesktopMode(unsigned int screenId)
{
    if(screenId < getScreenCount())
    {
        // Directly forward to the OS-specific implementation
        return priv::VideoModeImpl::getDesktopMode(screenId);
    }

    err() << "Desktop mode for screen with ID: " << screenId << " requested. No device with specified ID found." << std::endl;
    return VideoMode();
}


////////////////////////////////////////////////////////////
const std::vector<VideoMode>& VideoMode::getFullscreenModes(unsigned int screenId)
{
    static std::vector<std::vector<VideoMode> > modesPerScreen;

    // Populate the array on first call
    if (modesPerScreen.empty())
    {
        modesPerScreen = priv::VideoModeImpl::getFullscreenModes();

        for(int i = 0; i < modesPerScreen.size(); ++i)
        {
            std::sort(modesPerScreen[i].begin(), modesPerScreen[i].end(), std::greater<VideoMode>());
        }
    }

    if (screenId < modesPerScreen.size())
    {
        return modesPerScreen[screenId];
    }

    err() << "Fullscreen modes for screen with ID: " << screenId << " requested. No device with specified ID found." << std::endl;
    return std::vector<VideoMode>(); // reference to temporary :(
}


////////////////////////////////////////////////////////////
const Screen VideoMode::getScreenInfo(unsigned int id)
{
    return priv::VideoModeImpl::getScreenInfo(id);
}


////////////////////////////////////////////////////////////
unsigned int VideoMode::getScreenCount()
{
    return priv::VideoModeImpl::getScreenCount();
}



////////////////////////////////////////////////////////////
bool VideoMode::isValid() const
{
    if(screenId < getScreenCount())
    {
        const std::vector<VideoMode>& modes = getFullscreenModes(screenId);

        return std::find(modes.begin(), modes.end(), *this) != modes.end();
    }

    err() << "The screen with ID " << screenId << " is not available. Please use sf::VideoMode::getScreenCount() to get the available number of screens." << std::endl;
    return false;
}


////////////////////////////////////////////////////////////
bool operator ==(const VideoMode& left, const VideoMode& right)
{
    return (left.screenId     == right.screenId)     &&
           (left.width        == right.width)        &&
           (left.height       == right.height)       &&
           (left.bitsPerPixel == right.bitsPerPixel);
}


////////////////////////////////////////////////////////////
bool operator !=(const VideoMode& left, const VideoMode& right)
{
    return !(left == right);
}


////////////////////////////////////////////////////////////
bool operator <(const VideoMode& left, const VideoMode& right)
{
    if (left.bitsPerPixel == right.bitsPerPixel)
    {
        if (left.width == right.width)
        {
            return left.height < right.height;
        }
        else
        {
            return left.width < right.width;
        }
    }
    else
    {
        return left.bitsPerPixel < right.bitsPerPixel;
    }
}


////////////////////////////////////////////////////////////
bool operator >(const VideoMode& left, const VideoMode& right)
{
    return right < left;
}


////////////////////////////////////////////////////////////
bool operator <=(const VideoMode& left, const VideoMode& right)
{
    return !(right < left);
}


////////////////////////////////////////////////////////////
bool operator >=(const VideoMode& left, const VideoMode& right)
{
    return !(left < right);
}

} // namespace sf
