
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

#include "KSettingsReader.h"

KSettingsReader::KSettingsReader()
{

}

bool KSettingsReader::openFile(const wchar_t* fileName, int formatID)
{
	if (!KFile::isFileExists(fileName))
		return false;

	if (!settingsFile.openFile(fileName, KFile::KREAD))
		return false;

	settingsFile.setFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.readFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void KSettingsReader::readData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.readFile(buffer, size);
}

KString KSettingsReader::readString()
{
	int size = 0;
	settingsFile.readFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.readFile(buffer, size);

		return KString(buffer, KStringBehaviour::FREE_ON_DESTROY);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::readInt()
{
	int value = 0;
	settingsFile.readFile(&value, sizeof(int));

	return value;
}

float KSettingsReader::readFloat()
{
	float value = 0;
	settingsFile.readFile(&value, sizeof(float));

	return value;
}

double KSettingsReader::readDouble()
{
	double value = 0;
	settingsFile.readFile(&value, sizeof(double));

	return value;
}

bool KSettingsReader::readBool()
{
	bool value = 0;
	settingsFile.readFile(&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader()
{

}