
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
#include <stdint.h>

/*
    TOTP authenticator generates time-based one-time passwords (TOTPs) for two-factor authentication (2FA).
    These passwords, typically 6 digits, are valid for a short period (usually 30 seconds) and change frequently,
    adding an extra layer of security to online accounts.
*/
class KTOTPAuth
{
protected:
    // generates a cryptographically secure random key
    static int generateSecretKey(uint8_t* key, DWORD keyLength);

    // encodes key to Base32 (for compatibility with authenticator apps)
    static void encodeBase32(const uint8_t* input, size_t inputLength, char* output);

    // converts key to Base32 format string
    // make sure to free the returned string.
    static char* keyToBase32String(const uint8_t* key, size_t keyLength);

    // make sure to free the returned string.
    static char* totpCodeToString(uint32_t totpCode);

    /**
     * Decode Base32 string to binary key
     *
     * @param str Base32 encoded string (case insensitive, padding optional)
     * @param key Output buffer for decoded bytes
     * @param keyLength Maximum size of output buffer
     * @return Number of bytes decoded, or -1 on error
     */
    static int base32StringToKey(const char* str, uint8_t* key, size_t keyLength);

    // get current Unix timestamp
    static uint32_t getCurrentTimestamp();

    // generates TOTP for specific timestamp
    static uint32_t generateTOTPForTime(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep, uint32_t timestamp);

    // generates TOTP for current timestamp
    static uint32_t generateTOTPForCurrentTime(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep);

    // validates TOTP code with time window tolerance.
    // returns: 0 = invalid, 1 = valid (current), 2 = valid (previous), 3 = valid (next)
    // windowTolerance: +1 means 30 seconds next and previous also check.
    static int _validateTOTPCode(uint8_t* hmacKey, uint8_t keyLength, uint32_t inputCode, uint32_t timeStep = 30, int windowTolerance = 1);

    // uses UrlEscapeW winapi. Not RFC 3986 Compliant. but enough for totp requirements.
    // maximum text size is 512.
    static KString urlEncode(const KString& text);
public:

    /* calculates remaining seconds until next TOTP code.
    * Example:
        int lastRemaining = -1;
        while (true)
        {
            int remaining = KTOTPAuth::getRemainingSeconds();

            if (lastRemaining != -1 && remaining > lastRemaining)
            {
                totpCode = KTOTPAuth::getTOTPCodeForCurrentTime(totpKey);
                ::wprintf(L"code = %s\n", (const wchar_t*)totpCode);
            }
            lastRemaining = remaining;
            ::Sleep(1000);
        }
    */
    static int getRemainingSeconds(uint32_t timeStep = 30);

    static KString generateTOTPKey();

    // generate a string which can be used to create a QR code.
    // issuer & username will be converted to url-encode format. no need to convert it yourself.
    static KString generateQRText(const KString& key, const KString& issuer, const KString& username);

    // returns empty string on error.
    // important: do not compare returned string with user entered code. Instead, use validateTOTPCode method.
    // validateTOTPCode method correctly checks for prev/next codes also. it will prevent clock drift issues.
    // this method should only be use for displaying current code.
    static KString getTOTPCodeForCurrentTime(const KString& key, uint32_t timeStep = 30);

    // validates TOTP code with time window tolerance.
    // returns: 0 = invalid, 1 = valid (current), 2 = valid (previous), 3 = valid (next)
    // windowTolerance: +1 means 30 seconds next and previous also check.
    static int validateTOTPCode(const KString& key, const KString& inputCode, uint32_t timeStep = 30, int windowTolerance = 1);
};

