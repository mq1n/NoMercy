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

#include "vec4.h"
#include "vec3.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	class AxisAlignedBoundingBox;
	class OrientedBoundingBox;
	class FrustumPlanes;

	class IntersectionTest
	{
	public:
		enum Result
		{
			outside,
			inside,
			intersecting,
		};

		enum OverlapResult
		{
			disjoint,
			overlap
		};

		static Result plane_aabb(const Vec4f &plane, const AxisAlignedBoundingBox &aabb);
		static Result plane_obb(const Vec4f &plane, const OrientedBoundingBox &obb);
		static OverlapResult sphere(const Vec3f &center1, float radius1, const Vec3f &center2, float radius2);
		static OverlapResult sphere_aabb(const Vec3f &center, float radius, const AxisAlignedBoundingBox &aabb);
		static OverlapResult aabb(const AxisAlignedBoundingBox &a, const AxisAlignedBoundingBox &b);
		static Result frustum_aabb(const FrustumPlanes &frustum, const AxisAlignedBoundingBox &box);
		static Result frustum_obb(const FrustumPlanes &frustum, const OrientedBoundingBox &box);
		static OverlapResult ray_aabb(const Vec3f &ray_start, const Vec3f &ray_end, const AxisAlignedBoundingBox &box);
	};

	/// \}
}
