
/*
	Copyright (C) 2013-2022 CrownSoft
  
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
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::CreateDir(const KString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool KDirectory::RemoveDir(const KString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

KString KDirectory::GetModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetModuleFilePath(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t* path = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetParentDir(const KString& filePath)
{
	wchar_t *path = ::_wcsdup(filePath);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	path[0] = 0;
	::GetTempPathW(MAX_PATH + 1, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetAllUserDataDir()
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	path[0] = 0;
	::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetLoggedInUserFolderPath(int csidl)
{
	DWORD dwProcessId;
	::GetWindowThreadProcessId(::GetShellWindow(), &dwProcessId);

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

	HANDLE tokenHandle = NULL;
	::OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_IMPERSONATE, &tokenHandle);
	::CloseHandle(hProcess);

	wchar_t* path = (wchar_t*)::malloc(MAX_PATH * sizeof(wchar_t));
	path[0] = 0;
	::SHGetFolderPathW(NULL, csidl, tokenHandle, 0, path);
	::CloseHandle(tokenHandle);

	KString configDir(path, KString::FREE_TEXT_WHEN_DONE);
	return configDir;
}

KString KDirectory::GetRoamingFolder()
{
	return KDirectory::GetLoggedInUserFolderPath(CSIDL_APPDATA);
}

KString KDirectory::GetNonRoamingFolder()
{
	return KDirectory::GetLoggedInUserFolderPath(CSIDL_LOCAL_APPDATA);
}

KPointerList<KString*>* KDirectory::ScanFolderForExtension(const KString& folderPath, const KString& extension)
{
	KPointerList<KString*>* result = new KPointerList<KString*>(32, false);
	WIN32_FIND_DATAW findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	KString searchPath = folderPath + L"\\*." + extension;

	hFind = ::FindFirstFileW(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		return result;

	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			result->AddPointer(new KString(findData.cFileName));
		}
	} while (::FindNextFileW(hFind, &findData) != 0);

	::FindClose(hFind);

	return result;
}