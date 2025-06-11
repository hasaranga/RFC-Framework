
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

#include"../core/CoreModule.h"
#include "KWindow.h"
#include <shellapi.h>
#include <commctrl.h>
#include <type_traits> // std::is_base_of

#define RFC_NOTIFY_ICON_MESSAGE WM_APP + 101

// adds an icon into the systemtray and handles mouse messages.
// detects taskbar re-creation and adds the icon again.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KNotifyIconHandler : public T
{
protected:
	HICON notifyIconHandle;
	KString notifyIconToolTipText;
	UINT taskbarRestartMsg;

	virtual LRESULT onNotifyIconMessage(WPARAM wParam, LPARAM lParam)
	{
		if (lParam == WM_LBUTTONUP)
			this->onNotifyIconLeftClick();
		else if (lParam == WM_RBUTTONUP)
			this->onNotifyIconRightClick();

		return 0;
	}

	virtual void createNotifyIcon(HWND window, HICON icon, const KString& toolTipText)
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
	virtual LRESULT onTaskBarReCreate(WPARAM wParam, LPARAM lParam)
	{
		if (notifyIconHandle)
			this->createNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);

		return 0;
	}

	// override this method in your subclass and show popup menu.
	virtual void onNotifyIconRightClick()
	{
		::SetForegroundWindow(this->compHWND);
		// show you popup menu here...
	}

	// override this method in your subclass.
	virtual void onNotifyIconLeftClick()
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
	virtual void addNotifyIcon(WORD iconResourceID, const KString& tooltipText)
	{
		// supports high dpi.
		// LoadIconMetric: only for system tray. cannot use for a window. because multiple window can have different dpi.
		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		notifyIconToolTipText = tooltipText;

		this->createNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);
	}

	virtual void updateNotifyIcon(WORD iconResourceID)
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
	virtual void updateNotifyIconToolTip(const KString& tooltipText)
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

	virtual void destroyNotifyIcon()
	{
		NOTIFYICONDATAW nid = { 0 };
		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;

		::Shell_NotifyIconW(NIM_DELETE, &nid);
	}

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == RFC_NOTIFY_ICON_MESSAGE)
			return this->onNotifyIconMessage(wParam, lParam);
		else if (msg == taskbarRestartMsg)
			return this->onTaskBarReCreate(wParam, lParam);
		else
			return T::windowProc(hwnd, msg, wParam, lParam);
	}
};
