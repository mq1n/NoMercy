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

#include "../Resources/xml_resource_node.h"
#include "../../Core/IOData/file_system.h"
#include <vector>
#include <memory>

namespace clan
{
	/// \addtogroup clanXML_Resources clanXML Resources
	/// \{

	class IODevice;
	class XMLResourceNode;
	class FileSystem;
	class XMLResourceDocument_Impl;

	/// \brief XML Resource Document.
	class XMLResourceDocument
	{
	public:
		/// \brief Construct a XMLResourceDocument.
		XMLResourceDocument();

		/// \brief Constructs a XMLResourceDocument
		///
		/// \param filename = String
		XMLResourceDocument(const std::string &filename);

		/// \brief Constructs a XMLResourceDocument
		///
		/// \param filename = String
		/// \param directory = Virtual Directory
		XMLResourceDocument(const std::string &filename, FileSystem fs);

		/// \brief Constructs a XMLResourceDocument
		///
		/// \param file = IODevice
		/// \param directory = Virtual Directory
		XMLResourceDocument(IODevice file, const std::string &base_path, FileSystem fs);

		/// \brief Constructs a XMLResourceDocument
		///
		/// \param other = XMLResourceDocument
		XMLResourceDocument(const XMLResourceDocument &other);

		~XMLResourceDocument();

		/// \brief Returns true if a resource exists.
		bool resource_exists(const std::string &resource_id) const;

		/// \brief Returns all the resource sections available.
		std::vector<std::string> get_section_names() const;

		/// \brief Returns a list of all resources available.
		/** <p>The returned resources are in the form "section/subsection/.../resourcename".</p>*/
		std::vector<std::string> get_resource_names() const;

		std::vector<std::string> get_resource_names(const std::string &section) const;

		/// \brief Returns a list of all resources available matching a given type.
		/** <p>The returned resources are in the form "section/subsection/.../resourcename".</p>*/
		std::vector<std::string> get_resource_names_of_type(const std::string &type) const;

		std::vector<std::string> get_resource_names_of_type(
			const std::string &type,
			const std::string &section) const;

		/// \brief Returns Resource representing the given resource.
		XMLResourceNode get_resource(
			const std::string &resource_id) const;

		/// \brief Returns the value of a boolean resource. (using the value attribute)
		bool get_boolean_resource(
			const std::string &resource_id,
			bool default_value) const;

		/// \brief Returns the value of an integer resource. (using the value attribute)
		int get_integer_resource(
			const std::string &resource_id,
			int default_value) const;

		/// \brief Returns the value of an string resource. (using the value attribute)
		std::string get_string_resource(
			const std::string &resource_id,
			const std::string &default_value) const;

		XMLResourceDocument &operator =(const XMLResourceDocument &copy);

		bool operator ==(const XMLResourceDocument &that) const;

		/// \brief Add resources from an other resource document.
		/** <p>This function only makes the resource document search other documents, it
			does not copy the resources into this document.</p>*/
		void add_resources(const XMLResourceDocument& additional_resources);

		/// \brief Remove resources from an other resource document.
		void remove_resources(const XMLResourceDocument& additional_resources);

		/// \brief Construct a new resource object.
		XMLResourceNode create_resource(const std::string &resource_id, const std::string &type);

		/// \brief Destroy resource object.
		void destroy_resource(const std::string &resource_id);

		/// \brief Save resource XML tree to file.
		void save(const std::string &filename);

		/// \brief Save
		///
		/// \param filename = the filename to save
		/// \param directory = Virtual Directory
		void save(const std::string &filename, const FileSystem &file_system);

		/// \brief Save
		///
		/// \param file = IODevice
		void save(IODevice file);

		/// \brief Load resource XML tree from file.
		void load(const std::string &filename);

		/// \brief Load
		///
		/// \param filename = the filename to save
		/// \param directory = Virtual Directory
		void load(const std::string &filename, const FileSystem &file_system);

		/// \brief Load
		///
		/// \param file = the file to load
		/// \param directory = Virtual Directory
		void load(IODevice file, const std::string &base_path = std::string(), const FileSystem &file_system = FileSystem());

	private:
		/// \brief Constructs a XMLResourceDocument
		///
		/// \param XMLResourceDocument_Impl = Weak Ptr
		XMLResourceDocument(std::weak_ptr<XMLResourceDocument_Impl> &impl);

		std::shared_ptr<XMLResourceDocument_Impl> impl;

		friend class XMLResourceNode;
		friend class XMLResourceDocument_Impl;
	};

	/// \}
}
