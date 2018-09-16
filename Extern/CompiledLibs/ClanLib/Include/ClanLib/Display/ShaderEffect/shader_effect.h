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
*/

#pragma once

#include <memory>
#include "../Render/graphic_context.h"

namespace clan
{
	/// \addtogroup clanDisplay_Display clanDisplay Display
	/// \{

	class ShaderEffect_Impl;
	class ShaderEffectDescription;

	/// \brief Shader Feffect
	class ShaderEffect
	{
	public:
		/// \brief Constructs a null instance
		ShaderEffect();

		/// \brief Constructs a shader effect.
		///
		/// \param description Shader effect description.
		/// \param gc Graphics context in which to create the shader effect.
		ShaderEffect(GraphicContext &gc, const ShaderEffectDescription &description);

		bool is_null() const;
		explicit operator bool() const { return bool(impl); }

		void dispatch(GraphicContext &gc, int x, int y = 1, int z = 1);
		void draw(GraphicContext &gc);

	private:
		std::shared_ptr<ShaderEffect_Impl> impl;
	};

	/// \}
}
