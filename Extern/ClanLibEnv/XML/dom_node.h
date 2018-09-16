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

	class DomElement;
	class DomAttr;
	class DomText;
	class DomCDATASection;
	class DomEntityReference;
	class DomEntity;
	class DomProcessingInstruction;
	class DomComment;
	class DomDocument;
	class DomDocumentType;
	class DomDocumentFragment;
	class DomNotation;
	class DomNodeList;
	class DomNamedNodeMap;
	class DomNode_Impl;

	/// \brief DOM Node class.
	///
	/// <p>The Node interface is the primary datatype for the entire Document Object Model.
	///    It represents a single node in the document tree. While all objects implementing
	///    the Node interface expose methods for dealing with children, not all objects
	///    implementing the Node interface may have children. For example, Text nodes may
	///    not have children, and adding children to such nodes results in a DOMException
	///    being thrown.</p>
	///    <p>The attributes 'node_name', 'node_value' and 'attributes' are included as a
	///    mechanism to get at node information without casting down to the specific derived
	///    interface. In cases where there is no obvious mapping of these attributes for a
	///    specific node_type (e.g., node_value for an Element or attributes for a Comment),
	///    this returns null. Note that the specialized interfaces may contain additional
	///    and more convenient mechanisms to get and set the relevant information.</p>
	class DomNode
	{
	public:
		DomNode();

		/// \brief Constructs a DomNode
		///
		/// \param copy = Dom Node
		DomNode(const DomNode &copy);

		~DomNode();

		/// \brief An integer indicating which type of node this is.
		enum NodeType
		{
			NULL_NODE = 0,
			ELEMENT_NODE = 1,
			ATTRIBUTE_NODE = 2,
			TEXT_NODE = 3,
			CDATA_SECTION_NODE = 4,
			ENTITY_REFERENCE_NODE = 5,
			ENTITY_NODE = 6,
			PROCESSING_INSTRUCTION_NODE = 7,
			COMMENT_NODE = 8,
			DOCUMENT_NODE = 9,
			DOCUMENT_TYPE_NODE = 10,
			DOCUMENT_FRAGMENT_NODE = 11,
			NOTATION_NODE = 12
		};

		/// \brief Returns the node name.
		/** <p>The return value vary according to the node type as follows:</p>
			<ul>
			<li>Element: name of tag</li>
			<li>Attr: name of attribute</li>
			<li>Text: "#text"</li>
			<li>CDataSection: "#cdata-section"</li>
			<li>EntityReference: name of entity referenced</li>
			<li>Entity: entity name</li>
			<li>ProcessingInstruction: target</li>
			<li>Comment: "#comment"</li>
			<li>Document: "#document"</li>
			<li>DocumentType: document type name</li>
			<li>DocumentFragment: "#document-fragment"</li>
			<li>Notation: notation name</li>
			</ul>*/
		DomString get_node_name() const;

		/// \brief Returns the namespace URI of this node.
		DomString get_namespace_uri() const;

		/// \brief Returns the namespace prefix of the node.
		/** <p>For nodes of any type other than ELEMENT_NODE and ATTRIBUTE_NODE and
			nodes created with a DOM Level 1 method, such as create_element from the
			Document interface, this is always an empty string.</p>*/
		DomString get_prefix() const;

		/// \brief Sets the namespace prefix of the node.
		/** <p>Note that setting this attribute, when permitted, changes the node_name
			attribute, which holds the qualified name, as well as the tag_name and name
			attributes of the Element and Attr interfaces, when applicable.</p>
			<p>Note also that changing the prefix of an attribute that is known to
			have a default value, does not make a new attribute with the default value
			and the original prefix appear, since the namespace_uri and local_name do
			not change.</p>*/
		void set_prefix(const DomString &prefix);

		/// \brief Returns local part of the qualified name of this node.
		/** <p>For nodes of any type other than ELEMENT_NODE and ATTRIBUTE_NODE and
			nodes created with a DOM Level 1 method, such as create_element from the
			Document interface, this is always an empty string.</p>*/
		DomString get_local_name() const;

		/// \brief Returns the node value.
		/** <p>The return value vary according to the node type as follows:</p>
			<ul>
			<li>Element: null</li>
			<li>Attr: value of attribute</li>
			<li>Text: content of text node</li>
			<li>CDataSection: content of CDATA Section</li>
			<li>EntityReference: null</li>
			<li>Entity: null</li>
			<li>ProcessingInstruction: entire content excluding the target</li>
			<li>Comment: content of the comment</li>
			<li>Document: null</li>
			<li>DocumentType: null</li>
			<li>DocumentFragment: null</li>
			<li>Notation: null</li>
			</ul>*/
		DomString get_node_value() const;

		/// \brief Sets the node value.
		void set_node_value(const DomString &value);

		/// \brief Returns the node type (one of those in the NodeType enum).
		unsigned short get_node_type() const;

		/// \brief Returns the parent of this node.
		/** <p>All nodes, except Document, DocumentFragment, and Attr may have a parent.
			However, if a node has just been created and not yet added to the tree, or if
			it has been removed from the tree, this is null.</p>*/
		DomNode get_parent_node() const;

		/// \brief Returns a NodeList that contains all children of this node.
		/** <p>If there are no children, this is a NodeList containing no nodes. The content
			of the returned NodeList is "live" in the sense that, for instance, changes to
			the children of the node object that it was created from are immediately reflected
			in the nodes returned by the NodeList accessors; it is not a static snapshot of
			the content of the node. This is true for every NodeList, including the ones
			returned by the getElementsByTagName method.</p>*/
		DomNodeList get_child_nodes() const;

		/// \brief The first child of this node.
		/** <p>If there is no such node, this returns a null node.</p>*/
		DomNode get_first_child() const;

		/// \brief The last child of this node.
		/** <p>If there is no such node, this returns a null node.</p>*/
		DomNode get_last_child() const;

		/// \brief The node immediately preceding this node.
		/** <p>If there is no such node, this returns a null node.</p>*/
		DomNode get_previous_sibling() const;

		/// \brief The node immediately following this node.
		/** <p>If there is no such node, this returns a null node.</p>*/
		DomNode get_next_sibling() const;

		/// \brief A NamedNodeMap containing the attributes of this node (if it is an Element) or null otherwise.
		DomNamedNodeMap get_attributes() const;

		/// \brief The Document object associated with this node.
		/** <p>This is also the Document object used to create new nodes. When this node is a Document this is null.</p>*/
		DomDocument get_owner_document() const;

		/// \brief Returns true if this is a null node.
		bool is_null() const;

		/// \brief Returns true if this is an element node.
		bool is_element() const;

		/// \brief Returns true if this is an attribute node.
		bool is_attr() const;

		/// \brief Returns true if this is a text node.
		bool is_text() const;

		/// \brief Returns true if this is a CDATA section node.
		bool is_cdata_section() const;

		/// \brief Returns true if this is an entity reference node.
		bool is_entity_reference() const;

		/// \brief Returns true if this is an entity node.
		bool is_entity() const;

		/// \brief Returns true if this is a processing instruction node.
		bool is_processing_instruction() const;

		/// \brief Returns true if this is a comment node.
		bool is_comment() const;

		/// \brief Returns true if this is a document node.
		bool is_document() const;

		/// \brief Returns true if this is a document type node.
		bool is_document_type() const;

		/// \brief Returns true if this is a document fragment node.
		bool is_document_fragment() const;

		/// \brief Returns true if this is a notation node.
		bool is_notation() const;

		/// \brief Tests whether the DOM implementation implements a specific feature and that feature is supported by this node.
		bool is_supported(const DomString &feature, const DomString &version) const;

		/// \brief Returns true if this node (if its an element) has any attributes.
		bool has_attributes() const;

		/// \brief Returns true if this node has any children.
		bool has_child_nodes() const;

		/// \brief Copy assignment operator.
		/** <p>All objects in the DOM are handles to the underlying implementation. Therefore this doesn't
			actually copy contents between two DomNode's, but instead change the two DomNode's to point
			at the same node in the DOM.</p>*/
		DomNode &operator =(const DomNode &copy);

		/// \brief Compare operator.
		bool operator ==(const DomNode &other) const;

		/// \brief Compare operator.
		bool operator !=(const DomNode &other) const;

		/// \brief Merges any adjacent Text nodes.
		/** <p>Puts all Text nodes in the full depth of the sub-tree underneath this node, including
			attribute nodes, into a "normal" form where only structure (e.g., elements, comments,
			processing instructions, CDATA sections, and entity references) separates Text nodes, i.e.,
			there are neither adjacent Text nodes nor empty Text nodes.</p>
			<p>This can be used to ensure that the DOM view of a document is the same as if it were
			saved and re-loaded, and is useful when operations (such as XPointer lookups)
			that depend on a particular document tree structure are to be used.</p>
			<p><b>Note:</b> In cases where the document contains CDATASections, the normalize operation
			alone may not be sufficient, since XPointers do not differentiate between Text nodes and
			CDATASection nodes.</p>*/
		void normalize();

		/// \brief Inserts the node new_child before the existing child node ref_child.
		/** <p>If refChild is a null node, inserts new_child at the end of the list of children.</p>
			<p>If newChild is a DocumentFragment object, all of its children are inserted, in the same order,
			before ref_child. If the new_child is already in the tree, it is first removed.</p>
			\param new_child The node to insert.
			\param ref_child The reference node, i.e., the node before which the new node must be inserted.
			retval: The node being inserted.*/
		DomNode insert_before(DomNode &new_child, DomNode &ref_child);

		/// \brief Replaces the child node old_child with new_child in the list of children.
		/** <p>If the new_child is already in the tree, it is first removed.</p>
			\param new_child The new node to put in the child list.
			\param old_child The node being replaced in the list.
			retval: The node replaced.*/
		DomNode replace_child(DomNode &new_child, DomNode &old_child);

		/// \brief Removes the child node indicated by old_child from the list of children, and returns it.
		DomNode remove_child(DomNode &old_child);

		/// \brief Adds the node new_child to the end of the list of children of this node.
		/** <p>If the new_child is already in the tree, it is first removed.</p>*/
		DomNode append_child(DomNode new_child);

		/// \brief Returns a duplicate of this node, i.e., serves as a generic copy constructor for nodes.
		/** <p>The duplicate node has no parent.</p>
			<p>Cloning an Element copies all attributes and their values, including those generated
			by the XML processor to represent defaulted attributes, but this method does not copy
			any text it contains unless it is a deep clone, since the text is contained in a child
			Text node. Cloning any other type of node simply returns a copy of this node.</p>
			\param deep If true, recursively clone the subtree under the specified node; if false, clone only the node itself (and its attributes, if it is an Element).
			retval: The duplicate node.*/
		DomNode clone_node(bool deep) const;

		/// \brief Returns the Element interface to this node.
		/** <p>If the node is not an Element node, then it returns a null node.</p>*/
		DomElement to_element() const;

		/// \brief Returns the Attribute interface to this node.
		/** <p>If the node is not an Attribute node, then it returns a null node.</p>*/
		DomAttr to_attr() const;

		/// \brief Returns the Text interface to this node.
		/** <p>If the node is not a Text node, then it returns a null node.</p>*/
		DomText to_text() const;

		/// \brief Returns the CDATA Section interface to this node.
		/** <p>If the node is not a CDATA Section node, then it returns a null node.</p>*/
		DomCDATASection to_cdata_section() const;

		/// \brief Returns the Entity Reference interface to this node.
		/** <p>If the node is not an Entity Reference node, then it returns a null node.</p>*/
		DomEntityReference to_entity_reference() const;

		/// \brief Returns the Entity interface to this node.
		/** <p>If the node is not an Entity node, then it returns a null node.</p>*/
		DomEntity to_entity() const;

		/// \brief Returns the Processing Instruction interface to this node.
		/** <p>If the node is not a Processing Instrucion node, then it returns a null node.</p>*/
		DomProcessingInstruction to_processing_instruction() const;

		/// \brief Returns the Comment interface to this node.
		/** <p>If the node is not a Comment node, then it returns a null node.</p>*/
		DomComment to_comment() const;

		/// \brief Returns the Document interface to this node.
		/** <p>If the node is not a Document node, then it returns a null node.</p>*/
		DomDocument to_document() const;

		/// \brief Returns the Document Type interface to this node.
		/** <p>If the node is not a Document Type node, then it returns a null node.</p>*/
		DomDocumentType to_document_type() const;

		/// \brief Returns the Document Fragment interface to this node.
		/** <p>If the node is not a DocumentFragment node, then it returns a null node.</p>*/
		DomDocumentFragment to_document_fragment() const;

		/// \brief Returns the Notation interface to this node.
		/** <p>If the node is not a Notation node, then it returns a null node.</p>*/
		DomNotation to_notation() const;

		/// \brief Returns the first child node with the specified node name.
		/** <p>Returns a null node if no child is found.</p>*/
		DomNode named_item(const DomString &name) const;

		/// \brief Retrieves the first child node with the specified namespace URI and local name.
		DomNode named_item_ns(
			const DomString &namespace_uri,
			const DomString &local_name) const;

		/// \brief Searches the node tree upwards for the namespace URI of the given qualified name.
		DomString find_namespace_uri(const DomString &qualified_name) const;

		/// \brief Searches the node tree upwards for the prefix name for the namespace URI.
		DomString find_prefix(const DomString &namespace_uri) const;

		/// \brief Returns all the nodes matching the specified xpath expression using this node as the context node.
		std::vector<DomNode> select_nodes(const DomString &xpath_expression) const;

		/// \brief Returns the first node matching the specified xpath expression using this node as the context node.
		DomNode select_node(const DomString &xpath_expression) const;

		/// \brief Returns the first node value matching the specified xpath expression using this node as the context node.
		std::string select_string(const DomString &xpath_expression) const;

		/// \brief Returns the first node value (as integer) matching the specified xpath expression using this node as the context node.
		int select_int(const DomString &xpath_expression) const;

		/// \brief Returns the first node value (as float) matching the specified xpath expression using this node as the context node.
		float select_float(const DomString &xpath_expression) const;

		/// \brief Returns the first node value (as boolean) matching the specified xpath expression using this node as the context node.
		bool select_bool(const DomString &xpath_expression) const;

	protected:
		DomNode(DomDocument doc, unsigned short node_type);
		DomNode(const std::shared_ptr<DomNode_Impl> &impl);

		std::shared_ptr<DomNode_Impl> impl;

		friend class DomDocument;
		friend class DomNamedNodeMap;
	};

	/// \}
}
