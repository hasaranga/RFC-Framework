
/*
	RFC - KPlatformUtil.h
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

#ifndef _RFC_KPLATFORMUTIL_H_
#define _RFC_KPLATFORMUTIL_H_

#include "../text/KString.h"
#include <windows.h>
#include <stdio.h>
#include "../containers/KPointerList.h"
#include "../gui/KMenuItem.h"
#include "../threads/KTimer.h"

/**
	Singleton class which can be use to get HINSTANCE of your application.
	e.g. @code
	HINSTANCE hInstance=PlatformUtil::GetInstance()->GetAppInstance();
	@endcode
*/
class RFC_API KPlatformUtil
{
private:

	static KPlatformUtil *_instance;

	KPlatformUtil();

protected:
	HINSTANCE hInstance;
	CRITICAL_SECTION g_csCount;
	int classCount;
	int timerCount;
	int controlCount;
	UINT menuItemCount;

	KPointerList<KMenuItem*> *menuItemList;
	KPointerList<KTimer*> *timerList;

public:

	static KPlatformUtil* GetInstance();

	void SetAppHInstance(HINSTANCE hInstance);

	HINSTANCE GetAppHInstance();

	UINT GenerateControlID();

	KString GenerateClassName();
	UINT GenerateMenuItemID(KMenuItem *menuItem);
	KMenuItem* GetMenuItemByID(UINT id);

	UINT GenerateTimerID(KTimer *timer);
	KTimer* GetTimerByID(UINT id);

	~KPlatformUtil();

};


#endif