
/*
	Copyright (C) 2013-2022 CrownSoft

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

#include "Core.h"

void InitRFC()
{
	// initialize modules
	RFCModuleInitFunc* initFuncList = KModuleManager::RFCModuleInitFuncList();
	for (int i = 0; i < MAX_RFC_MODULE_COUNT; ++i)
	{
		if (initFuncList[i])
		{
			if (!initFuncList[i]())
			{
				char strIndex[10];
				::_itoa_s(i, strIndex, 10);
				char msg[64];
				::strcpy_s(msg, "RFC Module Initialization error!\n\nModule index: ");
				::strcat_s(msg, strIndex);
				::MessageBoxA(0, msg, "RFC - Error", MB_ICONERROR);
				::ExitProcess(1);
			}
		}
	}
}

void DeInitRFC()
{
	// free modules
	RFCModuleFreeFunc* freeFuncList = KModuleManager::RFCModuleFreeFuncList();
	for (int i = (MAX_RFC_MODULE_COUNT-1); i >= 0; --i)
	{
		if (freeFuncList[i])
			freeFuncList[i]();
	}
}