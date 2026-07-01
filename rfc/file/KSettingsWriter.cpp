
/*
	Copyright (C) 2013-2026 CrownSoft
  
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

KSettingsWriter::KSettingsWriter() noexcept : streamPtr(nullptr){}

KSettingsWriter::KSettingsWriter(KStream* stream) noexcept : streamPtr(stream){}

bool KSettingsWriter::openFile(const wchar_t* fileName, int formatID) noexcept
{
	if (KFile::isFileExists(fileName))
		KFile::deleteFile(fileName);

	if (!settingsFile.openFile(fileName, KFile::KWRITE))
		return false;

	streamPtr = &settingsFile;
	return streamPtr->writeStream((BYTE*)&formatID, sizeof(int));
}

void KSettingsWriter::writeData(DWORD size, void *buffer) noexcept
{
	if (buffer && streamPtr)
		streamPtr->writeStream((BYTE*)buffer, size);
}

void KSettingsWriter::writeString(const KString& text) noexcept
{
	if (!streamPtr)
		return;

	int size = text.length();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		streamPtr->writeStream((BYTE*)&size, sizeof(int));
		streamPtr->writeStream((BYTE*)(const wchar_t*)text, size);
	}
	else // write only empty size
	{
		streamPtr->writeStream((BYTE*)&size, sizeof(int));
	}
}

void KSettingsWriter::writeInt(int value) noexcept
{
	if (streamPtr)
		streamPtr->writeStream((BYTE*)&value, sizeof(int));
}

void KSettingsWriter::writeUInt(unsigned int value) noexcept
{
	if (streamPtr)
		streamPtr->writeStream((BYTE*)&value, sizeof(unsigned int));
}

void KSettingsWriter::writeFloat(float value) noexcept
{
	if (streamPtr)
		streamPtr->writeStream((BYTE*)&value, sizeof(float));
}

void KSettingsWriter::writeDouble(double value) noexcept
{
	if (streamPtr)
		streamPtr->writeStream((BYTE*)&value, sizeof(double));
}

void KSettingsWriter::writeBool(bool value) noexcept
{
	if (streamPtr)
		streamPtr->writeStream((BYTE*)&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter() noexcept
{

}