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
**    Harry Storbacka
*/


#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "StackTrace.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"

#include <DbgHelp.h>
#include <lazy_importer.hpp>

inline std::string CreateFormatString(const char* c_szFormat, ...)
{
	char pTmpString[8096] = { 0 };

	va_list vaArgList;
	va_start(vaArgList, c_szFormat);
	vsprintf(pTmpString, c_szFormat, vaArgList);
	va_end(vaArgList);

	return pTmpString;
}


CStackTrace::CStackTrace()
{
	num_frames = capture_stack_trace(1, MAX_FRAME_COUNT, frames);
	for (int i = num_frames; i < MAX_FRAME_COUNT; i++)
		frames[i] = nullptr;
}

int CStackTrace::capture_stack_trace(int frames_to_skip, int max_frames, void **out_frames)
{
	if (max_frames > 32)
		max_frames = 32;

	auto wCapturedFrames = LI_FIND(RtlCaptureStackBackTrace)(frames_to_skip + 1, max_frames, out_frames, nullptr);
	return wCapturedFrames;
}

std::vector <std::string> CStackTrace::get_stack_frames_text(void **frames, int num_frames)
{
	static std::recursive_mutex mutex;
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);

	auto vBacktraceText = std::vector<std::string>();

	auto result = LI_FIND(SymInitialize)(NtCurrentProcess, NULL, TRUE);
	if (!result)
		return vBacktraceText;

	for (auto i = 0; i < num_frames; i++)
	{
		unsigned char buffer[sizeof(IMAGEHLP_SYMBOL64) + 128];
		auto symbol64 = reinterpret_cast<IMAGEHLP_SYMBOL64*>(buffer);
		memset(symbol64, 0, sizeof(IMAGEHLP_SYMBOL64) + 128);
		symbol64->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		symbol64->MaxNameLength = 128;

		DWORD64 displacement = 0;
		auto result = LI_FIND(SymGetSymFromAddr64)(NtCurrentProcess, (DWORD64)frames[i], &displacement, symbol64);
		if (result)
		{
			IMAGEHLP_LINE64 line64;
			memset(&line64, 0, sizeof(IMAGEHLP_LINE64));
			line64.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			auto dwDisplacement = 0UL;
			result = LI_FIND(SymGetLineFromAddr64)(NtCurrentProcess, (DWORD64)frames[i], &dwDisplacement, &line64);
			if (result)
			{
				vBacktraceText.push_back
				(
					CreateFormatString(xorstr("%s (%s, line %u)").crypt_get(), symbol64->Name, line64.FileName, (int)line64.LineNumber)
				);
			}
			else
			{
				vBacktraceText.push_back(symbol64->Name);
			}
		}
	}

	LI_FIND(SymCleanup)(NtCurrentProcess);
	return vBacktraceText;
}

std::vector<std::string> CStackTrace::get_stack_trace()
{
	return get_stack_frames_text(frames, num_frames);
}

void CStackTrace::print_stack_trace()
{
	auto stack_trace = get_stack_trace();
	for (auto & elem : stack_trace)
	{
		printf("\r\n    at %s\n", elem.c_str());
//		DEBUG_LOG(LL_CRI, "\r\n    at %s", elem.c_str());
	}
}
