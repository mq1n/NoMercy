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
**    Mark Page
*/

#pragma once

#include <memory>
#include "pixel_buffer.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class PerlinNoise_Impl;

	/// \brief Perlin Noise Generator class
	class PerlinNoise
	{
	public:
		/// \brief Constructor
		PerlinNoise();

		/// \brief Destructor
		virtual ~PerlinNoise();

		/// \brief Create the perlin noise
		///
		/// Remember to call set_size() with the height set to 1.\n
		/// The period of these parameters are from 0 to 255
		///
		/// \param start_x = Start x position of the noise
		/// \param end_x = End x position of the noise
		/// \param start_y = Start y position of the noise
		/// \param end_y = End y position of the noise
		PixelBuffer create_noise1d(float start_x, float end_x);

		/// \brief Create the perlin noise
		///
		/// The period of these parameters are from 0 to 255
		///
		/// \param start_x = Start x position of the noise
		/// \param end_x = End x position of the noise
		/// \param start_y = Start y position of the noise
		/// \param end_y = End y position of the noise
		PixelBuffer create_noise2d(float start_x, float end_x, float start_y, float end_y);

		/// \brief Create the perlin noise
		///
		/// The period of these parameters are from 0 to 255
		///
		/// \param start_x = Start x position of the noise
		/// \param end_x = End x position of the noise
		/// \param start_y = Start y position of the noise
		/// \param end_y = End y position of the noise
		/// \param z_position = The z position of the noise
		PixelBuffer create_noise3d(float start_x, float end_x, float start_y, float end_y, float z_position);

		/// \brief Create the perlin noise
		///
		/// The period of these parameters are from 0 to 255
		///
		/// \param start_x = Start x position of the noise
		/// \param end_x = End x position of the noise
		/// \param start_y = Start y position of the noise
		/// \param end_y = End y position of the noise
		/// \param z_position = The z position of the noise
		/// \param w_position = The w position of the noise
		PixelBuffer create_noise4d(float start_x, float end_x, float start_y, float end_y, float z_position, float w_position);

		/// \brief Get the size of the output pixelbuffer
		Size get_size() const;

		/// \brief Get the format of the output pixelbuffer
		TextureFormat get_format() const;

		/// \brief Get the amplitude of the perlin noise
		float get_amplitude() const;

		/// \brief Get the number of octaves of the perlin noise
		int get_octaves() const;

		/// \brief Set the permutation table
		///
		/// If this function is not used, this class uses rand() to create a permutation table instead
		///
		/// \param table = The permutation table
		/// \param size = The permutation table size (2,4,8,16,32,64,128,256). Usually 256 (the maximum) unless a lower periodic table is required
		void set_permutations(const unsigned char *table, unsigned int size = 256);

		/// \brief Set the size of the output pixelbuffer
		///
		/// If this function is not used, the size defaults to 256x256
		///
		/// \param width = Output pixelbuffer width
		/// \param height = Output pixelbuffer height
		void set_size(int width = 256, int height = 256);

		/// \brief Set the size of the output pixelbuffer
		///
		/// If this function is not used, the size defaults to 256x256
		///
		/// \param width = Output pixelbuffer width
		/// \param height = Output pixelbuffer height
		void set_size(const Size &size);

		/// \brief Set the format of the output pixelbuffer
		///
		/// If this function is not used, the format defaults to tf_rgb8\n
		/// These following formats are supported:\n
		///		tf_rgba8, tf_rgb8, tf_r8, tf_r32f
		///
		/// \param format = The specified format
		void set_format(TextureFormat texture_format = tf_rgb8);

		/// \brief Set the amplitude of the perlin noise
		///
		/// If this function is not used, the amplitude defaults to 1.0f
		///
		/// \param amplitude = The amplitude to set
		void set_amplitude(float amplitude = 1.0f);

		/// \brief Set the number of octaves of the perlin noise
		///
		/// If this function is not used, the octaves count defaults to 1
		///
		/// \param octaves = The number of octaves to set
		void set_octaves(int octaves = 1);

	private:
		std::shared_ptr<PerlinNoise_Impl> impl;
	};

	/// \}
}
