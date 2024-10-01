
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

#pragma once

#include "KComponent.h"
#include "KIcon.h"
#include "KHostPanel.h"
#include "../containers/ContainersModule.h"

#define RFC_CUSTOM_MESSAGE WM_APP + 100

class KDPIChangeListener
{
public:
	virtual void OnDPIChange(HWND hwnd, int newDPI) = 0;
};

enum class KCloseOperation { DestroyAndExit, Hide, Nothing };

class KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;
	KDPIChangeListener* dpiChangeListener;
	bool enableDPIUnawareMode;
	KPointerList<KComponent*> componentList;
	KCloseOperation closeOperation;
	DPI_AWARENESS_CONTEXT prevDPIContext;
	bool dpiAwarenessContextChanged;
	KIcon* windowIcon;
	HICON largeIconHandle, smallIconHandle;

	void UpdateWindowIconForNewDPI();

public:
	KWindow();

	virtual bool Create(bool requireInitialMessages = false) override;

	virtual void Flash();

	// can only call after create.
	virtual void SetIcon(KIcon* icon);

	virtual void SetCloseOperation(KCloseOperation closeOperation);

	virtual void OnClose();

	virtual void OnDestroy();

	// Custom messages are used to send a signal/data from worker thread to gui thread.
	virtual void PostCustomMessage(WPARAM msgID, LPARAM param);

	virtual void OnCustomMessage(WPARAM msgID, LPARAM param);

	virtual void CenterScreen();

	// puts our window on same monitor as given window + centered
	virtual void CenterOnSameMonitor(HWND window);

	/**
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool AddComponent(KComponent* component, bool requireInitialMessages = false);

	// Can be also use to remove a container. Also destroys the hwnd.
	virtual void RemoveComponent(KComponent* component);

	// use this method to add KHostPanel to the window.
	virtual bool AddContainer(KHostPanel* container, bool requireInitialMessages = false);

	virtual bool SetClientAreaSize(int width, int height);

	virtual void SetDPIChangeListener(KDPIChangeListener* dpiChangeListener);

	// Mixed-Mode DPI Scaling - window scaled by the system. can only call before create.
	// InitRFC must be called with KDPIAwareness::MIXEDMODE_ONLY
	// Only works with Win10 or higher
	virtual void SetEnableDPIUnawareMode(bool enable);

	// In mixed-mode dpi unaware window, before adding any child we need to set current thread dpi mode to unaware mode.
	// by default this method automatically called with AddComponent method.
	// if you add a child without calling AddComponent then you have to call ApplyDPIUnawareModeToThread method first.
	virtual void ApplyDPIUnawareModeToThread();

	// after adding the child, we need to restore the last dpi mode of the thread.
	// Mixed-Mode only
	virtual void RestoreDPIModeOfThread();

	static bool IsOffScreen(int posX, int posY);

	virtual bool GetClientAreaSize(int* width, int* height);

	virtual void OnMoved();

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if the window and controls are in different dpi scale. (use KDPIChangeListener)
	virtual void OnResized();

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KWindow();
};

