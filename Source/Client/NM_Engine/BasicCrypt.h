#pragma once
#include <Windows.h>

static void DecryptBuffer(uint8_t * lpBuf, std::size_t dwSize, uint8_t pKey)
{
	for (std::size_t i = 0; i < dwSize; i++)
	{
		lpBuf[i] ^= pKey;
		lpBuf[i] += (uint8_t)i;
		lpBuf[i] ^= (uint8_t)i + 8;
	}
}

static void EncryptBuffer(uint8_t * lpBuf, std::size_t dwSize, uint8_t pKey)
{
	for (std::size_t i = 0; i < dwSize; i++)
	{
		lpBuf[i] ^= (uint8_t)i + 8;
		lpBuf[i] -= (uint8_t)i;
		lpBuf[i] ^= pKey;
	}
}
