
/*
	RFC - KFile.cpp
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

#include "KFile.h"

KFile::KFile()
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const wchar_t* fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::openFile(const wchar_t* fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool KFile::closeFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::getFileHandle()
{
	return fileHandle;
}

KFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD KFile::readFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::writeFile(const void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::setFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool KFile::setFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::getFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool KFile::setFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::getFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* KFile::readAsData()
{
	const DWORD fileSize = getFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool KFile::writeString(const KString& text, bool isUnicode)
{
	if (isUnicode)
	{
		void* buffer = (void*)(const wchar_t*)text;
		const DWORD numberOfBytesToWrite = text.length() * sizeof(wchar_t);
		const DWORD numberOfBytesWritten = writeFile(buffer, numberOfBytesToWrite);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
	else
	{
		void* buffer = (void*)KString::toAnsiString(text);
		const DWORD numberOfBytesToWrite = text.length() * sizeof(char);
		const DWORD numberOfBytesWritten = writeFile(buffer, numberOfBytesToWrite);
		::free(buffer);
		return (numberOfBytesWritten == numberOfBytesToWrite);
	}
}

KString KFile::readAsString(bool isUnicode)
{
	DWORD fileSize = getFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (isUnicode)
			{
				return KString((const wchar_t*)buffer, KStringBehaviour::FREE_ON_DESTROY);
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

bool KFile::deleteFile(const wchar_t* fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool KFile::copyFile(const wchar_t* sourceFileName, const wchar_t* destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

KString KFile::getFileNameFromPath(const wchar_t* path)
{
	const wchar_t* fileNamePtr = ::PathFindFileNameW(path);

	if (path != fileNamePtr)
		return KString(fileNamePtr, KStringBehaviour::MAKE_A_COPY);

	return KString();
}

KString KFile::getFileExtension(const wchar_t* path)
{
	const wchar_t* extPtr = ::PathFindExtensionW(path);
	return KString(extPtr, KStringBehaviour::MAKE_A_COPY);
}

bool KFile::isFileExists(const wchar_t* fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

KFile::~KFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}