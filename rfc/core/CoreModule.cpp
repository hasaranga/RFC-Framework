
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

#include "CoreModule.h"

HINSTANCE CoreModuleInitParams::hInstance = 0;
bool CoreModuleInitParams::initCOMAsSTA = true;
KDPIAwareness CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE;

class RFC_CoreModule {
public:
	static bool RFCModuleInit()
	{
		if (!CoreModuleInitParams::hInstance)
		{
			//hInstance = ::GetModuleHandleW(NULL); // not work for dll
			::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&InitRFC, &CoreModuleInitParams::hInstance);
		}

		KApplication::hInstance = CoreModuleInitParams::hInstance;
		KApplication::dpiAwareness = CoreModuleInitParams::dpiAwareness;

		if (KApplication::dpiAwareness != KDPIAwareness::UNAWARE_MODE)
		{
			KDPIUtility::InitDPIFunctions();
			KDPIUtility::MakeProcessDPIAware(KApplication::dpiAwareness);
		}

		if (CoreModuleInitParams::initCOMAsSTA)
			::CoInitialize(NULL); //Initializes COM as STA.

		return true;
	}

	static void RFCModuleFree()
	{
		if (CoreModuleInitParams::initCOMAsSTA)
			::CoUninitialize();
	}
};

REGISTER_RFC_MODULE(0, RFC_CoreModule)