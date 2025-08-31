
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

#include "KTOTPAuth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wincrypt.h>
#include <shlwapi.h>
#include "totpmcu/TOTP.h"

// Base32 decode lookup table (RFC 4648)
// Maps Base32 characters to their 5-bit values
// Invalid characters map to 255
const uint8_t KTOTPAuth_base32_decode_table[256] = {
    // 0-31: Invalid
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    // 32-47: Space, !, ", #, $, %, &, ', (, ), *, +, comma, -, ., /
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    // 48-63: 0-9, :, ;, <, =, >, ?, @
    255,255, 26, 27, 28, 29, 30, 31,255,255,255,255,255,  0,255,255, // 2-7, =
    // 64-79: @, A-O
    255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    // 80-95: P-Z, [, \, ], ^, _
     15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
     // 96-111: `, a-o
     255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
     // 112-127: p-z, {, |, }, ~, DEL
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
      // 128-255: Extended ASCII (all invalid)
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

int KTOTPAuth::generateSecretKey(uint8_t* key, DWORD keyLength)
{
    HCRYPTPROV hProv = 0;

    // Acquire cryptographic provider
    if (!::CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        return 0;

    // Generate random bytes
    if (!::CryptGenRandom(hProv, keyLength, key))
    {
        ::CryptReleaseContext(hProv, 0);
        return 0;
    }

    ::CryptReleaseContext(hProv, 0);
    return 1;
}

void KTOTPAuth::encodeBase32(const uint8_t* input, size_t inputLength, char* output)
{
    // Base32 encoding table (RFC 4648)
    const char base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    size_t i = 0, j = 0;
    uint32_t buffer = 0;
    int bitsLeft = 0;

    while (i < inputLength)
    {
        buffer = (buffer << 8) | input[i++];
        bitsLeft += 8;

        while (bitsLeft >= 5)
        {
            output[j++] = base32_alphabet[(buffer >> (bitsLeft - 5)) & 0x1F];
            bitsLeft -= 5;
        }
    }

    if (bitsLeft > 0)
        output[j++] = base32_alphabet[(buffer << (5 - bitsLeft)) & 0x1F];

    // Add padding
    while (j % 8 != 0)
    {
        output[j++] = '=';
    }

    output[j] = '\0';
}

char* KTOTPAuth::keyToBase32String(const uint8_t* key, size_t keyLength)
{
    // Calculate required buffer size for Base32
    size_t base32Length = ((keyLength * 8) + 4) / 5;  // Round up
    base32Length = ((base32Length + 7) / 8) * 8;      // Pad to multiple of 8

    char* base32Key = (char*)::malloc(base32Length + 1);
    encodeBase32(key, keyLength, base32Key);
    return base32Key;
}

char* KTOTPAuth::totpCodeToString(uint32_t totpCode)
{
    char* result = (char*)::malloc(7 * sizeof(char));

    // sprintf_s is Microsoft's secure version
    ::sprintf_s(result, 7, "%06u", totpCode);
    return result;
}

int KTOTPAuth::base32StringToKey(const char* str, uint8_t* key, size_t keyLength)
{
    if (!str || !key || keyLength == 0)
    {
        return -1; // Invalid parameters
    }

    // Calculate input length, ignoring padding and whitespace
    size_t inputLen = ::strlen(str);
    if (inputLen == 0)
        return -1; // Empty string

    // Remove padding characters and count actual data length
    size_t dataLen = 0;
    for (size_t i = 0; i < inputLen; i++)
    {
        char c = str[i];
        if (c != '=' && !isspace(c))
            dataLen++;
    }

    if (dataLen == 0)
        return -1; // No valid data

    // Calculate expected output length
    // Each 8 Base32 characters encode 5 bytes
    size_t expectedOutputLen = (dataLen * 5) / 8;

    if (expectedOutputLen > keyLength)
        return -1; // Output buffer too small

    // Decode process
    uint32_t buffer = 0;        // Accumulates decoded bits
    int bitsInBuffer = 0;       // Number of valid bits in buffer
    size_t outputPos = 0;       // Current output position

    for (size_t i = 0; i < inputLen && outputPos < keyLength; i++)
    {
        char c = str[i];

        // Skip padding and whitespace
        if (c == '=' || isspace(c))
            continue;

        // Look up character value
        uint8_t value = KTOTPAuth_base32_decode_table[(unsigned char)c];
        if (value == 255)
            return -1; // Invalid Base32 character

        // Add 5 bits to buffer
        buffer = (buffer << 5) | value;
        bitsInBuffer += 5;

        // Extract complete bytes (8 bits each)
        while (bitsInBuffer >= 8)
        {
            bitsInBuffer -= 8;
            key[outputPos++] = (uint8_t)(buffer >> bitsInBuffer);
            buffer &= (1 << bitsInBuffer) - 1; // Clear extracted bits
        }
    }

    // Handle remaining bits (should be less than 8)
    // In valid Base32, remaining bits should be zero-padded
    if (bitsInBuffer > 0)
    {
        // Verify remaining bits are zero (proper padding)
        if (buffer != 0)
        {
            // Some implementations are lenient about this
            // Uncomment the next line for strict validation
            // return -1; // Invalid padding bits
        }
    }

    return (int)outputPos; // Return number of bytes decoded
}

uint32_t KTOTPAuth::getCurrentTimestamp()
{
    return (uint32_t)::time(NULL);
}

uint32_t KTOTPAuth::generateTOTPForTime(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep, uint32_t timestamp)
{
    totpmcu::TOTPGen::TOTP(hmacKey, keyLength, timeStep);
    return  totpmcu::TOTPGen::getCodeFromTimestamp(timestamp);
}

uint32_t KTOTPAuth::generateTOTPForCurrentTime(uint8_t* hmacKey, uint8_t keyLength, uint32_t timeStep)
{
    uint32_t currentTime = getCurrentTimestamp();
    return generateTOTPForTime(hmacKey, keyLength, timeStep, currentTime);
}

int KTOTPAuth::_validateTOTPCode(uint8_t* hmacKey, uint8_t keyLength, uint32_t inputCode, uint32_t timeStep, int windowTolerance)
{
    uint32_t currentTime = getCurrentTimestamp();

    // Check multiple time windows based on tolerance
    for (int i = -windowTolerance; i <= windowTolerance; i++)
    {
        uint32_t testTime = currentTime + (i * timeStep);
        uint32_t expectedCode = generateTOTPForTime(hmacKey, keyLength, timeStep, testTime);

        if (expectedCode == inputCode)
        {
            if (i == 0)
                return 1;      // Current window
            else if (i == -1)
                return 2; // Previous window  
            else if (i == 1)
                return 3;  // Next window
            else
                return 4;              // Other window (if tolerance > 1)
        }
    }

    return 0; // Invalid code
}

int KTOTPAuth::getRemainingSeconds(uint32_t timeStep)
{
    uint32_t currentTime = KTOTPAuth::getCurrentTimestamp();
    return timeStep - (currentTime % timeStep);
}

KString KTOTPAuth::generateTOTPKey()
{
    uint8_t hmacKey[20] = { 0 }; // 160-bit key (recommended for SHA-1)
    uint8_t keyLength = (uint8_t)sizeof(hmacKey);

    KTOTPAuth::generateSecretKey(hmacKey, keyLength);
    char* ansiKey = KTOTPAuth::keyToBase32String(hmacKey, keyLength);
    KString strKey(ansiKey);
    ::free(ansiKey);

    return strKey;
}

KString KTOTPAuth::getTOTPCodeForCurrentTime(const KString& key, uint32_t timeStep)
{
    uint8_t hmacKey[20] = { 0 }; // 160-bit key (recommended for SHA-1)
    uint8_t keyLength = (uint8_t)sizeof(hmacKey);

    char* ansiKey = KString::toAnsiString(key);
    const int res = KTOTPAuth::base32StringToKey(ansiKey, hmacKey, keyLength);
    ::free(ansiKey);

    if (res == -1)
        return KString();

    uint32_t totpCode = KTOTPAuth::generateTOTPForCurrentTime(hmacKey, keyLength, timeStep);
    char* totpCodeAnsi = KTOTPAuth::totpCodeToString(totpCode);
    KString strTOTPCode(totpCodeAnsi);
    ::free(totpCodeAnsi);

    return strTOTPCode;
}

int KTOTPAuth::validateTOTPCode(const KString& key, const KString& inputCode, uint32_t timeStep, int windowTolerance)
{
    uint8_t hmacKey[20] = { 0 }; // 160-bit key (recommended for SHA-1)
    uint8_t keyLength = (uint8_t)sizeof(hmacKey);

    char* ansiKey = KString::toAnsiString(key);
    const int res = base32StringToKey(ansiKey, hmacKey, keyLength);
    ::free(ansiKey);

    if (res == -1)
        return 0;

    uint32_t inputCodeInt = (uint32_t)_wtoi(inputCode);
    return KTOTPAuth::_validateTOTPCode(hmacKey, keyLength, inputCodeInt, timeStep, windowTolerance);
}

KString KTOTPAuth::generateQRText(const KString& key, const KString& issuer, const KString& username)
{
    KString encodedIssuer = KTOTPAuth::urlEncode(issuer);
    KString encodedUsername = KTOTPAuth::urlEncode(username);

    return CONST_TXT("otpauth://totp/") + encodedIssuer + CONST_TXT("%3A") + encodedUsername + CONST_TXT("?secret=") +
        key + CONST_TXT("&issuer=") + encodedIssuer;
}

KString KTOTPAuth::urlEncode(const KString& text)
{
    wchar_t output[512];
    output[0] = 0;
    DWORD size = sizeof(output) / sizeof(wchar_t);
    ::UrlEscapeW((const wchar_t*)text, output, &size, 0);

    return KString(output, KStringBehaviour::MAKE_A_COPY);
}