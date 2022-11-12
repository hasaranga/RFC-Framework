
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

#include "GUIModule.h"
#include <commctrl.h>

class RFC_GUIModule 
{
public:
	static bool RFCModuleInit()
	{		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);
		
		KGUIProc::AtomComponent = ::GlobalAddAtomW(L"RFCComponent");
		KGUIProc::AtomOldProc = ::GlobalAddAtomW(L"RFCOldProc");

		return true;
	}

	static void RFCModuleFree()
	{
		::GlobalDeleteAtom(KGUIProc::AtomComponent);
		::GlobalDeleteAtom(KGUIProc::AtomOldProc);

		// delete singletons
		KFont::DeleteDefaultFont();
		delete KIDGenerator::GetInstance();
	}
};

REGISTER_RFC_MODULE(1, RFC_GUIModule)