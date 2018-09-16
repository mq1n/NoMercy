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

#ifdef __cplusplus_cli
#pragma managed(push, on)
#include <vcclr.h>
#endif

namespace clan
{
	/// \addtogroup clanCore_Text clanCore Text
	/// \{

	/// \brief String helper functions.
	class StringHelp
	{
	public:
		static std::vector<std::string> split_text(
			const std::string &text,
			const std::string &split_string,
			bool skip_empty = true);

		/// \brief Wide char to utf-8
		///
		/// \param value = Wide char value
		/// \return The UTF-8 String
		static std::string wchar_to_utf8(wchar_t value);

		/// \brief unicode char to utf-8
		///
		/// \param value = unicode char value
		/// \return The UTF-8 String
		static std::string unicode_to_utf8(unsigned int value);

		/// \brief Trim
		///
		/// \param text = String Ref
		///
		/// \return Temp String
		static std::string trim(const std::string &text);

		/// \brief Removes trailing zeros from floating point numbers
		///
		/// If the input string is "42.21000", then this function returns "42.21".
		/// If the string is "42.00000", then it returns "42".
		static std::string remove_trailing_zeros(std::string text);
		static std::wstring remove_trailing_zeros(std::wstring text);

		/// \brief Compare
		///
		/// \param a = String Ref8
		/// \param b = String Ref8
		/// \param case_insensitive = bool
		///
		/// \return int
		static int compare(const std::string &a, const std::string &b, bool case_insensitive = false);

		/// \brief Compare
		///
		/// \param a = String Ref16
		/// \param b = String Ref16
		/// \param case_insensitive = bool
		///
		/// \return int
		static int compare(const std::wstring &a, const std::wstring &b, bool case_insensitive = false);

		/// \brief Text to upper
		///
		/// \param s = String Ref
		///
		/// \return Temp String
		static std::string text_to_upper(const std::string &s);

		/// \brief Local8 to upper
		///
		/// \param s = String Ref8
		///
		/// \return Temp String8
		static std::string local8_to_upper(const std::string &s);

		/// \brief Ucs2 to upper
		///
		/// \param s = String Ref16
		///
		/// \return Temp String16
		static std::wstring ucs2_to_upper(const std::wstring &s);

		/// \brief Text to lower
		///
		/// \param s = String Ref
		///
		/// \return Temp String
		static std::string text_to_lower(const std::string &s);

		/// \brief Local8 to lower
		///
		/// \param s = String Ref8
		///
		/// \return Temp String8
		static std::string local8_to_lower(const std::string &s);

		/// \brief Ucs2 to lower
		///
		/// \param s = String Ref16
		///
		/// \return Temp String16
		static std::wstring ucs2_to_lower(const std::wstring &s);

		/// \brief Float to text
		///
		/// \param value = value
		/// \param num_decimal_places = value
		///
		/// \return Temp String
		static std::string float_to_text(float value, int num_decimal_places = 6, bool remove_trailing_zeros = true);

		/// \brief Float to local8
		///
		/// \param value = value
		/// \param num_decimal_places = value
		///
		/// \return Temp String8
		static std::string float_to_local8(float value, int num_decimal_places = 6, bool remove_trailing_zeros = true);

		/// \brief Float to ucs2
		///
		/// \param value = value
		/// \param num_decimal_places = value
		///
		/// \return Temp String16
		static std::wstring float_to_ucs2(float value, int num_decimal_places = 6, bool remove_trailing_zeros = true);

		/// \brief Text to float
		///
		/// \param value = String Ref
		///
		/// \return float
		static float text_to_float(const std::string &value);

		/// \brief Local8 to float
		///
		/// \param value = String Ref8
		///
		/// \return float
		static float local8_to_float(const std::string &value);

		/// \brief Ucs2 to float
		///
		/// \param value = String Ref16
		///
		/// \return float
		static float ucs2_to_float(const std::wstring &value);

		/// \brief Double to text
		///
		/// \param value = value
		/// \param num_decimal_places = value
		///
		/// \return Temp String
		static std::string double_to_text(double value, int num_decimal_places = 6);

		/// \brief Double to local8
		///
		/// \param value = value
		/// \param num_decimal_places = value
		///
		/// \return Temp String8
		static std::string double_to_local8(double value, int num_decimal_places = 6);

		/// \brief Double to ucs2
		///
		/// \param value = value
		/// \param num_decimal_places = value
		///
		/// \return Temp String16
		static std::wstring double_to_ucs2(double value, int num_decimal_places = 6);

		/// \brief Text to double
		///
		/// \param value = String Ref
		///
		/// \return double
		static double text_to_double(const std::string &value);

