
/*
	Copyright (C) 2013-2026 CrownSoft
  
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

// copy supported font definition.
class KFontType
{
public:
	KString fontFace;
	Logical fontSize;
	bool isBold;
	bool isItalic;
	bool isUnderline;
	bool isAntiAliased;
	bool isVertical;

	bool compare(const KFontType& o) const noexcept
	{
		if (fontSize != o.fontSize)    
			return false;
		if (isBold != o.isBold)      
			return false;
		if (isItalic != o.isItalic)    
			return false;
		if (isUnderline != o.isUnderline) 
			return false;
		if (isAntiAliased != o.isAntiAliased) 
			return false;
		if (isVertical != o.isVertical)  
			return false;
		return fontFace.compare(o.fontFace); // compare string finally.
	}

	KFontType() noexcept : fontFace(L"Segoe UI"), fontSize(14),
		isBold(false), isItalic(false), isUnderline(false), 
		isAntiAliased(true), isVertical(false){}

	KFontType(KString fontFace, Logical fontSize, bool isBold = false,
		bool isItalic = false, bool isUnderline = false, bool isAntiAliased = true, bool isVertical = false) noexcept:
		fontFace(fontFace), fontSize(fontSize), isBold(isBold), isItalic(isItalic), isUnderline(isUnderline),
		isAntiAliased(isAntiAliased), isVertical(isVertical) {}
	
	~KFontType() noexcept {}
};

class KFontHandle
{
private:
	HFONT hFont;
	volatile LONG refCount;

	~KFontHandle() noexcept
	{
		::DeleteObject(hFont);
	}
public:
	KFontHandle(HFONT hFont) noexcept : refCount(1), hFont(hFont) {}

	HFONT get() noexcept { return hFont; }

	void addRef() noexcept
	{
		::InterlockedIncrement(&refCount);
	}

	void release() noexcept
	{
		const LONG res = ::InterlockedDecrement(&refCount);
		if (res == 0)
			delete this;
	}

	KFontHandle(const KFontHandle&) = delete;
	KFontHandle& operator=(const KFontHandle&) = delete;
	KFontHandle(KFontHandle&&) = delete;
	KFontHandle& operator=(KFontHandle&&) = delete;
};

class KFontCache
{
private:
	struct Entry {
		KFontType type;
		int dpi;
		KFontHandle* handle;
	};

	static constexpr int MAX_ENTRIES = 20;
	static Entry entries[MAX_ENTRIES];
	static int count;

	static KFontHandle* createFontHandle(const KFontType& type, int dpi) noexcept
	{
		HFONT hFont = ::CreateFontW(type.fontSize * dpi / USER_DEFAULT_SCREEN_DPI,
			0, (type.isVertical ? 900 : 0), (type.isVertical ? 900 : 0), type.isBold ? FW_BOLD : FW_NORMAL,
			type.isItalic ? TRUE : FALSE, type.isUnderline ? TRUE : FALSE, 0, DEFAULT_CHARSET,
			0, 0, type.isAntiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, 
			VARIABLE_PITCH | FF_DONTCARE, type.fontFace);
		return new KFontHandle(hFont);
	}
public:

	/**
		load a font from a file. loaded font only available to this application.
		make sure to call removePrivateFont when done.
	*/
	static bool loadPrivateFont(const KString& path) noexcept
	{
		return (::AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
	}

	static void removePrivateFont(const KString& path) noexcept
	{
		::RemoveFontResourceExW(path, FR_PRIVATE, 0);
	}

	static KFontHandle* getFontHandle(const KFontType& type, int dpi) noexcept
	{
		// search
		for (int i = 0; i < count; ++i)
		{
			if (entries[i].dpi == dpi && entries[i].type.compare(type))
			{
				entries[i].handle->addRef();
				return entries[i].handle;
			}
		}

		// not found, let's create!
		KFontHandle* handle = createFontHandle(type, dpi);

		// cache is full. we don't store
		if (count >= MAX_ENTRIES)
		{
			DEBUG_PRINT("KFontCache is full!");
			return handle;
		}

		entries[count++] = { type, dpi, handle };
		handle->addRef();
		return handle;
	}

	// call to remove unused fonts or at framework shutdown.
	static void cleanup() noexcept
	{
		for (int i = 0; i < count; ++i)
		{
			entries[i].handle->release();
		}
		count = 0;
	}
};

// act as KFont.
// manages font type, font handle & KFontCache interactions.
class KFontReference
{
private:
	KFontType fontType;
	KFontHandle* fontHandle;
	int dpi;

public:
	KFontReference() noexcept
	{
		dpi = USER_DEFAULT_SCREEN_DPI;
		fontHandle = KFontCache::getFontHandle(fontType, dpi);
	}

	KFontReference(const KFontType& fontType, int dpi) noexcept
	{
		this->fontType = fontType;
		this->dpi = dpi;
		fontHandle = KFontCache::getFontHandle(fontType, dpi);
	}

	HFONT getFontHandle() noexcept
	{
		return fontHandle->get();
	}

	KFontType getFontType() noexcept
	{
		return fontType;
	}

	int getCurrentDPI() noexcept { return dpi; }

	void update(int newDPI) noexcept
	{
		if (dpi == newDPI)
			return;

		dpi = newDPI;

		if (fontHandle)
			fontHandle->release();

		fontHandle = KFontCache::getFontHandle(fontType, newDPI);
	}

	void update(const KFontType& fontType, int dpi) noexcept
	{
		if (fontHandle)
			fontHandle->release();

		this->fontType = fontType;
		this->dpi = dpi;
		fontHandle = KFontCache::getFontHandle(fontType, dpi);
	}

	~KFontReference() noexcept
	{
		if (fontHandle)
		{
			fontHandle->release();
			fontHandle = nullptr;
		}
	}

	KFontReference(const KFontReference&) = delete;
	KFontReference& operator=(const KFontReference&) = delete;
};