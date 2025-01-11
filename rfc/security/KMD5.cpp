
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

#include "KMD5.h"
#include "../file/FileModule.h"
#include "./md5/md5.h"

KMD5::KMD5()
{

}

KString KMD5::GenerateFromString(const KString& text)
{
	if (text.GetLength() == 0)
		return KString();

	char* ansiTxt = KString::ToAnsiString(text);
	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestString(ansiTxt);

	::free(ansiTxt);
	return KString(strMD5);
}

KString KMD5::GenerateFromFile(const KString& fileName)
{
	if (fileName.GetLength() == 0) // empty path
		return KString();

	if (!KFile::IsFileExists(fileName)) // file does not exists
		return KString();

	KFile file;
	file.OpenFile(fileName, KFile::KREAD, false);

	const DWORD fileSize = file.GetFileSize();
	file.CloseFile();

	if (fileSize==0) // empty file
		return KString();

	char* ansiFileName = KString::ToAnsiString(fileName);
	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestFile(ansiFileName);

	::free(ansiFileName);
	return KString(strMD5);
}

KMD5::~KMD5()
{

}