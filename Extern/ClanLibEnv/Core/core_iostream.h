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
**    Ingo Ruhnke
*/

#pragma once

#include <iosfwd>
#include <iostream>

namespace clan
{
	class Rect;
	class Rectf;
	class Rectd;
	class Point;
	class Pointf;
	class Pointd;
	class Size;
	class Sizef;
	class Sized;

	std::ostream& operator<<(std::ostream& s, const Rect& rect);
	std::ostream& operator<<(std::ostream& s, const Rectf& rect);
	std::ostream& operator<<(std::ostream& s, const Rectd& rect);
	std::ostream& operator<<(std::ostream& s, const Point& point);
	std::ostream& operator<<(std::ostream& s, const Pointf& point);
	std::ostream& operator<<(std::ostream& s, const Pointd& point);
	std::ostream& operator<<(std::ostream& s, const Size& size);
	std::ostream& operator<<(std::ostream& s, const Sizef& size);

	template<typename T>
	std::ostream& operator<<(std::ostream& s, const Vec2<T>& vec)
	{
		s << "["
			<< vec.x << ", "
			<< vec.y << "]";
		return s;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& s, const Vec3<T>& vec)
	{
		s << "["
			<< vec.x << ", "
			<< vec.y << ", "
			<< vec.z << "]";
		return s;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& s, const Vec4<T>& vec)
	{
		s << "["
			<< vec.x << ", "
			<< vec.y << ", "
			<< vec.z << ", "
			<< vec.w << "]";
		return s;
	}
}
