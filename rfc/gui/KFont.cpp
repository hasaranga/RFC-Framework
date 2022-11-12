
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

#include "KFont.h"

KFont* KFont::defaultInstance=0;

KFont::KFont()
{
	hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	customFont = false;
}

KFont::KFont(const KString& face, int sizeFor96DPI, bool bold, bool italic, bool underline, bool antiAliased, int requiredDPI)
{
	this->fontFace = face;
	this->fontSizeFor96DPI = sizeFor96DPI;
	this->isBold = bold;
	this->isItalic = italic;
	this->isUnderline = underline;
	this->isAntiAliased = antiAliased;
	this->currentDPI = requiredDPI;

	hFont = ::CreateFontW(fontSizeFor96DPI * requiredDPI / USER_DEFAULT_SCREEN_DPI, 
		0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, underline ? TRUE : FALSE, 0, DEFAULT_CHARSET,
		0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, face);

	if(hFont)
		customFont = true;
}

void KFont::SetDPI(int newDPI)
{
	if( customFont && (currentDPI != newDPI) )
	{
		::DeleteObject(hFont);
		hFont = ::CreateFontW(fontSizeFor96DPI * newDPI / USER_DEFAULT_SCREEN_DPI, 0, 0, 0, isBold ? FW_BOLD : FW_NORMAL,
			isItalic ? TRUE : FALSE, isUnderline ? TRUE : FALSE, 0, DEFAULT_CHARSET, 0, 0,
			isAntiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, 
			fontFace);

		currentDPI = newDPI;
	}
}

KFont* KFont::GetDefaultFont()
{
	if(KFont::defaultInstance == 0)
		KFont::defaultInstance = new KFont();

	return KFont::defaultInstance;
}

void KFont::DeleteDefaultFont()
{
	if (KFont::defaultInstance)
	{
		delete KFont::defaultInstance;
		KFont::defaultInstance = 0;
	}
}

bool KFont::IsDefaultFont()
{
	return !customFont;
}

bool KFont::LoadFont(const KString& path)
{
	return (::AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
}

void KFont::RemoveFont(const KString& path)
{
	::RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT KFont::GetFontHandle()
{
	return hFont;
}

KFont::operator HFONT()const
{
	return hFont;
}

KFont::~KFont()
{
	if(customFont)
		::DeleteObject(hFont);
}