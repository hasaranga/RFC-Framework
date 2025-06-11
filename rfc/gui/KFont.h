
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

#pragma once

#include "../core/CoreModule.h"

/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class KFont
{
protected:
	HFONT hFont;
	bool customFont;
	KString fontFace;
	int fontSizeFor96DPI;
	int currentDPI;
	bool isBold;
	bool isItalic;
	bool isUnderline;
	bool isAntiAliased;

public:
	/**
		Constructs default system Font object.
	*/
	KFont();

	/**
		Constructs custom font object.
		sizeFor96DPI = 14
		antiAliased = false, 
		requiredDPI = USER_DEFAULT_SCREEN_DPI
	*/
	KFont(const KString& face, int sizeFor96DPI, bool bold = false,
		bool italic = false, bool underline = false, bool antiAliased = true, int requiredDPI = USER_DEFAULT_SCREEN_DPI);

	// destroys the existing font handle.
	virtual bool load(const KString& face, int sizeFor96DPI, bool bold = false,
		bool italic = false, bool underline = false, bool antiAliased = true, int requiredDPI = USER_DEFAULT_SCREEN_DPI);

	virtual void setDPI(int newDPI);

	/**
		If you want to use system default font, then use this static method. Do not delete the returned instance!
	*/
	static KFont* getDefaultFont();

	virtual bool isDefaultFont();

	/**
		Load a font from a file. loaded font only available to this application.
		make sure to call removePrivateFont when done.
	*/
	static bool loadPrivateFont(const KString& path);

	static void removePrivateFont(const KString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT getFontHandle();

	operator HFONT()const;

	virtual ~KFont();

	// Delete copy/move operations to prevent duplication
	KFont(const KFont&) = delete;
	KFont& operator=(const KFont&) = delete;
	KFont(KFont&&) = delete;
	KFont& operator=(KFont&&) = delete;

private:
	RFC_LEAK_DETECTOR(KFont)
};

