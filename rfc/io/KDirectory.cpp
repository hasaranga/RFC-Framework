
/*
RFC - KDirectory.cpp
Copyright (C) 2013-2017 CrownSoft

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

#include "KDirectory.h"


KDirectory::KDirectory(){}

KDirectory::~KDirectory(){}

bool KDirectory::IsDirExists(const KString& dirName)
{
	DWORD dwAttrib = ::GetFileAttributesW((const wchar_t*)dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::CreateDir(const KString& dirName)
{
	return ::CreateDirectoryW((const wchar_t*)dirName, NULL) == 0 ? false : true;
}

bool KDirectory::RemoveDir(const KString& dirName)
{
	return ::RemoveDirectoryW((const wchar_t*)dirName) == 0 ? false : true;
}

KString KDirectory::GetModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	::ZeroMemory(path, (MAX_PATH * 2) * sizeof(wchar_t) );
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	KString strPath(path);
	::free(path);

	return strPath;
}

KString KDirectory::GetTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	::ZeroMemory(path, (MAX_PATH + 1) * sizeof(wchar_t) );
	::GetTempPathW(MAX_PATH + 1, path);

	KString strPath(path);
	::free(path);

	return strPath;
}

KString KDirectory::GetApplicationDataDir(bool isAllUsers)
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	::ZeroMemory(path, MAX_PATH * sizeof(wchar_t) );
	::SHGetFolderPathW(NULL, isAllUsers ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA, NULL, 0, path);

	KString strPath(path);
	::free(path);

	return strPath;
}