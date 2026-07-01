
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

#pragma once

#include "KComponent.h"
#include "KIcon.h"
#include "KHostPanel.h"
#include "../containers/ContainersModule.h"
#include <functional>

#define RFC_CUSTOM_MESSAGE WM_APP + 100

enum class KCloseOperation { DestroyAndExit, Hide, Nothing };

// for a window, position is physical and size is logical. therefore compLX and compLY are ignored. (logical values)
// always use physical values when saving/restore window position.
// always use logical values when saving/restore window size.
class KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;
	bool enableDPIUnawareMode;
	KPointerList<KComponent*, 24, false> componentList; // KHostPanel is also using 24.
	KCloseOperation closeOperation;
	DPI_AWARENESS_CONTEXT prevDPIContext;
	bool dpiAwarenessContextChanged;
	KIcon* windowIcon;
	HICON largeIconHandle, smallIconHandle;
	Physical compPX, compPY; // physical virtual desktop coordinates (use this instead of compLX,compLY)

	bool resizingForDPIChange;
	void updateWindowIconForNewDPI() noexcept;

public:
	std::function<void(KWindow* window,int newDPI)> onDPIChange; // called after dpi change.

	KWindow() noexcept;

	virtual bool create(bool requireInitialMessages = false) noexcept override;

	void flash() noexcept;

	// can only call after create.
	virtual void setIcon(KIcon* icon) noexcept;

	void setCloseOperation(KCloseOperation closeOperation) noexcept;

	virtual void onClose() noexcept;

	virtual void onDestroy() noexcept;

	virtual void onDynamicMenuItemPress(UINT itemID) noexcept;

	// can only call after create.
	void postCloseMessage() noexcept;

	// Custom messages are used to send a signal/data from worker thread to gui thread.
	// can only call after create.
	void postCustomMessage(WPARAM msgID, LPARAM param) noexcept;

	virtual void onCustomMessage(WPARAM msgID, LPARAM param) noexcept;

	// can only call after create.
	virtual void centerScreen() noexcept;

	// puts our window on same monitor as given window + centered
	// can only call after create.
	virtual void centerOnSameMonitor(HWND window) noexcept;

	/**
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		Adding a component does not mean that the window will own or delete the component - it's
		your responsibility to delete the component. you need to remove the component if you are
		deleting it before WM_DESTROY message arrived.
	*/
	virtual bool addComponent(KComponent* component, bool requireInitialMessages = false) noexcept;

	bool addComponent(KComponent& component, bool requireInitialMessages = false) noexcept;

	template<typename... Components>
	void addComponents(bool requireInitialMessages, Components&... comps) noexcept
	{
		(addComponent(comps, requireInitialMessages), ...); // fold expression (C++17+)
	}

	template<typename... Components>
	void addComponents(Components&... comps) noexcept
	{
		(addComponent(comps, false), ...); // fold expression (C++17+)
	}

	// Can be also use to remove a container. Also destroys the hwnd.
	// you need to remove the component if you are deleting it before WM_DESTROY message arrived.
	virtual void removeComponent(KComponent* component) noexcept;

	// use this method to add KHostPanel to the window.
	bool addContainer(KHostPanel* container, bool requireInitialMessages = false) noexcept;

	// not used. do not call. window dpi only changed by the OS using WM_DPICHANGED msg.
	virtual void setDPI(int newDPI) noexcept override { K_ASSERT(false, "deprecated method KWindow::setDPI called"); }

	// Mixed-Mode DPI Scaling - window scaled by the system. can only call before create.
	// InitRFC must be called with KDPIAwareness::MIXEDMODE_ONLY
	// Only works with Win10 or higher
	// must call before create method.
	void setEnableDPIUnawareMode(bool enable) noexcept;

	// In mixed-mode dpi unaware window, before adding any child we need to set current thread dpi mode to unaware mode.
	// by default this method automatically called with AddComponent method.
	// if you add a child without calling AddComponent then you have to call ApplyDPIUnawareModeToThread method first.
	void applyDPIUnawareModeToThread() noexcept;

	// after adding the child, we need to restore the last dpi mode of the thread.
	// Mixed-Mode only
	void restoreDPIModeOfThread() noexcept;

	// x & y are physical positions
	static bool isOffScreen(Physical x, Physical y) noexcept;

	bool isMinimized() noexcept;

	// do not call. instead use getPositionPhysical.
	virtual Logical getX() noexcept override { K_ASSERT(false, "deprecated method KWindow::getX called"); return 0; }
	virtual Logical getY() noexcept override { K_ASSERT(false, "deprecated method KWindow::getY called"); return 0; }

	// do not call. instead use setPositionPhysical.
	virtual void setPosition(Logical x, Logical y) noexcept override { K_ASSERT(false, "deprecated method setPosition called"); }

	// width & height are logical values.
	// can only call after create.
	virtual void setClientAreaSize(Logical width, Logical height) noexcept;

	// width & height are physical values.
	// can only call after create.
	virtual void setClientAreaSizePhysical(Physical width, Physical height) noexcept;

	// width & height are logical values
	// can only call after create.
	void getClientAreaSize(Logical& width, Logical& height) noexcept;

	// can be use to get the window size even if it were minimized.
	void getNormalSize(Logical& width, Logical& height) noexcept;

	// can be use to get the window position even if it were minimized.
	void getPositionPhysical(Physical& x, Physical& y) noexcept;

	virtual void setPositionPhysical(Physical x, Physical y) noexcept;

	virtual void onMoved() noexcept;

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if resizingForDPIChange is true. (to detect dpi changes use onDPIChange event)
	virtual void onResized(bool resizingForDPIChange) noexcept;

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	virtual ~KWindow() noexcept;
};

