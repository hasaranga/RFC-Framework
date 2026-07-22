
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

// ordered so relational operators (>=, <, ...) between two KOSVersion values compare
// capability, not just identity - e.g. `KSystemInfo::getOSVersion() >= KOSVersion::Windows11`.
// Unknown sorts below every named release (RtlGetVersion unavailable/failed, or an OS older
// than this framework's win7-or-higher floor) so a
// "feature needs at least X" check still comes out false instead of undefined.
enum class KOSVersion
{
    Unknown = 0,
    Windows7,
    Windows8,
    Windows8_1,
    Windows10,
    Windows11
};

typedef LONG(WINAPI* KRtlGetVersion)(OSVERSIONINFOW*);

class KSystemInfo
{
public:
    // real OS version, unaffected by the app-manifest compatibility GUIDs that gate
    // GetVersionEx/VerifyVersionInfoW - RtlGetVersion (ntdll, always loaded) reports the truth
    // regardless of manifest. windows 11 kept dwMajorVersion.dwMinorVersion at 10.0; only the
    // build number moved to 22000+. queried once and cached (OS doesn't change mid-process).
    static KOSVersion getOSVersion() noexcept
    {
        static KOSVersion cached = KOSVersion::Unknown;
        static bool queried = false;

        if (!queried)
        {
            queried = true;

            HMODULE hNtdll = ::GetModuleHandleW(L"ntdll.dll");
            if (hNtdll)
            {
                KRtlGetVersion pRtlGetVersion = reinterpret_cast<KRtlGetVersion>(
                    ::GetProcAddress(hNtdll, "RtlGetVersion"));

                if (pRtlGetVersion)
                {
                    OSVERSIONINFOW info{};
                    info.dwOSVersionInfoSize = sizeof(info);

                    if (pRtlGetVersion(&info) == 0) // STATUS_SUCCESS
                    {
                        const DWORD major = info.dwMajorVersion;
                        const DWORD minor = info.dwMinorVersion;
                        const DWORD build = info.dwBuildNumber;

                        if (major > 10)
                            cached = KOSVersion::Windows11; // future major bump - treat as at-least-11 until a new enumerator exists
                        else if (major == 10)
                            cached = (build >= 22000) ? KOSVersion::Windows11 : KOSVersion::Windows10;
                        else if ((major == 6) && (minor == 3))
                            cached = KOSVersion::Windows8_1;
                        else if ((major == 6) && (minor == 2))
                            cached = KOSVersion::Windows8;
                        else if ((major == 6) && (minor == 1))
                            cached = KOSVersion::Windows7;
                        // older than win7 (or an unrecognized combination) stays Unknown - this
                        // framework doesn't support it (FLEXModule.h: "win7 or higher").
                    }
                }
            }
        }

        return cached;
    }
};
