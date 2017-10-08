
/*
    RFC - KSettingsWriter.cpp
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

#include "KSettingsWriter.h"

KSettingsWriter::KSettingsWriter()
{

}

bool KSettingsWriter::OpenFile(const KString& fileName, int formatID)
{
	if (KFile::IsFileExists(fileName))
		KFile::DeleteFile(fileName);

	if (!settingsFile.OpenFile(fileName, KFile::KWRITE))
		return false;

	settingsFile.SetFilePointerToStart();
	settingsFile.WriteFile(&formatID, sizeof(int));

	return true;
}

void KSettingsWriter::WriteData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.WriteFile(buffer, size);
}

void KSettingsWriter::WriteString(const KString& text)
{
	int size = text.GetLength();
	if (size)
	{
		size = (size + 1)*sizeof(wchar_t);
		settingsFile.WriteFile(&size, sizeof(int));

		settingsFile.WriteFile((wchar_t*)(const wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.WriteFile(&size, sizeof(int));
	}
}

void KSettingsWriter::WriteInt(int value)
{
	settingsFile.WriteFile(&value, sizeof(int));
}

void KSettingsWriter::WriteFloat(float value)
{
	settingsFile.WriteFile(&value, sizeof(float));
}

void KSettingsWriter::WriteDouble(double value)
{
	settingsFile.WriteFile(&value, sizeof(double));
}

void KSettingsWriter::WriteBool(bool value)
{
	settingsFile.WriteFile(&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter()
{

}