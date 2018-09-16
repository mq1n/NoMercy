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
**    Harry Storbacka
**    Mark Page
*/

#pragma once

#include <memory>
#include "../../Core/Math/rect.h"
#include "texture_format.h"
#include "buffer_usage.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class Size;
	class Rect;
	class PixelFormat;
	class Color;
	class Colorf;
	class PixelBuffer;
	class PixelBuffer_Impl;
	class FileSystem;
	class IODevice;
	class GraphicContext;
	class PixelBufferProvider;
	class PixelConverter;

	/// \brief Pixel buffer prefered direction
	enum PixelBufferDirection
	{
		/// \brief Use of the pixel buffer is to send data to the gpu
		data_to_gpu,

		/// \brief Use of the pixel buffer is to retrieve data from the gpu
		data_from_gpu
	};

	/// \brief Pixel data container.
	class PixelBuffer
	{
	public:
		/// \brief Constructs a null instance
		PixelBuffer();

		/// \brief Constructs a PixelBuffer
		///
		/// \param width = value
		/// \param height = value
		/// \param sized_format = Pixel Format
		/// \param data = The data (0 = Allocate automatically to a boundary of 16 bytes)
		/// \param only_reference_data : true = Reference the data. false = Copy the data
		PixelBuffer(int width, int height, TextureFormat texture_format, const void *data = nullptr, bool only_reference_data = false);

		/// \brief Constructs a PixelBuffer
		///
		/// \param fullname = String Ref
		PixelBuffer(const std::string &fullname, bool srgb = false);

		/// \brief Constructs a PixelBuffer
		///
		/// \param filename = String Ref
		/// \param dir = Virtual Directory
		PixelBuffer(const std::string &filename, const FileSystem &file_system, bool srgb = false);

		/// \brief Constructs a PixelBuffer
		///
		/// \param file = IODevice
		/// \param image_type = String
		PixelBuffer(IODevice &file, const std::string &image_type, bool srgb = false);

		/// \brief Constructs a PixelBuffer
		///
		/// \param provider = Font Provider
		PixelBuffer(PixelBufferProvider *provider);

		virtual ~PixelBuffer();

		/// \brief Returns true if this object is invalid.
		bool is_null() const { return !impl; }
		explicit operator bool() const { return bool(impl); }

		/// \brief Throw an exception if this object is invalid.
		void throw_if_null() const;

		/// \brief Create a copy of the pixelbuffer that doesn't share data with the original pixel buffer.
		PixelBuffer copy() const;

		/// \brief Create a copy of the pixelbuffer that doesn't share data with the original pixel buffer.
		PixelBuffer copy(const Rect &rect) const;

		/// Retrieves the actual width of the buffer.
		int get_width() const;

		/// Retrieves the actual height of the buffer.
		int get_height() const;

		/// Retrieves the actual size of the buffer.
		Size get_size() const { return Size{ get_width(), get_height() }; }

		/// Returns the pitch (in bytes per scanline).
		int get_pitch() const;

		/** Retrieves the pixel ratio of this texture.
		 *  \return The display pixel ratio set for this texture.
		 *          A zero value implies that no pixel ratio has been set
		 */
		float get_pixel_ratio() const;

		/// Returns the device independent width of this texture.
		float get_dip_width() const { return get_width() / get_pixel_ratio(); }

		/// Returns the device independent height of this texture.
		float get_dip_height() const { return get_height() / get_pixel_ratio(); }

		/// Returns the device independent size of this texture.
		Sizef get_dip_size() const { return Sizef{ get_dip_width(), get_dip_height() }; }

		/// \brief Returns a pointer to the beginning of the pixel buffer.
		void *get_data();

		const void *get_data() const;

		/// \brief Returns true if this pixel buffer is a GPU based one
		bool is_gpu() const;

		template<typename Type> Type *get_data() { return reinterpret_cast<Type*>(get_data()); }
		template<typename Type> const Type *get_data() const { return reinterpret_cast<Type*>(get_data()); }

		/// \brief Returns a pointer to the beginning of the pixel buffer as 8 bit data.
		unsigned char *get_data_uint8() { return reinterpret_cast<unsigned char*>(get_data()); }
		const unsigned char *get_data_uint8() const { return reinterpret_cast<const unsigned char*>(get_data()); }

		/// \brief Returns a pointer to the beginning of the pixel buffer as 16 bit data.
		unsigned short *get_data_uint16() { return reinterpret_cast<unsigned short*>(get_data()); }
		const unsigned short *get_data_uint16() const { return reinterpret_cast<const unsigned short*>(get_data()); }

		/// \brief Returns a pointer to the beginning of the pixel buffer as 32 bit data.
		unsigned int *get_data_uint32() { return reinterpret_cast<unsigned int*>(get_data()); }
		const unsigned int *get_data_uint32() const { return reinterpret_cast<const unsigned int*>(get_data()); }

		/// \brief Returns a pointer to the beginning of a specific line.
		void *get_line(int line) { unsigned char *d = get_data_uint8(); return d + line * get_pitch(); }
		const void *get_line(int line) const { const unsigned char *d = get_data_uint8(); return d + line * get_pitch(); }

		/// \brief Returns a pointer to the beginning of a specific line as 8 bit data.
		unsigned char *get_line_uint8(int line) { return reinterpret_cast<unsigned char*>(get_line(line)); }
		const unsigned char *get_line_uint8(int line) const { return reinterpret_cast<const unsigned char*>(get_line(line)); }

		/// \brief Returns a pointer to the beginning of a specific line as 16 bit data.
		unsigned short *get_line_uint16(int line) { return reinterpret_cast<unsigned short*>(get_line(line)); }
		const unsigned short *get_line_uint16(int line) const { return reinterpret_cast<const unsigned short*>(get_line(line)); }

		/// \brief Returns a pointer to the beginning of a specific line as 32 bit data.
		unsigned int *get_line_uint32(int line) { return reinterpret_cast<unsigned int*>(get_line(line)); }
		const unsigned int *get_line_uint32(int line) const { return reinterpret_cast<const unsigned int*>(get_line(line)); }

		/// \brief Returns true if format has an alpha channel
		bool has_transparency() const;

		/// \brief Returns the number of bytes per pixel
		///
		/// \return Bytes per pixel. Exception thrown if not available (hint, use is_compressed() )
		unsigned int get_bytes_per_pixel() const;

		/// \brief Returns the number of bytes per compression block
		///
		/// \return Bytes per block. Exception thrown if not available (hint, use is_compressed() )
		unsigned int get_bytes_per_block() const;

		/// \brief Returns the size in bytes of the image data
		///
		/// \return The data size
		unsigned int get_data_size() const;

		/// \brief Returns the size in bytes of the image data
		///
		/// \return The data size
		static unsigned int get_data_size(const Size &size, TextureFormat texture_format);

		/// \brief Returns the number of bytes per pixel
		///
		/// \return Bytes per pixel. Exception thrown if not available (hint, use is_compressed() )
		static unsigned int get_bytes_per_pixel(TextureFormat texture_format);

		/// \brief Returns the number of bytes per compression block
		///
		/// \return Bytes per block. Exception thrown if not available (hint, use is_compressed() )
		static unsigned int get_bytes_per_block(TextureFormat texture_format);

		/// \brief Returns true if compressed
		bool is_compressed() const;

		/// \brief Returns true if compressed
		static bool is_compressed(TextureFormat texture_format);

		/// \brief Returns the pixel format
		TextureFormat get_format() const;

		/// \brief Get Provider
		///
		/// \return provider
		PixelBufferProvider *get_provider() const;

		/// \brief Return color of pixel at the specified coordinates.
		Colorf get_pixel(int x, int y);

		/// \brief Maps buffer into system memory.
		void lock(GraphicContext &gc, BufferAccess access);

		/// \brief Unmaps element buffer.
		void unlock();

		/// \brief Uploads data to buffer.
		void upload_data(GraphicContext &gc, const Rect &dest_rect, const void *data);

		/// \brief Copy source pixel buffer into this buffer, doing a format conversion if needed
		///
		/// \param source Source pixel buffer.
		void set_image(const PixelBuffer &source);

		/// \brief Copy source pixel buffer into this buffer, doing a format conversion if needed
		///
		/// \param source Source pixel buffer.
		void set_image(const PixelBuffer &source, PixelConverter &converter);

		/// \brief Copy source pixel buffer into this buffer, doing a format conversion if needed
		///
		/// \param source Source pixel buffer.
		/// \param dest_rect Destination position for copy.
		/// \param src_rect Source rectangle for copy.
		void set_subimage(const PixelBuffer &source, const Point &dest_pos, const Rect &src_rect);

		/// \brief Copy source pixel buffer into this buffer, doing a format conversion if needed
		///
		/// \param source Source pixel buffer.
		/// \param dest_rect Destination position for copy.
		/// \param src_rect Source rectangle for copy.
		void set_subimage(const PixelBuffer &source, const Point &dest_pos, const Rect &src_rect, PixelConverter &converter);

		/// \brief Downloads the pixel buffer to CPU memory
		///
		/// If the pixel buffer is already in CPU memory the function returns the current pixel buffer.
		PixelBuffer to_cpu(GraphicContext &gc);

		/// \brief Uploads the pixel buffer to GPU memory
		///
		/// If the pixel buffer is already in GPU memory the function returns the current pixel buffer.
		PixelBuffer to_gpu(GraphicContext &gc);

		/// \brief Converts current buffer to a new pixel format and returns the result.
		PixelBuffer to_format(TextureFormat texture_format) const;

		/// \brief Converts current buffer to a new pixel format and returns the result.
		PixelBuffer to_format(TextureFormat texture_format, PixelConverter &converter) const;

		/// \brief Flip the entire image vertically (turn it upside down)
		void flip_vertical();

		/// \brief Multiply the RGB components by the Alpha component
		///
		/// This is useful with certain blending functions
		void premultiply_alpha();

		/// \brief Multiply the RGB components by gamma value
		///
		/// Calling this function with 2.2 gamma converts a sRGB image into linear space.
		/// To convert from linear to sRGB use 1.0/2.2
		void premultiply_gamma(float gamma);

		/// Sets the display pixel ratio for this texture.
		void set_pixel_ratio(float ratio);

	private:
		std::shared_ptr<PixelBuffer_Impl> impl;
		friend class PixelBuffer_Impl;
	};

	/// \}
}
