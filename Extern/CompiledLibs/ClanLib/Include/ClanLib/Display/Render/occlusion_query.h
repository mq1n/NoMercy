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
**    Harry Storbacka
*/

#pragma once

#include <memory>
#include "graphic_context.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class OcclusionQuery_Impl;
	class GraphicContext;
	class OcclusionQueryProvider;

	/// \brief Graphics occlusion query class.
	class OcclusionQuery
	{
	public:
		/// \brief Constructs a null instance.
		OcclusionQuery();

		/// \brief Constructs an occlusion query object.
		OcclusionQuery(GraphicContext &context);

		virtual ~OcclusionQuery();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns the result of the occlusion query.
		int get_result();

		/// \brief Returns true if the GPU is ready to return the result.
		bool is_result_ready();

		/// \brief Get Provider
		///
		/// \return provider
		OcclusionQueryProvider *get_provider() const;

		/// \brief Start occlusion query.
		void begin();

		/// \brief Finish occlusion query.
		void end();

	private:
		std::shared_ptr<OcclusionQuery_Impl> impl;
	};

	/// \}
}
