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

	class DomNamedNodeMap;

	/// \brief DOM Document Type class.
	///
	///    <p>Each Document has a doctype attribute whose value is either null
	///    or a DocumentType object. The DocumentType interface in the DOM Level 1
	///    Core provides an interface to the list of entities that are defined for
	///    the document, and little else because the effect of namespaces and the
	///    various XML scheme efforts on DTD representation are not clearly understood
	///    as of this writing.</p>
	///    <p>The DOM Level 1 doesn't support editing DocumentType nodes.</p> 
	class DomDocumentType : public DomNode
	{
	public:
		/// \brief Constructs a DOM Document Type handle.
		DomDocumentType();

		DomDocumentType(
			const DomString &qualified_name,
			const DomString &public_id,
			const DomString &system_id);

		/// \brief Constructs a DomDocumentType
		///
		/// \param doc = Dom Document
		DomDocumentType(DomDocument &doc);

		/// \brief Constructs a DomDocumentType
		///
		/// \param impl = Shared Ptr
		DomDocumentType(const std::shared_ptr<DomNode_Impl> &impl);

		~DomDocumentType();

		/// \brief The name of DTD; i.e., the name immediately following the DOCTYPE keyword.
		DomString get_name() const;

		/// \brief A NamedNodeMap containing the general entities, both external and internal, declared in the DTD.
		DomNamedNodeMap get_entities() const;

		/// \brief A NamedNodeMap containing the notations declared in the DTD.
		DomNamedNodeMap get_notations() const;

		/// \brief The public identifier of the external subset.
		DomString get_public_id() const;

		/// \brief The system identifier of the external subset.
		DomString get_system_id() const;

		/// \brief The internal subset as a string.
		/** <p><b>Note:</b> The actual content returned depends on how much
			information is available to the implementation. This may vary
			depending on various parameters, including the XML processor
			used to build the document.</p>*/
		DomString get_internal_subset() const;
	};

	/// \}
}
