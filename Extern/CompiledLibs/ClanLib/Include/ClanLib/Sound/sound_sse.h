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

namespace clan
{
	/// \addtogroup clanSound_Audio_Mixing clanSound Audio Mixing
	/// \{

	/// \brief Sound related functions implemented as SIMD using SSE
	class SoundSSE
	{
	public:
		/// \brief Allocates memory that is 16-byte memory aligned
		static void *aligned_alloc(int size);

		/// \brief Free memory allocated with aligned_alloc
		static void aligned_free(void *ptr);

		/// \brief Unpacks 16 bit stereo samples into separate float channels
		static void unpack_16bit_stereo(short *input, int size, float *output[2]);

		/// \brief Unpacks 16 bit mono samples into a single float channel
		static void unpack_16bit_mono(short *input, int size, float *output);

		/// \brief Unpacks 8 bit stereo samples into separate float channels
		static void unpack_8bit_stereo(unsigned char *input, int size, float *output[2]);

		/// \brief Unpacks 8 bit mono samples into a single float channel
		static void unpack_8bit_mono(unsigned char *input, int size, float *output);

		/// \brief Unpacks float mono samples into a single float channel
		static void unpack_float_mono(float *input, int size, float *output);

		/// \brief Unpacks float stereo samples into separate float channels
		static void unpack_float_stereo(float *input, int size, float *output[2]);

		/// \brief Packs two float channels into a single 16 bit samples stream
		static void pack_16bit_stereo(float *input[2], int size, short *output);

		/// \brief Packs two float channels into a single float samples stream
		static void pack_float_stereo(float *input[2], int size, float *output);

		/// \brief Copy floats from one buffer to another
		static void copy_float(float *input, int size, float *output);

		/// \brief Multiplies floats with a float
		static void multiply_float(float *channel, int size, float volume);

		/// \brief Sets floats to a specific value
		static void set_float(float *channel, int size, float value);

		/// \brief Mixes one float channel with specified volume into another float channel
		static void mix_one_to_one(float *input, int size, float *output, float volume);

		/// \brief Mixes one float channel into many float channels with individual volumes for each channel
		static void mix_one_to_many(float *input, int size, float **output, float *volume, int channels);

		/// \brief Mixes many float channels into one float channel with individual volumes for each channel
		static void mix_many_to_one(float **input, float *volume, int channels, int size, float *output);
	};

	/// \}
}
