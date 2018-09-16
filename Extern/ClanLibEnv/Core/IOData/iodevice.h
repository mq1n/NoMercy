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
**    Mark Page
*/

#pragma once

#include "../System/cl_platform.h"
#include <memory>
#include <string>

namespace clan
{
	/// \addtogroup clanCore_I_O_Data clanCore I/O Data
	/// \{

	class IODeviceProvider;
	class IODevice_Impl;

	/// \brief I/O Device interface.
	///
	/// This class can store basic datatypes and retain portability (using the specified endian mode)\n
	///    The supported datatypes are: int64_t, int32_t, int16_t and int8_t\n
	///    The std::string datatype is supported - Using Size(int32_t), Charactor Data (std::string characters) 
	class IODevice
	{
	public:
		/// \brief Seeking modes.
		enum SeekMode
		{
			/// \brief Set to a specific value from the start
			seek_set,

			/// \brief Set relative to the current position
			seek_cur,

			/// \brief Set to a specific value from the end (use negative position)
			seek_end
		};

		/// \brief Constructs a null instance
		IODevice();

		/// \brief Constructs a IODevice
		///
		/// \param provider = IODevice Provider
		IODevice(IODeviceProvider *provider);

		~IODevice();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the size of data stream.
		/** <p>Returns -1 if the size is unknown.</p>
			\return The size (-1 if size is unknown)*/
		int get_size() const;

		/// \brief Returns the position in the data stream.
		/** <p>Returns -1 if the position is unknown.</p>
			\return The size (-1 if position is unknown)*/
		int get_position() const;

		/// \brief Returns true if the input source is in little endian mode.
		/** \return true if little endian*/
		bool is_little_endian() const;

		/// \brief Returns the provider for this object
		const IODeviceProvider *get_provider() const;

		/// \brief Returns the provider for this object
		IODeviceProvider *get_provider();

		/// \brief Send data to device.
		/** If the device databuffer is too small, it will be extended (ie grow memory block size or file size)
			\param data Data to send
			\param len Length to send
			\param send_all true to send all the data. false = send part of the data, if it would block
			\return size of data sent*/
		int send(const void *data, int len, bool send_all = true);

		/// \brief Receive data from device.
		///
		/// \param data Data to receive
		/// \param len Length to receive
		/// \param receive_all true to receive all the data. false = receive part of the data, if it would block
		///
		/// \return size of data received
		int receive(void *data, int len, bool receive_all = true);

		/// \brief Peek data from device (data is left in the buffer).
		///
		/// \param data Data to receive
		/// \param len Maximum length of data to receive
		/// \return size of data received.
		int peek(void *data, int len);

		/// \brief Seek in data stream.
		///
		/// \param position Position to use (usage depends on the seek mode)
		/// \param mode Seek mode
		/// \return false = Failed
		bool seek(int position, SeekMode mode = seek_set);

		/// \brief Alias for receive(data, len, receive_all)
		///
		/// \param data Data to receive
		/// \param len Length to receive
		/// \param receive_all true to receive all the data. false = receive part of the data, if it would block
		///
		/// \return size of data received
		int read(void *data, int len, bool receive_all = true);

		/// \brief Alias for send(data, len, send_all)
		///
		/// \param data Data to send
		/// \param len Length to send
		/// \param send_all true to send all the data. false = send part of the data, if it would block
		///
		/// \return size of data sent
		int write(const void *data, int len, bool send_all = true);

		/// \brief Changes input data endianess to the local systems mode.
		void set_system_mode();

		/// \brief Changes input data endianess to big endian mode. (Default is little endian)
		void set_big_endian_mode();

		/// \brief Changes input data endianess to little endian mode. This is the default setting.
		void set_little_endian_mode();

		/// \brief Writes a signed 64 bit integer to output source.
		///
		/// \param data Integer to write
		void write_int64(int64_t data);

		/// \brief Writes an unsigned 64 bit integer to output source.
		///
		/// \param data Integer to write
		void write_uint64(uint64_t data);

