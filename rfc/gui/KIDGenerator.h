
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

	static KIDGenerator*_instance;
	KIDGenerator();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	KPointerList<KMenuItem*> *menuItemList;
	KPointerList<KTimer*> *timerList;

public:

	static KIDGenerator* GetInstance();

	UINT GenerateControlID();

	// KApplication:hInstance must be valid before calling this method
	KString GenerateClassName();

	UINT GenerateMenuItemID(KMenuItem *menuItem);
	KMenuItem* GetMenuItemByID(UINT id);

	UINT GenerateTimerID(KTimer *timer);
	KTimer* GetTimerByID(UINT id);

	~KIDGenerator();

};
