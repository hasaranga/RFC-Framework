
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

// Readonly data storage model. Use Property Storage - Editor tool to write.
// Can be used when there is no order of storing data objects.

#pragma once

#include <windows.h>
#include "../containers/ContainersModule.h"

#pragma comment(lib, "Rpcrt4.lib")

#define PS_V1_HEADER    { 'P','S','0','1'}

namespace KPSPropertyTypes
{
    enum KPSPropertyTypes
    {
        STRING = 0,
        INTEGER = 1,
        DWORD = 2,
        FLOAT = 3,
        INT_ARRAY = 4,
        GUID = 5,
        FILE = 6
    };
}

class KPSProperty
{
public:
    // ======== stuff written to file ============
    wchar_t* name;
    int nameLength;

    int type; // KPSPropertyTypes

    wchar_t* strValue;
    int strValueLength;

    int intValue;
    DWORD dwordValue;
    float floatValue;

    int* intArray;
    int intArraySize;

    GUID guidValue;

    wchar_t* fileName;
    int fileNameLength;
    unsigned char* fileData;
    DWORD fileDataSize;
    // ============================================

    KPSProperty()
    {
        name = NULL;
        strValue = NULL;
        fileName = NULL;
        fileData = NULL;
        intArray = NULL;
        
        nameLength = 0;
        strValueLength = 0;
        type = KPSPropertyTypes::INTEGER;
        intValue = 0;
        dwordValue = 0;
        floatValue = 0.0f;
        fileNameLength = 0;
        fileDataSize = 0;
        intArraySize = 0;
    }

    virtual ~KPSProperty()
    {
        if (name)
            ::free(name);
        if (strValue)
            ::free(strValue);
        if (intArray)
            ::free(intArray);
        if (fileName)
            ::free(fileName);
        if (fileData)
            ::free(fileData);
    }
};

class KPSObject
{
public:
    // ======== stuff written to file ============
    wchar_t* name;
    int nameLength;
    GUID objectID;
    // ============================================

    KPointerList<KPSProperty*, 32, false> propertyList;

    KPSObject()
    {
        name = NULL;
        nameLength = 0;     
    }

    bool compare(const GUID& objectID)
    {
        return (::IsEqualGUID(objectID, this->objectID) == TRUE);
    }

    virtual ~KPSObject()
    {
        if (name)
            ::free(name);

        propertyList.deleteAll();
    }
};

class KPSPropertyView : public KPSProperty {

public:
    wchar_t* intArrayStr;
    wchar_t* guidValueStr;

    KPSObject* parentObject;

    KPSPropertyView()
    {
        parentObject = NULL;
        guidValueStr = NULL;
        intArrayStr = NULL;
    }

    virtual ~KPSPropertyView()
    {
        if (intArrayStr)
            ::free(intArrayStr);
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);
    }

    void generateIntArrayString()
    {
        if (intArrayStr)
            ::free(intArrayStr);

        intArrayStr = NULL;

        if (intArraySize == 0)
            return;

        intArrayStr = (wchar_t*)::malloc(sizeof(wchar_t) * (16 + 1) * intArraySize);
        intArrayStr[0] = 0;

        for (int i = 0; i < intArraySize; i++)
        {
            wchar_t buffer[16];
            ::_itow_s(intArray[i], buffer, 16, 10);

            ::wcscat_s(intArrayStr, 16, buffer);

            if (i != (intArraySize - 1)) // not the last one
                ::wcscat_s(intArrayStr, 16, L",");
        }
    }

    void generateIntArrayByString(wchar_t* text)
    {
        if (intArray)
            ::free(intArray);

        intArray = NULL;
        intArraySize = 0;

        int textLength = (int)::wcslen(text);
        int commaCount = 0;

        for (int i = 0; i < textLength; i++)
        {
            if (text[i] == L',')
                commaCount++;
        }

        intArray = (int*)::malloc(sizeof(int) * (commaCount + 1));

        wchar_t* str = text;
        wchar_t* end = str;

        while (*end) {
            int n = ::wcstol(str, &end, 10);
            intArray[intArraySize] = n;
            intArraySize++;

            if (intArraySize == (commaCount + 1))
                break;

            // skip non numeric characters
            while ((*end != 0) && ((*end < L'0') || (*end > L'9'))) {
                end++;
            }

            str = end;
        }

        generateIntArrayString();
    }

    void generateGUIDValueString()
    {
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);

        guidValueStr = NULL;

        ::UuidToStringW(&guidValue, (RPC_WSTR*)&guidValueStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool generateGUIDValueByString(const wchar_t* text)
    {
        bool success = true;

        if (::UuidFromStringW((RPC_WSTR)text, &guidValue) != RPC_S_OK)
        {
            ::UuidFromStringW((RPC_WSTR)L"00000000-0000-0000-0000-000000000000", &guidValue); // default value
            success = false;
        }

        generateGUIDValueString();
        return success;
    }
};

