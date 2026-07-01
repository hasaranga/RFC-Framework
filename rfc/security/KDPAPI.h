
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
#include <wincrypt.h>

// DPAPI can encrypt/decrypt data using machine master key.
// useful for preventing accidental leak of data.
class KDPAPI
{
public:

    // any user on this machine can decrypt
    // remember to call LocalFree(outBuffer) if this returned true.
    static bool encryptForMachine(const BYTE* data, DWORD dataSize,
        BYTE** outBuffer, DWORD* outSize, const wchar_t* label = NULL) noexcept
    {
        DATA_BLOB input = { dataSize, (BYTE*)data };
        DATA_BLOB output = { 0 };

        // CRYPTPROTECT_LOCAL_MACHINE = any user on this PC can decrypt
        if (::CryptProtectData(&input, label, NULL, NULL, NULL,
            CRYPTPROTECT_LOCAL_MACHINE | CRYPTPROTECT_UI_FORBIDDEN,
            &output)) {
            *outBuffer = output.pbData;
            *outSize = output.cbData;
            return true;
        }
        return false;
    }

    // remember to call LocalFree(outBuffer) if this returned true.
    static bool decryptForMachine(const BYTE* data, DWORD dataSize,
        BYTE** outBuffer, DWORD* outSize) noexcept
    {
        DATA_BLOB input = { dataSize, (BYTE*)data };
        DATA_BLOB output = { 0 };

        if (::CryptUnprotectData(&input, NULL, NULL, NULL, NULL,
            CRYPTPROTECT_UI_FORBIDDEN, &output)) {
            *outBuffer = output.pbData;
            *outSize = output.cbData;
            return true;
        }
        return false;
    }
};