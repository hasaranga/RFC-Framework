
/*
	RFC - KPlatformUtil.cpp
	Copyright (C) 2013-2018 CrownSoft
  
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

#include "KPlatformUtil.h"
#include "../rfc.h"

KPlatformUtil* KPlatformUtil::_instance=0;

KPlatformUtil::KPlatformUtil()
{
	RFC_INIT_VERIFIER;
	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	::InitializeCriticalSection(&g_csCount);
	menuItemList = new KPointerList<KMenuItem*>();
	timerList = new KPointerList<KTimer*>();
}

KPlatformUtil* KPlatformUtil::GetInstance()
{
	if(_instance)
		return _instance;
	_instance = new KPlatformUtil();
	return _instance;
}

UINT KPlatformUtil::GenerateControlID()
{
	::EnterCriticalSection(&g_csCount);
	controlCount++;
	::LeaveCriticalSection(&g_csCount);

	return controlCount + 100;
}

UINT KPlatformUtil::GenerateMenuItemID(KMenuItem *menuItem)
{
	::EnterCriticalSection(&g_csCount);
	menuItemCount++;
	menuItemList->AddPointer(menuItem);
	::LeaveCriticalSection(&g_csCount);

	return menuItemCount + 30000;
}

KMenuItem* KPlatformUtil::GetMenuItemByID(UINT id)
{
	return menuItemList->GetPointer(id - 30001);
}

KString KPlatformUtil::GenerateClassName()
{
	::EnterCriticalSection(&g_csCount);

	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'R';
	className[1] = L'F';
	className[2] = L'C';
	className[3] = L'_';

	::_itow((int)KApplication::hInstance, &className[4], 10);

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow(classCount, &className[lastPos + 1], 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	classCount++;
	::LeaveCriticalSection(&g_csCount);
	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

UINT KPlatformUtil::GenerateTimerID(KTimer *timer)
{
	::EnterCriticalSection(&g_csCount);
	timerCount++;
	timerList->AddPointer(timer);
	::LeaveCriticalSection(&g_csCount);

	return timerCount + 1000;
}

KTimer* KPlatformUtil::GetTimerByID(UINT id)
{
	return timerList->GetPointer(id - 1001);
}

KPlatformUtil::~KPlatformUtil()
{
	delete menuItemList;
	delete timerList;
	::DeleteCriticalSection(&g_csCount);
}