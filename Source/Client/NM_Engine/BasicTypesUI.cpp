#include "NM_Index.h"
#include "NM_Main.h"
#include "BasicTypesUI.h"

namespace gui
{
	/* point */
	point::point()
		: x(0), y(0)
	{

	}

	point::point(unsigned int x, unsigned int y)
		: x(x), y(y)
	{

	}

	bool point::operator==(const point& pt) const
	{
		return ((this->x == pt.x) && (this->y == pt.y));
	}

	bool point::operator!=(const point& pt) const
	{
		return ((this->x != pt.x) || (this->y != pt.y));
	}

	bool point::operator<(const point& pt) const
	{
		return ((this->y < pt.y) || (this->y == pt.y && this->x < pt.x));
	}

	bool point::operator<=(const point& pt) const
	{
		return ((this->y < pt.y) || (this->y == pt.y && this->x <= pt.x));
	}

	bool point::operator>(const point& pt) const
	{
		return ((this->y > pt.y) || (this->y == pt.y && this->x > pt.x));
	}

	bool point::operator>=(const point& pt) const
	{
		return ((this->y > pt.y) || (this->y == pt.y && this->x >= pt.x));
	}

	unsigned int point::get_x()
	{
		return this->x;
	}

	unsigned int point::get_y()
	{
		return this->y;
	}

	void point::set_x(unsigned int x)
	{
		this->x = x;
	}

	void point::set_y(unsigned int y)
	{
		this->y = y;
	}

	void point::set_xy(unsigned int x, unsigned int y)
	{
		this->x = x;
		this->y = y;
	}

	/* size */
	size::size()
		: width(0), height(0)
	{

	}

	size::size(unsigned int width, unsigned int height)
		: width(width), height(height)
	{

	}

	size::size(const size& sz)
		: width(sz.width), height(sz.height)
	{

	}

	bool size::is_zero() const
	{
		return (this->width == 0 || this->height == 0);
	}

	bool size::operator==(const size& sz) const
	{
		return ((this->width == sz.width) && (this->height == sz.height));
	}

	bool size::operator!=(const size& sz) const
	{
		return ((this->width != sz.width) || (this->height != sz.height));
	}

	unsigned int size::get_width()
	{
		return this->width;
	}

	unsigned int size::get_height()
	{
		return this->height;
	}

	void size::set_width(unsigned int width)
	{
		this->width = width;
	}

	void size::set_height(unsigned int height)
	{
		this->height = height;
	}

	void size::set_width_height(unsigned int width, unsigned int height)
	{
		this->width = width;
		this->height = height;
	}

	/* rectangle */
	rectangle::rectangle()
		: point(0, 0), size(0, 0)
	{

	}

	rectangle::rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
		: point(x, y), size(width, height)
	{

	}

	rectangle::rectangle(const size& sz)
		: point(0, 0), size(sz)
	{

	}

	rectangle::rectangle(const point& pt, const size& sz)
		: point(pt.x, pt.y), size(sz.width, sz.height)
	{

	}

	bool rectangle::operator==(const rectangle& rect) const
	{
		return ((this->width == rect.width) && (this->height == rect.height));
	}

	bool rectangle::operator!=(const rectangle& rect) const
	{
		return ((this->width != rect.width) || (this->height != rect.height));
	}

	rectangle& rectangle::operator=(const point& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}

	rectangle& rectangle::operator=(const size& sz)
	{
		width = sz.width;
		height = sz.height;
		return *this;
	}
}