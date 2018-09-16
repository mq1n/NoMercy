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

#include "dom_text.h"

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	/// \brief DOM CDATA Section.
	///
	///   <p>CDATA sections are used to escape blocks of text containing characters that would
	///    otherwise be regarded as markup. The only delimiter that is recognized in a CDATA
	///    section is the "]]>" string that ends the CDATA section. CDATA sections can not be
	///    nested. The primary purpose is for including material such as XML fragments, without
	///    needing to escape all the delimiters.</p>
	///    <p>The DOMString attribute of the Text node holds the text that is contained by the
	///    CDATA section. Note that this may contain characters that need to be escaped outside
	///    of CDATA sections and that, depending on the character encoding ("charset") chosen
	///    for serialization, it may be impossible to write out some characters as part of a
	///    CDATA section.</p>
	///    <p>The CDATASection interface inherits the CharacterData interface through the Text
	///    interface. Adjacent CDATASections nodes are not merged by use of the Element.normalize()
	///    method.</p> 
	class DomCDATASection : public DomText
	{
	public:
		/// \brief Constructs a DOM CDATA Section handle.
		DomCDATASection();

		/// \brief Constructs a DomCDATASection
		///
		/// \param doc = Dom Document
		/// \param data = Dom String
		DomCDATASection(DomDocument &doc, const DomString &data);

		/// \brief Constructs a DomCDATASection
		///
		/// \param impl = Shared Ptr
		DomCDATASection(const std::shared_ptr<DomNode_Impl> &impl);

		~DomCDATASection();
	};

	/// \}
}
