
/*
	RFC - KPlatformUtil.cpp
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

#include "KPlatformUtil.h"
#include "../rfc.h"

static const int rfc_InitialMenuItemCount	= 400;
static const int rfc_InitialTimerCount		= 40;
static const int rfc_InitialControlID		= 100;
static const int rfc_InitialMenuItemID		= 30000;
static const int rfc_InitialTimerID			= 1000;

KPlatformUtil* KPlatformUtil::_instance = 0;

KPlatformUtil::KPlatformUtil()
{
	RFC_INIT_VERIFIER;

	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	menuItemList = 0;
	timerList = 0;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::InitializeCriticalSection(&criticalSectionForCount);
	#endif
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
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	++controlCount;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return controlCount + rfc_InitialControlID;
}

UINT KPlatformUtil::GenerateMenuItemID(KMenuItem *menuItem)
{
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	if (menuItemList == 0) // generate on first call
		menuItemList = new KPointerList<KMenuItem*>(rfc_InitialMenuItemCount);

	++menuItemCount;
	menuItemList->AddPointer(menuItem);

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return menuItemCount + rfc_InitialMenuItemID;
}

KMenuItem* KPlatformUtil::GetMenuItemByID(UINT id)
{
	if (menuItemList)
		return menuItemList->GetPointer(id - (rfc_InitialMenuItemID + 1));
	return 0;
}

KString KPlatformUtil::GenerateClassName()
{ 
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

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

	++classCount;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

UINT KPlatformUtil::GenerateTimerID(KTimer *timer)
{
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	if (timerList == 0) // generate on first call
		timerList = new KPointerList<KTimer*>(rfc_InitialTimerCount);

	++timerCount;
	timerList->AddPointer(timer);

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return timerCount + rfc_InitialTimerID;
}

KTimer* KPlatformUtil::GetTimerByID(UINT id)
{
	if (timerList)
		return timerList->GetPointer(id - (rfc_InitialTimerID + 1));
	return 0;
}

KPlatformUtil::~KPlatformUtil()
{
	if (menuItemList)
		delete menuItemList;

	if (timerList)
		delete timerList;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::DeleteCriticalSection(&criticalSectionForCount);
	#endif
}