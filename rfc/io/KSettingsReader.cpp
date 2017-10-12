
/*
    RFC - KSettingsReader.cpp
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

#include "KSettingsReader.h"

KSettingsReader::KSettingsReader()
{

}

bool KSettingsReader::OpenFile(const KString& fileName, int formatID)
{
	if (!KFile::IsFileExists(fileName))
		return false;

	if (!settingsFile.OpenFile(fileName, KFile::KREAD))
		return false;

	settingsFile.SetFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.ReadFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void KSettingsReader::ReadData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.ReadFile(buffer, size);
}

KString KSettingsReader::ReadString()
{
	int size = 0;
	settingsFile.ReadFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.ReadFile(buffer, size);

		return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::ReadInt()
{
	int value = 0;
	settingsFile.ReadFile(&value, sizeof(int));

	return value;
}

float KSettingsReader::ReadFloat()
{
	float value = 0;
	settingsFile.ReadFile(&value, sizeof(float));

	return value;
}

double KSettingsReader::ReadDouble()
{
	double value = 0;
	settingsFile.ReadFile(&value, sizeof(double));

	return value;
}

bool KSettingsReader::ReadBool()
{
	bool value = 0;
	settingsFile.ReadFile(&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader()
{

}