class KPSObjectView : public KPSObject
{
public:
    wchar_t* objectIDStr; 

    KPSObjectView() : KPSObject()
    {
        objectIDStr = NULL;
    }

    void generateObjectID()
    {
        ::CoCreateGuid(&objectID);
    }

    void generateObjectIDString()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);

        objectIDStr = nullptr;

        ::UuidToStringW(&objectID, (RPC_WSTR*)&objectIDStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool generateIDByString(const wchar_t* text)
    {
        if (::UuidFromStringW((RPC_WSTR)text, &objectID) != RPC_S_OK)
            return false;

        generateObjectIDString();
        return true;
    }

    virtual ~KPSObjectView()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);
    }
};

class KPSReader
{
protected:

public:
    KPointerList<KPSObject*,16, false>* psObjectList;

    KPSReader()
    {
        psObjectList = NULL;
    }

    // do not free returned object.
    KPSObject* getPSObject(const GUID& objectID)
    {
        for (int i = 0; i < psObjectList->size(); i++)
        {
            KPSObject* psObject = psObjectList->get(i);
            if (psObject->compare(objectID))
                return psObject;
        }

        return NULL;
    }

    bool loadFromFile(const wchar_t* path, bool readNames = true)
    {
        if (psObjectList)
        {
            psObjectList->deleteAll();
            delete psObjectList;
            psObjectList = NULL;
        }

        HANDLE fileHandle = ::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fileHandle == INVALID_HANDLE_VALUE)
            return false;

