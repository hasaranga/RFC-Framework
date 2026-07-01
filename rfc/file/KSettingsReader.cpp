
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

#include "KSettingsReader.h"

KSettingsReader::KSettingsReader() noexcept : streamPtr(nullptr){}

KSettingsReader::KSettingsReader(KStream* stream) noexcept : streamPtr(stream) {}

bool KSettingsReader::openFile(const wchar_t* fileName, int formatID) noexcept
{
	if (!KFile::isFileExists(fileName))
		return false;

	if (!settingsFile.openFile(fileName, KFile::KREAD))
		return false;

	streamPtr = &settingsFile;

	int fileFormatID = 0;
	streamPtr->readStream((BYTE*)&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

bool KSettingsReader::readData(DWORD size, void *buffer) noexcept
{
	if (buffer && streamPtr)
		return streamPtr->readStream((BYTE*)buffer, size);

	return false;
}

KString KSettingsReader::readString() noexcept
{
	if (!streamPtr)
		return KString();

	int size = 0;
	streamPtr->readStream((BYTE*)&size, sizeof(int));

	if (size > 0)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		streamPtr->readStream((BYTE*)buffer, size);

		return KString(buffer, KStringBehaviour::FREE_ON_DESTROY);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::readInt() noexcept
{
	int value = 0;

	if (streamPtr)
		streamPtr->readStream((BYTE*)&value, sizeof(int));

	return value;
}

unsigned int KSettingsReader::readUInt() noexcept
{
	unsigned int value = 0;

	if (streamPtr)
		streamPtr->readStream((BYTE*)&value, sizeof(unsigned int));

	return value;
}

float KSettingsReader::readFloat() noexcept
{
	float value = 0;

	if (streamPtr)
		streamPtr->readStream((BYTE*)&value, sizeof(float));

	return value;
}

double KSettingsReader::readDouble() noexcept
{
	double value = 0;

	if (streamPtr)
		streamPtr->readStream((BYTE*)&value, sizeof(double));

	return value;
}

bool KSettingsReader::readBool() noexcept
{
	bool value = 0;

	if (streamPtr)
		streamPtr->readStream((BYTE*)&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader() noexcept
{

}