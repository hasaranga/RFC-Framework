
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

// RPC_WSTR is not defined in mingw.
#ifdef __MINGW32__
	typedef unsigned short* RPC_WSTR;
#endif

class KGuid
{
	GUID guid;
public:
	KGuid(bool generate = false) noexcept 
	{
		if(generate)
			KGuid::generateGUID(&guid);
	}

	KGuid(const GUID& other) noexcept
	{
		guid = other;
	}

	KGuid(const KGuid& other) noexcept
	{
		guid = other.guid;
	}

	KGuid(KGuid&& other) noexcept
	{
		guid = other.guid;
	}

	// Replaces this guid contents with another guid.
	const KGuid& operator= (const KGuid& other) noexcept
	{
		if (this != &other)
		{
			guid = other.guid;
		}
		return *this;
	}

	const KGuid& operator= (const GUID& other) noexcept
	{
		guid = other;	
		return *this;
	}

	// Move assignment. does not clear other guid.
	KGuid& operator= (KGuid&& other) noexcept
	{
		if (this != &other)
		{
			guid = other.guid;
		}
		return *this;
	}

	// compare with other guid
	bool operator==(const KGuid& other) const noexcept
	{
		return (::IsEqualGUID(guid, other.guid) == TRUE);
	}

	operator const GUID* ()const noexcept
	{
		return &guid;
	}

	operator GUID ()const noexcept
	{
		return guid;
	}

	GUID* data() noexcept
	{
		return &guid;
	}

	KString toString() const noexcept
	{
		return KGuid::guidToString(&guid);
	}

	bool compare(const KGuid& other) const noexcept
	{
		return (::IsEqualGUID(guid, other.guid) == TRUE);
	}

	bool compare(const GUID& other) const noexcept
	{
		return (::IsEqualGUID(guid, other) == TRUE);
	}

	static bool generateGUID(GUID* pGUID) noexcept
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static KString guidToString(const GUID* pGUID) noexcept
	{
		wchar_t* strGuid = nullptr;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		KString result(strGuid, KStringBehaviour::MAKE_A_COPY);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	static KString generateGUID() noexcept
	{
		GUID guid;

		if (KGuid::generateGUID(&guid))
			return KGuid::guidToString(&guid);

		return KString();
	}

	~KGuid() noexcept {}

private:
	RFC_LEAK_DETECTOR(KGuid)
};

