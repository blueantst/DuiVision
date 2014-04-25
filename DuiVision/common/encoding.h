//Encoding convert tool
#pragma once

class CEncodingUtil
{
public:
	CEncodingUtil()
	{
	}
	
	static CStringA UnicodeToAnsi(CStringW strValueW)
	{
		int nAnsiLen = ::WideCharToMultiByte(CP_ACP, NULL, strValueW, wcslen(strValueW.GetBuffer(0)), NULL, 0, NULL, NULL);
		char* pBuffer = new char[nAnsiLen+1];
		memset(pBuffer, 0, nAnsiLen+1);
		::WideCharToMultiByte(CP_ACP, NULL, strValueW.GetBuffer(0), wcslen(strValueW.GetBuffer(0)), (LPSTR)pBuffer, nAnsiLen, NULL, NULL);
		strValueW.ReleaseBuffer();
		CStringA strValueA = pBuffer;
		delete pBuffer;
		return strValueA;
	}

	static CStringW AnsiToUnicode(CStringA strValueA)
	{
		int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, strValueA.GetBuffer(0), strlen(strValueA.GetBuffer(0)), NULL, 0);
		wchar_t* wszString = new wchar_t[wcsLen + 1];
		wmemset(wszString, 0, wcsLen + 1);
		::MultiByteToWideChar(CP_ACP, NULL, strValueA.GetBuffer(0), strlen(strValueA.GetBuffer(0)), wszString, wcsLen);
		strValueA.ReleaseBuffer();
		CStringW strValueW = wszString;
		delete wszString;
		return strValueW;
	}
};
