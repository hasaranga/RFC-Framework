
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
#include <comdef.h>
#include <Wbemidl.h>

// can be use to generate unique id to identify given PC.
class KSystemID
{
public:

    // must initialize COM before calling this method.
    // strings will be empty on error.
    static bool get(KString& smbiosUUID, KString& diskSerial) noexcept
    {
        smbiosUUID = KString();
        diskSerial = KString();

        HRESULT hres;

        // Connect to WMI
        IWbemLocator* pLoc = NULL;
        hres = ::CoCreateInstance(CLSID_WbemLocator, 0,
            CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

        if (FAILED(hres))
            return false;

        IWbemServices* pSvc = NULL;
        hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),
            NULL, NULL, 0, NULL, 0, 0, &pSvc);

        if (FAILED(hres))
        {
            pLoc->Release();
            return false;
        }

        // Set security on the proxy
        hres = ::CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

        bool success = false;

        // Query 1: Get SMBIOS UUID
        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT UUID FROM Win32_ComputerSystemProduct"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

        if (SUCCEEDED(hres))
        {
            IWbemClassObject* pclsObj = NULL;
            ULONG uReturn = 0;

            if (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK && uReturn > 0)
            {
                VARIANT vtProp;
                if (pclsObj->Get(L"UUID", 0, &vtProp, 0, 0) == S_OK)
                {
                    if (vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL)
                    {
                        smbiosUUID = KString(static_cast<const wchar_t*>(vtProp.bstrVal), 
                            KStringBehaviour::MAKE_A_COPY).trim();
                    }
                    ::VariantClear(&vtProp);
                }
                pclsObj->Release();
            }
            pEnumerator->Release();
        }

        // Query 2: Get Disk Serial
        pEnumerator = NULL;
        hres = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT SerialNumber FROM Win32_DiskDrive WHERE Index = 0"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);

        if (SUCCEEDED(hres))
        {
            IWbemClassObject* pclsObj = NULL;
            ULONG uReturn = 0;

            if (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK && uReturn > 0)
            {
                VARIANT vtProp;
                if (pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0) == S_OK)
                {
                    if (vtProp.vt == VT_BSTR && vtProp.bstrVal != NULL)
                    {
                        diskSerial = KString(static_cast<const wchar_t*>(vtProp.bstrVal),
                            KStringBehaviour::MAKE_A_COPY).trim();
                    }
                   ::VariantClear(&vtProp);
                }
                pclsObj->Release();
            }
            pEnumerator->Release();
        }

        // Cleanup
        pSvc->Release();
        pLoc->Release();

        // Return true if we got at least one identifier
        success = smbiosUUID.isNotEmpty() || diskSerial.isNotEmpty();
        return success;
    }
};