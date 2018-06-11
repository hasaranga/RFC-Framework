
/*
	RFC - KBitmap.cpp
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

#include "../utils/KPlatformUtil.h"
#include "../graphics/KBitmap.h"

KBitmap::KBitmap()
{
	hBitmap = 0;
	appHInstance = KPlatformUtil::GetInstance()->GetAppHInstance();
}

bool KBitmap::LoadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(appHInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool KBitmap::LoadFromFile(const KString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(appHInstance, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

void KBitmap::DrawOnHDC(HDC hdc, int x, int y, int width, int height)
{
	HDC memHDC = CreateCompatibleDC(hdc);

	SelectObject(memHDC, hBitmap);
	BitBlt(hdc, x, y, width, height, memHDC, 0, 0, SRCCOPY);

	DeleteDC(memHDC);
}

HBITMAP KBitmap::GetHandle()
{
	return hBitmap;
}

KBitmap::~KBitmap()
{
	if(hBitmap)
		::DeleteObject(hBitmap);
}