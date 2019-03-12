
/*
	RFC - KPlatformUtil.h
	Copyright (C) 2013-2019 CrownSoft
  
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
	Singleton class which can be use to generate class names, timer ids etc...
	define "RFC_SINGLE_THREAD_COMP_CREATION" if your app does not creating components/menu items/timers
	within multiple threads.
	(this class is for internal use)
*/
class KPlatformUtil
{
private:
	RFC_LEAK_DETECTOR(KPlatformUtil)

	static KPlatformUtil *_instance;
	KPlatformUtil();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	KPointerList<KMenuItem*> *menuItemList;
	KPointerList<KTimer*> *timerList;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	CRITICAL_SECTION criticalSectionForCount;
	#endif

public:

	static KPlatformUtil* GetInstance();

	UINT GenerateControlID();

	// KApplication:hInstance must be valid before calling this method
	KString GenerateClassName();

	UINT GenerateMenuItemID(KMenuItem *menuItem);
	KMenuItem* GetMenuItemByID(UINT id);

	UINT GenerateTimerID(KTimer *timer);
	KTimer* GetTimerByID(UINT id);

	~KPlatformUtil();

};


#endif