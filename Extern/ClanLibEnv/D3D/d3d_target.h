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

#include "../Display/display_target.h"
#include <memory>
#include <d3d11.h>

namespace clan
{
	/// \addtogroup clanD3D_Display clanD3D Display
	/// \{

	class Texture;
	class PixelBuffer;
	class VertexArrayBuffer;
	class ElementArrayBuffer;
	class RenderBuffer;
	class UniformBuffer;
	class GraphicContext;

	/// \brief Display target for clanDisplay.
	class D3DTarget
	{
	public:
		/// \brief Returns true if this display target is the current target
		///
		/// This may change after a display window has been created
		static bool is_current();

		/// \brief Returns the Direct3D handle used by the texture object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Texture2D *get_texture2d_handle(const GraphicContext &gc, const Texture &texture);

		/// \brief Returns the Direct3D handle used by the pixel buffer object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Texture2D *get_texture2d_handle(const GraphicContext &gc, const PixelBuffer &pixel_buffer);

		/// \brief Returns the Direct3D handle used by the render buffer object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Texture2D *get_texture2d_handle(const GraphicContext &gc, const RenderBuffer &render_buffer);

		/// \brief Returns the Direct3D handle used by the buffer object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Buffer *get_buffer_handle(const GraphicContext &gc, const VertexArrayBuffer &buffer);

		/// \brief Returns the Direct3D handle used by the buffer object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Buffer *get_buffer_handle(const GraphicContext &gc, const ElementArrayBuffer &buffer);

		/// \brief Returns the Direct3D handle used by the buffer object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Buffer *get_buffer_handle(const GraphicContext &gc, const UniformBuffer &buffer);

		/// \brief Returns the Direct3D shader resource view handle used by the texture object
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11ShaderResourceView *get_srv_handle(const GraphicContext &gc, const Texture &texture);

		/// \brief Returns the Direct3D device handle used by the graphic context
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11Device *get_device_handle(const GraphicContext &gc);

		/// \brief Returns the Direct3D device context handle used by the graphic context
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11DeviceContext *get_device_context_handle(const GraphicContext &gc);

		/// \brief Returns the Direct3D swap chain handle used by the graphic context
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static IDXGISwapChain *get_swap_chain_handle(const GraphicContext &gc);

		/// \brief Returns the Direct3D render target view for the swap chain back buffer
		///
		/// The reference count on the COM object is not increased (AddRef is not called).
		static ID3D11RenderTargetView *get_back_buffer_rtv_handle(const GraphicContext &gc);

		/// \brief Throws an exception with the specified text if the passed result is an error condition
		static void throw_if_failed(const char *text, HRESULT result);

		/// \brief Set this display target to be the current target
		static void set_current();
	};

	/// \}
}
