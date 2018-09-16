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
#include "../soundformat.h"

namespace clan
{
	/// \addtogroup clanSound_Audio_Mixing clanSound Audio Mixing
	/// \{

	class SoundProvider_Session_Impl;

	/// \brief Sound provider playback session.
	///
	///  <p>Interface used by ClanLib during playback of sound providers.</p>
	class SoundProvider_Session
	{
	public:
		SoundProvider_Session();
		virtual ~SoundProvider_Session();

		/// \brief Returns the number of samples in the soundbuffer.
		virtual int get_num_samples() const = 0;

		/// \brief Returns the playback frequency of the input data.
		/** \return Playback frequency.*/
		virtual int get_frequency() const = 0;

		/// \brief Returns the current position in the playback stream.
		virtual int get_position() const = 0;

		/// \brief Returns the number of channels filled when get_data is called.
		virtual int get_num_channels() const = 0;

		/// \brief Enable/disable session looping.
		/** <p>If this function returns false (default), the clanSound mixer will manually
			try to simulate looping by setting the position to 0 when eof is encountered.</p>*/
		virtual bool set_looping(bool /* loop */) { return false; }

		/// \brief Returns true if no more input data is available.
		/** \return True if end of input data. False otherwise.*/
		virtual bool eof() const = 0;

		/// \brief Stops the current stream.
		virtual void stop() = 0;

		/// \brief Start/continue playing of the stream.
		/** \return True for success. False otherwise.*/
		virtual bool play() = 0;

		/// \brief Sets the position within the current stream.
		///
		/// \param pos Position to seek to.
		///
		/// \return True for success. False otherwise.
		virtual bool set_position(int pos) = 0;

		/// \brief Sets the end position within the current stream.
		///
		/// \param pos = End position.
		///
		/// \return True for success. False otherwise.
		virtual bool set_end_position(int pos) = 0;

		/// \brief Called when a playback session needs more sample data.
		///
		/// \param data_ptr = Points to a buffer that should be filled with sample data.
		/// \param data_requested = Samples of data requested.
		/// \return Number of samples actually filled with sample data.
		virtual int get_data(float **data_ptr, int data_requested) = 0;
	};

	/// \}
}
