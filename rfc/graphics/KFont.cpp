
/*
    RFC - KFont.cpp
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

#include"KFont.h"

KFont* KFont::defaultInstance=0;

KFont::KFont()
{
	hFont=(HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	customFont=false;
}

KFont::KFont(const KString& face, int size, bool bold, bool italic, bool antiAliased)
{
	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, 0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, (const wchar_t*)face);
	if(hFont)
		customFont=true;
}

KFont* KFont::GetDefaultFont()
{
	if(KFont::defaultInstance)
		return KFont::defaultInstance;
	KFont::defaultInstance=new KFont();
	return KFont::defaultInstance;
}

bool KFont::LoadFont(const KString& path)
{
	return AddFontResourceExW(path, FR_PRIVATE, 0) == 0 ? false : true;
}

void KFont::RemoveFont(const KString& path)
{
	RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT KFont::GetFontHandle()
{
	return hFont;
}

KFont::~KFont()
{
	if(customFont)
		::DeleteObject(hFont);
}