		/// \brief Local8 to double
		///
		/// \param value = String Ref8
		///
		/// \return double
		static double local8_to_double(const std::string &value);

		/// \brief Ucs2 to double
		///
		/// \param value = String Ref16
		///
		/// \return double
		static double ucs2_to_double(const std::wstring &value);

		/// \brief Int to text
		///
		/// \param value = value
		///
		/// \return Temp String
		static std::string int_to_text(int value);

		/// \brief Int to local8
		///
		/// \param value = value
		///
		/// \return Temp String8
		static std::string int_to_local8(int value);

		/// \brief Int to ucs2
		///
		/// \param value = value
		///
		/// \return Temp String16
		static std::wstring int_to_ucs2(int value);

		/// \brief Text to int
		///
		/// \param value = String Ref
		/// \param base = value
		///
		/// \return int
		static int text_to_int(const std::string &value, int base = 10);

		/// \brief Local8 to int
		///
		/// \param value = String Ref8
		/// \param base = value
		///
		/// \return int
		static int local8_to_int(const std::string &value, int base = 10);

		/// \brief Ucs2 to int
		///
		/// \param value = String Ref16
		/// \param base = value
		///
		/// \return int
		static int ucs2_to_int(const std::wstring &value, int base = 10);

		/// \brief Uint to text
		///
		/// \param value = value
		///
		/// \return Temp String
		static std::string uint_to_text(unsigned int value);

		/// \brief Uint to local8
		///
		/// \param value = value
		///
		/// \return Temp String8
		static std::string uint_to_local8(unsigned int value);

		/// \brief Uint to ucs2
		///
		/// \param value = value
		///
		/// \return Temp String16
		static std::wstring uint_to_ucs2(unsigned int value);

		static unsigned int text_to_uint(const std::string &value, int base = 10);

		static unsigned int local8_to_uint(const std::string &value, int base = 10);

		static unsigned int ucs2_to_uint(const std::wstring &value, int base = 10);

		/// \brief Long long to text
		///
		/// \param value = value
		///
		/// \return Temp String
		static std::string ll_to_text(long long value);

		/// \brief Long long to local8
		///
		/// \param value = value
		///
		/// \return Temp String8
		static std::string ll_to_local8(long long value);

		/// \brief Long long to ucs2
		///
		/// \param value = value
		///
		/// \return Temp String16
		static std::wstring ll_to_ucs2(long long value);

		/// \brief Text to long long
		///
		/// \param value = String Ref
		/// \param base = value
		///
		/// \return long long
		static long long text_to_ll(const std::string &value, int base = 10);

		/// \brief Local8 to long long
		///
		/// \param value = String Ref8
		/// \param base = value
		///
		/// \return long long
		static long long local8_to_ll(const std::string &value, int base = 10);

		/// \brief Ucs2 to long long
		///
		/// \param value = String Ref16
		/// \param base = value
		///
		/// \return long long
		static long long ucs2_to_ll(const std::wstring &value, int base = 10);

		/// \brief Unsigned long long to text
		///
		/// \param value = value
		///
		/// \return Temp String
		static std::string ull_to_text(unsigned long long value);

		/// \brief Unsigned long long to local8
		///
		/// \param value = value
		///
		/// \return Temp String8
		static std::string ull_to_local8(unsigned long long value);

		/// \brief Unsigned long long to ucs2
		///
		/// \param value = value
		///
		/// \return Temp String16
		static std::wstring ull_to_ucs2(unsigned long long value);

		/// \brief Text to unsigned long long
		///
		/// \param value = String Ref
		/// \param base = value
		///
		/// \return unsigned long long
		static unsigned long long text_to_ull(const std::string &value, int base = 10);

		/// \brief Local8 to unsigned long long
		///
		/// \param value = String Ref8
		/// \param base = value
		///
		/// \return unsigned long long
		static unsigned long long local8_to_ull(const std::string &value, int base = 10);

		/// \brief Ucs2 to unsigned long long
		///
		/// \param value = String Ref16
		/// \param base = value
		///
		/// \return unsigned long long
		static unsigned long long ucs2_to_ull(const std::wstring &value, int base = 10);

		/// \brief Bool to text
		///
		/// \param value = bool
		///
		/// \return Temp String
		static std::string bool_to_text(bool value);

		/// \brief Bool to local8
		///
		/// \param value = bool
		///
		/// \return Temp String8
		static std::string bool_to_local8(bool value);

		/// \brief Bool to ucs2
		///
		/// \param value = bool
		///
		/// \return Temp String16
		static std::wstring bool_to_ucs2(bool value);

