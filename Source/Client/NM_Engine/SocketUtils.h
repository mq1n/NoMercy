#pragma once
#include <string>
#include <algorithm> 
#include <cctype>

inline bool IsAlphaNumeric(const std::string& str)
{
	return std::all_of(str.begin(), str.end(), ::isalnum);
}

// trim from start (in place)
static inline void ltrim(std::string &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) 
{
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s)
{
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) 
{
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) 
{
	trim(s);
	return s;
}

static uint32_t ByteArrayToInteger(uint8_t* pData)
{
    uint32_t out = (pData[3] << 24) | (pData[2] << 16) | (pData[1] << 8) | (pData[0] << 0);
	return out;
}

static uint8_t* IntegerToByteArray(uint32_t nData)
{
	uint8_t out[4] = { (nData >> 0) & 0xFF, (nData >> 8) & 0xFF, (nData >> 16) & 0xFF, (nData >> 24) & 0xFF };
	return out;
}

