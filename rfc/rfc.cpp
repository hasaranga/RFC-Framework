
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

#include "rfc.h"

class InternalVariables
{
public:
	static KComponent *currentComponent;
	static HHOOK wnd_hook;
	static CRITICAL_SECTION g_csComponent; // guard currentComponent!
	static volatile int rfcRefCount;
};

KComponent* InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
volatile int InternalVariables::rfcRefCount = 0;
CRITICAL_SECTION InternalVariables::g_csComponent;

const wchar_t* InternalDefinitions::RFCPropText_Object = L"RFC";
const wchar_t* InternalDefinitions::RFCPropText_OldProc = L"RFCOldProc";

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
					::SetPropW(hwnd, InternalDefinitions::RFCPropText_Object, (HANDLE)InternalVariables::currentComponent);

					FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
					::SetPropW(hwnd, InternalDefinitions::RFCPropText_OldProc, (HANDLE)lpfnOldWndProc);

					if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // only sublcass if window proc is not GlobalWnd_Proc
						::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...

					// Call the next hook, if there is one
					LRESULT result = ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(InternalVariables::wnd_hook); // unhooking at here will also allow child creation at WM_CREATE

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

	KComponent *component = (KComponent*)::GetPropW(hwnd, InternalDefinitions::RFCPropText_Object);

	if(!component){ // just for safe!
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );
	}

	if(!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if(msg==WM_NCDESTROY){
		::RemovePropW(hwnd, InternalDefinitions::RFCPropText_Object);

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, InternalDefinitions::RFCPropText_OldProc);
		::RemovePropW(hwnd, InternalDefinitions::RFCPropText_OldProc);

		if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // common control or dialog
		{	
			::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
		}
		else{ // window or custom control
			return ::DefWindowProcW( hwnd, msg, wParam, lParam );
		}
	}

	return component->WindowProc(hwnd, msg, wParam, lParam);
}

HWND CreateRFCComponent(KComponent* component, bool subClassWindowProc)
{	
	if (subClassWindowProc)
	{
		::EnterCriticalSection(&InternalVariables::g_csComponent);

		InternalVariables::currentComponent = component;

		// install hook to get called before WM_CREATE_WINDOW msg!
		InternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		// pass current component as lpParam. so CBT proc can ignore other unknown windows.
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KPlatformUtil::GetInstance()->GetAppHInstance(), (LPVOID)component);

		// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc.
		//::UnhookWindowsHookEx(InternalVariables::wnd_hook);

		::LeaveCriticalSection(&InternalVariables::g_csComponent);

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KPlatformUtil::GetInstance()->GetAppHInstance(), 0);

		::SetPropW(hwnd, InternalDefinitions::RFCPropText_Object, (HANDLE)component);

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

			/*if(msg.message == WM_KEYDOWN)
			{
				if(VK_TAB == msg.wParam) // looking for TAB key!
				{
					if(msg.hwnd)
					{
						HWND parentHWND = ::GetParent(msg.hwnd);
						if(!parentHWND) // nothing selected! (top-level window)
						{
							HWND nextControl = ::GetNextDlgTabItem(msg.hwnd, NULL, FALSE);
							if(nextControl)
							{
								::SetFocus(nextControl);
								continue; // don't pass this message!
							}
						}else // user has already selected component!
						{
							HWND nextControl = ::GetNextDlgTabItem(parentHWND, msg.hwnd, FALSE);
							if(nextControl)
							{
								if((::GetKeyState(VK_CONTROL) & 0x8000) == 0) // user is not hold ctrl key!
								{
									::SetFocus(nextControl);
									continue; // don't pass this message!
								}
							}
						}
					}
				}
			}*/
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
	thread->SetHandle(::GetCurrentThread());
	thread->Run();

	return 0;	
}

bool CreateRFCThread(KThread* thread)
{
	if(thread)
	{
		HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, 0, NULL);
		if(handle)
			return true;
	}
	return false;
}

int HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KPlatformUtil::GetInstance()->GetAppHInstance(), MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KPlatformUtil::GetInstance()->GetAppHInstance(), MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

INT_PTR CALLBACK GlobalDlg_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if(comp)
		{
			comp->HotPlugInto(hwndDlg, true, true);
		}
		return FALSE;
	}
	return FALSE;
}

void InitRFC(HINSTANCE hInstance)
{
	if (!InternalVariables::rfcRefCount)
	{
		if (!hInstance)
			hInstance = ::GetModuleHandleW(NULL);

		KPlatformUtil::GetInstance()->SetAppHInstance(hInstance); // create instance for first time & initialize Utility class!
		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);

		::CoInitialize(NULL); //Initializes COM as STA.

		::InitializeCriticalSection(&InternalVariables::g_csComponent);
	}
	++InternalVariables::rfcRefCount;
}

void DeInitRFC()
{
	--InternalVariables::rfcRefCount;
	if (!InternalVariables::rfcRefCount)
	{
		::CoUninitialize();

		::DeleteCriticalSection(&InternalVariables::g_csComponent);

		// delete all singletons!
		delete KFont::GetDefaultFont();
		delete KPlatformUtil::GetInstance();
	}
}