
/*
    RFC - KDirectory.h
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

#ifndef _RFC_KDIRECTORY_H_
#define _RFC_KDIRECTORY_H_

#include <windows.h>
#include <shlobj.h>
#include "../text/KString.h"

/**
Can be use to manipulate dirs.
*/
class RFC_API KDirectory
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
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static KString GetTempDir();

	/**
		returns the the Application Data directory. if isAllUsers is true this function will return dir shared across all users.
		returns empty string on error.
	*/
	static KString GetApplicationDataDir(bool isAllUsers = false);

	virtual ~KDirectory();
};

#endif