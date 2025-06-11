
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

#pragma once

#include "../core/CoreModule.h"
#include "KComponent.h"

// all the methods must be called only from the gui thread.
class KGUIProc
{
public:
	static ATOM atomComponent;
	static ATOM atomOldProc;

	static LRESULT CALLBACK windowProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK dialogProc(HWND, UINT, WPARAM, LPARAM);

	/**
		set requireInitialMessages to true to receive initial messages lke WM_CREATE... (installs a hook)
	*/
	static HWND createComponent(KComponent* component, bool requireInitialMessages);

	/**
		hwnd can be window, custom control, dialog or common control.
		hwnd will be subclassed if it is a common control or dialog.
	*/
	static void attachRFCPropertiesToHWND(HWND hwnd, KComponent* component);

	static int hotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
	static HWND hotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
};

