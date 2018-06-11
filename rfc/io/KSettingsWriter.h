
/*
	RFC - KSettingsWriter.h
	Copyright (C) 2013-2018 CrownSoft
  
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

#ifndef _RFC_KSETTINGSWRITER_H_
#define _RFC_KSETTINGSWRITER_H_

#include <windows.h>
#include "KFile.h"

/**
	High performance configuration writing class.
*/
class RFC_API KSettingsWriter
{
protected:
	KFile settingsFile;

public:
	KSettingsWriter();

	virtual bool OpenFile(const KString& fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	virtual void WriteData(DWORD size, void *buffer);

	virtual void WriteString(const KString& text);

	virtual void WriteInt(int value);

	virtual void WriteFloat(float value);

	virtual void WriteDouble(double value);

	virtual void WriteBool(bool value);

	virtual ~KSettingsWriter();
};

#endif