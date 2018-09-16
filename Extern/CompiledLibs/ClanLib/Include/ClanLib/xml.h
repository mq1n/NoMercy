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

/// \brief <p>XML Put Text Here</p>
//! Global=XML

#pragma once

#ifdef WIN32
#pragma warning( disable : 4786)
#endif

#ifdef __cplusplus_cli
#pragma managed(push, off)
#endif

#include "XML/dom_processing_instruction.h"
#include "XML/dom_entity_reference.h"
#include "XML/dom_notation.h"
#include "XML/dom_exception.h"
#include "XML/dom_cdata_section.h"
#include "XML/dom_document_fragment.h"
#include "XML/dom_comment.h"
#include "XML/dom_attr.h"
#include "XML/dom_entity.h"
#include "XML/dom_document_type.h"
#include "XML/dom_document.h"
#include "XML/dom_implementation.h"
#include "XML/dom_node_list.h"
#include "XML/dom_text.h"
#include "XML/dom_node.h"
#include "XML/dom_character_data.h"
#include "XML/dom_named_node_map.h"
#include "XML/dom_element.h"
#include "XML/dom_string.h"
#include "XML/xml_tokenizer.h"
#include "XML/xml_writer.h"
#include "XML/xml_token.h"
#include "XML/xpath_evaluator.h"
#include "XML/xpath_object.h"
#include "XML/Resources/resource_factory.h"
#include "XML/Resources/xml_resource_node.h"
#include "XML/Resources/xml_resource_document.h"
#include "XML/Resources/xml_resource_manager.h"

#ifdef __cplusplus_cli
#pragma managed(pop)
#endif

#if defined(_MSC_VER)
	#if !defined(_MT)
		#error Your application is set to link with the single-threaded version of the run-time library. Go to project settings, in the C++ section, and change it to multi-threaded.
	#endif
	#if !defined(_DEBUG)
		#if defined(DLL)
			#pragma comment(lib, "clanXML-dll.lib")
		#elif defined(_DLL)
			#pragma comment(lib, "clanXML-static-mtdll.lib")
		#else
			#pragma comment(lib, "clanXML-static-mt.lib")
		#endif
	#else
		#if defined(DLL)
			#pragma comment(lib, "clanXML-dll-debug.lib")
		#elif defined(_DLL)
			#pragma comment(lib, "clanXML-static-mtdll-debug.lib")
		#else
			#pragma comment(lib, "clanXML-static-mt-debug.lib")
		#endif
	#endif
#endif
