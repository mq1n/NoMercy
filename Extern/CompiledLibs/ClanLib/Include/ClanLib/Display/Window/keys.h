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
 **    Chu Chin Kuan
 */

#pragma once

#include "input_code.h"

namespace clan
{
	//! Enumeration of keyboard keys recognized on ClanLib.
	enum class Key
	{
		none = 0x00,
		backspace = 0x08,
		tab = 0x09, // ASCII HT, Horizontal tab
		linefeed = 0x0a, // ASCII LF
		clear = 0x0b, // ASCII VT
		scroll = 0x0c, // ASCII FF, Clear screen
		key_return = 0x0d, // ASCII CR, Enter key
		select = 0x0e,
		execute = 0x1a,
		escape = 0x1b,
		pause = 0x1c,
		print = 0x1d,

		space = 0x20,

		prior = 0x21,
		next = 0x22,
		home = 0x23,
		end = 0x24,
		left = 0x25,
		up = 0x26,
		right = 0x27,
		down = 0x28,

		kanji = 0x29,
		convert = 0x2a,
		nonconvert = 0x2b,
		separator = 0x2c, // ASCII ','

		help = 0x3f, // ASCII '?'

		key_0 = '0', // 0x30
		key_1 = '1',
		key_2 = '2',
		key_3 = '3',
		key_4 = '4',
		key_5 = '5',
		key_6 = '6',
		key_7 = '7',
		key_8 = '8',
		key_9 = '9',

		a = 'A', // 0x41
		b = 'B',
		c = 'C',
		d = 'D',
		e = 'E',
		f = 'F',
		g = 'G',
		h = 'H',
		i = 'I',
		j = 'J',
		k = 'K',
		l = 'L',
		m = 'M',
		n = 'N',
		o = 'O',
		p = 'P',
		q = 'Q',
		r = 'R',
		s = 'S',
		t = 'T',
		u = 'U',
		v = 'V',
		w = 'W',
		x = 'X',
		y = 'Y',
		z = 'Z',

		f1 = 0x61, // ASCII 'a'
		f2 = 0x62,
		f3 = 0x63,
		f4 = 0x64,
		f5 = 0x65,
		f6 = 0x66,
		f7 = 0x67,
		f8 = 0x68,
		f9 = 0x69,
		f10 = 0x6a,
		f11 = 0x6b,
		f12 = 0x6c,
		f13 = 0x6d,
		f14 = 0x6e,
		f15 = 0x6f,
		f16 = 0x70,
		f17 = 0x71,
		f18 = 0x72,
		f19 = 0x73,
		f20 = 0x74,
		f21 = 0x75,
		f22 = 0x76,
		f23 = 0x77,
		f24 = 0x78,

		insert = 0x7e,
		key_delete = 0x7f, // ASCII DEL

		// Numpad numbers
		numpad_0 = 0x80,
		numpad_1 = 0x81,
		numpad_2 = 0x82,
		numpad_3 = 0x83,
		numpad_4 = 0x84,
		numpad_5 = 0x85,
		numpad_6 = 0x86,
		numpad_7 = 0x87,
		numpad_8 = 0x88,
		numpad_9 = 0x89,

		// Numpad buttons
		multiply = 0x8a,
		add = 0x8b,
		seperator = 0x8c,
		subtract = 0x8d,
		decimal = 0x8e,
		divide = 0x8f,

		// Modifiers
		lshift = 0xe0,
		rshift = 0xe1,
		lcontrol = 0xe2,
		rcontrol = 0xe3,
		lalt = 0xe4, loption = lalt,
		ralt = 0xe5, roption = ralt,
		lmenu = 0xe6, apps = lmenu,
		rmenu = 0xe7,
		lsuper = 0xe8, lcmd = lsuper,
		rsuper = 0xe9, rcmd = rsuper,

		scrolllock = 0xea,
		capslock = 0xeb,
		numlock = 0xec
	};

	Key decode_ic(InputCode);
}
