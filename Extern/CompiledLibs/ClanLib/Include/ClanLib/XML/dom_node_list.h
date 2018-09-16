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
**    Ingo Ruhnke
*/


#pragma once

#include <memory>
#include "dom_string.h"
#include <vector>

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	class DomNode;
	class DomNodeList_Impl;
	class DomDocument;

	/// \brief DOM Node List class.
	///
	///    <p>The NodeList interface provides the abstraction of an ordered collection of nodes,
	///    without defining or constraining how this collection is implemented.</p> 
	class DomNodeList
	{
	public:
		/// \brief Constructs a DOM NodeList handle.
		DomNodeList();

		/// \brief Constructs a DomNodeList
		///
		/// \param node = Dom Node
		/// \param tag_name = Dom String
		DomNodeList(DomNode &node, const DomString &tag_name);

		DomNodeList(
			DomNode &node,
			const DomString &namespace_uri,
			const DomString &name,
			bool local_name = false);

		~DomNodeList();

		/// \brief The number of nodes in the list.
		int get_length() const;

		/// \brief Returns the indexth item in the collection.
		/** <p>If index is greater than or equal to the number of nodes in the list, this returns an empty node.</p>*/
		DomNode item(unsigned long index) const;

		/// \brief Adds a DomNode to the list.
		void add_item(DomNode &to_add);

	private:
		/** std::shared_ptr<DomNodeList_Impl> impl;*/
		std::vector<DomNode> node_list;
	};

	/// \}
}
