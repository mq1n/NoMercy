/*
**  ClanLib SDK
**  Copyright (c) 1997-2016 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#pragma once

#include <memory>

namespace clan
{
	/// \addtogroup clanGL_Display clanGL Display
	/// \{

	class OpenGLContextDescription_Impl;

	/// \brief OpenGL context specific settings.
	class OpenGLContextDescription
	{
	public:
		/// \brief Constructs a window description with default values.
		OpenGLContextDescription();

		/// \brief Constructs a OpenGLContextDescription
		///
		/// \param desc = Display Window Description
		virtual ~OpenGLContextDescription();

		/// \brief Returns selected major version number
		int get_version_major() const;

		/// \brief Returns selected minor version number
		int get_version_minor() const;

		/// \brief Returns true if allowed lower versions than the selected version
		bool get_allow_lower_versions() const;

		/// \brief Returns true if the debug flag is set
		bool get_debug() const;

		/// \brief Returns true if the forward compatible flag is set
		bool get_forward_compatible() const;

		/// \brief Returns true if the core profile flag is set
		bool get_core_profile() const;

		/// \brief Returns true if the compatibility profile flag is set
		bool get_compatibility_profile() const;

		/// \brief Returns the layer plane value
		int get_layer_plane() const;

		/// \brief Select the OpenGL version number
		///
		/// Defaults to OpenGL 3.2 with lower versions (will drop to Legacy OpenGL without shader support if lower)
		///
		/// \param major = OpenGL major number (e.g. 4)
		/// \param minor = OpenGL minor number (e.g. 1)
		/// \param allow_lower_versions = Allow lower versions of the specified opengl
		void set_version(int major, int minor, bool allow_lower_versions);

		/// \brief Sets the debug flag
		///
		/// Debug contexts are intended for use during application development, and
		/// provide additional runtime checking, validation, and logging
		/// functionality while possibly incurring performance penalties
		///
		/// \param enable = true - Enable this option (Default is false)
		void set_debug(bool enable);

		/// \brief Sets the forward compatible flag
		///
		/// Forward-compatible contexts must not support functionality marked
		/// as deprecated by that version of the API, while a
		/// non-forward-compatible context must support all functionality in
		/// that version, deprecated or not.
		///
		/// \param enable = true - Enable this option (Default is false)
		void set_forward_compatible(bool enable);

		/// \brief Sets the core profile flag
		///
		/// If true, then a context implementing the core profile of OpenGL is returned
		///
		/// \param enable = true - Enable this option (Default is true)
		void set_core_profile(bool enable);

		/// \brief Sets the compatibility profile flag
		///
		/// If than a context implementing the compatibility profile is returned.
		///
		/// \param enable = true - Enable this option (Default is false)
		void set_compatibility_profile(bool enable);

		/// \brief Sets the layer plane value
		///
		/// Specifies the layer plane that the rendering context is bound to
		///
		/// \param value = The layer plane. (Default is 0 - the main plane)
		void set_layer_plane(int value);

	private:
		std::shared_ptr<OpenGLContextDescription_Impl> impl;
	};

	/// \}
}
