
/*
	RFC - KFile.cpp
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

#include "KFile.h"

KFile::KFile()
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::OpenFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool KFile::CloseFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::GetFileHandle()
{
	return fileHandle;
}

KFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD KFile::ReadFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::WriteFile(void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::SetFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool KFile::SetFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::GetFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool KFile::SetFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::GetFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* KFile::ReadAsData()
{
	const DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool KFile::WriteString(const KString& text, bool isUnicode)
{
	if (isUnicode)
	{
		void* buffer = (void*)(const wchar_t*)text;
		const DWORD numberOfBytesToWrite = text.GetLength() * sizeof(wchar_t);
		const DWORD numberOfBytesWritten = this->WriteFile(buffer, numberOfBytesToWrite);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
	else
	{
		void* buffer = (void*)KString::ToAnsiString(text);
		const DWORD numberOfBytesToWrite = text.GetLength() * sizeof(char);
		const DWORD numberOfBytesWritten = this->WriteFile(buffer, numberOfBytesToWrite);
		::free(buffer);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
}

KString KFile::ReadAsString(bool isUnicode)
{
	DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (isUnicode)
			{
				return KString((const wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}
			else
			{
				KString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return KString();
}

bool KFile::DeleteFile(const KString& fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool KFile::CopyFile(const KString& sourceFileName, const KString& destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

KString KFile::GetFileNameFromPath(const KString& path)
{
	const wchar_t* pathStr = (const wchar_t*)path;
	const wchar_t* fileNamePtr = ::PathFindFileNameW(pathStr);

	if (pathStr != fileNamePtr)
		return KString(fileNamePtr);

	return KString();
}

bool KFile::IsFileExists(const KString& fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

KFile::~KFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}