#pragma once

#include <Windows.h>

namespace gui
{
	class point
	{
		friend class rectangle;

	public:
		point();
		point(unsigned int x, unsigned int y);

		virtual bool operator==(const point& pt) const;
		virtual bool operator!=(const point& pt) const;
		virtual bool operator<(const point& pt) const;
		virtual bool operator<=(const point& pt) const;
		virtual bool operator>(const point& pt) const;
		virtual bool operator>=(const point& pt) const;

		unsigned int get_x();
		unsigned int get_y();

		void set_x(unsigned int x);
		void set_y(unsigned int y);
		void set_xy(unsigned int x, unsigned int y);

	private:
		unsigned int x;
		unsigned int y;
	};

	class size
	{
		friend class rectangle;

	public:
		size();
		size(unsigned int width, unsigned int height);
		size(const size& sz);

		bool is_zero() const;
		virtual bool operator==(const size& sz) const;
		virtual bool operator!=(const size& sz) const;

		unsigned int get_width();
		unsigned int get_height();

		void set_width(unsigned int width);
		void set_height(unsigned int height);
		void set_width_height(unsigned int width, unsigned int height);

	private:
		unsigned int width;
		unsigned int height;
	};

	class rectangle : public point, public size
	{
	public:
		rectangle();
		rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		rectangle(const size& sz);
		rectangle(const point& pt, const size& sz = size());

		bool operator==(const rectangle& rect) const;
		bool operator!=(const rectangle& rect) const;

		rectangle& operator=(const point& pt);
		rectangle& operator=(const size& sz);
	};
}