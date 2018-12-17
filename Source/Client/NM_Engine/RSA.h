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

#include "ClanLibEnv\Core\Crypto\rsa.h"
#include "ClanLibEnv\Core\Crypto\secret.h"
#include "ClanLibEnv\Core\Crypto\random.h"
#include "ClanLibEnv\Core\System\databuffer.h"
using namespace clan;

#define NET_CRYPT_KEY_LENGTH 32
#define RSA_KEY_ROUND 2

class Crypt_Client
{
	public:
		Crypt_Client() = default;

		void create_keypair()
		{
			RSA::create_keypair(m_Random, m_private_exponent, m_public_exponent, m_modulus);
		}

		void set_crypt_key(const DataBuffer &public_key)
		{
			m_WrappedCryptKey = public_key;
			m_CryptKey = RSA::decrypt(m_private_exponent, m_modulus, public_key);
		}


		DataBuffer get_modulus()
		{
			return m_modulus;
		}

		DataBuffer get_public_exponent()
		{
			return m_public_exponent;
		}

		Secret get_crypt_key()
		{
			return m_CryptKey;
		}


	private:
		Random m_Random;

		DataBuffer m_WrappedCryptKey;
		Secret m_CryptKey;

		Secret m_private_exponent;
		DataBuffer m_public_exponent;
		DataBuffer m_modulus;
};

class Crypt_Server
{
	public:
		Crypt_Server() = default;

		void create_keypair()
		{
			RSA::create_keypair(m_Random, m_private_exponent, m_public_exponent, m_modulus);
		}

		void create_crypt_key()
		{
			m_CryptKey = Secret(NET_CRYPT_KEY_LENGTH);
			m_Random.get_random_bytes(m_CryptKey.get_data(), m_CryptKey.get_size());
			m_WrappedCryptKey = RSA::encrypt(2, m_Random, m_public_exponent, m_modulus, m_CryptKey);
		}


		DataBuffer export_wrap_crypt_key(const DataBuffer &client_public_exponent, const DataBuffer &client_public_modulus)
		{
			return RSA::encrypt(RSA_KEY_ROUND, m_Random, client_public_exponent, client_public_modulus, m_CryptKey);
		}

		Secret get_crypt_key()
		{
			return m_CryptKey;
		}


	private:
		Random m_Random;
		Secret m_CryptKey;
		DataBuffer m_WrappedCryptKey;

		Secret m_private_exponent;
		DataBuffer m_public_exponent;
		DataBuffer m_modulus;
};

