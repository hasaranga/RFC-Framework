
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

#include "KLogger.h"
#include "KFile.h"

KLogger::KLogger(DWORD bufferSize)
{
	buffer = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;
	bufferIndex = 0;
	totalEvents = 0;
	totalMills = 0;
	bufferFull = false;
	isFirstCall = true;
}

bool KLogger::writeNewEvent(unsigned char eventType)
{
	if (!bufferFull)
	{
		if ((bufferIndex + 300) >= bufferSize) // assume each event data is not greater than 300 bytes
		{
			bufferFull = true;
			return false;
		}

		unsigned short secs = 0;
		unsigned short mills = 0;

		if (isFirstCall)
		{
			pCounter.startCounter();
			isFirstCall = false;
			totalMills = 0;
		}
		else{
			const double deltaMills = pCounter.endCounter();
			totalMills += (unsigned int)deltaMills;

			secs = (unsigned short)(totalMills/1000);
			mills = (unsigned short)(totalMills % 1000);

			pCounter.startCounter();
		}

		buffer[bufferIndex] = eventType; // write event type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = secs; // write secs
		bufferIndex += sizeof(unsigned short);

		*((unsigned short*)&buffer[bufferIndex]) = mills; // write mills
		bufferIndex += sizeof(unsigned short);

		totalEvents++;

		return true;
	}
	return false;
}

bool KLogger::endEvent()
{
	if (!bufferFull)
	{
		buffer[bufferIndex] = EVT_END; // write event end
		bufferIndex += sizeof(unsigned char);

		return true;
	}
	return false;
}

bool KLogger::addTextParam(const char *text, unsigned char textLength)
{
	if( (textLength < 255) && (!bufferFull) )
	{
		buffer[bufferIndex] = PARAM_STRING; // write param type
		bufferIndex += sizeof(unsigned char);

		buffer[bufferIndex] = textLength; // write data size
		bufferIndex += sizeof(unsigned char);

		for (int i = 0; i < textLength; i++) // write data
		{
			buffer[bufferIndex] = text[i];
			bufferIndex += sizeof(unsigned char);
		}

		return true;
	}
	return false;
}

bool KLogger::addIntParam(int value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_INT32; // write param type
		bufferIndex += sizeof(unsigned char);

		*((int*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(int);

		return true;
	}
	return false;
}

bool KLogger::addShortParam(unsigned short value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_SHORT16; // write param type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(unsigned short);

		return true;
	}
	return false;
}

bool KLogger::addFloatParam(float value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_FLOAT; // write param type
		bufferIndex += sizeof(unsigned char);

		*((float*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(float);

		return true;
	}
	return false;
}
	
bool KLogger::addDoubleParam(double value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_DOUBLE; // write param type
		bufferIndex += sizeof(unsigned char);

		*((double*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(double);

		return true;
	}
	return false;
}

bool KLogger::isBufferFull()
{
	return bufferFull;
}

bool KLogger::writeToFile(const KString &filePath)
{
	KFile file;

	if (KFile::isFileExists(filePath))
		KFile::deleteFile(filePath);

	if (file.openFile(filePath, KFile::KWRITE))
	{
		file.writeFile((void*)"RLOG", 4);
		file.writeFile(&totalEvents, 4);
		file.writeFile(buffer, bufferIndex);

		return true;
	}

	return false;
}

KLogger::~KLogger()
{
	free(buffer);
}