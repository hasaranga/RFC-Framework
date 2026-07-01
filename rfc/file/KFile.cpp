
/*
	RFC - KFile.cpp
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

#include "KFile.h"

KFile::KFile() noexcept
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const wchar_t* fileName, DWORD desiredAccess, bool autoCloseHandle) noexcept
{
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::openFile(const wchar_t* fileName, DWORD desiredAccess, bool autoCloseHandle) noexcept
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return fileHandle == INVALID_HANDLE_VALUE ? false : true;
}

bool KFile::closeFile() noexcept
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::getFileHandle() noexcept
{
	return fileHandle;
}

KFile::operator HANDLE()const noexcept
{
	return fileHandle;
}

DWORD KFile::readFile(void* buffer, DWORD numberOfBytesToRead) noexcept
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::writeFile(const void* buffer, DWORD numberOfBytesToWrite) noexcept
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::setFilePointerToStart() noexcept
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER ? false : true;
}

bool KFile::setFilePointerTo(long distance, DWORD startingPoint) noexcept
{
	return ::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER ? false : true;
}

DWORD KFile::getFilePointerPosition() noexcept
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool KFile::setFilePointerToEnd() noexcept
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER ? false : true;
}

DWORD KFile::getFileSize() noexcept
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return fileSize == INVALID_FILE_SIZE ? 0 : fileSize;
}

void* KFile::readAsData() noexcept
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

bool KFile::writeString(const KString& text, bool isUnicode) noexcept
{
	if (isUnicode)
	{
		void* buffer = (void*)(const wchar_t*)text;
		const DWORD numberOfBytesToWrite = text.length() * sizeof(wchar_t);
		const DWORD numberOfBytesWritten = writeFile(buffer, numberOfBytesToWrite);
		return numberOfBytesWritten == numberOfBytesToWrite;
	}
	else
	{
		void* buffer = (void*)KString::toUTF8String(text);
		const DWORD numberOfBytesToWrite = text.length() * sizeof(char);
		const DWORD numberOfBytesWritten = writeFile(buffer, numberOfBytesToWrite);
		::free(buffer);
		return numberOfBytesWritten == numberOfBytesToWrite;
	}
}

KString KFile::readAsString(bool isUnicode) noexcept
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

bool KFile::deleteFile(const wchar_t* fileName) noexcept
{
	return ::DeleteFileW(fileName) == 0 ? false : true;
}

bool KFile::copyFile(const wchar_t* sourceFileName, const wchar_t* destFileName) noexcept
{
	return ::CopyFileW(sourceFileName, destFileName, FALSE) == 0 ? false : true;
}

KString KFile::getFileNameFromPath(const wchar_t* path, bool withExtension) noexcept
{
	const wchar_t* fileNamePtr = ::PathFindFileNameW(path);

	if (path != fileNamePtr)
	{
		if (!withExtension)
		{
			// Find the last dot in the filename
			const wchar_t* dotPtr = ::wcsrchr(fileNamePtr, L'.');
			if (dotPtr != nullptr && dotPtr != fileNamePtr)
			{
				// Calculate length without extension
				const size_t lengthWithoutExt = dotPtr - fileNamePtr;

				wchar_t* strBuffer = (wchar_t*)::malloc(sizeof(wchar_t) * (lengthWithoutExt + 1));
				::wcsncpy_s(strBuffer, lengthWithoutExt + 1, fileNamePtr, lengthWithoutExt);
				strBuffer[lengthWithoutExt] = L'\0';

				return KString(strBuffer, KStringBehaviour::FREE_ON_DESTROY, (int)lengthWithoutExt);
			}
		}
		return KString(fileNamePtr, KStringBehaviour::MAKE_A_COPY);
	}
	return KString();
}

KString KFile::getFileExtension(const wchar_t* path) noexcept
{
	const wchar_t* extPtr = ::PathFindExtensionW(path);
	return KString(extPtr, KStringBehaviour::MAKE_A_COPY);
}

bool KFile::isFileExists(const wchar_t* fileName) noexcept
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);
	return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

KFile::~KFile() noexcept
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}

bool KFile::readStream(BYTE* buffer, DWORD bytesToRead) noexcept
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, bytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead == bytesToRead;
}

bool KFile::writeStream(const BYTE* buffer, DWORD bytesToWrite) noexcept
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, bytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten == bytesToWrite;
}