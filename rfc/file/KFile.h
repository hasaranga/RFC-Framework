
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
#include "KStream.h"
#include <shlwapi.h>

// macro to specify file format type in the first 4 bytes of file.
// use with KSettingsReader/Writer classes.
#define KFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

/**
	Can be use to read/write data from a file easily.
*/
class KFile : public KStream
{
protected:
	HANDLE fileHandle;
	bool autoCloseHandle;
	DWORD desiredAccess;

public:
	KFile() noexcept;

	/** 
		Used in file opening, to specify whether to open as read or write or both.
	*/
	enum FileAccessTypes
	{
		KREAD = GENERIC_READ,
		KWRITE = GENERIC_WRITE,
		KBOTH = GENERIC_READ | GENERIC_WRITE,
	};

	/** 
		If the file does not exist, it will be created.
	*/
	KFile(const wchar_t* fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true) noexcept;

	/** 
		If the file does not exist, it will be created.
	*/
	bool openFile(const wchar_t* fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true) noexcept;

	bool closeFile() noexcept;

	HANDLE getFileHandle() noexcept;

	operator HANDLE()const noexcept;

	/** 
		fills given buffer and returns number of bytes read.
	*/
	DWORD readFile(void* buffer, DWORD numberOfBytesToRead) noexcept;

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use getFileSize method. return value will be null on read error.
	*/
	void* readAsData() noexcept;

	KString readAsString(bool isUnicode = true) noexcept;

	/**
		returns number of bytes written.
	*/
	DWORD writeFile(const void* buffer, DWORD numberOfBytesToWrite) noexcept;

	bool writeString(const KString& text, bool isUnicode = true) noexcept;

	bool setFilePointerToStart() noexcept;

	/**
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	bool setFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN) noexcept;

	DWORD getFilePointerPosition() noexcept;

	bool setFilePointerToEnd() noexcept;

	/**
		returns zero on error
	*/
	DWORD getFileSize() noexcept;

	static bool deleteFile(const wchar_t* fileName) noexcept;

	static bool isFileExists(const wchar_t* fileName) noexcept;

	static bool copyFile(const wchar_t* sourceFileName, const wchar_t* destFileName) noexcept;

	/**
		returns the file name part of the path.
	*/
	static KString getFileNameFromPath(const wchar_t* path, bool withExtension = true) noexcept;

	static KString getFileExtension(const wchar_t* path) noexcept;

	~KFile() noexcept;

	// ============= KStream ===============
	bool readStream(BYTE* buffer, DWORD bytesToRead) noexcept override;
	bool writeStream(const BYTE* buffer, DWORD bytesToWrite) noexcept override;

private:
	RFC_LEAK_DETECTOR(KFile)
};


