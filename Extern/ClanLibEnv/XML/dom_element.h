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

	/// \brief DOM Element class.
	///
	/// <p>By far the vast majority of objects (apart from text) that authors encounter when
	///    traversing a document are Element nodes. Assume the following XML document:</p>
	///    <pre>
	///    &lt;elementExample id="demo"&gt;
	///     &lt;subelement1/&gt;
	///     &lt;subelement2&gt;&lt;subsubelement/&gt;&lt;/subelement2&gt;
	///    &lt;/elementExample/gt;
	///    </pre>
	///    <p>When represented using DOM, the top node is an Element node for "elementExample",
	///    which contains two child Element nodes, one for "subelement1" and one for "subelement2".
	///    "subelement1" contains no child nodes.</p>
	///    <p>Elements may have attributes associated with them; since the Element interface
	///    inherits from Node, the generic Node interface method getAttributes may be used to
	///    retrieve the set of all attributes for an element. There are methods on the Element
	///    interface to retrieve either an Attr object by name or an attribute value by name.
	///    In XML, where an attribute value may contain entity references, an Attr object should
	///    be retrieved to examine the possibly fairly complex sub-tree representing the attribute
	///    value.</p> 
	class DomElement : public DomNode
	{
	public:
		/// \brief Constructs a DOM Element handle.
		DomElement();

		DomElement(
			DomDocument &doc,
			const DomString &tag_name,
			const DomString &namespace_uri = DomString());

		/// \brief Constructs a DomElement
		///
		/// \param impl = Shared Ptr
		DomElement(const std::shared_ptr<DomNode_Impl> &impl);

		~DomElement();

		/// \brief Returns the name of the element.
		DomString get_tag_name() const;

		/// \brief Returns true if the element has the specified attribute.
		bool has_attribute(const DomString &name) const;

		/// \brief Returns true if the element has the specified attribute.
		bool has_attribute_ns(
			const DomString &namespace_uri,
			const DomString &local_name) const;

		/// \brief Returns the specified attribute.
		DomString get_attribute(const DomString &name) const;

		/// \brief Get attribute
		///
		/// \param name = Dom String
		/// \param default_value = Dom String
		///
		/// \return Dom String
		DomString get_attribute(const DomString &name, const DomString &default_value) const;

		/// \brief Returns the specified attribute.
		DomString get_attribute_ns(
			const DomString &namespace_uri,
			const DomString &local_name) const;

		DomString get_attribute_ns(
			const DomString &namespace_uri,
			const DomString &local_name,
			const DomString &default_value) const;

		/// \brief Adds a new attribute.
		/** <p>If an attribute with that name is already present in the element, its value is
			changed to be that of the value parameter.</p>
			<p>This value is a simple string, it is not parsed as it is being set. So any markup
			(such as syntax to be recognized as an entity reference) is treated as literal text, and
			is appropriately escaped by the implementation when it is written out. In order to
			assign an attribute value that contains entity references, the user must create an Attr
			node plus any Text and EntityReference nodes, build the appropriate subtree, and use
			set_attribute_node to assign it as the value of an attribute.</p>*/
		void set_attribute(const DomString &name, const DomString &value);

		void set_attribute_ns(
			const DomString &namespace_uri,
			const DomString &qualified_name,
			const DomString &value);

		/// \brief Removes an attribute by name.
		/** <p>If the removed attribute has a default value it is immediately replaced.</p>*/
		void remove_attribute(const DomString &name);

		void remove_attribute_ns(
			const DomString &namespace_uri,
			const DomString &local_name);

		/// \brief Get attribute node
		///
		/// \param name = Dom String
		///
		/// \return Dom Attr
		DomAttr get_attribute_node(const DomString &name) const;

		DomAttr get_attribute_node_ns(
			const DomString &namespace_uri,
			const DomString &local_name) const;

		/// \brief Set attribute node
		///
		/// \param attr = Dom Attr
		///
		/// \return Dom Attr
		DomAttr set_attribute_node(const DomAttr &attr);

		/// \brief Set attribute node ns
		///
		/// \param attr = Dom Attr
		///
		/// \return Dom Attr
		DomAttr set_attribute_node_ns(const DomAttr &attr);

		/// \brief Returns a NodeList of all descendant elements with a given tag name.
		/** <p>The descendant elements are returned in the order in which they would be
			encountered in a preorder traversal of the Element tree.</p>*/
		DomNodeList get_elements_by_tag_name(const DomString &name);

		DomNodeList get_elements_by_tag_name_ns(
			const DomString &namespace_uri,
			const DomString &local_name);

		/// \brief Returns the text of all child Text nodes appended together.
		std::string get_text() const;

		/// \brief Returns the text value of the child node with the given tag name
		/** Finds the first child node with the specified tag name, then returns the content of its first child text node*/
		DomString get_child_string(const DomString &name, const DomString &default_value = DomString()) const;

		/// \brief Returns the text value of the child node with the given local name and namespace
		/** Finds the first child node with the specified local name and namespace, then returns the content of its first child text node*/
		DomString get_child_string_ns(const DomString &namespace_uri, const DomString &local_name, const DomString &default_value = DomString()) const;

		/// \brief Returns the integer value of the child node with the given tag name
		/** Finds the first child node with the specified tag name, then returns the content of its first child text node as an integer*/
		int get_child_int(const DomString &name, int default_value = 0) const;

		/// \brief Returns the integer value of the child node with the given local name and namespace
		/** Finds the first child node with the specified local name and namespace, then returns the content of its first child text node as an integer*/
		int get_child_int_ns(const DomString &namespace_uri, const DomString &local_name, int default_value = 0) const;

		/// \brief Returns the boolean value of the child node with the given tag name
		/** Finds the first child node with the specified tag name, then returns the content of its first child text node as an boolean*/
		bool get_child_bool(const DomString &name, bool default_value = false) const;

		/// \brief Returns the boolean value of the child node with the given local name and namespace
		/** Finds the first child node with the specified local name and namespace, then returns the content of its first child text node as an boolean*/
		bool get_child_bool_ns(const DomString &namespace_uri, const DomString &local_name, bool default_value = false) const;

		/// \brief Sets the child node with the specified tag name to the given text value
		/** Sets the first child node with the specified tag name, creating it if it does not exist, to the given text value*/
		void set_child_string(const DomString &name, const DomString &value);

		/// \brief Sets the child node with the specified qualified name and namespace to the given text value
		/** Sets the first child node with the specified qualified name and namespace, creating it if it does not exist, to the given text value*/
		void set_child_string_ns(const DomString &namespace_uri, const DomString &qualified_name, const DomString &value);

		/// \brief Sets the child node with the specified tag name to the given integer value
		/** Sets the first child node with the specified tag name, creating it if it does not exist, to the given integer value*/
		void set_child_int(const DomString &name, int value);

		/// \brief Sets the child node with the specified qualified name and namespace to the given integer value
		/** Sets the first child node with the specified qualified name and namespace, creating it if it does not exist, to the given integer value*/
		void set_child_int_ns(const DomString &namespace_uri, const DomString &qualified_name, int value);

		/// \brief Sets the child node with the specified tag name to the given boolean value
		/** Sets the first child node with the specified tag name, creating it if it does not exist, to the given boolean value*/
		void set_child_bool(const DomString &name, bool value);

		/// \brief Sets the child node with the specified qualified name and namespace to the given boolean value
		/** Sets the first child node with the specified qualified name and namespace, creating it if it does not exist, to the given boolean value*/
		void set_child_bool_ns(const DomString &namespace_uri, const DomString &qualified_name, bool value);

		/// \brief Returns the first child node that is an element node
		DomElement get_first_child_element() const;

		/// \brief Returns the next sibling node that is an element node
		DomElement get_next_sibling_element() const;

		/// \brief Returns the integer value of the attribute node with the given tag name
		int get_attribute_int(const DomString &name, int default_value = 0) const;

		/// \brief Returns the integer value of the attribute node with the given local name and namespace
		int get_attribute_int_ns(const DomString &namespace_uri, const DomString &local_name, int default_value = 0) const;

		/// \brief Returns the boolean value of the attribute node with the given tag name
		bool get_attribute_bool(const DomString &name, bool default_value = false) const;

		/// \brief Returns the boolean value of the attribute node with the given local name and namespace
		bool get_attribute_bool_ns(const DomString &namespace_uri, const DomString &local_name, bool default_value = false) const;

		/// \brief Returns the floating-point value of the attribute node with the given tag name
		float get_attribute_float(const DomString &name, float default_value = 0.0f) const;

		/// \brief Returns the floating-point value of the attribute node with the given local name and namespace
		float get_attribute_float_ns(const DomString &namespace_uri, const DomString &local_name, float default_value = 0.0f) const;

		/// \brief Sets the attribute node with the specified tag name to the given integer value
		void set_attribute_int(const DomString &name, int value);

		/// \brief Sets the attribute node with the specified qualified name and namespace to the given integer value
		void set_attribute_int_ns(const DomString &namespace_uri, const DomString &qualified_name, int value);

		/// \brief Sets the attribute node with the specified tag name to the given boolean value
		void set_attribute_bool(const DomString &name, bool value);

		/// \brief Sets the attribute node with the specified qualified name and namespace to the given boolean value
		void set_attribute_bool_ns(const DomString &namespace_uri, const DomString &qualified_name, bool value);

		/// \brief Sets the attribute node with the specified tag name to the given floating-point value
		void set_attribute_float(const DomString &name, float value, int num_decimal_places = 6);

		/// \brief Sets the attribute node with the specified qualified name and namespace to the given floating-point value
		void set_attribute_float_ns(const DomString &namespace_uri, const DomString &qualified_name, float value, int num_decimal_places = 6);
	};

	/// \}
}