		/// \brief Text to bool
		///
		/// \param value = String Ref
		///
		/// \return bool
		static bool text_to_bool(const std::string &value);

		/// \brief Local8 to bool
		///
		/// \param value = String Ref8
		///
		/// \return bool
		static bool local8_to_bool(const std::string &value);

		/// \brief Ucs2 to bool
		///
		/// \param value = String Ref16
		///
		/// \return bool
		static bool ucs2_to_bool(const std::wstring &value);

		/// \brief Text to cp437
		///
		/// \param text = String Ref
		///
		/// \return Temp String8
		static std::string text_to_cp437(const std::string &text);

		/// \brief Ucs2 to cp437
		///
		/// \param text = String Ref16
		///
		/// \return Temp String8
		static std::string ucs2_to_cp437(const std::wstring &text);

		/// \brief Cp437 to text
		///
		/// \param cp437 = String Ref8
		///
		/// \return Temp String
		static std::string cp437_to_text(const std::string &cp437);

		/// \brief Cp437 to ucs2
		///
		/// \param cp437 = String Ref8
		///
		/// \return Temp String16
		static std::wstring cp437_to_ucs2(const std::string &cp437);

		/// \brief Ucs2 to latin1
		///
		/// \param ucs2 = String Ref16
		///
		/// \return Temp String8
		static std::string ucs2_to_latin1(const std::wstring &ucs2);

		/// \brief Ucs2 to latin9
		///
		/// \param ucs2 = String Ref16
		///
		/// \return Temp String8
		static std::string ucs2_to_latin9(const std::wstring &ucs2);

		/// \brief Ucs2 to local8
		///
		/// \param ucs2 = String Ref16
		///
		/// \return Temp String8
		static std::string ucs2_to_local8(const std::wstring &ucs2);

		/// \brief Ucs2 to utf8
		///
		/// \param ucs2 = String Ref16
		///
		/// \return Temp String8
		static std::string ucs2_to_utf8(const std::wstring &ucs2);

		/// \brief Ucs2 to text
		///
		/// \param ucs2 = String Ref16
		///
		/// \return Temp String
		static std::string ucs2_to_text(const std::wstring &ucs2);

		/// \brief Latin1 to ucs2
		///
		/// \param latin1 = String Ref8
		///
		/// \return Temp String16
		static std::wstring latin1_to_ucs2(const std::string &latin1);

		/// \brief Latin9 to ucs2
		///
		/// \param latin9 = String Ref8
		///
		/// \return Temp String16
		static std::wstring latin9_to_ucs2(const std::string &latin9);

		/// \brief Local8 to ucs2
		///
		/// \param local8 = String Ref8
		///
		/// \return Temp String16
		static std::wstring local8_to_ucs2(const std::string &local8);

		/// \brief Utf8 to ucs2
		///
		/// \param utf8 = String Ref8
		///
		/// \return Temp String16
		static std::wstring utf8_to_ucs2(const std::string &utf8);

		/// \brief Utf8 to text
		///
		/// \param utf8 = String Ref8
		///
		/// \return Temp String
		static std::string utf8_to_text(const std::string &utf8);

		static std::string::size_type utf8_length(const std::string &str);

		enum BOMType
		{
			bom_none,
			bom_utf32_be,
			bom_utf32_le,
			bom_utf16_be,
			bom_utf16_le,
			bom_utf8
		};

		/// \brief Detect bom
		///
		/// \param data = void
		/// \param size_type = String
		///
		/// \return BOMType
		static BOMType detect_bom(const void *data, std::string::size_type length);

#ifdef __cplusplus_cli
		/// \brief Clr to text
		///
		/// \param String = System
		///
		/// \return Temp String
		static std::string clr_to_text(const System::String ^str)
		{
			return ucs2_to_utf8(clr_to_ucs2(str));
		}

		static System::String ^text_to_clr(const std::string &str)
		{
			return ucs2_to_clr(utf8_to_ucs2(str));
		}

		/// \brief Clr to ucs2
		///
		/// \param String = System
		///
		/// \return Temp String16
		static std::wstring clr_to_ucs2(const System::String ^str)
		{
			cli::pin_ptr<const wchar_t> chars = PtrToStringChars(str);
			return std::wstring(chars);
		}

		static System::String ^ucs2_to_clr(const std::wstring &str)
		{
			return gcnew System::String(str.c_str());
		}
#endif

	private:
		static const char trailing_bytes_for_utf8[256];
		static const unsigned char bitmask_leadbyte_for_utf8[6];
	};

	/// \}
}

#ifdef __cplusplus_cli
#pragma managed(pop)
#endif

