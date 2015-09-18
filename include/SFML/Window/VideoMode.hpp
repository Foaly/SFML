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

#ifndef SFML_VIDEOMODE_HPP
#define SFML_VIDEOMODE_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Export.hpp>
#include <vector>


namespace sf
{
////////////////////////////////////////////////////////////
/// \brief VideoMode defines a video mode (width, height, bpp)
///
////////////////////////////////////////////////////////////
class SFML_WINDOW_API VideoMode
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructors initializes all members to 0.
    ///
    ////////////////////////////////////////////////////////////
    VideoMode();

    ////////////////////////////////////////////////////////////
    /// \brief Construct the video mode with its attributes
    ///
    /// \param modeWidth        Width in pixels
    /// \param modeHeight       Height in pixels
    /// \param modeBitsPerPixel Pixel depths in bits per pixel
    /// \param modeScreenIndex  Index of the screen this video mode is associated with
    ///
    ////////////////////////////////////////////////////////////
    VideoMode(unsigned int modeWidth, unsigned int modeHeight, unsigned int modeBitsPerPixel = 32, unsigned int modeScreenIndex = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current desktop video mode of the primary screen
    ///
    /// \return Current desktop video mode of the primary screen
    ///
    /// \deprecated sf::VideoMode::getDesktopMode() is deprected, use sf::Screen::get(0).desktopMode instead.
    ///
    ////////////////////////////////////////////////////////////
    static VideoMode getDesktopMode();

    ////////////////////////////////////////////////////////////
    /// \brief Retrieve all the video modes supported in fullscreen mode on the primary screen
    ///
    /// When creating a fullscreen window, the video mode is restricted
    /// to be compatible with what the graphics driver and monitor
    /// support. This function returns the complete list of all video
    /// modes that can be used in fullscreen mode on the primary screen.
    /// The returned array is sorted from best to worst, so that
    /// the first element will always give the best mode (higher
    /// width, height and bits-per-pixel).
    ///
    /// \return Array containing all the supported fullscreen modes
    ///
    /// \deprecated sf::VideoMode::getFullscreenModes() is deprected, use sf::Screen::get(0).fullscreenModes instead.
    ///
    ////////////////////////////////////////////////////////////
    static const std::vector<VideoMode>& getFullscreenModes();

    ////////////////////////////////////////////////////////////
    /// \brief Tell whether or not the video mode is valid
    ///
    /// Check whether the video mode is a supported fullscreen mode
    /// on the specified screen. The validity of video modes is only
    /// relevant when using fullscreen windows; otherwise any video
    /// mode can be used with no restriction.
    ///
    /// \return True if the video mode is valid for fullscreen mode
    ///
    ////////////////////////////////////////////////////////////
    bool isValid() const;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    unsigned int width;        ///< Video mode width, in pixels
    unsigned int height;       ///< Video mode height, in pixels
    unsigned int bitsPerPixel; ///< Video mode pixel depth, in bits per pixels
    unsigned int screenIndex;  ///< Index of the screen this video mode is associated with
};

////////////////////////////////////////////////////////////
/// \relates VideoMode
/// \brief Overload of == operator to compare two video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if modes are equal
///
////////////////////////////////////////////////////////////
SFML_WINDOW_API bool operator ==(const VideoMode& left, const VideoMode& right);

////////////////////////////////////////////////////////////
/// \relates VideoMode
/// \brief Overload of != operator to compare two video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if modes are different
///
////////////////////////////////////////////////////////////
SFML_WINDOW_API bool operator !=(const VideoMode& left, const VideoMode& right);

////////////////////////////////////////////////////////////
/// \relates VideoMode
/// \brief Overload of < operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is lesser than \a right
///
////////////////////////////////////////////////////////////
SFML_WINDOW_API bool operator <(const VideoMode& left, const VideoMode& right);

////////////////////////////////////////////////////////////
/// \relates VideoMode
/// \brief Overload of > operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is greater than \a right
///
////////////////////////////////////////////////////////////
SFML_WINDOW_API bool operator >(const VideoMode& left, const VideoMode& right);

////////////////////////////////////////////////////////////
/// \relates VideoMode
/// \brief Overload of <= operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is lesser or equal than \a right
///
////////////////////////////////////////////////////////////
SFML_WINDOW_API bool operator <=(const VideoMode& left, const VideoMode& right);

////////////////////////////////////////////////////////////
/// \relates VideoMode
/// \brief Overload of >= operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is greater or equal than \a right
///
////////////////////////////////////////////////////////////
SFML_WINDOW_API bool operator >=(const VideoMode& left, const VideoMode& right);

} // namespace sf


#endif // SFML_VIDEOMODE_HPP


////////////////////////////////////////////////////////////
/// \class sf::VideoMode
/// \ingroup window
///
/// A video mode is defined by a width and a height (in pixels),
/// a depth (in bits per pixel) and the index of the screen it
/// belongs too. Video modes are used to setup windows
/// (sf::Window) at creation time.
///
/// You can check if a custom video mode is a supported
/// fullscreen mode on the specified screen, by using
/// the isValid() method.
///
/// The main usage of video modes is for fullscreen mode:
/// indeed you must use a valid video mode supported by
/// the graphics card driver and the screen, otherwise
/// your window creation will just fail. To retrieve a
/// list of supported fullscreen modes of a screen, use
/// sf::Screen::get().
///
/// sf::VideoMode still provides two static functions to
/// get the fullscreen modes and the current desktop mode
/// of the <b>primary screen</b> for backward compatibility.
/// Note that these functions are deprecated and will be
/// removed in the future. Use sf::Screen instead.
///
///
/// Usage example:
/// \code
/// // Display a list of all the fullscreen video modes available on the primary screen
/// std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
/// for (std::size_t i = 0; i < modes.size(); ++i)
/// {
///     sf::VideoMode mode = modes[i];
///     std::cout << "Mode #" << i << ": "
///               << mode.width << "x" << mode.height << " - "
///               << mode.bitsPerPixel << " bpp" << std::endl;
/// }
///
/// // Create a window with the same pixel depth as the desktop on the primary screen
/// sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
/// window.create(sf::VideoMode(1024, 768, desktop.bitsPerPixel), "SFML window");
/// \endcode
///
/// \see sf::Screen
///
////////////////////////////////////////////////////////////
