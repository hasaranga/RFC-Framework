
/*
	RFC - KWindowTypes.h
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

#ifndef _RFC_KWINDOWTYPES_H_
#define _RFC_KWINDOWTYPES_H_

#include "KWindow.h"

class RFC_API KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual ~KHotPluggedDialog();
};

class RFC_API KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow();

	virtual ~KOverlappedWindow();
};

class RFC_API KFrame : public KWindow
{
public:
	KFrame();

	virtual ~KFrame();
};

class RFC_API KDialog : public KWindow
{
public:
	KDialog();

	virtual ~KDialog();
};

class RFC_API KToolWindow : public KWindow
{
public:
	KToolWindow();

	virtual ~KToolWindow();
};

#endif