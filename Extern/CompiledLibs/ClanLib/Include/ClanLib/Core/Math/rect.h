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
**    Kenneth Gangstoe
**    Harry Storbacka
**    Mark Page
*/

#pragma once

#include "vec2.h"
#include "size.h"
#include "point.h"
#include "origin.h"
#include "cl_math.h"

namespace clan
{
	/// \addtogroup clanCore_Math clanCore Math
	/// \{

	/// \brief 2D (left,top,right,bottom) rectangle structure.
	///
	/// These line templates are defined for: int (Rect), float (Rectf), double (Rectd)
	///
	template<typename Type>
	class Rectx
	{
	public:
		/// \brief Constructs an rectangle.
		///
		/// Initialised to zero
		Rectx() : left(0), top(0), right(0), bottom(0) {}

		/// \brief Constructs an rectangle.
		///
		/// \param s = Size
		Rectx(const Sizex<Type> &s) : left(0), top(0), right(s.width), bottom(s.height) {}

		/// \brief Constructs an rectangle.
		///
		/// \param new_left Initial left position of rectangle.
		/// \param new_top Initial top position of rectangle.
		/// \param new_right Initial right position of rectangle.
		/// \param new_bottom Initial bottom position of rectangle.
		Rectx(Type new_left, Type new_top, Type new_right, Type new_bottom)
			: left(new_left), top(new_top), right(new_right), bottom(new_bottom) {}

		/// \brief Constructs an rectangle.
		///
		/// \param p = Initial top-left position of rectangle.
		/// \param size Initial size of rectangle.
		Rectx(const Pointx<Type> &p, const Sizex<Type> &size)
			: left(p.x), top(p.y), right(p.x + size.width), bottom(p.y + size.height) {}

		/// \brief Constructs an rectangle.
		///
		/// \param new_left Initial left position of rectangle.
		/// \param new_top Initial top position of rectangle.
		/// \param size Initial size of rectangle.
		Rectx(Type new_left, Type new_top, const Sizex<Type> &size)
			: left(new_left), top(new_top), right(new_left + size.width), bottom(new_top + size.height) {}

		/// \brief Constructs an rectangle.
		///
		/// \param rect Initial rectangle position and size.
		Rectx(const Rectx<int> &rect);

		/// \brief Constructs an rectangle.
		///
		/// \param rect Initial rectangle position and size.
		Rectx(const Rectx<float> &rect);

		/// \brief Constructs an rectangle.
		///
		/// \param rect Initial rectangle position and size.
		Rectx(const Rectx<double> &rect);

		/// \brief Rect == Rect operator.
		bool operator==(const Rectx<Type> &r) const
		{
			return (left == r.left && top == r.top && right == r.right && bottom == r.bottom);
		}

		/// \brief Rect != Rect operator.
		bool operator!=(const Rectx<Type> &r) const
		{
			return (left != r.left || top != r.top || right != r.right || bottom != r.bottom);
		}

		/// \brief Rect *= operator.
		Rectx<Type> &operator*=(const Type &s)
		{
			left *= s; top *= s; right *= s; bottom *= s; return *this;
		}

		/// \brief Rect * operator.
		Rectx<Type> operator*(const Type &s) const
		{
			return Rectx<Type>(left*s, top*s, right*s, bottom *s);
		}

		static Rectx<Type> xywh(Type x, Type y, Type width, Type height) { return Rectx<Type>(x, y, x + width, y + height); }
		static Rectx<Type> ltrb(Type left, Type top, Type right, Type bottom) { return Rectx<Type>(left, top, right, bottom); }

		/// \brief X1-coordinate (left point inside the rectangle)
		Type left;

		/// \brief Y1-coordinate (top point inside the rectangle)
		Type top;

		/// \brief X2-coordinate (point outside the rectangle)
		Type right;

		/// \brief Y2-coordinate (point outside the rectange)
		Type bottom;

		/// \brief Returns the width of the rectangle.
		Type get_width() const { return right - left; }

		/// \brief Returns the height of the rectangle.
		Type get_height() const { return bottom - top; }

		/// \brief Returns the size of the rectangle.
		Sizex<Type> get_size() const { return Sizex<Type>(right - left, bottom - top); }

