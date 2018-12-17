#include "NM_Index.h"
#include "NM_Main.h"
#include "PatternTemplate.h"

CMemoryPattern::CMemoryPattern(LPCVOID c_lpPattern, const char* c_szMask, DWORD dwPatternOffset, DWORD dwAddroffset) :
	m_vPattern(strlen(c_szMask)), m_lpPattern(c_lpPattern), m_cszMask(c_szMask), m_dwPatternOffset(dwPatternOffset), m_dwAddrOffset(dwAddroffset)
{
	for (std::size_t i = 0; i < m_vPattern.size(); i++)
	{
		m_vPattern[i].first = c_szMask[i];
		m_vPattern[i].second = reinterpret_cast<const BYTE*>(c_lpPattern)[i];
	}
}

CMemoryPattern::~CMemoryPattern()
{
	m_vPattern.clear();

	m_lpPattern = nullptr;
	m_cszMask	= nullptr;

	m_dwPatternOffset	= 0;
	m_dwAddrOffset		= 0;
}


std::size_t CMemoryPattern::GetLength() const 
{ 
	return m_vPattern.size();
}

DWORD CMemoryPattern::GetPatternOffset() const
{
	return m_dwPatternOffset;
}

DWORD CMemoryPattern::GetAddrOffset() const
{
	return m_dwAddrOffset;
}


char CMemoryPattern::GetMask(std::size_t uiIndex) const
{ 
	return m_vPattern.at(uiIndex).first;
}

const char* CMemoryPattern::GetMask() const
{
	return m_cszMask;
}


BYTE CMemoryPattern::GetByte(std::size_t uiIndex) const
{ 
	return m_vPattern.at(uiIndex).second;
}

LPCVOID CMemoryPattern::GetBytes() const
{
	return m_lpPattern;
}

