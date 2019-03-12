
/*
    RFC - KRegistry.h
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

#ifndef _RFC_KREGISTRY_H_
#define _RFC_KREGISTRY_H_

#include"../config.h"
#include<windows.h>
#include"../text/KString.h"

class KRegistry
{

public:
	KRegistry();

	// returns true on success or if the key already exists.
	static bool CreateKey(HKEY hKeyRoot, const KString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool DeleteKey(HKEY hKeyRoot, const KString& subKey);

	static bool ReadString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString *result);

	static bool WriteString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value);

	static bool ReadDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD *result);

	static bool WriteDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void *buffer;
		DWORD bufferSize;
		if(KRegistry::ReadBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool ReadBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void **buffer, DWORD *buffSize);

	static bool WriteBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void *buffer, DWORD buffSize);

	virtual ~KRegistry();

};

#endif