		/// \brief Returns true if the rectangle contains the point.
		bool contains(const Vec2<Type> &p) const
		{
			return (p.x >= left && p.x < right)
				&& (p.y >= top && p.y < bottom);
		}

		/// \brief Returns the top-left point inside the rectangle
		Pointx<Type> get_top_left() const
		{
			return Pointx<Type>(left, top);
		}

		/// \brief Returns the top-right point outside the rectangle
		Pointx<Type> get_top_right() const
		{
			return Pointx<Type>(right, top);
		}

		/// \brief Returns the bottom-right point outside the rectangle
		Pointx<Type> get_bottom_right() const
		{
			return Pointx<Type>(right, bottom);
		}

		/// \brief Returns the bottom-left point outside the rectangle
		Pointx<Type> get_bottom_left() const
		{
			return Pointx<Type>(left, bottom);
		}

		/// \brief Returns true if rectangle passed is overlapping or inside this rectangle.
		bool is_overlapped(const Rectx<Type> &r) const
		{
			return (r.left < right && r.right > left && r.top < bottom && r.bottom > top);
		}

		/// \brief Returns true if rectangle passed is inside this rectangle
		bool is_inside(const Rectx<Type> &r) const
		{
			return ((left <= r.left)
				&& (top <= r.top)
				&& (right >= r.right)
				&& (bottom >= r.bottom));
		}

		/// \brief Returns another Rectx<Type> containing a rotated version of this one.
		///
		/// \param hotspot Point to rotate around.
		/// \param angle Angle to rotate.
		Rectx<Type> get_rot_bounds(const Vec2<Type> &hotspot, const Angle &angle) const;

		/// \brief Returns another Rectx<Type> containing a rotated version of this one.
		///
		/// \param origin Determines the hotspot point within the rectangle
		/// \param x Offsets applied negatively to the hotspot point
		/// \param y Offsets applied negatively to the hotspot point
		/// \param angle Angle
		Rectx<Type> get_rot_bounds(Origin origin, Type x, Type y, const Angle &angle) const;

		/// \brief Returns the center point of the rectangle.
		Pointx<Type> get_center() const
		{
			return Pointx<Type>((left + right) / 2, (top + bottom) / 2);
		}

		/// \brief Sets the top-left point of the rectangle.
		///
		/// \return reference to this object
		Rectx<Type> &set_top_left(const Vec2<Type>& p)
		{
			left = p.x;
			top = p.y;
			return *this;
		}

		/// \brief Sets the bottom-right point of the rectangle.
		///
		/// \return reference to this object
		Rectx<Type> &set_bottom_right(const Vec2<Type>& p)
		{
			right = p.x;
			bottom = p.y;
			return *this;
		}

		/// \brief Sets the width of the rectangle.
		///
		/// \return reference to this object
		Rectx<Type> &set_width(Type width)
		{
			right = left + width;
			return *this;
		}

		/// \brief Sets the height of the rectangle.
		///
		/// \return reference to this object
		Rectx<Type> &set_height(Type height)
		{
			bottom = top + height;
			return *this;
		}

		/// \brief Shrink the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &shrink(const Type &new_left, const Type &new_top, const Type &new_right, const Type &new_bottom)
		{
			left += new_left; top += new_top; right -= new_right; bottom -= new_bottom;
			return *this;
		};

		/// \brief Shrink the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &shrink(const Type &left_right, const Type &top_bottom)
		{
			left += left_right; top += top_bottom; right -= left_right; bottom -= top_bottom;
			return *this;
		};

		/// \brief Shrink the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &shrink(const Type &shrink)
		{
			left += shrink; top += shrink; right -= shrink; bottom -= shrink;
			return *this;
		};

		/// \brief Expand the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &expand(const Type &expand_left, const Type &expand_top, const Type &expand_right, const Type &expand_bottom)
		{
			left -= expand_left; top -= expand_top; right += expand_right; bottom += expand_bottom;
			return *this;
		};

		/// \brief Expand the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &expand(const Type &left_and_right, const Type &top_and_bottom)
		{
			left -= left_and_right;
			right += left_and_right;
			top -= top_and_bottom;
			bottom += top_and_bottom;
			return *this;
		};

		/// \brief Expand the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &expand(const Type &expand)
		{
			left -= expand;
			right += expand;
			top -= expand;
			bottom += expand;
			return *this;
		};

