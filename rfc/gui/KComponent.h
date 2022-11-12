
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

#pragma once

#include "../core/CoreModule.h"
#include "KFont.h"
#include "KCursor.h"

/**
	Base class of all W32 gui objects.
*/
class KComponent
{
protected:
	KString compClassName;
	KString compText;
	HWND compHWND;
	HWND compParentHWND;
	DWORD compDwStyle;
	DWORD compDwExStyle;
	UINT compCtlID;
	int compX;
	int compY;
	int compWidth;
	int compHeight;
	int compDPI;
	bool compVisible;
	bool compEnabled;
	bool isRegistered;
	KFont *compFont;
	KCursor *cursor;

public:
	WNDCLASSEXW wc;

	/**
		Constructs a standard win32 component.
		@param generateWindowClassDetails	set to false if you are not registering window class and using standard class name like BUTTON, STATIC etc... wc member is invalid if generateWindowClassDetails is false.
	*/
	KComponent(bool generateWindowClassDetails);
	
	/**
		Returns HWND of this component
	*/
	operator HWND()const;

	/**
		Called after hotplugged into a given HWND.
	*/
	virtual void OnHotPlug();

	/**
		HotPlugs given HWND. this method does not update current compFont and cursor variables.
		Set fetchInfo to true if you want to acquire all the information about this HWND. (width, height, position etc...)
		Set fetchInfo to false if you just need to receive events. (button click etc...)
	*/
	virtual void HotPlugInto(HWND component, bool fetchInfo = true);

	/**
		Sets mouse cursor of this component.
	*/
	virtual void SetMouseCursor(KCursor *cursor);

	/**
		@returns autogenerated unique class name for this component
	*/
	virtual KString GetComponentClassName();

	/**
		Registers the class name and creates the component. 
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		@returns false if registration failed or component creation failed.
	*/
	virtual bool Create(bool requireInitialMessages = false);

	virtual void Destroy();

	/**
		Handles internal window messages. (subclassed window proc)
		Important: Pass unprocessed messages to parent if you override this method.
	*/
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
		Receives messages like WM_COMMAND, WM_NOTIFY, WM_DRAWITEM from the parent window. (if it belongs to this component)
		Pass unprocessed messages to parent if you override this method.
		@returns true if msg processed.
	*/
	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	/**
		Identifier of the child component which can be used with WM_MEASUREITEM like messages.
		@returns zero for top level windows
	*/
	virtual UINT GetControlID();

	/**
		Sets font of this component
	*/
	virtual void SetFont(KFont *compFont);

	/**
		Returns font of this component
	*/
	virtual KFont* GetFont();

	/**
		Returns caption of this component
	*/
	virtual KString GetText();

	/**
		Sets caption of this component
	*/
	virtual void SetText(const KString& compText);

	virtual void SetHWND(HWND compHWND);

	/**
		Returns HWND of this component
	*/
	virtual HWND GetHWND();

	/**
		Changes parent of this component
	*/
	virtual void SetParentHWND(HWND compParentHWND);

	/**
		Returns parent of this component
	*/
	virtual HWND GetParentHWND();

	/**
		Returns style of this component
	*/
	virtual DWORD GetStyle();

	/**
		Sets style of this component
	*/
	virtual void SetStyle(DWORD compStyle);

	/**
		Returns exstyle of this component
	*/
	virtual DWORD GetExStyle();

	/**
		Sets exstyle of this component
	*/
	virtual void SetExStyle(DWORD compExStyle);

	/**
		Returns x position of this component which is relative to parent component.
	*/
	virtual int GetX();

	/**
		Returns y position of this component which is relative to parent component.
	*/
	virtual int GetY();

	/**
		Returns width of the component.
	*/
	virtual int GetWidth();

	/**
		Returns height of the component.
	*/
	virtual int GetHeight();

	virtual int GetDPI();

	/**
		Sets width and height of the component.
	*/
	virtual void SetSize(int compWidth, int compHeight);

	/**
		Sets x and y position of the component. x and y are relative to parent component
	*/
	virtual void SetPosition(int compX, int compY);

	virtual void SetDPI(int newDPI);

	/**
		Sets visible state of the component
	*/
	virtual void SetVisible(bool state);

	/**
		Returns visible state of the component
	*/
	virtual bool IsVisible();

	/**
		Returns the component is ready for user input or not
	*/
	virtual bool IsEnabled();

	/**
		Sets component's user input reading state
	*/
	virtual void SetEnabled(bool state);

	/**
		Brings component to front
	*/
	virtual void BringToFront();

	/**
		Grabs keyboard focus into this component
	*/
	virtual void SetKeyboardFocus();

	/**
		Repaints the component
	*/
	virtual void Repaint();

	virtual ~KComponent();

private:
	RFC_LEAK_DETECTOR(KComponent)
};


// macros to handle window messages

#define BEGIN_KMSG_HANDLER \
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
	{\
	switch(msg)\
	{

#define ON_KMSG(_KMsg,_KMsgHandler) \
	case _KMsg: return _KMsgHandler(wParam,lParam);

#define END_KMSG_HANDLER(_KComponentParentClass) \
	default: return _KComponentParentClass::WindowProc(hwnd,msg,wParam,lParam); \
	}\
	}
