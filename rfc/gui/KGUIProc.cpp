
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

#include "KGUIProc.h"

class RFCInternalVariables
{
public:
	static KComponent* currentComponent;
	static HHOOK wnd_hook;
};

KComponent* RFCInternalVariables::currentComponent = 0;
HHOOK RFCInternalVariables::wnd_hook = 0;

ATOM KGUIProc::AtomComponent;
ATOM KGUIProc::AtomOldProc;

LRESULT CALLBACK RFCCTL_CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);

	if (nCode == HCBT_CREATEWND) {
		HWND hwnd = (HWND)wParam;
		LPCBT_CREATEWNDW cbtCreateWnd = (LPCBT_CREATEWNDW)lParam;
		if (cbtCreateWnd)
		{
			if (cbtCreateWnd->lpcs)
			{
				if (cbtCreateWnd->lpcs->lpCreateParams == RFCInternalVariables::currentComponent) // only catch what we created. ignore unknown windows.
				{
					KGUIProc::AttachRFCPropertiesToHWND(hwnd, RFCInternalVariables::currentComponent);

					// Call the next hook, if there is one
					const LRESULT result = ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(RFCInternalVariables::wnd_hook); // unhooking at here will allow child creation at WM_CREATE. otherwise this will hook child also!

					return result;
				}
			}
		}
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(RFCInternalVariables::wnd_hook, nCode, wParam, lParam);
}

void KGUIProc::AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component)
{
	::SetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomComponent), (HANDLE)component);

	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc), (HANDLE)lpfnOldWndProc);

	if (lpfnOldWndProc != (void*)KGUIProc::WindowProc) // sublcass only if window proc is not KGUIProc::WindowProc (common control or dialog)
		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)KGUIProc::WindowProc); // subclassing...
}

LRESULT CALLBACK KGUIProc::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	KComponent* component = (KComponent*)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomComponent));

	if (!component) // for safe!
		return ::DefWindowProcW(hwnd, msg, wParam, lParam);

	if (!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if (msg == WM_NCDESTROY) {
		::RemovePropW(hwnd, MAKEINTATOM(KGUIProc::AtomComponent));

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc));
		::RemovePropW(hwnd, MAKEINTATOM(KGUIProc::AtomOldProc));

		if (lpfnOldWndProc)
		{
			if (lpfnOldWndProc != (void*)KGUIProc::WindowProc) // common control or dialog
			{
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
				return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
			}
		}
		// lpfnOldWndProc is not set or window or custom control
		return ::DefWindowProcW(hwnd, msg, wParam, lParam);
	}

	return component->WindowProc(hwnd, msg, wParam, lParam);
}

INT_PTR CALLBACK KGUIProc::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if (comp)
			comp->HotPlugInto(hwndDlg, true);

		return FALSE;
	}
	return FALSE;
}

HWND KGUIProc::CreateComponent(KComponent* component, bool requireInitialMessages)
{
	if (requireInitialMessages)
	{
		RFCInternalVariables::currentComponent = component;

		// install hook to get called before WM_NCCREATE msg! (we can receive WM_NCCREATE msg even for a standard control using this technique)
		RFCInternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		// pass current component as lpParam. so CBT proc can ignore other unknown windows.
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(),
			component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(),
			component->GetParentHWND(), (HMENU)(UINT_PTR)component->GetControlID(), KApplication::hInstance, (LPVOID)component);

		// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc.
		//::UnhookWindowsHookEx(RFCInternalVariables::wnd_hook);

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(),
			component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(),
			component->GetParentHWND(), (HMENU)(UINT_PTR)component->GetControlID(), KApplication::hInstance, 0);

		KGUIProc::AttachRFCPropertiesToHWND(hwnd, component);
		component->SetHWND(hwnd);

		return hwnd;
	}
}

int KGUIProc::HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, KGUIProc::DialogProc, (LPARAM)component);
}

HWND KGUIProc::HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, KGUIProc::DialogProc, (LPARAM)component);
}