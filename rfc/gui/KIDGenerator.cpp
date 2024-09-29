
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

#include "KIDGenerator.h"

static const int rfc_InitialMenuItemCount	= 400;
static const int rfc_InitialTimerCount		= 40;
static const int rfc_InitialControlID		= 100;
static const int rfc_InitialMenuItemID		= 30000;
static const int rfc_InitialTimerID			= 1000;

KIDGenerator* KIDGenerator::_instance = 0;

KIDGenerator::KIDGenerator()
{
	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	menuItemList = nullptr;
	timerList = nullptr;
}

KIDGenerator* KIDGenerator::GetInstance()
{
	if(_instance)
		return _instance;
	_instance = new KIDGenerator();
	return _instance;
}

UINT KIDGenerator::GenerateControlID()
{
	++controlCount;
	return controlCount + rfc_InitialControlID;
}

UINT KIDGenerator::GenerateMenuItemID(KMenuItem* menuItem)
{
	if (menuItemList == nullptr) // generate on first call
		menuItemList = new KPointerList<KMenuItem*>(rfc_InitialMenuItemCount);

	++menuItemCount;
	menuItemList->AddPointer(menuItem);

	return menuItemCount + rfc_InitialMenuItemID;
}

KMenuItem* KIDGenerator::GetMenuItemByID(UINT id)
{
	if (menuItemList)
		return menuItemList->GetPointer(id - (rfc_InitialMenuItemID + 1));
	return 0;
}

KString KIDGenerator::GenerateClassName()
{ 
	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'R';
	className[1] = L'F';
	className[2] = L'C';
	className[3] = L'_';

	int hInstance32 = HandleToLong(KApplication::hInstance);
	if (hInstance32 < 0)
		hInstance32 *= -1;

	::_itow_s(hInstance32, &className[4],28, 10); // 28 is buffer size

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow_s(classCount, &className[lastPos + 1],32 - (lastPos + 1), 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	++classCount;

	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

UINT KIDGenerator::GenerateTimerID(KTimer* timer)
{
	if (timerList == nullptr) // generate on first call
		timerList = new KPointerList<KTimer*>(rfc_InitialTimerCount);

	++timerCount;
	timerList->AddPointer(timer);

	return timerCount + rfc_InitialTimerID;
}

KTimer* KIDGenerator::GetTimerByID(UINT id)
{
	if (timerList)
		return timerList->GetPointer(id - (rfc_InitialTimerID + 1));
	return 0;
}

KIDGenerator::~KIDGenerator()
{
	if (menuItemList)
		delete menuItemList;

	if (timerList)
		delete timerList;
}