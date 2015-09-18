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

#ifndef SFML_SCREEN_HPP
#define SFML_SCREEN_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Export.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Rect.hpp>
#include <SFML/System/String.hpp>

#include <string>
#include <vector>

namespace sf
{

class Screen;

namespace priv
{
    struct ScreenAccess
    {
        ////////////////////////////////////////////////////////////
        /// A accessor struct, that allows to create Screen objects
        /// from within the priv namespace eventhough the constructor
        /// of Screen is private.
        ////////////////////////////////////////////////////////////
        static Screen construct();
    };
}

////////////////////////////////////////////////////////////
/// \brief Screen represents a screen (monitor, projector, TV screen, ...)
///
////////////////////////////////////////////////////////////
class SFML_WINDOW_API Screen
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Get the number of currently connected screens
    ///
    /// \return Current screen count
    ///
    ////////////////////////////////////////////////////////////
    static std::size_t count();

    ////////////////////////////////////////////////////////////
    /// \brief Get a screen based on its index
    ///
    /// If the screen with the specified index could not be found
    /// the default screen (with index 0) is returned.
    ///
    /// \param id Index of the screen we want to get
    ///
    /// \return Screen with specified index
    ///
    ////////////////////////////////////////////////////////////
    static const Screen& get(unsigned int index);

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    String                 name;            ///< Name of the screen
    unsigned int           index;           ///< Index of the screen
    IntRect                bounds;          ///< Bounds of the screen in virtual screen space
    IntRect                workingArea;     ///< Working area of the screen in virtual screen space
    unsigned int           refreshRate;     ///< Refresh rate of the screen
    Vector2u               dpi;             ///< Dpi (dots per inch) of the screen
    bool                   isPrimary;       ///< Is this the primary screen?
    std::vector<VideoMode> fullscreenModes; ///< Supported fullscreen modes of this screen
    VideoMode              desktopMode;     ///< Desktop mode of the screen

private:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructor initializes all member to 0.
    ///
    ////////////////////////////////////////////////////////////
    Screen();

    friend struct priv::ScreenAccess;
};


} // namespace sf


#endif // SFML_SCREEN_HPP


////////////////////////////////////////////////////////////
/// \class sf::Screen
/// \ingroup window
///
/// A screen is an abstraction for any output device connected
/// to the computers graphics card (for example monitors,
/// projectors or TV screens). sf::Screen provides two static
/// functions. sf::Screen::count() returns the number of connected
/// screens. sf::Screen::get(unsigned int id) returns a screen
/// object corresponding the given ID. The IDs are continuous
/// unsigned integer in range [0 ... count()[. The primary
/// screen is always at index [0]. The rest is sorted from
/// left to right in virtual screen space.
///
/// Calling get() with a valid ID returns a screen object, filled
/// with information about the screen (like name, ID, refresh
/// rate or DPI (dots per inch)).
///
/// The main usage of a screen is to get its supported
/// fullscreen video modes. When creating a fullscreen window,
/// the video mode is restricted to be compatible with what
/// the graphics card driver and screen support, otherwise the
/// window creation will just fail. The fullscreenModes
/// vector contains the complete list of all video modes that
/// can be used in fullscreen mode by this screen. It is sorted
/// from best to worst, so that the first element will always
/// give the best mode (higher width, height and bits-per-pixel).
///
/// desktopMode contains the mode the screen currently uses.
/// This allows you to build windows with the same size and
/// pixel depth as the current resolution.
///
/// The bounds of the screen are composed of its position
/// in virtual screen space and its size.
///
/// The working area of the screen is a subrect of the screens
/// bounds. It is the usable area of a screen in virtual screen
/// space that a window can cover. For example it excludes the
/// taskbar on windows or the dock and menu bar on OSX.
///
///
/// Usage example:
/// \code
/// // query screen count
/// std::size_t numberOfScreens = sf::Screen::count();
/// std::cout << "Number of connected Screens: " << numberOfScreens << std::endl;
///
/// // enumerate over all screens
/// for(std::size_t index = 0; index < numberOfScreens; ++index)
/// {
///     const sf::Screen& screen = sf::Screen::get(index);
///
///     // screen information
///     const sf::IntRect bounds = screen.bounds;
///     const sf::IntRect workingArea = screen.workingArea;
///     std::cout
///         << "\n=================================================="
///         << "\nScreen #" << screen.index
///         << "\n--------------------------------------------------"
///         << "\nName:         " << screen.name.toAnsiString()
///         << "\nPrimary:      " << std::boolalpha << screen.isPrimary
///         << "\nRefresh rate: " << screen.refreshRate << " Hz"
///         << "\nDPI:          (x: " << screen.dpi.x << ", y: " << screen.dpi.y << ")"
///         << "\nBounds:       (x: " << bounds.left << " y: " << bounds.top << " width: " << bounds.width << " height: " << bounds.height << ")"
///         << "\nWorking area: (x: " << workingArea.left << " y: " << workingArea.top << " width: " << workingArea.width << " height: " << workingArea.height << ")"
///         << std::endl << std::endl;
///
///     const std::vector<sf::VideoMode>& modes = screen.fullscreenModes;
///     std::cout << "Valid fullscreen resolutions for screen " << modes[0].screenIndex  << ":" << std::endl;
///
///     // fullscreen modes
///     for(std::size_t i = 0; i < modes.size(); ++i)
///     {
///         sf::VideoMode mode = modes[i];
///         std::cout << "Mode #" << i << ": " << mode.width << "x" << mode.height << " - " << mode.bitsPerPixel << " bpp" << std::endl;
///     }
///     std::cout << std::endl;
///
///     // desktop mode
///     const sf::VideoMode mode = screen.desktopMode;
///     std::cout << "Desktop mode for screen #" << index << ": " << mode.width << "x" << mode.height << " - " << mode.bitsPerPixel << " bpp";
///     std::cout << std::endl;
/// }
/// \endcode
///
////////////////////////////////////////////////////////////
