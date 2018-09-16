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
**    Kenneth Gangstoe
*/

#pragma once

#include <memory>
#include "../../Core/Math/origin.h"
#include "../../Core/Resources/resource.h"
#include "color.h"
#include "../Image/image_import_description.h"
#include "../Render/texture.h"

namespace clan
{
	/// \addtogroup clanDisplay_2D clanDisplay 2D
	/// \{

	class GraphicContext;
	class FileSystem;
	class Rect;
	class Size;
	class Rectf;
	class Image_Impl;
	class Texture2D;
	class Subtexture;
	class PixelBuffer;
	class ResourceManager;
	class XMLResourceDocument;
	class Canvas;
	class Quadf;

	/// \brief Image class.
	class Image
	{
	public:
		/// \brief Constructs a null instance.
		Image();

		/// \brief Constructs an image from a texture.
		///
		/// \param texture = Texture to get image data from
		/// \param rect = Position and size in texture to get image data from
		Image(Texture2D texture, const Rect &rect);

		/// \brief Constructs an image from a subtexture.
		///
		/// \param sub_texture = Subtexture to get image data from
		Image(Subtexture &sub_texture);

		/// \brief Constructs a Image from a pixelbuffer.
		///
		/// \param canvas = Canvas
		/// \param pixelbuffer = Pixelbuffer to get image data from
		/// \param rect = pixelbuffer rect
		Image(Canvas &canvas, const PixelBuffer &pixelbuffer, const Rect &rect);

		/// \brief Constructs a Image
		///
		/// \param canvas = Canvas
		/// \param filename Filename of image to load
		/// \param import_desc = Image Import Description
		Image(Canvas &canvas, const std::string &filename, const ImageImportDescription &import_desc = ImageImportDescription());

		/// \brief Constructs a Image
		///
		/// \param canvas = Canvas
		/// \param filename Filename of image to load
		/// \param dir = Virtual directory to load filename from
		/// \param import_desc = Image Import Description
		Image(Canvas &canvas, const std::string &filename, const FileSystem &fs, const ImageImportDescription &import_desc = ImageImportDescription());

		virtual ~Image();

		/// \brief Retrieves a Sprite resource from the resource manager
		///
		/// \param canvas = Canvas
		/// \param resources = Resource manager
		/// \param id = id
		static Resource<Image> resource(Canvas &canvas, const std::string &id, const ResourceManager &resources);

		/// \brief Loads a Sprite from a XML resource definition
		static Image load(Canvas &canvas, const std::string &id, const XMLResourceDocument &doc);

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Returns x scale.
		/** <p> 1.0f is normal scale, 2.0f is twice the size, etc. </p>*/
		float get_scale_x() const;

		/// \brief Returns y scale.
		/** <p> 1.0f is normal scale, 2.0f is twice the size, etc. </p>*/
		float get_scale_y() const;

		/// \brief Returns current alpha.
		/** <p> Alpha 0.0f is full transparency, and 1.0f is full visibility (solid). </p>*/
		float get_alpha() const;

		/// \brief Returns current color.
		/** <p> Alpha 0.0f is full transparency, and 1.0f is full visibility (solid). </p>*/
		Colorf get_color() const;

		/// \brief Returns translation hot-spot.
		void get_alignment(Origin &origin, float &x, float &y) const;

		/// \brief Return the texture of the image
		Subtexture get_texture() const;

		/// \brief Return the size of the image.
		Sizef get_size() const;

		/// \brief Return the width of the image.
		float get_width() const;

		/// \brief Return the height of the image.
		float get_height() const;

		/// \brief Equality operator
		bool operator==(const Image &other) const
		{
			return impl == other.impl;
		}

		/// \brief Inequality operator
		bool operator!=(const Image &other) const
		{
			return impl != other.impl;
		}

		/// \brief Less than operator
		bool operator<(const Image &other) const
		{
			return impl < other.impl;
		}

		/// \brief Copies all information from this image to another, excluding the graphics that remain shared
		Image clone() const;

		/// \brief Draw image on graphic context.
		///
		/// \param x, y Anchor position of where to render image. Actual rendering position depends on the anchor and the alignment mode.
		/// \param gc Graphic context on which to render upon.
		void draw(
			Canvas &canvas,
			float x,
			float y) const;

		/// \brief Draw image on graphic context.
		///
		/// \param gc Graphic context on which to render upon.
		/// \param src Source rectangle to draw. Use this is draw only part of the image.
		/// \param dest Rectangle to draw image in.
		void draw(
			Canvas &canvas,
			const Rectf &src,
			const Rectf &dest) const;

		/// \brief Draw image on graphic context.
		///
		/// \param gc Graphic context on which to render upon.
		/// \param dest Rectangle to draw image in.
		void draw(
			Canvas &canvas,
			const Rectf &dest) const;

		/// \brief Draw image on graphic context.
		///
		/// \param gc Graphic context on which to render upon.
		/// \param src Source rectangle to draw. Use this is draw only part of the image.
		/// \param dest Quad to draw image in.
		void draw(
			Canvas &canvas,
			const Rectf &src,
			const Quadf &dest) const;

		/// \brief Draw image on graphic context.
		///
		/// \param gc Graphic context on which to render upon.
		/// \param dest Quad to draw image in.
		void draw(
			Canvas &canvas,
			const Quadf &dest) const;

		/// \brief Set scale for x and y directions individually.
		/** <p> 1.0f is normal scale, 2.0f is twice the size, etc. </p>*/
		void set_scale(float x, float y);

		/// \brief Sets transparency.
		/** <p> Alpha 0.0f is full transparency, and 1.0f is full visibility (solid). </p>*/
		void set_alpha(float alpha);

		/// \brief Sets the color.
		/** <p> Alpha 0.0f is full transparency, and 1.0f is full visibility (solid). </p>*/
		void set_color(const Colorf &color);

		/// \brief Set color
		///
		/// \param c = Color
		void set_color(const Color& c) { Colorf color; color.r = c.get_red() / 255.0f; color.g = c.get_green() / 255.0f; color.b = c.get_blue() / 255.0f; color.a = c.get_alpha() / 255.0f; set_color(color); }

		/// \brief Sets translation hotspot.
		void set_alignment(Origin origin, float x = 0, float y = 0);

		void set_wrap_mode(
			TextureWrapMode wrap_s,
			TextureWrapMode wrap_t);

		/// \brief Set to true if a linear filter should be used for scaling up and down, false if a nearest-point filter should be used.
		void set_linear_filter(bool linear_filter = true);

		/** Upload image to sub-texture.
		*  \param canvas Canvas to use for the request.
		*  \param x       The horizontal point in the texture to write the new
		*                 sub-texture image onto.
		*  \param y       The vertical point in the texture to write the new
		*                 sub-texture image onto.
		*  \param image   Image to upload.
		*  \param level   Mipmap level-of-detail number.
		*/
		void set_subimage(
			Canvas &canvas,
			int x,
			int y,
			const PixelBuffer &image,
			const Rect &src_rect,
			int level = 0);

	private:
		std::shared_ptr<Image_Impl> impl;
	};

	/// \}
}
