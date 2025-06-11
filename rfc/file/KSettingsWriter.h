
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

#pragma once

#include "../core/CoreModule.h"
#include "KFile.h"

/**
	High performance configuration writing class.
*/
class KSettingsWriter
{
protected:
	KFile settingsFile;

public:
	KSettingsWriter();

	virtual bool openFile(const wchar_t* fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	virtual void writeData(DWORD size, void *buffer);

	virtual void writeString(const KString& text);

	virtual void writeInt(int value);

	virtual void writeFloat(float value);

	virtual void writeDouble(double value);

	virtual void writeBool(bool value);

	virtual ~KSettingsWriter();

private:
	RFC_LEAK_DETECTOR(KSettingsWriter)
};

