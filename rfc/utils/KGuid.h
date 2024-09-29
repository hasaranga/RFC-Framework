
/*
	Copyright (C) 2013-2022 CrownSoft

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

// RPC_WSTR is not defined in mingw.
#ifdef __MINGW32__
	typedef unsigned short* RPC_WSTR;
#endif

class KGuid
{
public:

	KGuid(){}

	static bool GenerateGUID(GUID* pGUID)
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static KString GenerateGUID()
	{
		GUID guid;

		if (KGuid::GenerateGUID(&guid))
			return KGuid::GUIDToString(&guid);

		return KString();
	}

	static KString GUIDToString(GUID* pGUID)
	{
		wchar_t* strGuid = nullptr;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		KString result(strGuid, KString::USE_COPY_OF_TEXT);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	virtual ~KGuid(){}

private:
	RFC_LEAK_DETECTOR(KGuid)
};