		/// \brief Writes a signed 32 bit integer to output source.
		///
		/// \param data Integer to write
		void write_int32(int32_t data);

		/// \brief Writes an unsigned 32 bit integer to output source.
		///
		/// \param data Integer to write
		void write_uint32(uint32_t data);

		/// \brief Writes a signed 16 bit integer to output source.
		///
		/// \param data Integer to write
		void write_int16(int16_t data);

		/// \brief Writes an unsigned 16 bit integer to output source.
		///
		/// \param data Integer to write
		void write_uint16(uint16_t data);

		/// \brief Writes a signed 8 bit integer to output source.
		///
		/// \param data Integer to write
		void write_int8(int8_t data);

		/// \brief Writes an unsigned 8 bit integer to output source.
		///
		/// \param data Integer to write
		void write_uint8(uint8_t data);

		/// \brief  Writes a float to output source.
		///
		/// \param data = Float to write
		///
		/// Warning, this is not portable.
		void write_float(float data);

		/// \brief  Writes a string to the output source.
		///
		/// \param str String to write
		///
		/// The binary format written to the output source is first an uint32 telling the length of the
		/// string, and then the string itself.
		void write_string_a(const std::string &str);

		/// \brief Writes a nul terminated string to the output source.
		///
		/// \param str String to write
		///
		/// The binary format written to the output source is the string content followed by the NUL character.
		void write_string_nul(const std::string &str);

		/// \brief Writes a text string to the output source.
		///
		/// \param str String to write
		///
		/// The binary format written to the output source is the string content appended with a native newline.
		/// On Windows the newline is CR+LF sequence and on other platforms it is only LF character.
		/// This function is intended for use with text files.
		void write_string_text(const std::string &str);

		/// \brief Reads a signed 64 bit integer from input source.
		/** \return The integer read.*/
		int64_t read_int64();

		/// \brief Reads an unsigned 64 bit integer from input source.
		/** \return The integer read.*/
		uint64_t read_uint64();

		/// \brief Reads a signed 32 bit integer from input source.
		/** \return The integer read.*/
		int32_t read_int32();

		/// \brief Reads an unsigned 32 bit integer from input source.
		/** \return The integer read.*/
		uint32_t read_uint32();

		/// \brief Reads a signed 16 bit integer from input source.
		/** \return The integer read.*/
		int16_t read_int16();

		/// \brief Reads an unsigned 16 bit integer from input source.
		/** \return The integer read.*/
		uint16_t read_uint16();

		/// \brief Reads a signed 8 bit integer from input source.
		/** \return The integer read.*/
		int8_t read_int8();

		/// \brief Reads an unsigned 8 bit integer from input source.
		/** \return The integer read.*/
		uint8_t read_uint8();

		/// \brief Reads a float from input source.
		/** Warning, this is not portable
			\return The float read.*/
		float read_float();

		/// \brief Reads a string from the input source.
		/** <p>The binary format expected in the input source is first an uint32 telling the length of the
			string, and then the string itself.</p>
			\return The string read.*/
		std::string read_string_a();

		/// \brief Reads a nul terminated string from the input source.
		/** The binary format expected in the input source is a nul terminated string.
			(The NUL termintor is read, so that the file position is set after the NUL)
			\return The string read.*/
		std::string read_string_nul();

		/// \brief  Reads a string from the input source where the source is a text file.
		///
		/// After reading the input source up to "read_until_chars", the file position is set to the first character that was not read.
		/// If the file contains NUL characters, then the input is read up to the NUL character, and the file position is set AFTER the NUL)
		///
		/// \param skip_initial_chars Ignore any of these characters at the start of the string. NULL = Do not ignore any characters
		/// \param read_until_chars Read from the input until any of these characters are found. NULL = Read until the end of the file
		/// \param allow_eof Allow EOF
		/// \return The string read.
		std::string read_string_text(const char *skip_initial_chars, const char *read_until_chars, bool allow_eof = true);

		/// \brief Create a new IODevice referencing the same resource.
		IODevice duplicate();

	protected:
		std::shared_ptr<IODevice_Impl> impl;
	};

	/// \}
}
