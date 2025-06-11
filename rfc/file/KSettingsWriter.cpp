
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

#include "KSettingsWriter.h"

KSettingsWriter::KSettingsWriter() {}

bool KSettingsWriter::openFile(const wchar_t* fileName, int formatID)
{
	if (KFile::isFileExists(fileName))
		KFile::deleteFile(fileName);

	if (!settingsFile.openFile(fileName, KFile::KWRITE))
		return false;

	settingsFile.setFilePointerToStart();
	settingsFile.writeFile(&formatID, sizeof(int));

	return true;
}

void KSettingsWriter::writeData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.writeFile(buffer, size);
}

void KSettingsWriter::writeString(const KString& text)
{
	int size = text.length();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.writeFile(&size, sizeof(int));

		settingsFile.writeFile((const wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.writeFile(&size, sizeof(int));
	}
}

void KSettingsWriter::writeInt(int value)
{
	settingsFile.writeFile(&value, sizeof(int));
}

void KSettingsWriter::writeFloat(float value)
{
	settingsFile.writeFile(&value, sizeof(float));
}

void KSettingsWriter::writeDouble(double value)
{
	settingsFile.writeFile(&value, sizeof(double));
}

void KSettingsWriter::writeBool(bool value)
{
	settingsFile.writeFile(&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter()
{

}