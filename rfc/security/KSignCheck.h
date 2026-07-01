
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
#include <bcrypt.h>

/*
======================================================================================
generate RSA-2048 keys:
    $openssl genrsa -out rsa_priv.pem 2048
    $openssl rsa -in rsa_priv.pem -pubout > rsa_pub.pem

create signature and encode to base64:
    $openssl dgst -sha256 -sign rsa_priv.pem helloworld.txt | openssl enc -base64 -A > signature.txt

convert base64 signature to binary and verify:
    $openssl enc -d -A -base64 -in signature.txt -out signature.bin
    $openssl dgst -sha256 -verify rsa_pub.pem -signature signature.bin helloworld.txt

======================================================================================

    // RSA-2048 public key (replace with your own generated key)
    static const CHAR szKey[] =
        "-----BEGIN PUBLIC KEY-----\n"
        "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2a2rwplBQL...\n"
        "-----END PUBLIC KEY-----";

    // RSA-2048 produces a 256-byte signature, base64 encoded = ~344 characters
    static const CHAR szSig[] =
        "base64encodedsignaturegoeshere...";

    static const UCHAR data[] = "Hello World2";

    if (KSignCheck::verify(szKey, szSig, data, sizeof(data) - 1) == S_OK)
        printf("verified");
    else
        printf("failed");

======================================================================================
notes:
    - RSA-2048 with SHA-256 provides 112-bit security strength, recommended
      by NIST until at least 2030.
    - RSA-2048 signatures are 256 bytes.
    - Base64-encoded RSA-2048 signatures are approximately 344 characters.
    - Verification is fast with RSA due to the small public exponent (e=65537).
    - If switching hash algorithm to SHA-512, increase the hash[] buffer to 64.
======================================================================================
*/

class KSignCheck
{
public:

    // https://stackoverflow.com/questions/75048954/nte-bad-signature-while-verifying-rsa-using-win32-api

    inline static ULONG BOOL_TO_ERROR(BOOL f) noexcept
    {
        return (f ? NOERROR : ::GetLastError());
    }

    HRESULT static stringToBin(_Out_ PDATA_BLOB pdb, _In_ ULONG dwFlags, _In_ PCSTR pszString, _In_ ULONG cchString = 0) noexcept
    {
        PUCHAR pb = 0;
        ULONG cb = 0;

        while (::CryptStringToBinaryA(pszString, cchString, dwFlags, pb, &cb, 0, 0))
        {
            if (pb)
            {
                pdb->pbData = pb, pdb->cbData = cb;
                return S_OK;
            }

            if (!(pb = (PUCHAR)::LocalAlloc(LMEM_FIXED, cb)))
            {
                break;
            }
        }

        return HRESULT_FROM_WIN32(::GetLastError());
    }


    HRESULT static verify(PCSTR szKey, PCSTR szSig, const UCHAR* pbData, ULONG cbData) noexcept
    {
        DATA_BLOB db;
        HRESULT hr;

        if (NOERROR == (hr = KSignCheck::stringToBin(&db, CRYPT_STRING_BASE64HEADER, szKey)))
        {
            ULONG cb;
            CERT_PUBLIC_KEY_INFO* publicKeyInfo;

            hr = BOOL_TO_ERROR(::CryptDecodeObjectEx(X509_ASN_ENCODING, X509_PUBLIC_KEY_INFO,
                db.pbData, db.cbData, CRYPT_DECODE_ALLOC_FLAG, 0, &publicKeyInfo, &cb));

            ::LocalFree(db.pbData);

            if (NOERROR == hr)
            {
                BCRYPT_KEY_HANDLE hKey;

                hr = BOOL_TO_ERROR(::CryptImportPublicKeyInfoEx2(X509_ASN_ENCODING, publicKeyInfo, 0, 0, &hKey));

                ::LocalFree(publicKeyInfo);

                if (NOERROR == hr)
                {
                    // SHA-256 produces a 32-byte hash regardless of RSA key size.
                    // RSA-2048 signatures are 256 bytes; CNG handles this automatically
                    // via BCRYPT_PAD_PKCS1 without any extra buffer size configuration.
                    // If switching to SHA-512, increase this buffer to 64 bytes.
                    UCHAR hash[32];

                    if (NOERROR == (hr = BOOL_TO_ERROR(::CryptHashCertificate2(BCRYPT_SHA256_ALGORITHM, 0, 0, pbData, cbData, hash, &(cb = sizeof(hash))))))
                    {
                        if (0 <= (hr = KSignCheck::stringToBin(&db, CRYPT_STRING_BASE64, szSig)))
                        {
                            BCRYPT_PKCS1_PADDING_INFO pi = { BCRYPT_SHA256_ALGORITHM };

                            if (0 > (hr = ::BCryptVerifySignature(hKey, &pi, hash, cb, db.pbData, db.cbData, BCRYPT_PAD_PKCS1)))
                            {
                                hr |= FACILITY_NT_BIT;
                            }

                            ::LocalFree(db.pbData);
                        }
                    }

                    ::BCryptDestroyKey(hKey);
                }
            }
        }

        return HRESULT_FROM_WIN32(hr);
    }
};

