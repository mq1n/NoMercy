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

#include <vector>

namespace clan
{
	/// \addtogroup clanCore_Text clanCore Text
	/// \{

	/// \brief String formatting class.
	///
	/// Combines a format string with arbitrary arguments to
	/// create a resulting string that contains the inserted
	/// arguments.
	///
	/// A format string consists of normal text and placeholders
	/// that are replaced by the given arguments. A placeholder
	/// is identified by a percentage symbol '%%' followed by an
	/// integer that represents the argument index.
	///
	/// Example:
	///
	/// \code{.cpp}
	/// clan::StringFormat format("Arg0: %0, Arg1: %1");
	/// format.set_arg(0, 10);
	/// format.set_arg(1, "test");
	/// std::string result = format.get_result();
	/// // result will now contain "Arg0: 10, Arg1: test"
	/// \endcode
	///
	/// The global function clan::string_format is a shortcut to
	/// create formatted strings. Note that the indexing starts
	/// at 1 when using this function. Therefore the first argument
	/// is automatically referenced by the placeholder %1.
	///
	/// Example:
	///
	/// \code{.cpp}
	/// std::string result = clan::string_format("1st arg: %1, 2nd arg: %2", 10, "test");
	/// // result will now contain "1st arg: 10, 2nd arg: test"
	/// \endcode
	class StringFormat
	{
	public:
		/// \brief Constructs a formatted string object.
		///
		/// \param format_string = Format string (see detailed description)
		StringFormat(const std::string &format_string);
		~StringFormat();

		/// \brief Retrieves the formatted string with all argument replacements
		const std::string &get_result() const;

		/// \brief Sets an argument (string version)
		///
		/// \param index = Argument index
		/// \param text = Argument value as a string
		void set_arg(int index, const std::string &text);

		/// \brief Sets an argument (int version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as an integer
		/// \param min_length = Minimum amount of digits to display
		void set_arg(int index, int value, int min_length = 0);

		/// \brief Sets an argument (unsigned int version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as an unsigned integer
		/// \param min_length = Minimum amount of digits to display
		void set_arg(int index, unsigned int value, int min_length = 0);

		/// \brief Sets an argument (long unsigned int version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as a long unsigned integer
		/// \param min_length = Minimum amount of digits to display
		void set_arg(int index, long unsigned int value, int min_length = 0);

		/// \brief Sets an argument (long long version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as a long long
		/// \param min_length = Minimum amount of digits to display
		void set_arg(int index, long long value, int min_length = 0);

		/// \brief Sets an argument (unsigned long long version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as an unsigned long long
		/// \param min_length = Minimum amount of digits to display
		void set_arg(int index, unsigned long long value, int min_length = 0);

		/// \brief Sets an argument (float version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as a floating point value
		void set_arg(int index, float value);

		/// \brief Sets an argument (double version)
		///
		/// \param index = Argument index
		/// \param value = Argument value as a floating point value with double precision
		void set_arg(int index, double value);

	private:
		/// \brief Creates an argument (placeholder)
		///
		/// \param index = Argument index
		/// \param start = Start position inside format string
		/// \param length = Length inside format string
		void create_arg(int index, int start, int length);

		std::string string;

		struct ArgPosition
		{
			ArgPosition() : start(0), length(-1) { }
			ArgPosition(int s, int l) : start(s), length(l) {}
			int start;
			int length;
		};

		std::vector<ArgPosition> args;
	};

	/// \brief See clan::StringFormat for details.
	inline std::string string_format(const std::string &format)
	{
		return format;
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1>
	std::string string_format(const std::string &format, Arg1 arg1)
	{
		StringFormat f(format); f.set_arg(1, arg1); return f.get_result();
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1, class Arg2>
	std::string string_format(const std::string &format, Arg1 arg1, Arg2 arg2)
	{
		StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); return f.get_result();
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1, class Arg2, class Arg3>
	std::string string_format(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); return f.get_result();
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1, class Arg2, class Arg3, class Arg4>
	std::string string_format(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); return f.get_result();
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
	std::string string_format(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
	{
		StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); f.set_arg(5, arg5); return f.get_result();
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
	std::string string_format(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
	{
		StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); f.set_arg(5, arg5); f.set_arg(6, arg6); return f.get_result();
	}

	/// \brief See clan::StringFormat for details.
	template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
	std::string string_format(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
	{
		StringFormat f(format); f.set_arg(1, arg1); f.set_arg(2, arg2); f.set_arg(3, arg3); f.set_arg(4, arg4); f.set_arg(5, arg5); f.set_arg(6, arg6); f.set_arg(7, arg7); return f.get_result();
	}

	/// \}
}
