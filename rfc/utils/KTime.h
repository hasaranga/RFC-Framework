
/*
	RFC - KTime.h
	Copyright (C) 2013-2019 CrownSoft

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

#ifndef _RFC_KTIME_H_
#define _RFC_KTIME_H_

#include <windows.h>
#include "../containers/KLeakDetector.h"

// __int64 is not defined in mingw.
#ifdef __MINGW32__
	#include <stdint.h>
	typedef int64_t _int64;
#endif

class KTime
{
public:

	KTime(){}

	// Returns time difference in units of 100 us.
	static _int64 Delta100us(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		union timeunion {
			FILETIME fileTime;
			ULARGE_INTEGER ul;
		};

		timeunion ft1;
		timeunion ft2;

		SystemTimeToFileTime(&time1, &ft1.fileTime);
		SystemTimeToFileTime(&time2, &ft2.fileTime);

		return ft2.ul.QuadPart - ft1.ul.QuadPart;
	}

	// Returns time difference in seconds.
	static _int64 DeltaSeconds(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		return (Delta100us(time1, time2) / 10000000);
	}

	// Returns time difference in minutes.
	static _int64 DeltaMinutes(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		return (DeltaSeconds(time1, time2) / 60);
	}

	// Returns time difference in hours.
	static _int64 DeltaHours(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		return (DeltaMinutes(time1, time2) / 60);
	}

	static void GetNow(SYSTEMTIME* time, const bool isLocalTime = true)
	{
		if (isLocalTime)
			::GetLocalTime(time);
		else
			::GetSystemTime(time);
	}

	virtual ~KTime(){}

private:
	RFC_LEAK_DETECTOR(KTime)
};

#endif