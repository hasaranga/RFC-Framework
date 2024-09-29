
/*
	Copyright (C) 2013-2024  CrownSoft

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

#include "Architecture.h"
#include <windows.h>
#include "KString.h"
#include "KApplication.h"
#include "KDPIUtility.h"
#include "KModuleManager.h"

void InitRFCModules();
void DeInitRFCModules();

// use within a dll functions. do not use inside of DllMain.
void RFCDllInit();
void RFCDllFree();

#define START_RFC_CONSOLE_APP(AppClass) \
int main() \
{ \
	CoreModuleInitParams::hInstance = 0; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE; \
	AppClass* application = new AppClass(); \
	application->ModifyModuleInitParams(); \
	::InitRFCModules(); \
	int argc = 0; \
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc); \
	KString **str_argv = (KString**)::malloc(argc * RFC_PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new KString(args[i], KString::STATIC_TEXT_DO_NOT_FREE);} \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(str_argv, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->Main(str_argv, argc); \
		}else{ \
			retVal = application->AnotherInstanceIsRunning(str_argv, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	for(int i = 0; i < argc; i++){delete str_argv[i];} \
	::DeInitRFCModules(); \
	::free((void*)str_argv); \
	::GlobalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_CONSOLE_APP_NO_CMD_ARGS(AppClass) \
int WINAPI main() \
{ \
	CoreModuleInitParams::hInstance = 0; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE; \
	AppClass* application = new AppClass(); \
	application->ModifyModuleInitParams(); \
	::InitRFCModules(); \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->Main(0, 0); \
		}else{ \
			retVal = application->AnotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	::DeInitRFCModules(); \
	return retVal; \
}

#define START_RFC_APPLICATION(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	AppClass* application = new AppClass(); \
	application->ModifyModuleInitParams(); \
	::InitRFCModules(); \
	int argc = 0; \
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc); \
	KString **str_argv = (KString**)::malloc(argc * RFC_PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new KString(args[i], KString::STATIC_TEXT_DO_NOT_FREE);} \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(str_argv, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->Main(str_argv, argc); \
		}else{ \
			retVal = application->AnotherInstanceIsRunning(str_argv, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	for(int i = 0; i < argc; i++){delete str_argv[i];} \
	::DeInitRFCModules(); \
	::free((void*)str_argv); \
	::GlobalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	AppClass* application = new AppClass(); \
	application->ModifyModuleInitParams(); \
	::InitRFCModules(); \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->Main(0, 0); \
		}else{ \
			retVal = application->AnotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	::DeInitRFCModules(); \
	return retVal; \
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef STYLE_MANIFEST_DEFINED
		#ifndef RFC_NO_MANIFEST
			#define STYLE_MANIFEST_DEFINED
			#ifdef RFC64
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#else
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#endif
		#endif
	#endif
#endif


#ifdef _DEBUG
	#define DEBUG_PRINT(x) OutputDebugStringA(x);
#else 
	#define DEBUG_PRINT(x) 
#endif