		/// \brief Translate the rect
		///
		/// \return reference to this object
		Rectx<Type> &translate(const Vec2<Type> &p)
		{
			left += p.x; top += p.y; right += p.x; bottom += p.y;
			return *this;
		};

		/// \brief Translate the rect
		///
		/// \return reference to this object
		Rectx<Type> &translate(const Sizex<Type> &p)
		{
			left += p.width; top += p.height; right += p.width; bottom += p.height;
			return *this;
		};

		/// \brief Translate the rect by another rect (only uses the left and top coords).
		///
		/// \return reference to this object
		Rectx<Type> &translate(const Rectx<Type> &p)
		{
			left += p.left; top += p.top; right += p.left; bottom += p.top;
			return *this;
		};

		/// \brief Translate the rect
		///
		/// \return reference to this object
		Rectx<Type> &translate(Type x, Type y)
		{
			left += x; top += y; right += x; bottom += y;
			return *this;
		};

		/// \brief Sets the size of the rectangle, maintaining top/left position.
		///
		/// \return reference to this object
		Rectx<Type> &set_size(const Sizex<Type> &size)
		{
			right = left + size.width;
			bottom = top + size.height;
			return *this;
		}

		/// \brief Calculates the intersection of two rectangles. 
		/// 
		/// <p>Rect values become: max left, max top, min right, min bottom.</p>
		///
		/// \return reference to this object
		Rectx<Type> &overlap(const Rectx<Type> &rect)
		{
			Rectx<Type> result;
			result.left = max(left, rect.left);
			result.right = min(right, rect.right);
			result.top = max(top, rect.top);
			result.bottom = min(bottom, rect.bottom);
			if (result.right < result.left)
				result.left = result.right;
			if (result.bottom < result.top)
				result.top = result.bottom;

			*this = result;
			return *this;
		}

		/// \brief Calculates the bounding rectangle of the rectangles. 
		/// 
		/// <p>Rect values become: min left, min top, max right, max bottom.</p>
		/// 
		/// \return reference to this object
		Rectx<Type> &bounding_rect(const Rectx<Type> &rect)
		{
			Rectx<Type> result;
			result.left = min(left, rect.left);
			result.right = max(right, rect.right);
			result.top = min(top, rect.top);
			result.bottom = max(bottom, rect.bottom);
			*this = result;
			return *this;
		}

		/// \brief Normalize rectangle.
		///
		/// Sets the width to 0 if found a negative width
		/// Sets the height to 0 if found a negative height
		///
		/// \return reference to this object
		Rectx<Type> &normalize()
		{
			if (left > right)
				right = left;

			if (top > bottom)
				bottom = top;

			return *this;
		}

		/// \brief Applies an origin and offset pair to this rectangle
		///
		/// \param origin The new origin to adjust to from default upper-left position
		/// \param x, y Offsets applied negatively to each corner of the rectangle
		///
		/// \return reference to this object
		Rectx<Type> &apply_alignment(Origin origin, Type x, Type y)
		{
			Vec2<Type> offset = Vec2<Type>::calc_origin(origin, get_size());
			offset.x -= x;
			offset.y -= y;

			left += offset.x;
			top += offset.y;
			right += offset.x;
			bottom += offset.y;
			return *this;
		}

		/// \brief Clip according to the specified clip rectangle.
		///
		/// \return reference to this object
		Rectx<Type> &clip(const Rectx<Type> &cr)
		{
			top = max(top, cr.top);
			left = max(left, cr.left);
			right = min(right, cr.right);
			bottom = min(bottom, cr.bottom);
			top = min(top, bottom);
			left = min(left, right);
			return *this;
		}
	};

	template<>
	inline Rectx<int>::Rectx(const Rectx<float> &rect)
		: left(static_cast<int> (floor(rect.left + 0.5f))),
		top(static_cast<int> (floor(rect.top + 0.5f))),
		right(static_cast<int> (floor(rect.right + 0.5f))),
		bottom(static_cast<int> (floor(rect.bottom + 0.5f)))
	{
	}

	template<>
	inline Rectx<int>::Rectx(const Rectx<double> &rect)
		: left(static_cast<int> (floor(rect.left + 0.5))),
		top(static_cast<int> (floor(rect.top + 0.5))),
		right(static_cast<int> (floor(rect.right + 0.5))),
		bottom(static_cast<int> (floor(rect.bottom + 0.5)))
	{
	}

