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
#include <vector>

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class EarClipResult_Impl;
	class EarClipTriangulator_Triangle;

	/// \brief Ear clipping triangulation result structure.
	class EarClipResult
	{
	public:
		/// \brief Constructs an ear clipping result structure.
		EarClipResult(int num_triangles);

		virtual ~EarClipResult();

		/// \brief Returns the triangles for this result.
		std::vector<EarClipTriangulator_Triangle> &get_triangles();

		/// \brief return a reference to a triangle in the triangulation.
		/** Memory  is preallocated for all triangles at creation. Use the returned reference to modify the triangles.*/
		EarClipTriangulator_Triangle &get_triangle(int index);

	private:
		std::shared_ptr<EarClipResult_Impl> impl;
	};

	/// \}
}
