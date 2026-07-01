
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

#pragma once

#include "../core/CoreModule.h"
#include "KStream.h"
#include "KFile.h"

/**
	High performance configuration writing class.
*/
class KSettingsWriter
{
protected:
	KFile settingsFile;
	KStream* streamPtr;
public:
	KSettingsWriter() noexcept;

	/**
		Constructor that accepts an existing KStream reference.
		The stream object should already be opened for writing.
		KSettingsWriter will not use formatID or close the stream object. it just write data.
	*/
	KSettingsWriter(KStream* stream) noexcept;

	bool openFile(const wchar_t* fileName, int formatID) noexcept;

	/**
		save struct, array or whatever...
	*/
	void writeData(DWORD size, void *buffer) noexcept;

	template<typename T>
	void writeData(T* buffer) noexcept
	{
		if (buffer && streamPtr)
			streamPtr->writeStream((BYTE*)buffer, sizeof(T));
	}

	void writeString(const KString& text) noexcept;

	void writeInt(int value) noexcept;

	void writeUInt(unsigned int value) noexcept;

	void writeFloat(float value) noexcept;

	void writeDouble(double value) noexcept;

	void writeBool(bool value) noexcept;

	~KSettingsWriter() noexcept;

private:
	RFC_LEAK_DETECTOR(KSettingsWriter)
};

