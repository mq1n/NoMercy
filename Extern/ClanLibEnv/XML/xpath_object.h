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
**    Thomas Gottschalk Larsen
*/


#pragma once

#include <memory>
#include <vector>

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	class DomNode;
	class XPathObject_Impl;

	/// \brief XPath result object.
	class XPathObject
	{
	public:
		XPathObject();
		XPathObject(bool value);
		XPathObject(double value);
		XPathObject(size_t value);
		XPathObject(const std::string &value);
		XPathObject(const std::vector<DomNode> &value);

		enum Type
		{
			type_null,
			type_node_set,
			type_boolean,
			type_number,
			type_string
		};

		/// \brief Get Type
		///
		/// \return type
		Type get_type() const;

		/// \brief Is Null
		///
		/// \return true = null
		bool is_null() const;

		std::vector<DomNode> get_node_set() const;

		/// \brief Get Boolean
		///
		/// \return boolean
		bool get_boolean() const;

		/// \brief Get Number
		///
		/// \return number
		double get_number() const;

		/// \brief Get String
		///
		/// \return string
		std::string get_string() const;

		/// \brief Set null
		void set_null();

		/// \brief Set node set
		///
		/// \param vector = Dom Node
		void set_node_set(const std::vector<DomNode> &node_set);

		/// \brief Set boolean
		///
		/// \param value = bool
		void set_boolean(bool value);

		/// \brief Set number
		///
		/// \param value = value
		void set_number(double value);

		/// \brief Set string
		///
		/// \param str = String Ref
		void set_string(const std::string &str);

	private:
		std::shared_ptr<XPathObject_Impl> impl;
	};

	/// \}
}
