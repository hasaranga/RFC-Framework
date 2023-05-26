
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

#pragma once

#include "../core/CoreModule.h"
#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class KDirectory
{
public:
	KDirectory();

	static bool IsDirExists(const KString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool CreateDir(const KString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool RemoveDir(const KString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static KString GetModuleDir(HMODULE hModule);

	/**
		returns the parent directory of given file.
	*/
	static KString GetParentDir(const KString& filePath);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static KString GetTempDir();

	/**
		returns the all user data directory. Requires admin priviledges for writing to this dir.
		returns empty string on error.
	*/
	static KString GetAllUserDataDir();

	// known path for the logged in user of the pc. (not run as user)
	/*
		CSIDL_ADMINTOOLS
		CSIDL_APPDATA
		CSIDL_COMMON_ADMINTOOLS
		CSIDL_COMMON_APPDATA
		CSIDL_COMMON_DOCUMENTS
		CSIDL_COOKIES
		CSIDL_FLAG_CREATE
		CSIDL_FLAG_DONT_VERIFY
		CSIDL_HISTORY
		CSIDL_INTERNET_CACHE
		CSIDL_LOCAL_APPDATA
		CSIDL_MYPICTURES
		CSIDL_PERSONAL
		CSIDL_PROGRAM_FILES
		CSIDL_PROGRAM_FILES_COMMON
		CSIDL_SYSTEM
		CSIDL_WINDOWS
	*/
	static KString GetLoggedInUserFolderPath(int csidl);

	// path for logged in user of pc (not run as user)
	static KString GetRoamingFolder();

	// path for logged in user of pc (not run as user)
	static KString GetNonRoamingFolder();

	virtual ~KDirectory();

private:
	RFC_LEAK_DETECTOR(KDirectory)
};

