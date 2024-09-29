
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

#include "Architecture.h"
#include "Core.h"
#include "KApplication.h"
#include "KDPIUtility.h"
#include "KLeakDetector.h"
#include "KModuleManager.h"
#include "KString.h"
#include <windows.h>

// link default libs here so we don't need to link them from commandline(Clang).

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Comdlg32.lib")

class CoreModuleInitParams {
public:
	/**
		If you are in EXE, then hInstance is HINSTANCE provided by WinMain. Or you can use zero.
		If you are in DLL, then hInstance is HINSTANCE provided by DllMain or HMODULE of the DLL. Or you can use zero.
		If you are in Console app, then use zero.
		The defalut value is zero.
	*/
	static HINSTANCE hInstance;

	static bool initCOMAsSTA; // initializes COM as STA. default value is true
	static KDPIAwareness dpiAwareness; // default value is UNAWARE_MODE
};