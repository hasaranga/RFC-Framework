
/*
	RFC Framework v0.2.6
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

#ifndef _RFC_H_
#define _RFC_H_

#include <windows.h>
#include <commctrl.h>

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
	#include <assert.h>
#endif

#ifdef _MSC_VER
	#pragma comment(lib, "Comctl32.lib")
#endif

#include "config.h"

#include "utils/KPlatformUtil.h"
#include "utils/KPerformanceCounter.h"
#include "utils/KRegistry.h"
#include "KApplication.h"
#include "text/KString.h"
#include "graphics/KFont.h"
#include "graphics/KIcon.h"
#include "graphics/KBitmap.h"
#include "graphics/KCursor.h"
#include "graphics/KGraphics.h"
#include "gui/KComponent.h"
#include "gui/KWindow.h"
#include "gui/KWindowTypes.h"
#include "gui/KButtonListener.h"
#include "gui/KButton.h"
#include "gui/KLabel.h"
#include "gui/KCheckBox.h"
#include "gui/KRadioButton.h"
#include "gui/KPushButton.h"
#include "gui/KGroupBox.h"
#include "gui/KProgressBar.h"
#include "gui/KMenuItemListener.h"
#include "gui/KMenuItem.h"
#include "gui/KMenu.h"
#include "gui/KMenuBar.h"
#include "gui/KTextBox.h"
#include "gui/KPasswordBox.h"
#include "gui/KTextArea.h"
#include "gui/KListBox.h"
#include "gui/KListBoxListener.h"
#include "gui/KComboBox.h"
#include "gui/KComboBoxListener.h"
#include "gui/KTrackBar.h"
#include "gui/KTrackBarListener.h"
#include "gui/KCommonDialogBox.h"
#include "gui/KNumericField.h"
#include "gui/KGridViewListener.h"
#include "gui/KGridView.h"
#include "gui/KGlyphButton.h"
#include "gui/KMenuButton.h"
#include "gui/KToolTip.h"
#include "threads/KThread.h"
#include "threads/KRunnable.h"
#include "threads/KTimer.h"
#include "threads/KTimerListener.h"
#include "io/KFile.h"
#include "io/KDirectory.h"
#include "io/KSettingsWriter.h"
#include "io/KSettingsReader.h"
#include "io/KLogger.h"
#include "security/KMD5.h"
#include "security/KSHA1.h"
#include "containers/KPointerList.h"
#include "containers/KScopedClassPointer.h"
#include "containers/KScopedMemoryBlock.h"
#include "containers/KScopedCriticalSection.h"
#include "containers/KLeakDetector.h"
#include "containers/KScopedGdiObject.h"

RFC_API LRESULT CALLBACK GlobalWnd_Proc(HWND,UINT,WPARAM,LPARAM);
RFC_API INT_PTR CALLBACK GlobalDlg_Proc(HWND, UINT, WPARAM, LPARAM);
RFC_API DWORD WINAPI GlobalThread_Proc(LPVOID);

/**
	set requireInitialMessages to true to receive initial messages lke WM_CREATE... (installs a hook)
	define "RFC_SINGLE_THREAD_COMP_CREATION" if your app does not create components within multiple threads.
*/
RFC_API HWND CreateRFCComponent(KComponent* component, bool requireInitialMessages);
RFC_API bool CreateRFCThread(KThread* thread);

RFC_API void DoMessagePump(bool handleTabKey = true);

/**
	Important: hInstance is current module HINSTANCE.
	If you are in EXE, then hInstance is HINSTANCE provided by WinMain.
	If you are in DLL, then hInstance is HINSTANCE provided by DllMain or HMODULE of the DLL.
	If you are in Console app, then pass zero.
*/
RFC_API void InitRFC(HINSTANCE hInstance);
RFC_API void DeInitRFC();

/** 
	hwnd can be window, custom control, dialog or common control.
	hwnd will be subclassed if it common control or dialog.
*/
RFC_API void AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component);

RFC_API int HotPlugAndRunDialogBox(WORD resourceID,HWND parentHwnd,KComponent* component);
RFC_API HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);

#define START_RFC_APPLICATION(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitRFC(hInstance);\
	int argc = 0;\
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc);\
	KString **str_argv = (KString**)::malloc(argc * PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new KString(args[i], KString::STATIC_TEXT_DO_NOT_FREE);}\
	AppClass* application = new AppClass();\
	int retVal = application->Main(str_argv, argc);\
	delete application;\
	for(int i = 0; i < argc; i++){delete str_argv[i];}\
	::DeInitRFC();\
	::free((void*)str_argv);\
	::GlobalFree(args);\
	return retVal;\
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitRFC(hInstance);\
	AppClass* application = new AppClass();\
	int retVal = application->Main(0, 0);\
	delete application;\
	::DeInitRFC();\
	return retVal;\
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef RFC_DLL
		#ifndef RFC_NO_MANIFEST
			#ifdef RFC64
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#else
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#endif
		#endif
	#endif
#endif

#define BEGIN_KMSG_HANDLER \
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
	{\
	switch(msg)\
	{

#define ON_KMSG(_KMsg,_KMsgHandler) \
	case _KMsg: return _KMsgHandler(wParam,lParam);

#define END_KMSG_HANDLER(_KComponentParentClass) \
	default: return _KComponentParentClass::WindowProc(hwnd,msg,wParam,lParam); \
	}\
	}

#ifndef RFC_NO_UNICODE_STR
	#define T(stringLiteral) KString(L##stringLiteral)
#else
	#define T(stringLiteral) KString(stringLiteral)
#endif

#define KFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

// generates filter text for KFILE_FILTER("Text Files", "txt") as follows: L"Text Files\0*.txt\0"
#define KFILE_FILTER(desc, ext) L##desc L"\0*." L##ext L"\0"

class RFC_API InternalDefinitions
{
public:
	static ATOM RFCPropAtom_Component;
	static ATOM RFCPropAtom_OldProc;
};

#ifdef _DEBUG
	#ifdef _MSC_VER
		#define RFC_INIT_VERIFIER _ASSERT_EXPR((KApplication::hInstance != 0), L"##### RFC Framework used before being initialized! Did you forget to call the InitRFC function? Or did you declared RFC class as a global variable? #####")
	#else
		#define RFC_INIT_VERIFIER assert((KApplication::hInstance != 0) && "##### RFC Framework used before being initialized! Did you forget to call the InitRFC function? Or did you declared RFC class as a global variable? #####")
	#endif
#else
	#define RFC_INIT_VERIFIER
#endif

#endif