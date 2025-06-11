
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

#include "KSHA1.h"
#include "../file/FileModule.h"
#include "./sha1/sha1.h"

KSHA1::KSHA1()
{

}

KString KSHA1::generateFromString(const KString& text)
{
	if (text.length() == 0)
		return KString();

	char* ansiTxt = KString::toAnsiString(text);

	ExtLibs::CSHA1 sha1;
	sha1.Update((const UINT_8*)ansiTxt, (unsigned int)::strlen(ansiTxt));
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	::free(ansiTxt);
	return KString(szReport);
}

KString KSHA1::generateFromFile(const KString& fileName)
{
	if (fileName.length() == 0) // empty path
		return KString();

	if (!KFile::isFileExists(fileName)) // file does not exists
		return KString();

	KFile file;
	file.openFile(fileName, KFile::KREAD, false);

	const DWORD fileSize = file.getFileSize();
	file.closeFile();

	if (fileSize == 0) // empty file
		return KString();

	char* ansiFileName = KString::toAnsiString(fileName);

	ExtLibs::CSHA1 sha1;
	sha1.HashFile(ansiFileName);
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	::free(ansiFileName);
	return KString(szReport);
}

KSHA1::~KSHA1()
{

}