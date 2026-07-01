
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

class KBufferReadStream : public KStream
{
protected:
	BYTE* bufferPtr;
	DWORD bufferSize;
	DWORD bufferPos; // in bytes
public:
	KBufferReadStream(BYTE* buffer, DWORD szBuffer) noexcept : bufferPtr(buffer),
		bufferSize(szBuffer), bufferPos(0){}

	virtual ~KBufferReadStream() noexcept = default;


	// ============= KStream ===============

	bool readStream(BYTE* buffer, DWORD bytesToRead) noexcept override
	{
		if (!buffer || bytesToRead == 0)
			return false;

		// Check if we have enough data remaining in the buffer
		if (bufferPos + bytesToRead > bufferSize)
			return false;

		// Copy data from internal buffer to output buffer
		::memcpy(buffer, bufferPtr + bufferPos, bytesToRead);

		// Update buffer position
		bufferPos += bytesToRead;

		return true;
	}

	bool writeStream(const BYTE* buffer, DWORD bytesToWrite) noexcept override
	{
		return false;
	}
};

class KBufferWriteStream : public KStream
{
protected:
	BYTE* bufferPtr;
	DWORD bufferSize;
public:
	KBufferWriteStream() noexcept : bufferPtr(nullptr), bufferSize(0) {}

	virtual ~KBufferWriteStream() noexcept
	{
		if (bufferPtr)
			::free(bufferPtr);
	}

	const BYTE* data() const noexcept
	{
		return bufferPtr;
	}

	operator const BYTE* () const noexcept
	{
		return bufferPtr;
	}

	DWORD dataSize() const noexcept
	{
		return bufferSize;
	}

	// ============= KStream ===============

	bool readStream(BYTE* buffer, DWORD bytesToRead) noexcept override
	{
		return false;
	}

	bool writeStream(const BYTE* buffer, DWORD bytesToWrite) noexcept override
	{
		if (!buffer || bytesToWrite == 0)
			return false;

		if (bufferPtr)
		{
			const DWORD newBufferSize = bufferSize + bytesToWrite;
			bufferPtr = (BYTE*)realloc(bufferPtr, newBufferSize);
			::memcpy(bufferPtr + bufferSize, buffer, bytesToWrite);
			bufferSize = newBufferSize;
		}
		else
		{
			bufferSize = bytesToWrite;
			bufferPtr = (BYTE*)malloc(bufferSize);
			::memcpy(bufferPtr, buffer, bufferSize);
		}

		return true;
	}
};
