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

#include <string>

namespace clan
{
	/// Token types recognized by the style tokenzier
	enum class StyleTokenType
	{
		null,                 /// No more tokens
		ident,                /// Identifier
		atkeyword,            /// @ keyword
		string,               /// String
		invalid,              /// Syntax error
		hash,                 /// # Hash
		number,               /// Number
		percentage,           /// Percentage number
		dimension,            /// Number with dimension
		uri,                  /// Url function
		unicode_range,        /// Unicode range
		cdo,                  /// HTLM comment begin <!--
		cdc,                  /// HTML comment end -->
		colon,                /// :
		semi_colon,           /// ;
		curly_brace_begin,    /// {
		curly_brace_end,      /// }
		bracket_begin,        /// (
		bracket_end,          /// )
		square_bracket_begin, /// [
		square_bracket_end,   /// ]
		whitespace,           /// Whitespace
		comment,              /// C style comment block
		function,             /// Function name
		includes,             /// ~=
		dashmatch,            /// |=
		delim                 /// Any other character
	};

	/// Style token
	class StyleToken
	{
	public:
		/// Token type
		StyleTokenType type = StyleTokenType::null;

		/// Token value
		std::string value;

		/// Dimension for token value
		std::string dimension;
	};
}
