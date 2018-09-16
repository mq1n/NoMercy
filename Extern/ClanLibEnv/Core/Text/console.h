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
**    Kenneth Gangstoe
**    Mark Page
*/

#pragma once

// 'kbhit' was declared deprecated
#ifdef WIN32
#pragma warning(disable: 4996)
#endif

#include "string_format.h"
#include "string_help.h"
#ifdef WIN32
#include <conio.h>
#else
#include <unistd.h>
#endif

namespace clan
{
	/// \addtogroup clanCore_Text clanCore Text
	/// \{

	/// \brief Console access helper class.
	class Console
	{
	public:
		/// \brief Writes text to the console window.
		static void write(const std::string &text);

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		template <class Arg1>
		static void write(const std::string &format, Arg1 arg1)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			write(f.get_result());
		}

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		template <class Arg1, class Arg2>
		static void write(const std::string &format, Arg1 arg1, Arg2 arg2)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			write(f.get_result());
		}

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		template <class Arg1, class Arg2, class Arg3>
		static void write(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			write(f.get_result());
		}

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		template <class Arg1, class Arg2, class Arg3, class Arg4>
		static void write(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			write(f.get_result());
		}

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		/// \param arg5 = Arg5
		template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
		static void write(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			f.set_arg(arg5);
			write(f.get_result());
		}

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		/// \param arg5 = Arg5
		/// \param arg6 = Arg6
		template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
		static void write(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			f.set_arg(arg5);
			f.set_arg(arg6);
			write(f.get_result());
		}

		/// \brief Write
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		/// \param arg5 = Arg5
		/// \param arg6 = Arg6
		/// \param arg7 = Arg7
		template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
		static void write(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			f.set_arg(arg5);
			f.set_arg(arg6);
			f.set_arg(arg7);
			write(f.get_result());
		}

		/// \brief Writes text to the console window and then advances to a new line.
		static void write_line(const std::string &text)
		{
			write(text);
#ifdef WIN32
			write("\r\n");
#else
			write("\n");
#endif
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		template <class Arg1>
		static void write_line(const std::string &format, Arg1 arg1)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			write_line(f.get_result());
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		template <class Arg1, class Arg2>
		static void write_line(const std::string &format, Arg1 arg1, Arg2 arg2)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			write_line(f.get_result());
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		template <class Arg1, class Arg2, class Arg3>
		static void write_line(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			write_line(f.get_result());
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		template <class Arg1, class Arg2, class Arg3, class Arg4>
		static void write_line(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			write_line(f.get_result());
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		/// \param arg5 = Arg5
		template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
		static void write_line(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			f.set_arg(5, arg5);
			write_line(f.get_result());
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		/// \param arg5 = Arg5
		/// \param arg6 = Arg6
		template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
		static void write_line(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			f.set_arg(5, arg5);
			f.set_arg(6, arg6);
			write_line(f.get_result());
		}

		/// \brief Write line
		///
		/// \param format = String Ref
		/// \param arg1 = Arg1
		/// \param arg2 = Arg2
		/// \param arg3 = Arg3
		/// \param arg4 = Arg4
		/// \param arg5 = Arg5
		/// \param arg6 = Arg6
		/// \param arg7 = Arg7
		template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
		static void write_line(const std::string &format, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6, Arg7 arg7)
		{
			StringFormat f(format);
			f.set_arg(1, arg1);
			f.set_arg(2, arg2);
			f.set_arg(3, arg3);
			f.set_arg(4, arg4);
			f.set_arg(5, arg5);
			f.set_arg(6, arg6);
			f.set_arg(7, arg7);
			write_line(f.get_result());
		}

		/// \brief Block until a key is pressed in the console window.
		///
		/// This function does not do anything on Linux.
		static void wait_for_key();
	};

	/// \}
}
