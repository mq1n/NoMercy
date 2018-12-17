#pragma once

class CMemoryPattern
{
	public:
		CMemoryPattern(LPCVOID c_lpPattern, const char* c_szMask, DWORD dwPatternOffset, DWORD dwAddroffset);
		~CMemoryPattern();

		std::size_t GetLength() const;
		DWORD		GetPatternOffset() const;
		DWORD		GetAddrOffset() const;

		char		GetMask(std::size_t uiIndex) const;
		const char* GetMask() const;

		BYTE		GetByte(std::size_t uiIndex) const;
		LPCVOID		GetBytes() const;

	private:
		const char*		m_cszMask;
		LPCVOID			m_lpPattern;

		DWORD			m_dwPatternOffset;
		DWORD			m_dwAddrOffset;

		std::vector < std::pair <char, BYTE> > m_vPattern;
};

