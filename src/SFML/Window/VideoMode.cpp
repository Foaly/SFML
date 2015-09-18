////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
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
#include <SFML/Window/Screen.hpp>
#include <SFML/System/Err.hpp>
#include <algorithm>


namespace sf
{
////////////////////////////////////////////////////////////
VideoMode::VideoMode() :
width       (0),
height      (0),
bitsPerPixel(0),
screenIndex (0)
{

}


////////////////////////////////////////////////////////////
VideoMode::VideoMode(unsigned int modeWidth, unsigned int modeHeight, unsigned int modeBitsPerPixel, unsigned int modeScreenIndex) :
width       (modeWidth),
height      (modeHeight),
bitsPerPixel(modeBitsPerPixel),
screenIndex (modeScreenIndex)
{

}


////////////////////////////////////////////////////////////
VideoMode VideoMode::getDesktopMode()
{
    return Screen::get(0).desktopMode;
}


////////////////////////////////////////////////////////////
const std::vector<VideoMode>& VideoMode::getFullscreenModes()
{
    return Screen::get(0).fullscreenModes;
}


////////////////////////////////////////////////////////////
bool VideoMode::isValid() const
{
    if(screenIndex < Screen::count())
    {
        const std::vector<VideoMode>& modes = Screen::get(screenIndex).fullscreenModes;

        return std::find(modes.begin(), modes.end(), *this) != modes.end();
    }

    err() << "The screen with index " << screenIndex << " is not available. Please use sf::Screen::count() to get the available number of screens." << std::endl;
    return false;
}


////////////////////////////////////////////////////////////
bool operator ==(const VideoMode& left, const VideoMode& right)
{
    return (left.screenIndex  == right.screenIndex)  &&
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
