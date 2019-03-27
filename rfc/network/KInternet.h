
/*
	RFC - KInternet.h
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

#ifndef _RFC_KINTERNET_H_
#define _RFC_KINTERNET_H_

#include <windows.h>
#include "../text/KString.h"
#include "../containers/KLeakDetector.h"
#include <winhttp.h>

class KInternet
{
public:

	KInternet();

	/**
		this method automatically applies the browser proxy settings if available.
		no need to use win8.1 only WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY flag when calling WinHttpOpen.
	*/
	static void ApplyProxySettings(const wchar_t* url, HINTERNET hInternet);

	/**
		convert string into url format.
	*/
	static KString UrlEncodeString(const KString &text);

	/**
		converts url formatted string into regular string.
	*/
	static KString UrlDecodeString(const KString &text);

	/**
		this method posts data to given url. post data must be in url enocoded format.
		url is domain name without "http://" prefix.
		objectName is prefixed with "/".
		postData should be like this "data1=Hello&data2=World".
		ignoreCertificateErros parameter will be ignored if isHttps is false.
		this method automatically applies the browser proxy settings if available.
	*/
	static KString PostText(const wchar_t* url,
		const wchar_t* objectName,
		const bool isHttps,
		const char* postData,
		const int postDataLength,
		const bool ignoreCertificateErros = true,
		const wchar_t* userAgent = L"RFC Application/1.0");

	virtual ~KInternet();

private:
	RFC_LEAK_DETECTOR(KInternet)
};

#endif