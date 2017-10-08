
/*
    RFC - KIcon.cpp
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

#include "../utils/KPlatformUtil.h"
#include "../graphics/KIcon.h"

KIcon::KIcon()
{
	hIcon = 0;
	appHInstance = KPlatformUtil::GetInstance()->GetAppHInstance();
}

bool KIcon::LoadFromResource(WORD resourceID)
{
	hIcon = (HICON)::LoadImageW(appHInstance, MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

bool KIcon::LoadFromFile(const KString& filePath)
{
	hIcon = (HICON)::LoadImageW(appHInstance, (const wchar_t*)filePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

HICON KIcon::GetHandle()
{
	return hIcon;
}

KIcon::~KIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}