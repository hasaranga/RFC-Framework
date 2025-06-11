
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

#include "KDirectory.h"


KDirectory::KDirectory(){}

KDirectory::~KDirectory(){}

bool KDirectory::isDirExists(const KString& dirName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::createDir(const KString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool KDirectory::removeDir(const KString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

void KDirectory::getModuleDir(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars)
{
	outBuffer[0] = 0;
	::GetModuleFileNameW(hModule, outBuffer, bufferSizeInWChars);

	wchar_t *p;
	for (p = outBuffer; *p; p++) {}	// find end
	for (; p > outBuffer && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it
}

void KDirectory::getModuleFilePath(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars)
{
	outBuffer[0] = 0;
	::GetModuleFileNameW(hModule, outBuffer, bufferSizeInWChars);
}

void KDirectory::getParentDir(const wchar_t* filePath, wchar_t* outBuffer, int bufferSizeInWChars)
{
	::wcscpy_s(outBuffer, bufferSizeInWChars, filePath);

	wchar_t *p;
	for (p = outBuffer; *p; p++) {}	// find end
	for (; p > outBuffer && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it
}

void KDirectory::getTempDir(wchar_t* outBuffer, int bufferSizeInWChars)
{
	outBuffer[0] = 0;
	::GetTempPathW(bufferSizeInWChars, outBuffer);
}

void KDirectory::getAllUserDataDir(wchar_t* outBuffer)
{
	outBuffer[0] = 0;
	::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, outBuffer);
}

void KDirectory::getLoggedInUserFolderPath(int csidl, wchar_t* outBuffer)
{
	DWORD dwProcessId;
	::GetWindowThreadProcessId(::GetShellWindow(), &dwProcessId);

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

	HANDLE tokenHandle = NULL;
	::OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_IMPERSONATE, &tokenHandle);
	::CloseHandle(hProcess);

	outBuffer[0] = 0;
	::SHGetFolderPathW(NULL, csidl, tokenHandle, 0, outBuffer);
	::CloseHandle(tokenHandle);
}

void KDirectory::getRoamingFolder(wchar_t* outBuffer)
{
	KDirectory::getLoggedInUserFolderPath(CSIDL_APPDATA, outBuffer);
}

void KDirectory::getNonRoamingFolder(wchar_t* outBuffer)
{
	KDirectory::getLoggedInUserFolderPath(CSIDL_LOCAL_APPDATA, outBuffer);
}

KPointerList<KString*,32, false>* KDirectory::scanFolderForExtension(const KString& folderPath, const KString& extension)
{
	KPointerList<KString*,32,false>* result = new KPointerList<KString*,32,false>();
	WIN32_FIND_DATAW findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	KString searchPath(folderPath + L"\\*." + extension);

	hFind = ::FindFirstFileW(searchPath, &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		return result;

	do
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			result->add(new KString(findData.cFileName,KStringBehaviour::MAKE_A_COPY));
		}
	} while (::FindNextFileW(hFind, &findData) != 0);

	::FindClose(hFind);

	return result;
}