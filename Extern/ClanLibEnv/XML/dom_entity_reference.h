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

#include "dom_node.h"

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	/// \brief DOM Entity Reference class.
	///
	///    <p>EntityReference objects may be inserted into the structure model when an entity reference
	///    is in the source document, or when the user wishes to insert an entity reference. Note that
	///    character references and references to predefined entities are considered to be expanded by
	///    the HTML or XML processor so that characters are represented by their Unicode equivalent rather
	///    than by an entity reference. Moreover, the XML processor may completely expand references to
	///    entities while building the structure model, instead of providing EntityReference objects. If
	///    it does provide such objects, then for a given EntityReference node, it may be that there is no
	///    Entity node representing the referenced entity; but if such an Entity exists, then the child list
	///    of the EntityReference node is the same as that of the Entity node. As with the Entity node, all
	///    descendants of the EntityReference are readonly.</p> 
	class DomEntityReference : public DomNode
	{
	public:
		/// \brief Constructs a DOM Entity Reference handle.
		DomEntityReference();

		/// \brief Constructs a DomEntityReference
		///
		/// \param doc = Dom Document
		/// \param name = Dom String
		DomEntityReference(DomDocument &doc, const DomString &name);

		/// \brief Constructs a DomEntityReference
		///
		/// \param impl = Shared Ptr
		DomEntityReference(const std::shared_ptr<DomNode_Impl> &impl);

		~DomEntityReference();
	};

	/// \}
}
