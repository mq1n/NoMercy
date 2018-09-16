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

namespace clan
{
	/// Unit of a style value
	enum class StyleDimension
	{
		px,   /// device independent pixel (96 dpi)
		em,   /// relative to font-size length property
		pt,   /// point, 1/72 inch
		mm,   /// millimeter
		cm,   /// centimeter
		in,   /// inch, 1in is equal to 2.54cm
		pc,   /// picas, 1pc is equal to 12pt
		ex,   /// x-height, 1ex is equal to 0.5em
		ch,   /// used advance measure of the "0" glyph found in the font used to render it
		rem,  /// computed value of 'font-size' on the root element
		vw,   /// 1/100 viewport (initial containing box) width
		vh,   /// 1/100 viewport (initial containing box) height
		vmin, /// the smaller of vw or vh
		vmax, /// the larger of vw or vh
		deg,  /// degrees (360 in a full circle)
		grad, /// gradians/gons/grades (400 in a full circle)
		rad,  /// radians (2*PI in a full circle)
		turn, /// turns (1 in a full circle)
		s,    /// seconds
		ms,   /// milliseconds
		hz,   /// hertz
		khz,  /// kilohertz
		dpi,  /// dots per inch
		dpcm, /// dots per cm
		dppx  /// dots per px unit
	};
}
