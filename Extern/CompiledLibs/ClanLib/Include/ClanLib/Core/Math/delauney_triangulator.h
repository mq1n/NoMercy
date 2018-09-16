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
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief Vertex in the delauney triangulation.
	class DelauneyTriangulator_Vertex
	{
	public:
		/// \brief Data pointer given when adding the vertex.
		void *data;

		/// \brief X position of vertex.
		float x;

		/// \brief Y position of vertex.
		float y;
	};

	/// \brief Triangle generated from a delauney triangulation.
	class DelauneyTriangulator_Triangle
	{
	public:
		/// \brief First point in the triangle.
		DelauneyTriangulator_Vertex *vertex_A;

		/// \brief Second point in the triangle.
		DelauneyTriangulator_Vertex *vertex_B;

		/// \brief Third point in the triangle.
		DelauneyTriangulator_Vertex *vertex_C;
	};

	class DelauneyTriangulator_Impl;

	/// \brief Delauney triangulator.
	///
	///    <p>This class uses the <a href="http://astronomy.swin.edu.au/~pbourke/terrain/triangulate/">
	///    delauney triangulation algorithm</a> to produce
	///    triangles between a list of points.</p>
	class DelauneyTriangulator
	{
	public:
		/// \brief Creates a triangulator object.
		DelauneyTriangulator();

		virtual ~DelauneyTriangulator();

		/// \brief Returns the list of vertices in the triangulation.
		const std::vector<DelauneyTriangulator_Vertex> &get_vertices() const;

		/// \brief Returns the resulting triangles produced from triangulation.
		const std::vector<DelauneyTriangulator_Triangle> &get_triangles() const;

		/// \brief This function specifies a point to be used in the triangulation.
		void add_vertex(float x, float y, void *data);

		/// \brief Converts passed points into triangles.
		void generate();

	private:
		std::shared_ptr<DelauneyTriangulator_Impl> impl;
	};

	/// \}
}
