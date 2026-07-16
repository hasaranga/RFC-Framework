
/*
	Copyright (C) 2013-2026 CrownSoft
  
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

#include "KApplication.h"

HINSTANCE KApplication::hInstance = 0;
KDPIAwareness KApplication::dpiAwareness;

KApplication::KApplication() noexcept
{
}

void KApplication::modifyModuleInitParams() noexcept
{

}

int KApplication::main(wchar_t** argv, int argc) noexcept
{
	return 0;
}

bool KApplication::allowMultipleInstances() noexcept
{
	return true;
}

int KApplication::anotherInstanceIsRunning(wchar_t** argv, int argc) noexcept
{
	return 0;
}

const wchar_t* KApplication::getApplicationID() noexcept
{
	return L"RFC_APPLICATION";
}

void KApplication::messageLoop(bool handleTabKey) noexcept
{
	MSG msg;

	while (::GetMessageW(&msg, NULL, 0, 0))
	{
		if (handleTabKey)
		{
			// a focused window that answers WM_GETDLGCODE with DLGC_WANTALLKEYS (e.g. a
			// custom control doing its own keyboard handling, a native multiline edit) must
			// get its keyboard messages through the plain TranslateMessage/DispatchMessage
			// pair below. handing them to IsDialogMessage only works while the focus sits on
			// a CHILD window: with focus on the top-level window ITSELF, IsDialogMessage
			// consumes WM_KEYDOWN without running TranslateMessage, so WM_CHAR is never
			// generated - KeyDown-driven ops (arrows/delete/ctrl+V) keep working while
			// typing is silently dead. so ask the target window first and route
			// want-all-keys messages around the dialog manager entirely.
			if ((msg.message >= WM_KEYFIRST) && (msg.message <= WM_KEYLAST))
			{
				if (::SendMessageW(msg.hwnd, WM_GETDLGCODE, msg.wParam, (LPARAM)&msg) & DLGC_WANTALLKEYS)
				{
					::TranslateMessage(&msg);
					::DispatchMessageW(&msg);
					continue;
				}
			}

			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

KApplication::~KApplication() noexcept
{
}
