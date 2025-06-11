
/*
	Copyright (C) 2013-2025 CrownSoft
  
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

#include "KRegistry.h"

KRegistry::KRegistry()
{

}

bool KRegistry::createKey(HKEY hKeyRoot, const KString& subKey)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	::RegCloseKey(hkey);
	return true;
}

bool KRegistry::deleteKey(HKEY hKeyRoot, const KString& subKey)
{
	return ::RegDeleteKeyW(hKeyRoot, subKey) == ERROR_SUCCESS ? true : false;
}

bool KRegistry::readString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			if (requiredBytes == 0) // value might be empty
			{
				*result = KString();
			}
			else{
				void* buffer = ::malloc(requiredBytes + sizeof(wchar_t)); // + for strings which doesn't have ending null
				::ZeroMemory(buffer, requiredBytes + sizeof(wchar_t)); // zero the ending null

				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)buffer, &requiredBytes);
				*result = KString((wchar_t*)buffer, KStringBehaviour::FREE_ON_DESTROY);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::writeString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const int bCount = (value.length() + 1) * sizeof(wchar_t); // +1 for ending null
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_SZ, (LPBYTE)(const wchar_t*)value, bCount);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::readDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		const LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, &dwType, (LPBYTE)result, &dwSize);
		::RegCloseKey(hkey);

		if (ret == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool KRegistry::writeDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const DWORD dwSize = sizeof(DWORD);
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_DWORD, (LPBYTE)&value, dwSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::readBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void** buffer, DWORD* buffSize)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			*buffSize = requiredBytes;
			if (requiredBytes == 0) // value might be empty
			{
				*buffer = 0;
			}
			else{
				*buffer = ::malloc(requiredBytes);
				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)*buffer, &requiredBytes);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::writeBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void* buffer, DWORD buffSize)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_BINARY, (LPBYTE)buffer, buffSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

KRegistry::~KRegistry()
{

}