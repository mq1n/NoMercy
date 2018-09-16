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
	/// \addtogroup clanSound_Audio_Mixing clanSound Audio Mixing
	/// \{

	class SoundCard;
	class SoundFilter;
	class SoundBuffer;
	class SoundBuffer_Session_Impl;
	class SoundOutput;

	/// \brief SoundBuffer_Session provides control over a playing soundeffect.
	///
	///    <p>Whenever a soundbuffer is played, it returns a SoundBuffer_Session
	///    class, which can be used to control the sound (its volume, pitch,
	///    pan, position). It can also be used to retrigger the sound
	///    or to stop it.</p> 
	class SoundBuffer_Session
	{
	public:
		/// \brief Creates a null instance
		SoundBuffer_Session();
		virtual ~SoundBuffer_Session();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the current sample position of the playback.
		int get_position() const;

		/// \brief Returns the sample position relative to the full length.
		///
		/// The value returned will be between 0 and 1, where 0
		///    means the session is at the beginning, and 1
		///   means that the soundeffect has reached the end.
		float get_position_relative() const;

		/// \brief Returns the total length (in samples) of the sound buffer played.
		///
		/// Value returned will be -1 if the length is unknown (in
		///    case of non-static soundeffects like streamed sound)
		int get_length() const;

		/// \brief Returns the frequency of the session.
		int get_frequency() const;

		/// \brief Returns the linear relative volume of the soundeffect.
		///
		/// 0 means the soundeffect is muted, 1 means the soundeffect
		///    is playing at "max" volume.
		float get_volume() const;

		/// \brief Returns the current pan (in a measure from -1 -> 1).
		///
		/// -1 means the soundeffect is only playing in the left speaker,
		///   and 1 means the soundeffect is only playing in the right speaker.
		float get_pan() const;

		/// \brief Returns whether this session loops
		///
		/// \return true if session should loop, false otherwise
		bool get_looping() const;

		/// \brief Returns true if the session is playing
		bool is_playing();

		/// \brief Sets the session position to 'new_pos'.
		///
		/// \param new_pos = The new position of the session.
		/// \return Returns true if operation completed succesfully.
		bool set_position(int new_pos);

		/// \brief Sets the relative position of the session.
		///
		/// Value must be between 0 and 1, where 0 sets the
		///    session to the beginning, and 1 sets it
		///    to the end of the sound buffer.
		///
		/// \param new_pos New relative position.
		/// \return Returns true if operation completed succesfully.
		bool set_position_relative(float new_pos);

		/// \brief Sets the end position within the current stream.
		///
		/// \param pos = End position.
		///
		/// \return True for success. False otherwise.
		bool set_end_position(int pos);

		/// \brief Sets the frequency of the session.
		///
		/// \param new_freq New frequency of session.
		void set_frequency(int new_freq);

		/// \brief Sets the volume of the session in a relative measure (0->1)
		///
		/// A value of 0 will effectively mute the sound (although it will
		///    still be sampled), and a value of 1 will set the volume to "max".
		///
		///    \param new_volume New volume of session.
		///    \return Returns true if the operation completed succesfully.
		void set_volume(float new_volume);

		/// \brief Sets the panning of the session played in measures from -1 -> 1
		///
		/// Setting the pan with a value of -1 will pan the session to the
		///    extreme left (left speaker only), 1 will pan the session to the
		///  extreme right (right speaker only).
		///
		///    \param new_pan New pan of the session played.
		///    \return Returns true if the operation completed sucecsfully.
		void set_pan(float new_pan);

		/// \brief Starts playback of the session.
		void play();

		/// \brief Stops playback of the session.
		void stop();

		/// \brief Determines whether this session should loop
		///
		/// \param loop true if session should loop, false otherwise
		void set_looping(bool loop);

		/// \brief Adds the sound filter to the session. See SoundFilter for details.
		///
		/// \param filter Sound filter to pass sound through.
		void add_filter(SoundFilter &filter);

		/// \brief Remove the sound filter from the session. See SoundFilter for details.
		void remove_filter(SoundFilter &filter);

	private:
		SoundBuffer_Session(SoundBuffer &soundbuffer, bool looping, SoundOutput &output);
		std::shared_ptr<SoundBuffer_Session_Impl> impl;

		friend class SoundBuffer;
		friend class SoundOutput_Impl;
	};

	/// \}
}
