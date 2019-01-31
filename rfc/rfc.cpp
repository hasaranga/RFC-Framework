
/*
	RFC - rfc.cpp
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

#include "rfc.h"

class InternalVariables
{
public:
	static KComponent *currentComponent;
	static HHOOK wnd_hook;
	static volatile int rfcRefCount;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	static CRITICAL_SECTION csForCurrentComponent; // guard currentComponent!
	#endif
};

KComponent*	InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
volatile int InternalVariables::rfcRefCount = 0;

#ifndef RFC_SINGLE_THREAD_COMP_CREATION
CRITICAL_SECTION InternalVariables::csForCurrentComponent;
#endif

ATOM InternalDefinitions::RFCPropAtom_Component;
ATOM InternalDefinitions::RFCPropAtom_OldProc;

void AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component)
{
	::SetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component), (HANDLE)component);

	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc), (HANDLE)lpfnOldWndProc);

	if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // sublcass only if window proc is not GlobalWnd_Proc (common control or dialog)
		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...
}

LRESULT CALLBACK RFCCTL_CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode < 0)
		return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

	if(nCode==HCBT_CREATEWND){
		HWND hwnd=(HWND)wParam;
		LPCBT_CREATEWNDW cbtCreateWnd = (LPCBT_CREATEWNDW)lParam;
		if (cbtCreateWnd)
		{
			if (cbtCreateWnd->lpcs)
			{
				if (cbtCreateWnd->lpcs->lpCreateParams == InternalVariables::currentComponent) // only catch what we created. ignore unknown windows.
				{
					::AttachRFCPropertiesToHWND(hwnd, InternalVariables::currentComponent);

					// Call the next hook, if there is one
					LRESULT result = ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(InternalVariables::wnd_hook); // unhooking at here will allow child creation at WM_CREATE. otherwise this will hook child also!

					return result;
				}
			}
		}
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK GlobalWnd_Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	KComponent *component = (KComponent*)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));

	if(!component) // for safe!
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );

	if(!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if(msg == WM_NCDESTROY){
		::RemovePropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc));
		::RemovePropW(hwnd,  MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc));

		if (lpfnOldWndProc)
		{
			if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // common control or dialog
			{
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
				return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
			}
		}
		 // lpfnOldWndProc is not set or window or custom control
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );		
	}

	return component->WindowProc(hwnd, msg, wParam, lParam);
}

HWND CreateRFCComponent(KComponent* component, bool requireInitialMessages)
{	
	if (requireInitialMessages)
	{
		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::EnterCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		InternalVariables::currentComponent = component;

		// install hook to get called before WM_CREATE_WINDOW msg!
		InternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		// pass current component as lpParam. so CBT proc can ignore other unknown windows.
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KApplication::hInstance, (LPVOID)component);

		// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc.
		//::UnhookWindowsHookEx(InternalVariables::wnd_hook);

		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::LeaveCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KApplication::hInstance, 0);
		::AttachRFCPropertiesToHWND(hwnd, component);
		component->SetHWND(hwnd);

		return hwnd;
	}
}

void DoMessagePump(bool handleTabKey)
{
	MSG msg;
	
	while (::GetMessageW(&msg, NULL, 0, 0)) 
	{
		if(handleTabKey)
		{
			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	} 
}

DWORD WINAPI GlobalThread_Proc(LPVOID lpParameter)
{
	if(lpParameter == 0) // for safe!
		return 0;

	KThread* thread = (KThread*)lpParameter;
	thread->Run();

	return 0;	
}

bool CreateRFCThread(KThread* thread)
{
	if(thread)
	{
		HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL); // create thread in suspended state. so we can set the handle field.
		if (handle)
		{
			thread->SetHandle(handle);
			::ResumeThread(handle);

			return true;
		}
	}
	return false;
}

int HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

INT_PTR CALLBACK GlobalDlg_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if(comp)
			comp->HotPlugInto(hwndDlg, true);

		return FALSE;
	}
	return FALSE;
}

void InitRFC(HINSTANCE hInstance)
{
	if (!InternalVariables::rfcRefCount)
	{
		if (!hInstance)
		{
			//hInstance = ::GetModuleHandleW(NULL); // not work for dll
			::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&InitRFC, &hInstance);
		}

		KApplication::hInstance = hInstance;

		KPlatformUtil::GetInstance(); // create instance for first time
		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);

		::CoInitialize(NULL); //Initializes COM as STA.

		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::InitializeCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		InternalDefinitions::RFCPropAtom_Component = ::GlobalAddAtomW(L"RFCComponent");
		InternalDefinitions::RFCPropAtom_OldProc = ::GlobalAddAtomW(L"RFCOldProc");
	}
	++InternalVariables::rfcRefCount;
}

void DeInitRFC()
{
	RFC_INIT_VERIFIER;

	--InternalVariables::rfcRefCount;
	if (!InternalVariables::rfcRefCount)
	{
		::CoUninitialize();

		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::DeleteCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		::GlobalDeleteAtom(InternalDefinitions::RFCPropAtom_Component);
		::GlobalDeleteAtom(InternalDefinitions::RFCPropAtom_OldProc);

		// delete all singletons!
		delete KFont::GetDefaultFont();
		delete KPlatformUtil::GetInstance();
	}
}