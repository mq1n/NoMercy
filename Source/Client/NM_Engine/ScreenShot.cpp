#include "NM_Index.h"
#include "NM_Main.h"
#include "NM_App.h"
#include "DynamicWinapi.h"
#include "WinVerHelper.h"
#include "Defines.h"
#include "TempFile.h"
#include "FileHelper.h"
#include <GdiPlus.h>

using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	ImageCodecInfo * pImageCodecInfo = NULL;

	UINT uiNum, uiSize = 0;
	GetImageEncodersSize(&uiNum, &uiSize);
	if (uiSize == 0)
		return -1; // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(uiSize));
	if (pImageCodecInfo == NULL)
		return -2; // Failure

	GetImageEncoders(uiNum, uiSize, pImageCodecInfo);

	for (UINT j = 0; j < uiNum; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; // Success
		}
	}

	free(pImageCodecInfo);
	return -3; // Failure
}

bool BitmapToJpg(const std::wstring & wszFileName, HBITMAP hbmpImage, int iWidth, int iHeight, DWORD dwQuality)
{
	// TODO: Convert from memory
	auto pBmpData = Gdiplus::Bitmap::FromHBITMAP(hbmpImage, NULL);
	if (!pBmpData)
		return false;

	std::string szFormat = xorstr("image/jpeg").crypt_get();
	std::wstring wszFormat(szFormat.begin(), szFormat.end());

	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &dwQuality;

	CLSID jpgClsid;
	int result = GetEncoderClsid(wszFormat.c_str(), &jpgClsid);
	if (result >= 0)
	{
		pBmpData->Save(wszFileName.c_str(), &jpgClsid, &encoderParameters);
		delete pBmpData;
		return true;
	}

	DEBUG_LOG(LL_ERR, "BitmapToJpg: Encoding failed!");
	delete pBmpData;
	return false;
}

bool CreateScreenshotEx(std::string * pszData)
{
	static std::recursive_mutex mutex;
	std::unique_lock<std::recursive_mutex> mutex_lock(mutex);

	/// Temp File
	auto pTempFile = std::make_unique<CTempFile>(xorstr("nm1").crypt_get());
	if (!pTempFile || !pTempFile.get() || pTempFile->IsNull())
	{
		DEBUG_LOG(LL_ERR, "Temp file create fail! Error: %u", g_winapiApiTable->GetLastError());
		return false;
	}

	DEBUG_LOG(LL_SYS, "Temp file created! File: %s", pTempFile->GetFileName());

	/// GDI+ Init
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	auto sGDI = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (sGDI != Status::Ok) {
		DEBUG_LOG(LL_ERR, "GdiplusStartup fail! Error: %u", (int)sGDI);
		pTempFile->Destroy();
		return false;
	}

	/// Screen configs
	RECT rcDesktop;
	HWND hwDesktop = g_winapiApiTable->GetDesktopWindow();
	if (g_winapiApiTable->GetWindowRect(hwDesktop, &rcDesktop) == FALSE) {
		DEBUG_LOG(LL_ERR, "GetWindowRect fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	int iWidth = rcDesktop.right;
	int iHeight = rcDesktop.bottom;

	/// Create screenshot
	auto hDCScreen = g_winapiApiTable->GetDC(NULL);
	if (hDCScreen == NULL) {
		DEBUG_LOG(LL_ERR, "hDCScreen fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	auto hDC = g_winapiApiTable->CreateCompatibleDC(hDCScreen);
	if (hDC == NULL) {
		DEBUG_LOG(LL_ERR, "hDC fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	auto hBitmap = g_winapiApiTable->CreateCompatibleBitmap(hDCScreen, iWidth, iHeight);
	if (hBitmap == NULL) {
		DEBUG_LOG(LL_ERR, "hBitmap fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	auto hGdiObj = g_winapiApiTable->SelectObject(hDC, hBitmap);
	if (g_winapiApiTable->BitBlt(hDC, 0, 0, iWidth, iHeight, hDCScreen, 0, 0, SRCCOPY) == FALSE) {
		DEBUG_LOG(LL_ERR, "BitBlt fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	std::string szTmpFileName = pTempFile->GetFileName();
	std::wstring wszName(szTmpFileName.begin(), szTmpFileName.end());
	if (!BitmapToJpg(wszName, hBitmap, iWidth, iHeight, 85)) {
		DEBUG_LOG(LL_ERR, "BitmapToJpg fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	/// Copy screenshot to Memory
	std::string szOutput = "";
	auto bF2mRet = CFileFunctions::File2Mem(szTmpFileName, &szOutput);
	if (!bF2mRet) {
		DEBUG_LOG(LL_ERR, "File2Mem fail! Error: %u", g_winapiApiTable->GetLastError());
		pTempFile->Destroy();
		GdiplusShutdown(gdiplusToken);
		return false;
	}

	if (pszData)
		*pszData = szOutput;
//	DEBUG_LOG(LL_SYS, "SaveScreenShot: Temp file: %s succesfully sended to server!", szTmpFileName.c_str());

	/// Finalize
	pTempFile->Destroy();
	DeleteFileA(szTmpFileName.c_str());

	/// Deinit GDI+
	GdiplusShutdown(gdiplusToken);
	return true;
}

bool CreateScreenshot(std::string * pszData)
{
	auto bRet = false;
	__try 
	{
		bRet = CreateScreenshotEx(pszData);
	}
	__except (1) { }
	return bRet;
}