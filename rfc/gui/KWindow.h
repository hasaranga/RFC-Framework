
/*
	RFC - KWindow.h
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

#ifndef _RFC_KWINDOW_H_
#define _RFC_KWINDOW_H_

#include "KComponent.h"
#include "../graphics/KIcon.h"

class RFC_API KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;

public:
	KWindow();

	virtual void Flash();

	virtual void SetIcon(KIcon *icon);

	virtual void Destroy();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual void CenterScreen();

	/**
		set subClassWindowProc value to true if component is owner-drawn.
		Otherwise WindowProc will be disabled & you will not receive WM_MEASUREITEM like messages into the EventProc.
	*/
	virtual bool AddComponent(KComponent *component, bool subClassWindowProc = false);

	virtual bool SetClientAreaSize(int width, int height);

	virtual bool GetClientAreaSize(int *width, int *height);

	virtual void OnMoved();

	virtual void OnResized();

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual ~KWindow();
};

#endif