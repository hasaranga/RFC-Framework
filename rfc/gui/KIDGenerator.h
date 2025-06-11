
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
#include "../containers/ContainersModule.h"
#include "KMenuItem.h"
#include "KTimer.h"
#include <stdio.h>

/**
	Singleton class which can be used to generate class names, timer ids etc...
	Methods are not thread safe. So, only call from the gui thread.
	(this class is for internal use)
*/
class KIDGenerator
{
private:
	RFC_LEAK_DETECTOR(KIDGenerator)
	KIDGenerator();
	~KIDGenerator();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	static const int rfc_InitialMenuItemCount = 20;
	static const int rfc_InitialTimerCount = 10;

	static const int rfc_InitialControlID = 100;
	static const int rfc_InitialMenuItemID = 30000;
	static const int rfc_InitialTimerID = 1000;

	KPointerList<KMenuItem*, rfc_InitialMenuItemCount, false> menuItemList;
	KPointerList<KTimer*, rfc_InitialTimerCount, false> timerList;

public:
	// do not delete the returned instance.
	static KIDGenerator* getInstance();

	UINT generateControlID();

	// KApplication:hInstance must be valid before calling this method
	// can generate up to 9999 class names.
	void generateClassName(KString& stringToModify);

	UINT generateMenuItemID(KMenuItem* menuItem);
	KMenuItem* getMenuItemByID(UINT id);

	UINT generateTimerID(KTimer* timer);
	KTimer* getTimerByID(UINT id);
};
