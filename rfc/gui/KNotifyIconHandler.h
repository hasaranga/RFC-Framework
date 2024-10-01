
/*
	Copyright (C) 2013-2024 CrownSoft

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

#include"../core/CoreModule.h"
#include <shellapi.h>
#include <commctrl.h>

#define RFC_NOTIFY_ICON_MESSAGE WM_APP + 101

// adds an icon into the systemtray and handles mouse messages.
// detects taskbar re-creation and adds the icon again.
template <class T>
class KNotifyIconHandler : public T
{
protected:
	HICON notifyIconHandle;
	KString notifyIconToolTipText;
	UINT taskbarRestartMsg;

	virtual LRESULT OnNotifyIconMessage(WPARAM wParam, LPARAM lParam)
	{
		if (lParam == WM_LBUTTONUP)
			this->OnNotifyIconLeftClick();
		else if (lParam == WM_RBUTTONUP)
			this->OnNotifyIconRightClick();

		return 0;
	}

	virtual void CreateNotifyIcon(HWND window, HICON icon, const KString& toolTipText)
	{
		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = window;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = RFC_NOTIFY_ICON_MESSAGE;
		nid.hIcon = icon;
		::wcscpy_s(nid.szTip, toolTipText);
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

		::Shell_NotifyIconW(NIM_ADD, &nid);
	}

	// on explorer crash
	virtual LRESULT OnTaskBarReCreate(WPARAM wParam, LPARAM lParam)
	{
		if (notifyIconHandle)
			this->CreateNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);

		return 0;
	}

	// override this method in your subclass and show popup menu.
	virtual void OnNotifyIconRightClick()
	{
		::SetForegroundWindow(this->compHWND);
		// show you popup menu here...
	}

	// override this method in your subclass.
	virtual void OnNotifyIconLeftClick()
	{
		::SetForegroundWindow(this->compHWND);
	}

public:
	KNotifyIconHandler()
	{
		notifyIconHandle = 0;
		taskbarRestartMsg = ::RegisterWindowMessageW(L"TaskbarCreated");
	}

	virtual ~KNotifyIconHandler()
	{
		if (notifyIconHandle)
			::DestroyIcon(notifyIconHandle);
	}

	// window must be created.
	// maximum tooltip text size is 128
	virtual void AddNotifyIcon(WORD iconResourceID, const KString& tooltipText)
	{
		// supports high dpi.
		// LoadIconMetric: only for system tray. cannot use for a window. because multiple window can have different dpi.
		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		notifyIconToolTipText = tooltipText;

		this->CreateNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);
	}

	virtual void UpdateNotifyIcon(WORD iconResourceID)
	{
		if (notifyIconHandle)
			::DestroyIcon(notifyIconHandle);

		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.hIcon = notifyIconHandle;
		nid.uFlags = NIF_ICON;

		::Shell_NotifyIconW(NIM_MODIFY, &nid);
	}

	// maximum tooltip text size is 128
	virtual void UpdateNotifyIconToolTip(const KString& tooltipText)
	{
		notifyIconToolTipText = tooltipText;

		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		::wcscpy_s(nid.szTip, tooltipText);
		nid.uFlags = NIF_TIP;

		::Shell_NotifyIconW(NIM_MODIFY, &nid);
	}

	virtual void DestroyNotifyIcon()
	{
		NOTIFYICONDATAW nid = { 0 };
		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;

		::Shell_NotifyIconW(NIM_DELETE, &nid);
	}

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == RFC_NOTIFY_ICON_MESSAGE)
			return this->OnNotifyIconMessage(wParam, lParam);
		else if (msg == taskbarRestartMsg)
			return this->OnTaskBarReCreate(wParam, lParam);
		else
			return T::WindowProc(hwnd, msg, wParam, lParam);
	}
};
