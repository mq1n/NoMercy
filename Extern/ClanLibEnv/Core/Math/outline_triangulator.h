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
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class OutlineTriangulator_Impl;

	/// \brief Polygon Tesselator.
	///
	/// This class uses constrained delauney triangulation to convert polygon outlines into triangles.
	class OutlineTriangulator
	{
	public:
		/// \brief Creates a tessellation object.
		OutlineTriangulator();

		virtual ~OutlineTriangulator();

		/// \brief This function specifies a vertex on a polygon.
		void add_vertex(float x, float y, void *data);

		/// \brief Mark next contour in polygon path.
		void next_contour();

		/// \brief Mark next polygon.
		void next_polygon();

		/// \brief Converts passed polygons into triangles.
		void generate();

	private:
		std::shared_ptr<OutlineTriangulator_Impl> impl;
	};

	/// \}
}
