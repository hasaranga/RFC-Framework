
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

#include "KIDGenerator.h"

KIDGenerator::KIDGenerator()
{
	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
}

KIDGenerator* KIDGenerator::getInstance()
{
	static KIDGenerator defaultInstance;  // Created once, on first use (Meyer's Singleton)
	return &defaultInstance;
}

UINT KIDGenerator::generateControlID()
{
	++controlCount;
	return controlCount + rfc_InitialControlID;
}

UINT KIDGenerator::generateMenuItemID(KMenuItem* menuItem)
{
	++menuItemCount;
	menuItemList.add(menuItem);

	return menuItemCount + rfc_InitialMenuItemID;
}

KMenuItem* KIDGenerator::getMenuItemByID(UINT id)
{
	return menuItemList.get(id - (rfc_InitialMenuItemID + 1));
}

void KIDGenerator::generateClassName(KString& stringToModify)
{ 
	// can generate up to 9999 class names.
	wchar_t* classNameBuffer;
	int* pLength;
	stringToModify.accessRawSSOBuffer(&classNameBuffer, &pLength);

	int hInstance32 = ::HandleToLong(KApplication::hInstance);
	if (hInstance32 < 0)
		hInstance32 *= -1;

	::_itow_s(hInstance32, classNameBuffer, KString::SSO_BUFFER_SIZE, 16);  // using hex code reduces char usage.

	int pos = (int)::wcslen(classNameBuffer);
	if (pos < 11) {  // Safety check
		::_itow_s(classCount, &classNameBuffer[pos], KString::SSO_BUFFER_SIZE - pos, 10);
		classCount++;

		*pLength = (int)::wcslen(classNameBuffer);
	}
	else
	{
		K_ASSERT(false, "class count overflow in KIDGenerator::generateClassName");
	}
}

UINT KIDGenerator::generateTimerID(KTimer* timer)
{
	++timerCount;
	timerList.add(timer);

	return timerCount + rfc_InitialTimerID;
}

KTimer* KIDGenerator::getTimerByID(UINT id)
{
	return timerList.get(id - (rfc_InitialTimerID + 1));
}

KIDGenerator::~KIDGenerator() {}