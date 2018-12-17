#pragma once

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
**    Mark Page
**    (if your name is missing here, please add it)
*/

#include "Defines.h"

#include "ClanLibEnv\Core\Crypto\aes256_decrypt.h"
#include "ClanLibEnv\Core\Crypto\aes256_encrypt.h"
#include "ClanLibEnv\Core\System\databuffer.h"
using namespace clan;


namespace NAES256
{
	static void convert_ascii(const char *src, std::vector<unsigned char> &dest)
	{
		std::size_t len = strlen(src) / 2;
		dest.resize(len);

		for (std::size_t cnt = 0; cnt < len; cnt++)
		{
			unsigned int let1 = toupper(*(src++));
			unsigned int let2 = toupper(*(src++));
			let1 >= 'A' ? let1 = let1 - 'A' + 10 : let1 = let1 - '0';
			let2 >= 'A' ? let2 = let2 - 'A' + 10 : let2 = let2 - '0';
			dest[cnt] = let1 << 4 | let2;
		}

	}

	static DataBuffer Encrypt(const uint8_t * pData, std::int32_t iSize, const uint8_t * pKey)
	{
		std::vector<unsigned char> iv;
		convert_ascii(NETWORK_AES_IV, iv);

		AES256_Encrypt aes256_encrypt;
		aes256_encrypt.set_padding(true);

		aes256_encrypt.set_iv(&iv[0]);
		aes256_encrypt.set_key(pKey);
		aes256_encrypt.add(pData, iSize);
		aes256_encrypt.calculate();

		DataBuffer buffer = aes256_encrypt.get_data();
		return buffer;
	}
	static DataBuffer Decrypt(const uint8_t * pData, std::int32_t iSize, const uint8_t * pKey)
	{
		std::vector<unsigned char> iv;
		convert_ascii(NETWORK_AES_IV, iv);

		AES256_Decrypt aes256_decrypt;
		aes256_decrypt.set_padding(true);

		aes256_decrypt.set_iv(&iv[0]);
		aes256_decrypt.set_key(pKey);
		aes256_decrypt.add(pData, iSize);

		bool result = aes256_decrypt.calculate();

		DataBuffer buffer2 = aes256_decrypt.get_data();
		return buffer2;
	}
}

