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

#ifndef SFML_SCREENIMPL_HPP
#define SFML_SCREENIMPL_HPP


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Screen.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief OS-specific implementation of screen functions
///
////////////////////////////////////////////////////////////
class ScreenImpl
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

};

} //namespace priv

} //namespace sf

#endif // SFML_SCREENIMPL_HPP
