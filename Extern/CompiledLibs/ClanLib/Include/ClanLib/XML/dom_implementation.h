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

#include <memory>
#include "dom_string.h"

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	class DomImplementation_Impl;
	class DomDocument;
	class DomDocumentType;

	/// \brief DOM Implementation class.
	///
	///    <p>The DOM Implementation interface provides a number of methods for performing
	///    operations that are independent of any particular instance of the document object model.</p>
	class DomImplementation
	{
	public:
		/// \brief Constructs a DOM Implementation handle.
		DomImplementation();

		/// \brief Constructs a DomImplementation
		///
		/// \param doc = Dom Document
		DomImplementation(DomDocument &doc);

		~DomImplementation();

		/// \brief Test if the DOM implementation implements a specific feature.
		///
		/// \param feature The package name of the feature to test. In Level 1, the legal values are "HTML" and "XML" (case-insensitive).
		/// \param version This is the version number of the package name to test. In Level 1, this is the string "1.0". If the version is not specified, supporting any version of the feature will cause the method to return true.
		/// retval: true if the feature is implemented in the specified version, false otherwise.
		bool has_feature(const DomString &feature, const DomString &version);

		/// \brief Creates an empty DocumentType node.
		/** <p>Entity declarations and notations are not made available.
			Entity reference expansions and default attribute additions
			do not occur. It is expected that a future version of the DOM
			will provide a way for populating a DocumentType.</p>*/
		DomDocumentType create_document_type(
			const DomString &qualified_name,
			const DomString &public_id,
			const DomString &system_id);

		/// \brief Creates an XML Document object of the specified type with its document element.
		DomDocument create_document(
			const DomString &namespace_uri,
			const DomString &qualified_name,
			const DomDocumentType &doctype);

	private:
		std::shared_ptr<DomImplementation_Impl> impl;
	};

	/// \}
}
