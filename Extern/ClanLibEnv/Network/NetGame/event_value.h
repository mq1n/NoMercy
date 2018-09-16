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

#include "../../Core/System/databuffer.h"

namespace clan
{
	/// \addtogroup clanNetwork_NetGame clanNetwork NetGame
	/// \{

	/// \brief NetGameEventValue
	class NetGameEventValue
	{
	public:
		enum Type
		{
			null,
			integer,
			uinteger,
			character,
			ucharacter,
			string,
			boolean,
			number,
			complex,
			binary
		};

		NetGameEventValue();

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = value
		NetGameEventValue(int value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = value
		NetGameEventValue(unsigned int value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = value
		NetGameEventValue(char value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = value
		NetGameEventValue(unsigned char value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = value
		NetGameEventValue(float value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = String
		NetGameEventValue(const std::string &value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param str = char
		NetGameEventValue(const char *str);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param str = wchar_t
		NetGameEventValue(const wchar_t *str);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = bool
		explicit NetGameEventValue(bool value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param value = DataBuffer
		NetGameEventValue(const DataBuffer &value);

		/// \brief Constructs a NetGameEventValue
		///
		/// \param type = Type
		NetGameEventValue(Type type);

		/// \brief Get Type
		///
		/// \return type
		Type get_type() const;

		/// \brief Is Null
		///
		/// \return true = null
		bool is_null() const;

		/// \brief Is Uinteger
		///
		/// \return true = uinteger
		bool is_uinteger() const;

		/// \brief Is Integer
		///
		/// \return true = integer
		bool is_integer() const;

		/// \brief Is Ucharacter
		///
		/// \return true = ucharacter
		bool is_ucharacter() const;

		/// \brief Is Character
		///
		/// \return true = character
		bool is_character() const;

		/// \brief Is Number
		///
		/// \return true = number
		bool is_number() const;

		/// \brief Is String
		///
		/// \return true = string
		bool is_string() const;

		/// \brief Is Boolean
		///
		/// \return true = boolean
		bool is_boolean() const;

		/// \brief Is Binary
		///
		/// \return true = binary
		bool is_binary() const;

		/// \brief Is Complex
		///
		/// \return true = complex
		bool is_complex() const;

		unsigned int get_member_count() const;
		const NetGameEventValue &get_member(unsigned int index) const;

		/// \brief Add member
		///
		/// \param value = Net Game Event Value
		void add_member(const NetGameEventValue &value);

		/// \brief Set member
		///
		/// \param index = value
		/// \param value = Net Game Event Value
		void set_member(unsigned int index, const NetGameEventValue &value);

		/// \brief To unsigned integer
		///
		/// \return unsigned int
		unsigned int get_uinteger() const;

		/// \brief To integer
		///
		/// \return int
		int get_integer() const;

		/// \brief To unsigned character
		///
		/// \return unsigned char
		unsigned int get_ucharacter() const;

		/// \brief To character
		///
		/// \return char
		int get_character() const;

		/// \brief To number
		///
		/// \return float
		float get_number() const;

		/// \brief To string
		///
		/// \return String
		std::string get_string() const;

		/// \brief To boolean
		///
		/// \return bool
		bool get_boolean() const;

		/// \brief To binary
		///
		/// \return binary
		DataBuffer get_binary() const;

		inline operator unsigned int() const { return get_uinteger(); }
		inline operator int() const { return get_integer(); }
		inline operator unsigned char() const { return get_ucharacter(); }
		inline operator float() const { return get_number(); }
		inline operator std::string() const { return get_string(); }
		inline operator bool() const { return get_boolean(); }
		inline operator DataBuffer() const { return get_binary(); }

		/// Helper function to obtain a string representation of an EventValue object.
		static std::string to_string(const NetGameEventValue &);

	private:
		/// \brief Throw if not complex
		void throw_if_not_complex() const;

		Type type;
		union
		{
			int value_int;
			unsigned int value_uint;
			char value_char;
			unsigned char value_uchar;
			float value_float;
			bool value_bool;
		};
		std::string value_string;
		DataBuffer value_binary;
		std::vector<NetGameEventValue> value_complex;
	};

	/// \}
}

