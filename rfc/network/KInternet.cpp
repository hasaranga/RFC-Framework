
/*
	RFC - KInternet.cpp
	Copyright (C) 2013-2019 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	  
*/

// mingw does not ship with winhttp. So, this class is not available for mingw compiler.
#ifndef __MINGW32__

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include "KInternet.h"
#include <string.h>
#include <stdio.h>

KInternet::KInternet(){}

KInternet::~KInternet(){}

void KInternet::ApplyProxySettings(const wchar_t* url, HINTERNET hInternet)
{
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig;
	WINHTTP_PROXY_INFO proxyInfoTemp, proxyInfo;
	WINHTTP_AUTOPROXY_OPTIONS OptPAC;

	::ZeroMemory(&proxyConfig, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	::ZeroMemory(&proxyInfo, sizeof(WINHTTP_PROXY_INFO));

	if (::WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
	{
		if (proxyConfig.lpszProxy) {
			proxyInfo.lpszProxy = proxyConfig.lpszProxy;
			proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxyInfo.lpszProxyBypass = NULL;
		}

		if (proxyConfig.lpszAutoConfigUrl) {
			// Script proxy pac
			OptPAC.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
			OptPAC.lpszAutoConfigUrl = proxyConfig.lpszAutoConfigUrl;
			OptPAC.dwAutoDetectFlags = 0;
			OptPAC.fAutoLogonIfChallenged = TRUE;
			OptPAC.lpvReserved = 0;
			OptPAC.dwReserved = 0;

			if (::WinHttpGetProxyForUrl(hInternet, url, &OptPAC, &proxyInfoTemp))
				::memcpy(&proxyInfo, &proxyInfoTemp, sizeof(WINHTTP_PROXY_INFO));
		}

		if (proxyConfig.fAutoDetect) {
			// Autodetect proxy
			OptPAC.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
			OptPAC.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
			OptPAC.fAutoLogonIfChallenged = TRUE;
			OptPAC.lpszAutoConfigUrl = NULL;
			OptPAC.lpvReserved = 0;
			OptPAC.dwReserved = 0;

			if (::WinHttpGetProxyForUrl(hInternet, url, &OptPAC, &proxyInfoTemp))
				::memcpy(&proxyInfo, &proxyInfoTemp, sizeof(WINHTTP_PROXY_INFO));
		}

		if (proxyInfo.lpszProxy)
			::WinHttpSetOption(hInternet, WINHTTP_OPTION_PROXY, &proxyInfo, sizeof(WINHTTP_PROXY_INFO));
	}
}

KString KInternet::UrlEncodeString(const KString &text)
{
	if (text.GetLength() == 0)
		return KString();

	KString new_str;
	char c;
	int ic;
	const char* chars = text;
	char bufHex[10];
	int len = text.GetLength();

	for (int i = 0; i < len; i++)
	{
		c = chars[i];
		ic = c;

		if (c == ' ')
		{
			new_str = new_str.AppendStaticText(L"+", 1, true);
		}
		else if (::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{		
			char tmp[] = { c, 0 };
			new_str = new_str.Append(KString(tmp));
		}
		else
		{
			::sprintf(bufHex, "%X", c);

			if (ic < 16)
				new_str = new_str.AppendStaticText(L"%0", 2, true);
			else
				new_str = new_str.AppendStaticText(L"%", 1, true);

			new_str = new_str.Append(KString(bufHex));
		}
	}
	return new_str;
}

KString KInternet::UrlDecodeString(const KString &text)
{
	if (text.GetLength() == 0)
		return KString();

	KString ret;
	const char* str = text;

	char ch;
	int i, ii, len = text.GetLength();

	for (i = 0; i < len; i++)
	{
		if (str[i] != '%')
		{
			if (str[i] == '+')
			{
				ret = ret.AppendStaticText(L" ", 1, true);
			}
			else
			{
				char tmp[] = { str[i], 0 };
				ret = ret.Append(KString(tmp));
			}
		}
		else
		{
			KString sub(text.SubString(i + 1, i + 2));
			::sscanf(sub, "%x", &ii);
			ch = static_cast<char>(ii);

			char tmp[] = { ch, 0 };
			ret = ret.Append(KString(tmp));

			i = i + 2;
		}
	}
	return ret;
}

KString KInternet::PostText(const wchar_t* url,
	const wchar_t* objectName,
	const bool isHttps,
	const char* postData,
	const int postDataLength,
	const bool ignoreCertificateErros,
	const wchar_t* userAgent)
{
	HINTERNET hInternet = 0, hConnect = 0, hRequest = 0;
	BOOL resultOK = FALSE;
	KString receivedText;

	//hInternet = ::WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	hInternet = ::WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_NO_PROXY, 0, WINHTTP_NO_PROXY_BYPASS, 0);

	if (hInternet)
		KInternet::ApplyProxySettings(url, hInternet);

	if (hInternet)
		hConnect = ::WinHttpConnect(hInternet, url, INTERNET_DEFAULT_PORT, 0);

	if (hConnect)
		hRequest = ::WinHttpOpenRequest(hConnect, L"POST", objectName, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, isHttps ? (WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE) : WINHTTP_FLAG_REFRESH);

	if (hRequest)
	{
		if (ignoreCertificateErros)
		{
			DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
			::WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(DWORD));
		}

		resultOK = ::WinHttpSendRequest(hRequest, L"Content-Type: application/x-www-form-urlencoded\r\n", -1, (LPVOID)postData, postDataLength, postDataLength, 0); // Send a request
	}

	if (resultOK)
		resultOK = ::WinHttpReceiveResponse(hRequest, NULL);

	if (resultOK)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;

		do
		{
			dwSize = 0;
			if (::WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				char* outBuffer = new char[dwSize + 1];
				::ZeroMemory(outBuffer, dwSize + 1);

				if (::WinHttpReadData(hRequest, (LPVOID)outBuffer, dwSize, &dwDownloaded))
				{
					receivedText = receivedText + KString(outBuffer);
				}

				delete[] outBuffer;
			}

		} while (dwSize > 0);
	}

	if (hRequest)
		::WinHttpCloseHandle(hRequest);

	if (hConnect)
		::WinHttpCloseHandle(hConnect);

	if (hInternet)
		::WinHttpCloseHandle(hInternet);

	return receivedText;
}

#endif