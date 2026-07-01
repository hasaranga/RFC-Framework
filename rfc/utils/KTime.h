
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
#include <stdio.h>
#include <cstdint>
#include <time.h>

class KTime
{
public:

	KTime() noexcept {}

	// Returns time difference in units of 100 us. returns absolute difference.
	static uint64_t delta100us(const SYSTEMTIME& time1, const SYSTEMTIME& time2) noexcept
	{
		union timeunion {
			FILETIME fileTime;
			ULARGE_INTEGER ul;
		};

		timeunion ft1;
		timeunion ft2;

		::SystemTimeToFileTime(&time1, &ft1.fileTime);
		::SystemTimeToFileTime(&time2, &ft2.fileTime);

		if (ft2.ul.QuadPart > ft1.ul.QuadPart)
			return ft2.ul.QuadPart - ft1.ul.QuadPart;
		else
			return ft1.ul.QuadPart - ft2.ul.QuadPart;
	}

	// Returns time difference in seconds. returns absolute difference.
	static uint64_t deltaSeconds(const SYSTEMTIME& time1, const SYSTEMTIME& time2) noexcept
	{
		return (delta100us(time1, time2) / 10000000);
	}

	// Returns time difference in minutes. returns absolute difference.
	static uint64_t deltaMinutes(const SYSTEMTIME& time1, const SYSTEMTIME& time2) noexcept
	{
		return (deltaSeconds(time1, time2) / 60);
	}

	// Returns time difference in hours. returns absolute difference.
	static uint64_t deltaHours(const SYSTEMTIME& time1, const SYSTEMTIME& time2) noexcept
	{
		return (deltaMinutes(time1, time2) / 60);
	}

	// Returns time difference in days. returns absolute difference.
	static uint64_t deltaDays(const SYSTEMTIME& time1, const SYSTEMTIME& time2) noexcept
	{
		// 100us units per day: 24h * 60m * 60s * 10,000,000 (100-ns intervals per second)
		return (delta100us(time1, time2) / 864000000000ULL);
	}

	static void getNow(SYSTEMTIME* time, const bool isLocalTime = true) noexcept
	{
		if (isLocalTime)
			::GetLocalTime(time);
		else
			::GetSystemTime(time);
	}

	// string format: 2026-02-26
	static bool parseDateString(const wchar_t* dateStr, SYSTEMTIME* st) noexcept
	{
		ZeroMemory(st, sizeof(SYSTEMTIME));

		int year, month, day;
		if (::swscanf_s(dateStr, L"%d-%d-%d", &year, &month, &day) != 3)
			return false;

		st->wYear = (WORD)year;
		st->wMonth = (WORD)month;
		st->wDay = (WORD)day;
		// Leave wHour, wMinute, wSecond as 0 (midnight)

		return true;
	}

	// Converts a SYSTEMTIME structure to a single 64-bit FILETIME value.
	// FILETIME represents time as the number of 100-nanosecond intervals since January 1, 1601 (UTC),
	// making it suitable for direct arithmetic and comparison operations with other FILETIME values.
	static uint64_t toFileTime(const SYSTEMTIME& time) noexcept
	{
		FILETIME fileTime = {};
		::SystemTimeToFileTime(&time, &fileTime);
		return ((uint64_t)fileTime.dwHighDateTime << 32) | fileTime.dwLowDateTime;
	}

	// converts given utc time to local time zone.
	static bool convertUTCToLocal(const SYSTEMTIME& utcTime, SYSTEMTIME* localTime)
	{
		FILETIME ftUtc, ftLocal;

		// Convert SYSTEMTIME -> FILETIME (UTC)
		if (!SystemTimeToFileTime(&utcTime, &ftUtc))
			return false;

		// Convert UTC FILETIME -> Local FILETIME
		if (!FileTimeToLocalFileTime(&ftUtc, &ftLocal))
			return false;

		// Convert local FILETIME -> SYSTEMTIME
		if (!FileTimeToSystemTime(&ftLocal, localTime))
			return false;

		return true;
	}

	// converts given systemtime to "2026 February 02" format.
	static KString toDateString(const SYSTEMTIME& st)
	{
		struct tm t = {};
		t.tm_year = st.wYear - 1900;
		t.tm_mon = st.wMonth - 1;
		t.tm_mday = st.wDay;

		// Let the runtime fill in the rest (needed for wcsftime)
		::mktime(&t);

		wchar_t buf[64] = {};
		::wcsftime(buf, 64, L"%Y %B %d", &t);

		return KString(buf, KStringBehaviour::MAKE_A_COPY);
	}

	~KTime() noexcept {}

private:
	RFC_LEAK_DETECTOR(KTime)
};