        DWORD fileSize = ::GetFileSize(fileHandle, NULL);
        fileSize = (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;

        if (fileSize == 0)
        {
            ::CloseHandle(fileHandle);
            return false;
        }

        char fileHeader[4];
        DWORD bytesRead;
        ::ReadFile(fileHandle, fileHeader, 4, &bytesRead, NULL);

        char psFileHeader[4] = PS_V1_HEADER;

        for (int i = 0; i < 4; ++i)
        {
            if (psFileHeader[i] != fileHeader[i])
            {
                ::CloseHandle(fileHandle);
                return false;
            }
        }

        unsigned int objectCount = 0;
        ::ReadFile(fileHandle, &objectCount, sizeof(unsigned int), &bytesRead, NULL);

        if (objectCount == 0)
        {
            ::CloseHandle(fileHandle);
            return false;
        }

        psObjectList = new KPointerList<KPSObject*, 16, false>();

        for (unsigned int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
        {
            GUID objectID;
            ::ReadFile(fileHandle, &objectID, sizeof(GUID), &bytesRead, NULL);

            int nameLength;
            ::ReadFile(fileHandle, &nameLength, sizeof(int), &bytesRead, NULL);

            wchar_t* objectName = NULL;
            if (readNames)
            {
                objectName = (wchar_t*)::malloc(sizeof(wchar_t) * (nameLength + 1));
                ::ReadFile(fileHandle, objectName, sizeof(wchar_t) * nameLength, &bytesRead, NULL);
                objectName[nameLength] = 0;
            }
            else // ignore name
            {
                ::SetFilePointer(fileHandle, nameLength * sizeof(wchar_t), 0, FILE_CURRENT); 
            }

            unsigned int propertyCount;
            ::ReadFile(fileHandle, &propertyCount, sizeof(unsigned int), &bytesRead, NULL);


            if (propertyCount == 0) // ignore the objects which doesn't have properties.
            {
                if (objectName)
                    ::free(objectName);

                continue;
            }

            KPSObject* psObject = new KPSObject();
            psObject->objectID = objectID;
            psObject->nameLength = nameLength;
            psObject->name = objectName;

            for (unsigned int propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex)
            {
                KPSProperty* psProperty = new KPSProperty();

                ::ReadFile(fileHandle, &psProperty->nameLength, sizeof(int), &bytesRead, NULL);

                if (readNames)
                {
                    psProperty->name = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->nameLength + 1));
                    ::ReadFile(fileHandle, psProperty->name, sizeof(wchar_t) * psProperty->nameLength, &bytesRead, NULL);
                    psProperty->name[psProperty->nameLength] = 0;
                }
                else // ignore name
                {
                    ::SetFilePointer(fileHandle, psProperty->nameLength * sizeof(wchar_t), 0, FILE_CURRENT);
                }

                ::ReadFile(fileHandle, &psProperty->type, sizeof(int), &bytesRead, NULL);

                if (psProperty->type == KPSPropertyTypes::STRING) // string
                {
                    ::ReadFile(fileHandle, &psProperty->strValueLength, sizeof(int), &bytesRead, NULL);
                    psProperty->strValue = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->strValueLength + 1));
                    ::ReadFile(fileHandle, psProperty->strValue, sizeof(wchar_t) * psProperty->strValueLength, &bytesRead, NULL);
                    psProperty->strValue[psProperty->strValueLength] = 0;
                }
                else if (psProperty->type == KPSPropertyTypes::INTEGER) // int
                {
                    ::ReadFile(fileHandle, &psProperty->intValue, sizeof(int), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::DWORD) // DWORD
                {
                    ::ReadFile(fileHandle, &psProperty->dwordValue, sizeof(DWORD), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::FLOAT) // float
                {
                    ::ReadFile(fileHandle, &psProperty->floatValue, sizeof(float), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::INT_ARRAY) // int array
                {
                    ::ReadFile(fileHandle, &psProperty->intArraySize, sizeof(int), &bytesRead, NULL);
                    if (psProperty->intArraySize)
                    {
                        psProperty->intArray = (int*)::malloc(sizeof(int) * psProperty->intArraySize);
                        ::ReadFile(fileHandle, psProperty->intArray, sizeof(int) * psProperty->intArraySize, &bytesRead, NULL);
                    }
                }
                else if (psProperty->type == KPSPropertyTypes::GUID) // guid
                {
                    ::ReadFile(fileHandle, &psProperty->guidValue, sizeof(GUID), &bytesRead, NULL);
                }
                else // file
                {
                    ::ReadFile(fileHandle, &psProperty->fileNameLength, sizeof(int), &bytesRead, NULL);
                    psProperty->fileName = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->fileNameLength + 1));
                    ::ReadFile(fileHandle, psProperty->fileName, sizeof(wchar_t) * psProperty->fileNameLength, &bytesRead, NULL);
                    psProperty->fileName[psProperty->fileNameLength] = 0;
                    ::ReadFile(fileHandle, &psProperty->fileDataSize, sizeof(DWORD), &bytesRead, NULL);
                    if (psProperty->fileDataSize)
                    {
                        psProperty->fileData = (unsigned char*)::malloc(psProperty->fileDataSize);
                        ::ReadFile(fileHandle, psProperty->fileData, psProperty->fileDataSize, &bytesRead, NULL);
                    }
                }

                psObject->propertyList.add(psProperty);
            }

            psObjectList->add(psObject);
        }

        ::CloseHandle(fileHandle);
        return true;
    }

    virtual ~KPSReader() 
    {
        if (psObjectList)
        {
            psObjectList->deleteAll();
            delete psObjectList;
        }
    }
};

