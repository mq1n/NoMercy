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

#include "dom_character_data.h"

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	/// \brief DOM Comment class.
	///
	///    <p>This represents the content of a comment, i.e., all the characters between the starting
	///    '&lt;!--' and ending '--&gt;'. Note that this is the definition of a comment in XML, and,
	///    in practice, HTML, although some HTML tools may implement the full SGML comment structure.</p>
	class DomComment : public DomCharacterData
	{
	public:
		/// \brief Constructs a DOM Comment handle.
		DomComment();

		/// \brief Constructs a DomComment
		///
		/// \param doc = Dom Document
		/// \param data = Dom String
		DomComment(DomDocument &doc, const DomString &data);

		/// \brief Constructs a DomComment
		///
		/// \param impl = Shared Ptr
		DomComment(const std::shared_ptr<DomNode_Impl> &impl);

		~DomComment();
	};

	/// \}
}