	template<typename Type>
	inline Rectx<Type>::Rectx(const Rectx<int> &rect)
		: left(static_cast<Type> (rect.left)), top(static_cast<Type> (rect.top)),
		right(static_cast<Type> (rect.right)), bottom(static_cast<Type> (rect.bottom))
	{
	}

	template<typename Type>
	inline Rectx<Type>::Rectx(const Rectx<float> &rect)
		: left(static_cast<Type> (rect.left)), top(static_cast<Type> (rect.top)),
		right(static_cast<Type> (rect.right)), bottom(static_cast<Type> (rect.bottom))
	{
	}

	template<typename Type>
	inline Rectx<Type>::Rectx(const Rectx<double> &rect)
		: left(static_cast<Type> (rect.left)), top(static_cast<Type> (rect.top)),
		right(static_cast<Type> (rect.right)), bottom(static_cast<Type> (rect.bottom))
	{
	}

	/// \brief 2D (left,top,right,bottom) rectangle structure - Integer
	class Rect : public Rectx<int>
	{
	public:
		Rect() : Rectx<int>() {}
		Rect(const Sizex<int> &s) : Rectx<int>(s) {}
		Rect(int new_left, int new_top, int new_right, int new_bottom) : Rectx<int>(new_left, new_top, new_right, new_bottom) {}
		Rect(const Pointx<int> &p, const Sizex<int> &size) : Rectx<int>(p, size) {}
		Rect(const Rectx<int> &rect) : Rectx<int>(rect) {}
		Rect(const Rectx<float> &rect) : Rectx<int>(rect) {}
		Rect(const Rectx<double> &rect) : Rectx<int>(rect) {}
		Rect(int new_left, int new_top, const Sizex<int> &size) : Rectx<int>(new_left, new_top, size) {}
	};

	/// \brief 2D (left,top,right,bottom) rectangle structure - Float
	class Rectf : public Rectx<float>
	{
	public:
		Rectf() : Rectx<float>() {}
		Rectf(const Sizex<int> &s) : Rectx<float>(s) {}
		Rectf(const Sizex<float> &s) : Rectx<float>(s) {}
		Rectf(float new_left, float new_top, float new_right, float new_bottom) : Rectx<float>(new_left, new_top, new_right, new_bottom) {}
		Rectf(const Pointx<float> &p, const Sizex<float> &size) : Rectx<float>(p, size) {}
		Rectf(const Rectx<int> &rect) : Rectx<float>(rect) {}
		Rectf(const Rectx<float> &rect) : Rectx<float>(rect) {}
		Rectf(const Rectx<double> &rect) : Rectx<float>(rect) {}
		Rectf(float new_left, float new_top, const Sizex<float> &size) : Rectx<float>(new_left, new_top, size) {}
	};

	/// \brief 2D (left,top,right,bottom) rectangle structure - Double
	class Rectd : public Rectx<double>
	{
	public:
		Rectd() : Rectx<double>() {}
		Rectd(const Sizex<int> &s) : Rectx<double>(s) {}
		Rectd(const Sizex<float> &s) : Rectx<double>(s) {}
		Rectd(const Sizex<double> &s) : Rectx<double>(s) {}
		Rectd(double new_left, double new_top, double new_right, double new_bottom) : Rectx<double>(new_left, new_top, new_right, new_bottom) {}
		Rectd(const Pointx<double> &p, const Sizex<double> &size) : Rectx<double>(p, size) {}
		Rectd(const Rectx<int> &rect) : Rectx<double>(rect) {}
		Rectd(const Rectx<float> &rect) : Rectx<double>(rect) {}
		Rectd(const Rectx<double> &rect) : Rectx<double>(rect) {}
		Rectd(double new_left, double new_top, const Sizex<double> &size) : Rectx<double>(new_left, new_top, size) {}
	};

	inline Rect RectPS(int x, int y, int width, int height)
	{
		return Rect(x, y, x + width, y + height);
	}

	inline Rectf RectfPS(float x, float y, float width, float height)
	{
		return Rectf(x, y, x + width, y + height);
	}

	inline Rectd RectdPS(double x, double y, double width, double height)
	{
		return Rectd(x, y, x + width, y + height);
	}

	/// \}
}
