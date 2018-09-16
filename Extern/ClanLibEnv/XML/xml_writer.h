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

namespace clan
{
	/// \addtogroup clanXML_XML clanXML XML
	/// \{

	class IODevice;
	class XMLToken;
	class XMLWriter_Impl;

	/// \brief The XML Writer writes a XML file based on XML tokens.
	class XMLWriter
	{
	public:
		XMLWriter();

		/// \brief Constructs a XMLWriter
		///
		/// \param copy = XMLWriter
		XMLWriter(const XMLWriter &copy);

		/// \brief Constructs a XMLWriter
		///
		/// \param output = IODevice
		XMLWriter(IODevice &output);

		virtual ~XMLWriter();

		/// \brief Returns the insert whitespace flag.
		bool get_insert_whitespace() const;

		/// \brief Inserts whitespace between tags if enabled.
		void set_insert_whitespace(bool enable);

		/// \brief Write token to file.
		void write(const XMLToken &token);

	private:
		std::shared_ptr<XMLWriter_Impl> impl;
	};

	/// \}
}
