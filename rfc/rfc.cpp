
/*
    RFC - rfc.cpp
    Copyright (C) 2013-2017 CrownSoft
  
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

#include"rfc.h"

class InternalVariables
{
public:
	static KComponent *currentComponent;
	static HHOOK wnd_hook;
	static CRITICAL_SECTION g_csComponent; // guard currentComponent!

	static const wchar_t* RFCPropText_Object;
	static const wchar_t* RFCPropText_OldProc;
	static const wchar_t* RFCPropText_ClsName;

	static volatile int rfcRefCount;
};

KComponent* InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
const wchar_t* InternalVariables::RFCPropText_Object = L"RFC";
const wchar_t* InternalVariables::RFCPropText_OldProc = L"RFCOldProc";
const wchar_t* InternalVariables::RFCPropText_ClsName = L"RFCClsName";
volatile int InternalVariables::rfcRefCount = 0;
CRITICAL_SECTION InternalVariables::g_csComponent;


LRESULT CALLBACK RFCCTL_CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode < 0)
		return CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

	if(nCode==HCBT_CREATEWND){
		HWND hwnd=(HWND)wParam;
		SetPropW(hwnd, InternalVariables::RFCPropText_Object, (HANDLE)InternalVariables::currentComponent);
		FARPROC lpfnOldWndProc = (FARPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
		SetPropW(hwnd, InternalVariables::RFCPropText_OldProc, (HANDLE)lpfnOldWndProc);
		SetPropW(hwnd, InternalVariables::RFCPropText_ClsName, (HANDLE)(const wchar_t*)InternalVariables::currentComponent->GetComponentClassName());

		SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)GlobalWnd_Proc); // subclassing...
	}

	// Call the next hook, if there is one
	return CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK GlobalWnd_Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	KComponent *component = (KComponent*)GetPropW(hwnd, InternalVariables::RFCPropText_Object);

	if(!component){ // just for safe!
		return DefWindowProcW( hwnd, msg, wParam, lParam );
	}

	if(!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if(msg==WM_NCDESTROY){
		RemovePropW(hwnd, InternalVariables::RFCPropText_Object);
		RemovePropW(hwnd, InternalVariables::RFCPropText_ClsName);

		FARPROC lpfnOldWndProc = (FARPROC)GetPropW(hwnd, InternalVariables::RFCPropText_OldProc);
		if (lpfnOldWndProc)
		{
			RemovePropW(hwnd, InternalVariables::RFCPropText_OldProc);
			SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
			return CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
		}
	}

	return component->WindowProc(hwnd,msg,wParam,lParam);
}

HWND CreateRFCComponent(KComponent* component)
{
	// we make thread safe this function!

	EnterCriticalSection(&InternalVariables::g_csComponent);

	InternalVariables::currentComponent = component;

	// install hook to receive WM_CREATE_WINDOW msg!
	InternalVariables::wnd_hook = SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, GetCurrentThreadId());

	HWND hwnd=CreateWindowExW(component->GetExStyle(),(const wchar_t*)component->GetComponentClassName(),(const wchar_t*)component->GetText(),component->GetStyle(),component->GetX(),component->GetY(),component->GetWidth(),component->GetHeight(),component->GetParentHWND(),0,KPlatformUtil::GetInstance()->GetAppHInstance(),0);

	UnhookWindowsHookEx(InternalVariables::wnd_hook);

	LeaveCriticalSection(&InternalVariables::g_csComponent);

	return hwnd;
}

void DoMessagePump(bool handleTabKey)
{
	MSG msg;
	
	while (GetMessageW(&msg, NULL, 0, 0)) 
	{
		if(handleTabKey)
		{
			if(msg.message==WM_KEYDOWN)
			{
				if(VK_TAB==msg.wParam) // looking for TAB key!
				{
					if(msg.hwnd)
					{
						HWND parentHWND=GetParent(msg.hwnd);
						if(!parentHWND) // nothing selected! (top-level window)
						{
							HWND nextControl=GetNextDlgTabItem(msg.hwnd,NULL,FALSE);
							if(nextControl)
							{
								SetFocus(nextControl);
								continue; // don't pass this message!
							}
						}else // user has already selected component!
						{
							HWND nextControl=GetNextDlgTabItem(parentHWND,msg.hwnd,FALSE);
							if(nextControl)
							{
								if((GetKeyState(VK_CONTROL) & 0x8000)==0) // user is not hold ctrl key!
								{
									SetFocus(nextControl);
									continue; // don't pass this message!
								}
							}
						}
					}
				}
			}
		}
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	} 
}

DWORD WINAPI GlobalThread_Proc(LPVOID lpParameter)
{
	if(lpParameter==0) // for safe!
		return 0;

	KThread* thread=(KThread*)lpParameter;
	thread->SetHandle(GetCurrentThread());
	thread->Run();

	return 0;	
}

bool CreateRFCThread(KThread* thread)
{
	if(thread)
	{
		HANDLE handle=CreateThread(NULL,0,GlobalThread_Proc,thread,0,NULL);
		if(handle)
			return true;
	}
	return false;
}

int HotPlugAndRunDialogBox(WORD resourceID,HWND parentHwnd,KComponent* component)
{
	return (int)DialogBoxParamW(KPlatformUtil::GetInstance()->GetAppHInstance(),MAKEINTRESOURCEW(resourceID),parentHwnd,GlobalDlg_Proc,(LPARAM)component);
}

HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return CreateDialogParamW(KPlatformUtil::GetInstance()->GetAppHInstance(), MAKEINTRESOURCEW(resourceID), parentHwnd, GlobalDlg_Proc, (LPARAM)component);
}

INT_PTR CALLBACK GlobalDlg_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG)
	{
		KComponent* comp=(KComponent*)lParam;
		if(comp)
		{
			comp->HotPlugInto(hwndDlg);
		}
		return FALSE;
	}
	return FALSE;
}

void InitRFC(HINSTANCE hInstance)
{
	if (!InternalVariables::rfcRefCount)
	{
		#ifdef _MSC_VER
			#ifdef RFC_FORCE_LFH   // low fragmentation heap feature (for xp)
			typedef enum _HEAP_INFORMATION_CLASS {
				HeapCompatibilityInformation
			} HEAP_INFORMATION_CLASS;
			typedef BOOL(WINAPI* PFN_HeapSetInformation)(HANDLE HeapHandle, HEAP_INFORMATION_CLASS HeapInformationClass, PVOID HeapInformation, SIZE_T HeapInformationLength);

			HMODULE kernelMod = GetModuleHandleW(L"kernel32");
			if (kernelMod)
			{
				PFN_HeapSetInformation pfnHeapSetInformation = (PFN_HeapSetInformation)GetProcAddress(kernelMod, "HeapSetInformation");
				if (pfnHeapSetInformation)
				{
					ULONG HeapInformation = 2; // HEAP_LFH = 2
					HANDLE hHeap = HeapCreate(0, 0, 0);
					(*pfnHeapSetInformation)(hHeap, HeapCompatibilityInformation, &HeapInformation, sizeof(ULONG));
					#if _MSC_VER >= 1300 // no way to get the pointer to the CRT heap in VC 6.0 (_crtheap)
						(*pfnHeapSetInformation)((HANDLE)_get_heap_handle(), HeapCompatibilityInformation, &HeapInformation, sizeof(ULONG));
					#endif
				}
			}

			#endif
		#endif

		if (!hInstance)
			hInstance = ::GetModuleHandleW(NULL);

		KPlatformUtil::GetInstance()->SetAppHInstance(hInstance); // create instance for first time & initialize Utility class!
		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize=sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC=ICC_WIN95_CLASSES;
		InitCommonControlsEx(&icx);

		CoInitialize(NULL); //Initializes COM as STA.

		InitializeCriticalSection(&InternalVariables::g_csComponent);
	}
	++InternalVariables::rfcRefCount;
}

void DeInitRFC()
{
	--InternalVariables::rfcRefCount;
	if (!InternalVariables::rfcRefCount)
	{
		CoUninitialize();

		DeleteCriticalSection(&InternalVariables::g_csComponent);

		// delete all singletons!
		delete KFont::GetDefaultFont();
		delete KPlatformUtil::GetInstance();
	}
}