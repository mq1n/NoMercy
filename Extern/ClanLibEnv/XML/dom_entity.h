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

	/// \brief DOM Entity class.
	///
	///   <p>This interface represents an entity, either parsed or unparsed, in an
	///    XML document. Note that this models the entity itself not the entity
	///    declaration. Entity declaration modeling has been left for a later Level
	///    of the DOM specification.</p>
	class DomEntity : public DomNode
	{
	public:
		/// \brief Constructs a DOM Entity handle.
		DomEntity();

		/// \brief Constructs a DomEntity
		///
		/// \param impl = Shared Ptr
		DomEntity(const std::shared_ptr<DomNode_Impl> &impl);

		~DomEntity();

		/// \brief The public identifier associated with the entity.
		/** <p>If the public identifier was not specified, this is null.</p>*/
		DomString get_public_id() const;

		/// \brief The system identifier associated with the entity.
		/** <p>If the system identifier was not specified, this is null.</p>*/
		DomString get_system_id() const;

		/// \brief For unparsed entities, the name of the notation for the entity.
		/** <p>For parsed entities, this is null.</p>*/
		DomString get_notation_name() const;
	};

	/// \}
}
