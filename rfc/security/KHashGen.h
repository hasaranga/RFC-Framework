
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
#include <bcrypt.h>
#include <stdio.h>

enum class KHashAlgorithm
{
    MD5,
    SHA1,
    SHA256
};

class KHashGen
{
public:
    KHashGen() noexcept = default;
    ~KHashGen() noexcept = default;

    // returns empty string on failure.
    // utf8 version of the text will be used to generate the hash.
    static KString generate(const KString& text, KHashAlgorithm algorithm) noexcept
    {
        BCRYPT_ALG_HANDLE hAlg = NULL;
        BCRYPT_HASH_HANDLE hHash = NULL;

        if (text.length() == 0)
            return KString();

        LPCWSTR algorithmId;
        int hashSize;

        switch (algorithm)
        {
        case KHashAlgorithm::MD5:
            algorithmId = BCRYPT_MD5_ALGORITHM;
            hashSize = 16;
            break;
        case KHashAlgorithm::SHA1:
            algorithmId = BCRYPT_SHA1_ALGORITHM;
            hashSize = 20;
            break;
        case KHashAlgorithm::SHA256:
            algorithmId = BCRYPT_SHA256_ALGORITHM;
            hashSize = 32;
            break;
        default:
            return KString();
        }

        BYTE hashBytes[32] = {};  // sized for the largest (SHA256)
        char ansiHex[65] = {};    // sized for the largest (SHA256)

        char* ansiText = KString::toUTF8String(text);

        auto cleanup = [&]() {
            ::free(ansiText);
            if (hHash)
                ::BCryptDestroyHash(hHash);
            if (hAlg)
                ::BCryptCloseAlgorithmProvider(hAlg, 0);
        };

        if (!BCRYPT_SUCCESS(::BCryptOpenAlgorithmProvider(&hAlg, algorithmId, NULL, 0)))
        {
            cleanup();
            return KString();
        }

        if (!BCRYPT_SUCCESS(::BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0)))
        {
            cleanup();
            return KString();
        }

        if (!BCRYPT_SUCCESS(::BCryptHashData(hHash, (PUCHAR)ansiText, (ULONG)text.length(), 0)))
        {
            cleanup();
            return KString();
        }

        if (!BCRYPT_SUCCESS(::BCryptFinishHash(hHash, hashBytes, hashSize, 0)))
        {
            cleanup();
            return KString();
        }

        for (int i = 0; i < hashSize; i++)
            ::sprintf_s(ansiHex + i * 2, 3, "%02x", hashBytes[i]);

        ansiHex[hashSize * 2] = '\0';

        cleanup();
        return KString(ansiHex);
    }
};