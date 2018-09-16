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
*/


#pragma once

#include "../System/cl_platform.h"
#include "../Crypto/sha1.h"
#include "../Crypto/md5.h"
#include "../Crypto/sha224.h"
#include "../Crypto/sha256.h"
#include "../Crypto/sha384.h"
#include "../Crypto/sha512.h"
#include "../Crypto/sha512_224.h"
#include "../Crypto/sha512_256.h"

namespace clan
{
	/// \addtogroup clanCore_Crypto clanCore Crypto
	/// \{

	/// \brief A Collection of checksum functions.
	class HashFunctions
	{
	public:
		/// \brief Calculate a CRC32 checksum on the data. 
		static uint32_t crc32(const void *data, int size, uint32_t running_crc = 0);

		/// \brief Calculate a CRC32 checksum on the data. 
		static uint32_t adler32(const void *data, int size, uint32_t running_adler32 = 0);

		/// \brief Generate SHA-1 hash from data.
		static std::string sha1(const void *data, int size, bool uppercase = false);

		/// \brief SHA1
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha1(const std::string &data, bool uppercase = false);

		/// \brief SHA1
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha1(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA1
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha1(const void *data, int size, unsigned char out_hash[20]);

		/// \brief SHA1
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha1(const std::string &data, unsigned char out_hash[20]);

		/// \brief SHA1
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha1(const DataBuffer &data, unsigned char out_hash[20]);

		/// \brief Generate SHA-1 hash from data.
		static std::string md5(const void *data, int size, bool uppercase = false);

		/// \brief MD5
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string md5(const std::string &data, bool uppercase = false);

		/// \brief MD5
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string md5(const DataBuffer &data, bool uppercase = false);

		/// \brief MD5
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void md5(const void *data, int size, unsigned char out_hash[16]);

		/// \brief MD5
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void md5(const std::string &data, unsigned char out_hash[16]);

		/// \brief MD5
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void md5(const DataBuffer &data, unsigned char out_hash[16]);

		/// \brief Generate SHA-224 hash from data.
		static std::string sha224(const void *data, int size, bool uppercase = false);

		/// \brief SHA224
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha224(const std::string &data, bool uppercase = false);

		/// \brief SHA224
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha224(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA224
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha224(const void *data, int size, unsigned char out_hash[28]);

		/// \brief SHA224
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha224(const std::string &data, unsigned char out_hash[28]);

		/// \brief SHA224
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha224(const DataBuffer &data, unsigned char out_hash[28]);


		/// \brief Generate SHA-256 hash from data.
		static std::string sha256(const void *data, int size, bool uppercase = false);

		/// \brief SHA256
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha256(const std::string &data, bool uppercase = false);

		/// \brief SHA256
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha256(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA256
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha256(const void *data, int size, unsigned char out_hash[32]);

		/// \brief SHA256
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha256(const std::string &data, unsigned char out_hash[32]);

		/// \brief SHA256
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha256(const DataBuffer &data, unsigned char out_hash[32]);

		/// \brief Generate SHA-384 hash from data.
		static std::string sha384(const void *data, int size, bool uppercase = false);

		/// \brief SHA384
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha384(const std::string &data, bool uppercase = false);

		/// \brief SHA384
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha384(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA384
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha384(const void *data, int size, unsigned char out_hash[48]);

		/// \brief SHA384
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha384(const std::string &data, unsigned char out_hash[48]);

		/// \brief SHA384
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha384(const DataBuffer &data, unsigned char out_hash[48]);

		/// \brief Generate SHA-512 hash from data.
		static std::string sha512(const void *data, int size, bool uppercase = false);

		/// \brief SHA512
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha512(const std::string &data, bool uppercase = false);

		/// \brief SHA512
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha512(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA512
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha512(const void *data, int size, unsigned char out_hash[64]);

		/// \brief SHA512
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha512(const std::string &data, unsigned char out_hash[64]);

		/// \brief SHA512
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha512(const DataBuffer &data, unsigned char out_hash[64]);

		/// \brief Generate SHA-512_224 hash from data.
		static std::string sha512_224(const void *data, int size, bool uppercase = false);

		/// \brief SHA512_224
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha512_224(const std::string &data, bool uppercase = false);

		/// \brief SHA512_224
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha512_224(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA512_224
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha512_224(const void *data, int size, unsigned char out_hash[28]);

		/// \brief SHA512_224
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha512_224(const std::string &data, unsigned char out_hash[28]);

		/// \brief SHA512_224
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha512_224(const DataBuffer &data, unsigned char out_hash[28]);


		/// \brief Generate SHA-512_256 hash from data.
		static std::string sha512_256(const void *data, int size, bool uppercase = false);

		/// \brief SHA512_256
		///
		/// \param data = String Ref8
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha512_256(const std::string &data, bool uppercase = false);

		/// \brief SHA512_256
		///
		/// \param data = Data Buffer
		/// \param uppercase = bool
		///
		/// \return String8
		static std::string sha512_256(const DataBuffer &data, bool uppercase = false);

		/// \brief SHA512_256
		///
		/// \param data = void
		/// \param size = value
		/// \param out_hash = char
		static void sha512_256(const void *data, int size, unsigned char out_hash[32]);

		/// \brief SHA512_256
		///
		/// \param data = String Ref8
		/// \param out_hash = char
		static void sha512_256(const std::string &data, unsigned char out_hash[32]);

		/// \brief SHA512_256
		///
		/// \param data = Data Buffer
		/// \param out_hash = char
		static void sha512_256(const DataBuffer &data, unsigned char out_hash[32]);
	};

	/// \}
}
