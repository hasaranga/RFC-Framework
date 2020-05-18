
// ========== Generated With RFC Amalgamator v1.0 ==========


#include"rfc_amalgamated.h"


// =========== KApplication.cpp ===========

/*
	RFC - KApplication.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


HINSTANCE KApplication::hInstance = 0;

KApplication::KApplication()
{
}

int KApplication::Main(KString **argv, int argc)
{
	return 0;
}

bool KApplication::AllowMultipleInstances()
{
	return true;
}

int KApplication::AnotherInstanceIsRunning(KString **argv, int argc)
{
	return 0;
}

const wchar_t* KApplication::GetApplicationID()
{
	return L"RFC_APPLICATION";
}

KApplication::~KApplication()
{
}

// =========== rfc.cpp ===========

/*
	RFC - rfc.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

#include <process.h>

class InternalVariables
{
public:
	static KComponent *currentComponent;
	static HHOOK wnd_hook;
	static volatile int rfcRefCount;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	static CRITICAL_SECTION csForCurrentComponent; // guard currentComponent!
	#endif
};

KComponent*	InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
volatile int InternalVariables::rfcRefCount = 0;

#ifndef RFC_SINGLE_THREAD_COMP_CREATION
CRITICAL_SECTION InternalVariables::csForCurrentComponent;
#endif

ATOM InternalDefinitions::RFCPropAtom_Component;
ATOM InternalDefinitions::RFCPropAtom_OldProc;

void AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component)
{
	::SetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component), (HANDLE)component);

	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc), (HANDLE)lpfnOldWndProc);

	if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // sublcass only if window proc is not GlobalWnd_Proc (common control or dialog)
		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...
}

LRESULT CALLBACK RFCCTL_CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode < 0)
		return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

	if(nCode==HCBT_CREATEWND){
		HWND hwnd=(HWND)wParam;
		LPCBT_CREATEWNDW cbtCreateWnd = (LPCBT_CREATEWNDW)lParam;
		if (cbtCreateWnd)
		{
			if (cbtCreateWnd->lpcs)
			{
				if (cbtCreateWnd->lpcs->lpCreateParams == InternalVariables::currentComponent) // only catch what we created. ignore unknown windows.
				{
					::AttachRFCPropertiesToHWND(hwnd, InternalVariables::currentComponent);

					// Call the next hook, if there is one
					const LRESULT result = ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(InternalVariables::wnd_hook); // unhooking at here will allow child creation at WM_CREATE. otherwise this will hook child also!

					return result;
				}
			}
		}
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK GlobalWnd_Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	KComponent *component = (KComponent*)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));

	if(!component) // for safe!
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );

	if(!component->GetHWND()) // window recieve msg for the first time!
		component->SetHWND(hwnd);

	if(msg == WM_NCDESTROY){
		::RemovePropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc));
		::RemovePropW(hwnd,  MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc));

		if (lpfnOldWndProc)
		{
			if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // common control or dialog
			{
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
				return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
			}
		}
		 // lpfnOldWndProc is not set or window or custom control
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );		
	}

	return component->WindowProc(hwnd, msg, wParam, lParam);
}

HWND CreateRFCComponent(KComponent* component, bool requireInitialMessages)
{	
	if (requireInitialMessages)
	{
		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::EnterCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		InternalVariables::currentComponent = component;

		// install hook to get called before WM_CREATE_WINDOW msg!
		InternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &RFCCTL_CBTProc, 0, ::GetCurrentThreadId());

		// pass current component as lpParam. so CBT proc can ignore other unknown windows.
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KApplication::hInstance, (LPVOID)component);

		// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc.
		//::UnhookWindowsHookEx(InternalVariables::wnd_hook);

		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::LeaveCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		return hwnd;
	}
	else
	{
		HWND hwnd = ::CreateWindowExW(component->GetExStyle(), component->GetComponentClassName(), component->GetText(), component->GetStyle(), component->GetX(), component->GetY(), component->GetWidth(), component->GetHeight(), component->GetParentHWND(), (HMENU)component->GetControlID(), KApplication::hInstance, 0);
		::AttachRFCPropertiesToHWND(hwnd, component);
		component->SetHWND(hwnd);

		return hwnd;
	}
}

void DoMessagePump(bool handleTabKey)
{
	MSG msg;
	
	while (::GetMessageW(&msg, NULL, 0, 0)) 
	{
		if(handleTabKey)
		{
			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	} 
}

unsigned __stdcall GlobalThread_Proc(void* lpParameter)
{
	if(lpParameter == 0) // for safe!
		return 0;

	KThread* thread = (KThread*)lpParameter;
	thread->Run();

	return 0;	
}

bool CreateRFCThread(KThread* thread)
{
	if(thread)
	{
		// create thread in suspended state. so we can set the handle field.
		HANDLE handle = (HANDLE)::_beginthreadex(NULL, 0, GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);
		//HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);

		if (handle)
		{
			thread->SetHandle(handle);
			::ResumeThread(handle);

			return true;
		}
	}
	return false;
}

int HotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return (int)::DialogBoxParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component)
{
	return ::CreateDialogParamW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), parentHwnd, ::GlobalDlg_Proc, (LPARAM)component);
}

INT_PTR CALLBACK GlobalDlg_Proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG)
	{
		KComponent* comp = (KComponent*)lParam;
		if(comp)
			comp->HotPlugInto(hwndDlg, true);

		return FALSE;
	}
	return FALSE;
}

void InitRFC(HINSTANCE hInstance)
{
	if (!InternalVariables::rfcRefCount)
	{
		if (!hInstance)
		{
			//hInstance = ::GetModuleHandleW(NULL); // not work for dll
			::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&InitRFC, &hInstance);
		}

		KApplication::hInstance = hInstance;

		KPlatformUtil::GetInstance(); // create instance for first time
		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);

		::CoInitialize(NULL); //Initializes COM as STA.

		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::InitializeCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		InternalDefinitions::RFCPropAtom_Component = ::GlobalAddAtomW(L"RFCComponent");
		InternalDefinitions::RFCPropAtom_OldProc = ::GlobalAddAtomW(L"RFCOldProc");
	}
	++InternalVariables::rfcRefCount;
}

void DeInitRFC()
{
	RFC_INIT_VERIFIER;

	--InternalVariables::rfcRefCount;
	if (!InternalVariables::rfcRefCount)
	{
		::CoUninitialize();

		#ifndef RFC_SINGLE_THREAD_COMP_CREATION
		::DeleteCriticalSection(&InternalVariables::csForCurrentComponent);
		#endif

		::GlobalDeleteAtom(InternalDefinitions::RFCPropAtom_Component);
		::GlobalDeleteAtom(InternalDefinitions::RFCPropAtom_OldProc);

		// delete all singletons!
		delete KFont::GetDefaultFont();
		delete KPlatformUtil::GetInstance();
	}
}

// =========== sha1.cpp ===========
/*
	100% free public domain implementation of the SHA-1 algorithm
	by Dominik Reichl <dominik.reichl@t-online.de>
	Web: http://www.dominik-reichl.de/

	Version 1.7 - 2006-12-21
	- Fixed buffer underrun warning which appeared when compiling with
	  Borland C Builder (thanks to Rex Bloom and Tim Gallagher for the
	  patch)
	- Breaking change: ReportHash writes the final hash to the start
	  of the buffer, i.e. it's not appending it to the string any more
	- Made some function parameters const
	- Added Visual Studio 2005 project files to demo project

	Version 1.6 - 2005-02-07 (thanks to Howard Kapustein for patches)
	- You can set the endianness in your files, no need to modify the
	  header file of the CSHA1 class any more
	- Aligned data support
	- Made support/compilation of the utility functions (ReportHash
	  and HashFile) optional (useful when bytes count, for example in
	  embedded environments)

	Version 1.5 - 2005-01-01
	- 64-bit compiler compatibility added
	- Made variable wiping optional (define SHA1_WIPE_VARIABLES)
	- Removed unnecessary variable initializations
	- ROL32 improvement for the Microsoft compiler (using _rotl)

	======== Test Vectors (from FIPS PUB 180-1) ========

	SHA1("abc") =
		A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D

	SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") =
		84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1

	SHA1(A million repetitions of "a") =
		34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/


using namespace ExtLibs;

#ifdef SHA1_UTILITY_FUNCTIONS
#define SHA1_MAX_FILE_BUFFER 8000
#endif

// Rotate x bits to the left
#ifndef ROL32
#ifdef _MSC_VER
#define ROL32(_val32, _nBits) _rotl(_val32, _nBits)
#else
#define ROL32(_val32, _nBits) (((_val32)<<(_nBits))|((_val32)>>(32-(_nBits))))
#endif
#endif

#ifdef SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (m_block->l[i] = \
	(ROL32(m_block->l[i],24) & 0xFF00FF00) | (ROL32(m_block->l[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (m_block->l[i])
#endif

#define SHABLK(i) (m_block->l[i&15] = ROL32(m_block->l[(i+13)&15] ^ m_block->l[(i+8)&15] \
	^ m_block->l[(i+2)&15] ^ m_block->l[i&15],1))

// SHA-1 rounds
#define _R0(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R1(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R2(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5); w=ROL32(w,30); }
#define _R3(v,w,x,y,z,i) { z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5); w=ROL32(w,30); }
#define _R4(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5); w=ROL32(w,30); }

CSHA1::CSHA1()
{
	m_block = (SHA1_WORKSPACE_BLOCK*)m_workspace;

	Reset();
}

CSHA1::~CSHA1()
{
	Reset();
}

void CSHA1::Reset()
{
	// SHA1 initialization constants
	m_state[0] = 0x67452301;
	m_state[1] = 0xEFCDAB89;
	m_state[2] = 0x98BADCFE;
	m_state[3] = 0x10325476;
	m_state[4] = 0xC3D2E1F0;

	m_count[0] = 0;
	m_count[1] = 0;
}

void CSHA1::Transform(UINT_32* pState, const UINT_8* pBuffer)
{
	UINT_32 a = pState[0], b = pState[1], c = pState[2], d = pState[3], e = pState[4];

	memcpy(m_block, pBuffer, 64);

	// 4 rounds of 20 operations each. Loop unrolled.
	_R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
	_R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
	_R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
	_R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
	_R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
	_R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
	_R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
	_R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
	_R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
	_R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
	_R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
	_R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
	_R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
	_R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
	_R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
	_R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
	_R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
	_R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
	_R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
	_R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

	// Add the working vars back into state
	pState[0] += a;
	pState[1] += b;
	pState[2] += c;
	pState[3] += d;
	pState[4] += e;

	// Wipe variables
#ifdef SHA1_WIPE_VARIABLES
	a = b = c = d = e = 0;
#endif
}

// Use this function to hash in binary data and strings
void CSHA1::Update(const UINT_8* pData, UINT_32 uLen)
{
	UINT_32 i, j;

	j = (m_count[0] >> 3) & 63;

	if((m_count[0] += (uLen << 3)) < (uLen << 3))
		m_count[1]++;

	m_count[1] += (uLen >> 29);

	if((j + uLen) > 63)
	{
		i = 64 - j;
		memcpy(&m_buffer[j], pData, i);
		Transform(m_state, m_buffer);

		for( ; (i + 63) < uLen; i += 64)
			Transform(m_state, &pData[i]);

		j = 0;
	}
	else i = 0;

	if((uLen - i) != 0)
		memcpy(&m_buffer[j], &pData[i], uLen - i);
}

#ifdef SHA1_UTILITY_FUNCTIONS
// Hash in file contents
bool CSHA1::HashFile(const char* szFileName)
{
	unsigned long ulFileSize, ulRest, ulBlocks;
	unsigned long i;
	UINT_8 uData[SHA1_MAX_FILE_BUFFER];
	FILE* fIn;

	if(szFileName == NULL) return false;

	fIn = fopen(szFileName, "rb");
	if(fIn == NULL) return false;

	fseek(fIn, 0, SEEK_END);
	ulFileSize = (unsigned long)ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	if(ulFileSize != 0)
	{
		ulBlocks = ulFileSize / SHA1_MAX_FILE_BUFFER;
		ulRest = ulFileSize % SHA1_MAX_FILE_BUFFER;
	}
	else
	{
		ulBlocks = 0;
		ulRest = 0;
	}

	for(i = 0; i < ulBlocks; i++)
	{
		fread(uData, 1, SHA1_MAX_FILE_BUFFER, fIn);
		Update((UINT_8*)uData, SHA1_MAX_FILE_BUFFER);
	}

	if(ulRest != 0)
	{
		fread(uData, 1, ulRest, fIn);
		Update((UINT_8*)uData, ulRest);
	}

	fclose(fIn); fIn = NULL;
	return true;
}
#endif

void CSHA1::Final()
{
	UINT_32 i;
	UINT_8 finalcount[8];

	for(i = 0; i < 8; i++)
		finalcount[i] = (UINT_8)((m_count[((i >= 4) ? 0 : 1)]
			>> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

	Update((UINT_8*)"\200", 1);

	while ((m_count[0] & 504) != 448)
		Update((UINT_8*)"\0", 1);

	Update(finalcount, 8); // Cause a SHA1Transform()

	for(i = 0; i < 20; i++)
		m_digest[i] = (UINT_8)((m_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);

	// Wipe variables for security reasons
#ifdef SHA1_WIPE_VARIABLES
	memset(m_buffer, 0, 64);
	memset(m_state, 0, 20);
	memset(m_count, 0, 8);
	memset(finalcount, 0, 8);
	Transform(m_state, m_buffer);
#endif
}

#ifdef SHA1_UTILITY_FUNCTIONS
// Get the final hash as a pre-formatted string
void CSHA1::ReportHash(char* szReport, unsigned char uReportType) const
{
	unsigned char i;
	char szTemp[16];

	if(szReport == NULL) return;

	if(uReportType == REPORT_HEX)
	{
		sprintf(szTemp, "%02X", m_digest[0]);
		strcpy(szReport, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf(szTemp, "%02X", m_digest[i]);
			strcat(szReport, szTemp);
		}
	}
	else if(uReportType == REPORT_DIGIT)
	{
		sprintf(szTemp, "%u", m_digest[0]);
		strcpy(szReport, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf(szTemp, " %u", m_digest[i]);
			strcat(szReport, szTemp);
		}
	}
	else strcpy(szReport, "Error: Unknown report type!");
}
#endif

// Get the raw message digest
void CSHA1::GetHash(UINT_8* puDest) const
{
	memcpy(puDest, m_digest, 20);
}

// =========== KBitmap.cpp ===========

/*
	RFC - KBitmap.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KBitmap::KBitmap()
{
	RFC_INIT_VERIFIER;
	hBitmap = 0;
}

KBitmap::operator HBITMAP()const
{
	return hBitmap;
}

bool KBitmap::LoadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool KBitmap::LoadFromFile(const KString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(KApplication::hInstance, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
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

// =========== KCursor.cpp ===========

/*
	RFC - KCursor.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KCursor::KCursor()
{
	RFC_INIT_VERIFIER;
	hCursor = 0;
}

bool KCursor::LoadFromResource(WORD resourceID)
{
	hCursor = (HCURSOR)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

bool KCursor::LoadFromFile(const KString& filePath)
{
	hCursor = (HCURSOR)::LoadImageW(KApplication::hInstance, filePath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

HCURSOR KCursor::GetHandle()
{
	return hCursor;
}

KCursor::operator HCURSOR()const
{
	return hCursor;
}

KCursor::~KCursor()
{
	if(hCursor)
		::DestroyCursor(hCursor);
}

// =========== KFont.cpp ===========

/*
	RFC - KFont.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KFont* KFont::defaultInstance=0;

KFont::KFont()
{
	RFC_INIT_VERIFIER;
	hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	customFont = false;
}

KFont::KFont(const KString& face, int size, bool bold, bool italic, bool antiAliased)
{
	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, 0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE, face);
	if(hFont)
		customFont = true;
}

KFont* KFont::GetDefaultFont()
{
	if(KFont::defaultInstance)
		return KFont::defaultInstance;
	KFont::defaultInstance = new KFont();
	return KFont::defaultInstance;
}

bool KFont::LoadFont(const KString& path)
{
	return (AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
}

void KFont::RemoveFont(const KString& path)
{
	RemoveFontResourceExW(path, FR_PRIVATE, 0);
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

// =========== KGraphics.cpp ===========

/*
	RFC - KGraphics.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KGraphics::KGraphics(){}

KGraphics::~KGraphics(){}

void KGraphics::Draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	KGraphics::Draw3dRect(hdc, startX, startY, 2, height, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	KGraphics::Draw3dRect(hdc, startX, startY, width, 2, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void KGraphics::Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void KGraphics::Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	KGraphics::FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	KGraphics::FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	KGraphics::FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void KGraphics::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	KGraphics::FillSolidRect(hdc, &rect, color);
}

void KGraphics::FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	const COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

RECT KGraphics::CalculateTextSize(const wchar_t *text, HFONT hFont)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = {0, 0, 0, 0};

	::DrawText(hDC, text, ::lstrlenW(text), &sz, DT_CALCRECT | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz;
}

// =========== KIcon.cpp ===========

/*
	RFC - KIcon.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KIcon::KIcon()
{
	RFC_INIT_VERIFIER;
	hIcon = 0;
}

bool KIcon::LoadFromResource(WORD resourceID)
{
	hIcon = (HICON)::LoadImageW(KApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

bool KIcon::LoadFromFile(const KString& filePath)
{
	hIcon = (HICON)::LoadImageW(KApplication::hInstance, filePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

HICON KIcon::GetHandle()
{
	return hIcon;
}

KIcon::operator HICON()const
{
	return hIcon;
}

KIcon::~KIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}

// =========== KButton.cpp ===========

/*
	RFC - KButton.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KButton::KButton() : KComponent(false)
{
	listener = 0;

	compClassName.AssignStaticText(TXT_WITH_LEN("BUTTON"));
	compText.AssignStaticText(TXT_WITH_LEN("Button"));

	compWidth = 100;
	compHeight = 30;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY | WS_TABSTOP;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

void KButton::SetListener(KButtonListener *listener)
{
	this->listener = listener;
}

KButtonListener* KButton::GetListener()
{
	return listener;
}

void KButton::OnPress()
{
	if(listener)
		listener->OnButtonPress(this);
}

bool KButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == BN_CLICKED))
	{
		this->OnPress();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KButton::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!

		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KButton::~KButton()
{
}

// =========== KButtonListener.cpp ===========

/*
	RFC - KButtonListener.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KButtonListener::KButtonListener(){}

KButtonListener::~KButtonListener(){}

void KButtonListener::OnButtonPress(KButton *button){}

// =========== KCheckBox.cpp ===========

/*
	RFC - KCheckBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KCheckBox::KCheckBox()
{
	checked = false;
	compText.AssignStaticText(TXT_WITH_LEN("CheckBox"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_NOTIFY | WS_TABSTOP;
}

bool KCheckBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register BUTTON class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KCheckBox::OnPress()
{
	if(::SendMessageW(compHWND, BM_GETCHECK, 0, 0) == BST_CHECKED)
		checked = true;
	else
		checked = false;

	if(listener)
		listener->OnButtonPress(this);
}

bool KCheckBox::IsChecked()
{
	return checked;
}

void KCheckBox::SetCheckedState(bool state)
{
	checked = state;

	if(compHWND)
		::SendMessageW(compHWND, BM_SETCHECK, checked, 0);
}

KCheckBox::~KCheckBox()
{
}

// =========== KComboBox.cpp ===========

/*
	RFC - KComboBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KComboBox::KComboBox(bool sort) : KComponent(false)
{
	listener = 0;
	selectedItemIndex = -1;

	compClassName.AssignStaticText(TXT_WITH_LEN("COMBOBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = WS_VSCROLL | CBS_DROPDOWNLIST | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;

	if(sort)
		compDwStyle = compDwStyle | CBS_SORT;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	stringList = new KPointerList<KString*>(50);
}

void KComboBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KComboBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, CB_DELETESTRING, index, 0);
}

void KComboBox::RemoveItem(const KString& text)
{
	const int itemIndex = this->GetItemIndex(text);
	if(itemIndex > -1)
		this->RemoveItem(itemIndex);
}

int KComboBox::GetItemIndex(const KString& text)
{
	const int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if(stringList->GetPointer(i)->Compare(text))
				return i;
		}
	}
	return -1;
}

int KComboBox::GetItemCount()
{
	return stringList->GetSize();
}

int KComboBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, CB_GETCURSEL, 0, 0);
		if(index != CB_ERR)
			return index;
	}
	return -1;		
}

KString KComboBox::GetSelectedItem()
{
	const int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

void KComboBox::ClearList()
{
	stringList->DeleteAll(true);
	if(compHWND)
		::SendMessageW(compHWND, CB_RESETCONTENT, 0, 0);
}

void KComboBox::SelectItem(int index)
{
	selectedItemIndex = index;
	if(compHWND)
		::SendMessageW(compHWND, CB_SETCURSEL, index, 0);
}

bool KComboBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if ((msg == WM_COMMAND) && (HIWORD(wParam) == CBN_SELENDOK))
	{
		this->OnItemSelect();

		*result = 0;
		return true;
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KComboBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register COMBOBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(selectedItemIndex > -1)
			::SendMessageW(compHWND, CB_SETCURSEL, selectedItemIndex, 0);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KComboBox::SetListener(KComboBoxListener *listener)
{
	this->listener = listener;
}

void KComboBox::OnItemSelect()
{
	if(listener)
		listener->OnComboBoxItemSelect(this);
}

KComboBox::~KComboBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}



// =========== KComboBoxListener.cpp ===========

/*
	RFC - KComboBoxListener.h
	Copyright (C) 2013-2019 CrownSoft
  
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


KComboBoxListener::KComboBoxListener(){}

KComboBoxListener::~KComboBoxListener(){}

void KComboBoxListener::OnComboBoxItemSelect(KComboBox *comboBox){}

// =========== KCommonDialogBox.cpp ===========

/*
	RFC - KCommonDialogBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif 


const wchar_t* const RFC_OSD_REG_LOCATION = L"Software\\CrownSoft\\RFC\\OSD";

bool KCommonDialogBox::ShowOpenFileDialog(KWindow *window, 
	const KString& title, 
	const wchar_t* filter, 
	KString *fileName, 
	bool saveLastLocation, 
	const wchar_t* dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!	
	wchar_t *buff = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	buff[0] = 0;
 
	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	KString lastLocation;
	if (saveLastLocation)
	{
		KRegistry::ReadString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.GetLength() > 0)
			ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
	}

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetOpenFileNameW(&ofn))
	{
		KString path(buff, KString::FREE_TEXT_WHEN_DONE);
		*fileName = path;

		if (saveLastLocation)
		{
			KString parentDir(KDirectory::GetParentDir(path).AppendStaticText(L"\\", 1, true));

			KRegistry::CreateKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::WriteString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, parentDir);
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

bool KCommonDialogBox::ShowSaveFileDialog(KWindow *window, 
	const KString& title, 
	const wchar_t* filter, 
	KString *fileName,
	bool saveLastLocation,
	const wchar_t* dialogGuid)
{
	// assumes MAX_PATH * 2 is enough!
	wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
	buff[0] = 0;

	OPENFILENAMEW ofn;
	::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	KString lastLocation;
	if (saveLastLocation)
	{		
		KRegistry::ReadString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, &lastLocation);

		if (lastLocation.GetLength() > 0)
			ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
	}

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = (window != NULL) ? window->GetHWND() : NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = buff;
	ofn.nMaxFile = MAX_PATH * 2;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title;

	if(::GetSaveFileNameW(&ofn))
	{
		KString path(buff, KString::FREE_TEXT_WHEN_DONE);
		*fileName = path;

		if (saveLastLocation)
		{
			KString parentDir(KDirectory::GetParentDir(path).AppendStaticText(L"\\", 1, true));

			KRegistry::CreateKey(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION);	// if not exists
			KRegistry::WriteString(HKEY_CURRENT_USER, RFC_OSD_REG_LOCATION, dialogGuid, parentDir);
		}

		return true;
	}
	else
	{
		::free(buff);
		return false;
	}
}

// =========== KComponent.cpp ===========

/*
	RFC - KComponent.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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




KComponent::KComponent(bool generateWindowClassDetails)
{
	RFC_INIT_VERIFIER;
	isRegistered = false;

	KPlatformUtil *platformUtil = KPlatformUtil::GetInstance();
	compCtlID = platformUtil->GenerateControlID();

	compHWND = 0;
	compParentHWND = 0;
	compDwStyle = 0;
	compDwExStyle = 0;
	cursor = 0;
	compX = CW_USEDEFAULT;
	compY = CW_USEDEFAULT;
	compWidth = CW_USEDEFAULT;
	compHeight = CW_USEDEFAULT;
	compVisible = true;
	compEnabled = true;

	if (generateWindowClassDetails)
	{
		compClassName = platformUtil->GenerateClassName();
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = 0;
		wc.lpszMenuName = 0;
		wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIconSm = 0;
		wc.style = 0;
		wc.hInstance = KApplication::hInstance;
		wc.lpszClassName = compClassName;

		wc.lpfnWndProc = ::GlobalWnd_Proc;
	}

	compFont = KFont::GetDefaultFont();
}

KComponent::operator HWND()const
{
	return compHWND;
}

void KComponent::OnHotPlug()
{

}

void KComponent::HotPlugInto(HWND component, bool fetchInfo)
{
	compHWND = component;

	if (fetchInfo)
	{
		wchar_t *clsName = (wchar_t*)::malloc(256 * sizeof(wchar_t));  // assume 256 is enough
		clsName[0] = 0;
		::GetClassNameW(compHWND, clsName, 256);
		compClassName = KString(clsName, KString::FREE_TEXT_WHEN_DONE);

		::GetClassInfoExW(KApplication::hInstance, compClassName, &wc);

		compCtlID = (UINT)::GetWindowLongPtrW(compHWND, GWL_ID);

		RECT rect;
		::GetWindowRect(compHWND, &rect);
		compWidth = rect.right - rect.left;
		compHeight = rect.bottom - rect.top;
		compX = rect.left;
		compY = rect.top;

		compVisible = (::IsWindowVisible(compHWND) ? true : false);
		compEnabled = (::IsWindowEnabled(compHWND) ? true : false);

		compDwStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_STYLE);
		compDwExStyle = (DWORD)::GetWindowLongPtrW(compHWND, GWL_EXSTYLE);

		compParentHWND = ::GetParent(compHWND);

		wchar_t *buff = (wchar_t*)::malloc(256 * sizeof(wchar_t)); // assume 256 is enough
		buff[0] = 0;
		::GetWindowTextW(compHWND, buff, 256);
		compText = KString(buff, KString::FREE_TEXT_WHEN_DONE);
	}

	::AttachRFCPropertiesToHWND(compHWND, (KComponent*)this);	

	this->OnHotPlug();
}

UINT KComponent::GetControlID()
{
	return compCtlID;
}

void KComponent::SetMouseCursor(KCursor *cursor)
{
	this->cursor = cursor;
	if(compHWND)
		::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());
}

KString KComponent::GetComponentClassName()
{
	return compClassName;
}

bool KComponent::Create(bool requireInitialMessages)
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered = true;

	::CreateRFCComponent(this, requireInitialMessages);

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled ? TRUE : FALSE);
		::ShowWindow(compHWND, compVisible ? SW_SHOW : SW_HIDE);

		if(cursor)
			::SetClassLongPtrW(compHWND, GCLP_HCURSOR, (LONG_PTR)cursor->GetHandle());

		return true;
	}
	return false;
}

LRESULT KComponent::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::RFCPropAtom_OldProc));
	if(lpfnOldWndProc)
		if((void*)lpfnOldWndProc != (void*)::GlobalWnd_Proc) // it's a subclassed common control or hot-plugged dialog! RFCOldProc of subclassed control|dialog is not GlobalWnd_Proc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
	return ::DefWindowProcW(hwnd, msg, wParam, lParam); // custom control or window
}

bool KComponent::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	return false;
}

void KComponent::SetFont(KFont *compFont)
{
	this->compFont = compFont;
	if(compHWND)
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0));
}

KFont* KComponent::GetFont()
{
	return compFont;
}

KString KComponent::GetText()
{
	return compText;
}

void KComponent::SetText(const KString& compText)
{
	this->compText = compText;
	if(compHWND)
		::SetWindowTextW(compHWND, this->compText);
}

void KComponent::SetHWND(HWND compHWND)
{
	this->compHWND = compHWND;
}

HWND KComponent::GetHWND()
{
	return compHWND;
}

void KComponent::SetParentHWND(HWND compParentHWND)
{
	this->compParentHWND = compParentHWND;
	if(compHWND)
		::SetParent(compHWND, compParentHWND);
}

HWND KComponent::GetParentHWND()
{
	return compParentHWND;
}

DWORD KComponent::GetStyle()
{
	return compDwStyle;
}

void KComponent::SetStyle(DWORD compStyle)
{
	this->compDwStyle = compStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_STYLE, compStyle);
}

DWORD KComponent::GetExStyle()
{
	return compDwExStyle;
}

void KComponent::SetExStyle(DWORD compDwExStyle)
{
	this->compDwExStyle = compDwExStyle;
	if(compHWND)
		::SetWindowLongPtrW(compHWND, GWL_EXSTYLE, compDwExStyle);
}

int KComponent::GetX()
{
	return compX; 
}

int KComponent::GetY()
{
	return compY;
}

int KComponent::GetWidth()
{
	return compWidth;
}

int KComponent::GetHeight()
{
	return compHeight;
}

void KComponent::SetSize(int compWidth, int compHeight)
{
	this->compWidth = compWidth;
	this->compHeight = compHeight;

	if(compHWND)
		::SetWindowPos(compHWND, 0, 0, 0, compWidth, compHeight, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::SetPosition(int compX, int compY)
{
	this->compX = compX;
	this->compY = compY;

	if(compHWND)
		::SetWindowPos(compHWND, 0, compX, compY, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void KComponent::SetVisible(bool state)
{
	compVisible = state;
	if(compHWND)
		::ShowWindow(compHWND, state ? SW_SHOW : SW_HIDE);
}

bool KComponent::IsVisible()
{
	if (compHWND)
	{
		compVisible = (::IsWindowVisible(compHWND) == TRUE);
		return compVisible;
	}

	return false;
}

bool KComponent::IsEnabled()
{
	if (compHWND)
		compEnabled = (::IsWindowEnabled(compHWND) == TRUE);

	return compEnabled;
}

void KComponent::SetEnabled(bool state)
{
	compEnabled = state;

	if(compHWND)
		::EnableWindow(compHWND, compEnabled);
}

void KComponent::BringToFront()
{
	if(compHWND)
		::BringWindowToTop(compHWND);
}

void KComponent::SetKeyboardFocus()
{
	if(compHWND)
		::SetFocus(compHWND);
}

void KComponent::Repaint()
{
	if (compHWND)
	{
		::InvalidateRect(compHWND, NULL, TRUE);
		::UpdateWindow(compHWND); // instant update
	}
}

KComponent::~KComponent()
{
	if(isRegistered)
		::UnregisterClassW(compClassName, KApplication::hInstance);
}

// =========== KGlyphButton.cpp ===========

/*
	RFC - KGlyphButton.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KGlyphButton::KGlyphButton()
{
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
}

KGlyphButton::~KGlyphButton()
{
}

void KGlyphButton::SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->Repaint();
}

bool KGlyphButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (glyphFont)
	{
		if (msg == WM_NOTIFY)
		{
			if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
			{
				LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)lParam;

				*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

				if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
				{
					*result = CDRF_NOTIFYPOSTPAINT;
				}
				else if (CDDS_POSTPAINT == lpNMCD->dwDrawStage) //  postpaint stage
				{
					const RECT rc = lpNMCD->rc;
					const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED | CDIS_GRAYED)) != 0;

					HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, glyphFont->GetFontHandle());
					const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);
					const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

					RECT rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw glyph

					::SetBkMode(lpNMCD->hdc, oldBkMode);
					::SetTextColor(lpNMCD->hdc, oldTextColor);
					::SelectObject(lpNMCD->hdc, oldFont);

					*result = CDRF_DODEFAULT;
				}

				return true; // indicate that we processed this msg & result is valid.
			}
		}
	}

	return KButton::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KGridView.cpp ===========

/*
	RFC - KGridView.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KGridView::KGridView(bool sortItems) : KComponent(false)
{
	itemCount = 0;
	colCount = 0;
	listener = 0;

	compClassName.AssignStaticText(TXT_WITH_LEN("SysListView32"));

	compWidth = 300;
	compHeight = 200;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
	compDwExStyle = WS_EX_WINDOWEDGE;

	if (sortItems)
		compDwStyle |= LVS_SORTASCENDING;
}

KGridView::~KGridView(){}

void KGridView::SetListener(KGridViewListener *listener)
{
	this->listener = listener;
}

KGridViewListener* KGridView::GetListener()
{
	return listener;
}

void KGridView::InsertRecord(KString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (*columnsData[0]);
	lvi.iItem = itemCount;

	const int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem = { 0 };
		lvItem.iSubItem = i;
		lvItem.pszText = (*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

void KGridView::InsertRecordTo(int rowIndex, KString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (*columnsData[0]);
	lvi.iItem = rowIndex;

	const int row = (int)::SendMessageW(compHWND, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem= { 0 };
		lvItem.iSubItem = i;
		lvItem.pszText = (*columnsData[i]);

		::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

KString KGridView::GetRecordAt(int rowIndex, int columnIndex)
{
	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(compHWND, LVM_GETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
}

int KGridView::GetSelectedRow()
{
	return ListView_GetNextItem(compHWND, -1, LVNI_SELECTED);
}

void KGridView::RemoveRecordAt(int rowIndex)
{
	if (ListView_DeleteItem(compHWND, rowIndex))
		--itemCount;
}

void KGridView::RemoveAll()
{
	ListView_DeleteAllItems(compHWND);
	itemCount = 0;
}

void KGridView::UpdateRecordAt(int rowIndex, int columnIndex, const KString& text)
{
	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.pszText = text;

	::SendMessageW(compHWND, LVM_SETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void KGridView::SetColumnWidth(int columnIndex, int columnWidth)
{
	ListView_SetColumnWidth(compHWND, columnIndex, columnWidth);
}

int KGridView::GetColumnWidth(int columnIndex)
{
	return ListView_GetColumnWidth(compHWND, columnIndex);
}

void KGridView::CreateColumn(const KString& text, int columnWidth)
{
	LVCOLUMN lvc = { 0 };

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = columnWidth;
	lvc.pszText = text;
	lvc.iSubItem = colCount;

	::SendMessageW(compHWND, LVM_INSERTCOLUMNW, (WPARAM)colCount, (LPARAM)&lvc);

	++colCount;
}

bool KGridView::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				this->OnItemSelect();
				*result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)lParam)->code == NM_RCLICK) // List view item right click
		{
			this->OnItemRightClick();
			*result = 0;
			return true;
		}
		else if (((LPNMHDR)lParam)->code == NM_DBLCLK) // List view item double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KGridView::Create(bool requireInitialMessages)
{
	if (!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register WC_LISTVIEWW class!

	if (compHWND)
	{
		ListView_SetExtendedListViewStyle(compHWND, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

void KGridView::OnItemSelect()
{
	if (listener)
		listener->OnGridViewItemSelect(this);
}

void KGridView::OnItemRightClick()
{
	if (listener)
		listener->OnGridViewItemRightClick(this);
}

void KGridView::OnItemDoubleClick()
{
	if (listener)
		listener->OnGridViewItemDoubleClick(this);
}


// =========== KGridViewListener.cpp ===========

/*
	RFC - KGridViewListener.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KGridViewListener::KGridViewListener(){}

KGridViewListener::~KGridViewListener(){}

void KGridViewListener::OnGridViewItemSelect(KGridView *gridView){}

void KGridViewListener::OnGridViewItemRightClick(KGridView *gridView){}

void KGridViewListener::OnGridViewItemDoubleClick(KGridView *gridView){}

// =========== KGroupBox.cpp ===========

/*
	RFC - KGroupBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KGroupBox::KGroupBox()
{
	compText.AssignStaticText(TXT_WITH_LEN("GroupBox"));
	compWidth = 100;
	compHeight = 100;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_GROUPBOX;
}

KGroupBox::~KGroupBox()
{
}

// =========== KLabel.cpp ===========

/*
	RFC - KLabel.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KLabel::KLabel() : KComponent(false)
{
	compClassName.AssignStaticText(TXT_WITH_LEN("STATIC"));
	compText.AssignStaticText(TXT_WITH_LEN("Label"));

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_NOTIFY;
	compDwExStyle = WS_EX_WINDOWEDGE;
}

bool KLabel::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register Label class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KLabel::~KLabel()
{
}

// =========== KListBox.cpp ===========

/*
	RFC - KListBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KListBox::KListBox(bool multipleSelection, bool sort, bool vscroll) : KComponent(false)
{
	this->multipleSelection = multipleSelection;
	listener = 0;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	compClassName.AssignStaticText(TXT_WITH_LEN("LISTBOX"));

	compWidth = 100;
	compHeight = 100;

	compX = 0;
	compY = 0;

	compDwStyle = LBS_NOTIFY | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP;
	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	if(multipleSelection)
		compDwStyle = compDwStyle | LBS_MULTIPLESEL;
	if(sort)
		compDwStyle = compDwStyle | LBS_SORT;
	if(vscroll)
		compDwStyle = compDwStyle | WS_VSCROLL;

	stringList = new KPointerList<KString*>(100);
}

void KListBox::SetListener(KListBoxListener *listener)
{
	this->listener = listener;
}

void KListBox::AddItem(const KString& text)
{
	KString *str = new KString(text);
	stringList->AddPointer(str);

	if(compHWND)
		::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void KListBox::RemoveItem(int index)
{
	KString *text = stringList->GetPointer(index);
	if (text)
		delete text;

	stringList->RemovePointer(index);

	if(compHWND)	 
		::SendMessageW(compHWND, LB_DELETESTRING, index, 0);
}

void KListBox::RemoveItem(const KString& text)
{
	const int itemIndex = this->GetItemIndex(text);
	if(itemIndex > -1)
		this->RemoveItem(itemIndex);
}

int KListBox::GetItemIndex(const KString& text)
{
	const int listSize = stringList->GetSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if (stringList->GetPointer(i)->Compare(text))
				return i;
		}
	}
	return -1;
}

int KListBox::GetItemCount()
{
	return stringList->GetSize();
}

int KListBox::GetSelectedItemIndex()
{
	if(compHWND)
	{	 
		const int index = (int)::SendMessageW(compHWND, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

KString KListBox::GetSelectedItem()
{
	const int itemIndex = this->GetSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->GetPointer(itemIndex);
	return KString();
}

int KListBox::GetSelectedItems(int* itemArray, int itemCountInArray)
{
	if(compHWND)
	{	 
		const int items = (int)::SendMessageW(compHWND, LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);
		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void KListBox::ClearList()
{
	stringList->DeleteAll(true);

	if(compHWND)
		::SendMessageW(compHWND, LB_RESETCONTENT, 0, 0);
}

void KListBox::SelectItem(int index)
{
	selectedItemIndex = index;

	if(compHWND)
		::SendMessageW(compHWND, LB_SETCURSEL, index, 0);
}

void KListBox::SelectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(compHWND)
			::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool KListBox::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_COMMAND) 
	{
		if (HIWORD(wParam) == LBN_SELCHANGE) // listbox sel change!
		{
			this->OnItemSelect();
			*result = 0;
			return true;
		}
		else if (HIWORD(wParam) == LBN_DBLCLK) // listbox double click
		{
			this->OnItemDoubleClick();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KListBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register LISTBOX class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		const int listSize = stringList->GetSize();
		if(listSize)
		{
			for(int i = 0; i < listSize; i++)
				::SendMessageW(compHWND, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->GetPointer(i));
		}

		if(!multipleSelection) // single selction!
		{
			if(selectedItemIndex > -1)
				::SendMessageW(compHWND, LB_SETCURSEL, selectedItemIndex, 0);
		}else
		{
			if(selectedItemIndex>-1)
				::SendMessageW(compHWND, LB_SELITEMRANGE, TRUE, MAKELPARAM(selectedItemIndex, selectedItemEnd));
		}

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

void KListBox::OnItemSelect()
{
	if(listener)
		listener->OnListBoxItemSelect(this);
}

void KListBox::OnItemDoubleClick()
{
	if(listener)
		listener->OnListBoxItemDoubleClick(this);
}

KListBox::~KListBox()
{
	stringList->DeleteAll(false);
	delete stringList;
}

// =========== KListBoxListener.cpp ===========

/*
	RFC - KListBoxListener.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KListBoxListener::KListBoxListener(){}
KListBoxListener::~KListBoxListener(){}

void KListBoxListener::OnListBoxItemSelect(KListBox *listBox){}

void KListBoxListener::OnListBoxItemDoubleClick(KListBox *listBox){}

// =========== KMenu.cpp ===========

/*
	RFC - KMenu.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KMenu::KMenu()
{
	hMenu = ::CreatePopupMenu();
}

void KMenu::AddMenuItem(KMenuItem *menuItem)
{
	menuItem->AddToMenu(hMenu);
}

void KMenu::AddSubMenu(const KString& text, KMenu *menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu->GetMenuHandle(), text);
}

void KMenu::AddSeperator()
{
	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_SEPARATOR;

	::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
}

HMENU KMenu::GetMenuHandle()
{
	return hMenu;
}

void KMenu::PopUpMenu(KWindow *window)
{
	if(window)
	{
		POINT p;
		::GetCursorPos(&p);
		::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window->GetHWND(), NULL);
	}
}

KMenu::~KMenu()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuBar.cpp ===========

/*
	RFC - KMenuBar.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KMenuBar::KMenuBar()
{
	hMenu = ::CreateMenu();
}

void KMenuBar::AddMenu(const KString& text, KMenu *menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu->GetMenuHandle(), text);
}

void KMenuBar::AddToWindow(KWindow *window)
{
	HWND hwnd = window->GetHWND();
	if(hwnd)
		::SetMenu(hwnd, hMenu);
}

KMenuBar::~KMenuBar()
{
	::DestroyMenu(hMenu);
}

// =========== KMenuButton.cpp ===========

/*
RFC - KMenuButton.cpp
Copyright (C) 2013-2019 CrownSoft
  
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


KMenuButton::KMenuButton()
{
	buttonMenu = 0;
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
	arrowFont = new KFont(CONST_TXT("Webdings"), 18);
}	

KMenuButton::~KMenuButton()
{
	delete arrowFont;
}

void KMenuButton::SetMenu(KMenu *buttonMenu)
{
	this->buttonMenu = buttonMenu;
}

void KMenuButton::SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->Repaint();
}

void KMenuButton::OnPress()
{
	if (buttonMenu)
	{
		POINT point = {compX, compY};
		::ClientToScreen(compParentHWND, &point); // get screen cordinates

		::TrackPopupMenu(buttonMenu->GetMenuHandle(), TPM_LEFTBUTTON, point.x, point.y + compHeight, 0, compParentHWND, NULL);
	}
}

bool KMenuButton::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if (msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW) lParam;

			*result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				*result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				const RECT rc = lpNMCD->rc;
				KGraphics::Draw3dVLine(lpNMCD->hdc, rc.right - 22, rc.top + 6, rc.bottom - 12); // draw line

				const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFont->GetFontHandle());
				const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - 18, rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphFont) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFont->GetFontHandle());
					::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				*result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}

	return KButton::EventProc(msg, wParam, lParam, result); // pass unprocessed messages to parent
}

// =========== KMenuItem.cpp ===========

/*
	RFC - KMenuItem.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KMenuItem::KMenuItem()
{
	RFC_INIT_VERIFIER;
	hMenu = 0;
	listener = 0;
	enabled = true;
	checked = false;
	itemID = KPlatformUtil::GetInstance()->GenerateMenuItemID(this);
}

void KMenuItem::AddToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
	mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);

}

bool KMenuItem::IsChecked()
{
	return checked;
}

void KMenuItem::SetCheckedState(bool state)
{
	checked = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool KMenuItem::IsEnabled()
{
	return enabled; 
}

void KMenuItem::SetEnabled(bool state)
{
	enabled = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void KMenuItem::SetText(const KString& text)
{
	itemText = text;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)itemText;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)itemText);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

KString KMenuItem::GetText()
{
	return itemText;
}

UINT KMenuItem::GetItemID()
{
	return itemID;
}

HMENU KMenuItem::GetMenuHandle()
{
	return hMenu;
}

void KMenuItem::SetListener(KMenuItemListener *listener)
{
	this->listener = listener;
}

KMenuItemListener* KMenuItem::GetListener()
{
	return listener;
}

void KMenuItem::OnPress()
{
	if(listener)
		listener->OnMenuItemPress(this);
}

KMenuItem::~KMenuItem()
{
}


// =========== KMenuItemListener.cpp ===========

/*
	RFC - KMenuItemListener.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KMenuItemListener::KMenuItemListener(){}

KMenuItemListener::~KMenuItemListener(){}

void KMenuItemListener::OnMenuItemPress(KMenuItem *menuItem){}

// =========== KNumericField.cpp ===========

/*
	RFC - KNumericField.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KNumericField::KNumericField() : KTextBox(false)
{
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL | ES_NUMBER;
}

KNumericField::~KNumericField(){}

// =========== KPasswordBox.cpp ===========

/*
	RFC - KPasswordBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KPasswordBox::KPasswordBox(bool readOnly):KTextBox(readOnly)
{
	pwdChar = '*';
	compDwStyle = compDwStyle | ES_PASSWORD;
}

void KPasswordBox::SetPasswordChar(const char pwdChar)
{
	this->pwdChar = pwdChar;
	if(compHWND)
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		this->Repaint();
	}
}

char KPasswordBox::GetPasswordChar()
{
	return pwdChar;
}

bool KPasswordBox::Create()
{
	if(KTextBox::Create())
	{
		::SendMessageW(compHWND, EM_SETPASSWORDCHAR, pwdChar, 0);
		return true;
	}
	return false;
}

KPasswordBox::~KPasswordBox()
{
}

// =========== KProgressBar.cpp ===========

/*
	RFC - KProgressBar.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KProgressBar::KProgressBar(bool smooth, bool vertical) : KComponent(false)
{
	value = 0;

	compClassName.AssignStaticText(TXT_WITH_LEN("msctls_progress32"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS;
	compDwExStyle = WS_EX_WINDOWEDGE;

	if(smooth)
		compDwStyle = compDwStyle | PBS_SMOOTH;

	if(vertical)
		compDwStyle = compDwStyle | PBS_VERTICAL;
}

int KProgressBar::GetValue()
{
	return value;
}

void KProgressBar::SetValue(int value)
{
	this->value=value;

	if(compHWND)
		::SendMessageW(compHWND, PBM_SETPOS, value, 0);
}

bool KProgressBar::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register PROGRESS_CLASSW class!

	if(compHWND)
	{
		::SendMessageW(compHWND, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(compHWND, PBM_SETPOS, value, 0); // set current value!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;	
}

KProgressBar::~KProgressBar()
{
}

// =========== KPushButton.cpp ===========

/*
	RFC - KPushButton.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KPushButton::KPushButton()
{
	compText.AssignStaticText(TXT_WITH_LEN("Push Button"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_NOTIFY | WS_TABSTOP;
}

KPushButton::~KPushButton()
{
}

// =========== KRadioButton.cpp ===========

/*
	RFC - KRadioButton.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KRadioButton::KRadioButton()
{
	compText.AssignStaticText(TXT_WITH_LEN("RadioButton"));
	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | BS_RADIOBUTTON | BS_NOTIFY | WS_TABSTOP;
}

KRadioButton::~KRadioButton()
{
}

// =========== KTextArea.cpp ===========

/*
	RFC - KTextArea.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KTextArea::KTextArea(bool autoScroll, bool readOnly):KTextBox(readOnly)
{
	compWidth = 200;
	compHeight = 100;

	compDwStyle = compDwStyle | ES_MULTILINE | ES_WANTRETURN;

	if(autoScroll)
		compDwStyle = compDwStyle | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
	else
		compDwStyle = compDwStyle | WS_HSCROLL | WS_VSCROLL;
}

LRESULT KTextArea::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS; // to catch TAB key
	return KTextBox::WindowProc(hwnd, msg, wParam, lParam);
}

KTextArea::~KTextArea()
{
}

// =========== KTextBox.cpp ===========

/*
	RFC - KTextBox.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KTextBox::KTextBox(bool readOnly) : KComponent(false)
{
	compClassName.AssignStaticText(TXT_WITH_LEN("EDIT"));

	compWidth = 100;
	compHeight = 20;

	compX = 0;
	compY = 0;

	compDwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL;

	if(readOnly)
		compDwStyle = compDwStyle | ES_READONLY;

	compDwExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

KString KTextBox::GetText()
{
	if(compHWND)
	{
		const int length = ::GetWindowTextLengthW(compHWND);
		if(length)
		{
			const int size = (length + 1) * sizeof(wchar_t);
			wchar_t *text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(compHWND, text, size);
			compText = KString(text, KString::FREE_TEXT_WHEN_DONE);
		}else
		{
			compText = KString();
		}
	}
	return compText;
}


bool KTextBox::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register EDIT class!

	if(compHWND)
	{
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!
		::EnableWindow(compHWND, compEnabled);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}
	return false;
}

KTextBox::~KTextBox()
{
}

// =========== KToolTip.cpp ===========

/*
	RFC - KToolTip.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KToolTip::KToolTip() : KComponent(false)
{
	attachedCompHWND = 0;
	compClassName.AssignStaticText(TXT_WITH_LEN("tooltips_class32"));

	compDwStyle = WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX;
}

KToolTip::~KToolTip()
{
}

void KToolTip::AttachToComponent(KWindow *parentWindow, KComponent *attachedComponent)
{
	compParentHWND = parentWindow->GetHWND();
	attachedCompHWND = attachedComponent->GetHWND();

	compHWND = ::CreateWindowExW(0, compClassName, NULL, compDwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, compParentHWND, NULL, KApplication::hInstance, 0);

	if (compHWND)
	{
		::SetWindowPos(compHWND, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		::AttachRFCPropertiesToHWND(compHWND, (KComponent*)this);

		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = compText;

		SendMessageW(compHWND, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}
}

bool KToolTip::Create(bool requireInitialMessages)
{
	return false;
}

void KToolTip::SetText(const KString& compText)
{
	this->compText = compText;
	if (compHWND)
	{
		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = compParentHWND;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedCompHWND;
		toolInfo.lpszText = compText;
		toolInfo.hinst = KApplication::hInstance;

		SendMessageW(compHWND, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}


// =========== KTrackBar.cpp ===========

/*
	RFC - KTrackBar.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KTrackBar::KTrackBar(bool showTicks, bool vertical) : KComponent(false)
{
	listener = 0;
	rangeMin = 0;
	rangeMax = 100;
	value = 0;

	compWidth = 100;
	compHeight = 25;

	compX = 0;
	compY = 0;

	compDwStyle = (WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS) | (showTicks ? TBS_AUTOTICKS : TBS_NOTICKS) | (vertical ? TBS_VERT : TBS_HORZ);
	compDwExStyle = WS_EX_WINDOWEDGE;

	compClassName.AssignStaticText(TXT_WITH_LEN("msctls_trackbar32"));
}

void KTrackBar::SetRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));	
}

void KTrackBar::SetValue(int value)
{
	this->value = value;
	if(compHWND)
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);
}

void KTrackBar::OnChange()
{
	value = (int)::SendMessageW(compHWND, TBM_GETPOS, 0, 0);
	if(listener)
		listener->OnTrackBarChange(this);
}

void KTrackBar::SetListener(KTrackBarListener *listener)
{
	this->listener = listener;
}

int KTrackBar::GetValue()
{
	return value;
}

bool KTrackBar::EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
	if( (msg == WM_HSCROLL) || (msg == WM_VSCROLL) )
	{
		const int nScrollCode = (int)LOWORD(wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || 
			(TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || 
			(TB_PAGEDOWN == nScrollCode) || (TB_THUMBPOSITION == nScrollCode)) // its trackbar!
		{
			this->OnChange();
			*result = 0;
			return true;
		}
	}

	return KComponent::EventProc(msg, wParam, lParam, result);
}

bool KTrackBar::Create(bool requireInitialMessages)
{
	if(!compParentHWND) // user must specify parent handle!
		return false;

	::CreateRFCComponent(this, requireInitialMessages); // we dont need to register TRACKBAR_CLASSW class!

	if(compHWND)
	{
		::EnableWindow(compHWND, compEnabled);
		::SendMessageW(compHWND, WM_SETFONT, (WPARAM)compFont->GetFontHandle(), MAKELPARAM(true, 0)); // set font!	
		::SendMessageW(compHWND, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(rangeMin, rangeMax));	
		::SendMessageW(compHWND, TBM_SETPOS, TRUE, (LPARAM)value);

		if(compVisible)
			::ShowWindow(compHWND, SW_SHOW);

		return true;
	}

	return false;
}

KTrackBar::~KTrackBar()
{
}


// =========== KTrackBarListener.cpp ===========

/*
	RFC - KTrackBarListener.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KTrackBarListener::KTrackBarListener(){}

KTrackBarListener::~KTrackBarListener(){}

void KTrackBarListener::OnTrackBarChange(KTrackBar *trackBar){}

// =========== KWindow.cpp ===========

/*
	RFC - KWindow.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KWindow::KWindow() : KComponent(true)
{
	compText.AssignStaticText(TXT_WITH_LEN("KWindow"));

	compWidth = 400;
	compHeight = 200;

	compVisible = false;
	compDwStyle = WS_POPUP;
	compDwExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	compCtlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
}

void KWindow::Flash()
{
	::FlashWindow(compHWND, TRUE);
}

void KWindow::SetIcon(KIcon *icon)
{
	::SetClassLongPtrW(compHWND, GCLP_HICON, (LONG_PTR)icon->GetHandle());
}

void KWindow::Destroy()
{
	::DestroyWindow(compHWND);
}


void KWindow::OnClose()
{
	this->Destroy();
}

void KWindow::OnDestroy()
{
	::PostQuitMessage(0);
}

void KWindow::CenterScreen()
{
	this->SetPosition((::GetSystemMetrics(SM_CXSCREEN) - compWidth) / 2, (::GetSystemMetrics(SM_CYSCREEN) - compHeight) / 2);
}

bool KWindow::AddComponent(KComponent *component, bool requireInitialMessages)
{
	if(component)
	{
		if(compHWND)
		{
			component->SetParentHWND(compHWND);
			return component->Create(requireInitialMessages);
		}
	}
	return false;
}

bool KWindow::SetClientAreaSize(int width, int height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		wndRect.right = wndRect.left + width;
		wndRect.bottom = wndRect.top + height;

		::AdjustWindowRect(&wndRect, compDwStyle, ::GetMenu(compHWND) == NULL ? FALSE : TRUE);
		this->SetSize(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

		return true;
	}
	return false;
}

bool KWindow::GetClientAreaSize(int *width, int *height)
{
	if (compHWND)
	{
		RECT wndRect;
		::GetClientRect(compHWND, &wndRect);

		if (width)
			*width = wndRect.right - wndRect.left;

		if (height)
			*height = wndRect.bottom - wndRect.top;

		return true;
	}
	return false;
}

void KWindow::OnMoved()
{

}

void KWindow::OnResized()
{

}

LRESULT KWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent *component = (KComponent*)::GetPropW(((LPDRAWITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				KComponent *component = (KComponent*)::GetPropW(((LPNMHDR)lParam)->hwndFrom, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
			{
				KComponent *component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}				
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (wParam != 0) // ignore menus
				{
					KComponent *component = (KComponent*)::GetPropW(GetDlgItem(hwnd,((LPMEASUREITEMSTRUCT)lParam)->CtlID), MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}			
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				KComponent *component = (KComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)lParam)->hwndItem, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->EventProc(msg, wParam, lParam, &result))
						return result;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_TIMER:
			{
				KTimer *timer = KPlatformUtil::GetInstance()->GetTimerByID((UINT)wParam);
				if(timer)
				{
					timer->OnTimer();
					break;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compWidth = rect.right - rect.left;
				this->compHeight = rect.bottom - rect.top;

				this->OnResized();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window...
			{
				RECT rect;
				::GetWindowRect(compHWND, &rect);

				this->compX = rect.left;
				this->compY = rect.top;

				this->OnMoved();
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(wParam) == 0) && (lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					KMenuItem *menuItem = KPlatformUtil::GetInstance()->GetMenuItemByID(LOWORD(wParam));
					if(menuItem)
					{
						menuItem->OnPress();
						break;
					}
				}
				else if(lParam)// send to appropriate component
				{
					KComponent *component = (KComponent*)::GetPropW((HWND)lParam, MAKEINTATOM(InternalDefinitions::RFCPropAtom_Component));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->EventProc(msg, wParam, lParam, &result))
							return result;
					}
				}
				else if (LOWORD(wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate enter key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0); 

					return 0;
				}
				else if (LOWORD(wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate esc key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0); 

					return 0;
				}
			}
			return KComponent::WindowProc(hwnd, msg, wParam, lParam);

		case WM_ACTIVATE: // save last focused item when inactive
			if (wParam != WA_INACTIVE)
				return KComponent::WindowProc(hwnd, msg, wParam, lParam);
			this->lastFocusedChild = ::GetFocus();
			break;

		case WM_SETFOCUS:
			if (this->lastFocusedChild) // set focus to last item
				::SetFocus(this->lastFocusedChild);
			else // set focus to first child
				::SetFocus(::GetNextDlgTabItem(this->compHWND, NULL, FALSE));
			break;

		case WM_CLOSE:
			this->OnClose();
			break;

		case WM_DESTROY:
			this->OnDestroy();
			break;

		default:
			return KComponent::WindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


KWindow::~KWindow()
{

}

// =========== KWindowTypes.cpp ===========

/*
	RFC - KWindowTypes.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KHotPluggedDialog::KHotPluggedDialog(){}


void KHotPluggedDialog::OnClose()
{
	::EndDialog(compHWND, 0);
}

void KHotPluggedDialog::OnDestroy(){}

KHotPluggedDialog::~KHotPluggedDialog(){}


KOverlappedWindow::KOverlappedWindow()
{
	compText.AssignStaticText(TXT_WITH_LEN("KOverlapped Window"));
	compDwStyle = WS_OVERLAPPEDWINDOW;
}

KOverlappedWindow::~KOverlappedWindow(){}


KFrame::KFrame()
{
	compText.AssignStaticText(TXT_WITH_LEN("KFrame"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
}

KFrame::~KFrame(){}



KDialog::KDialog()
{
	compText.AssignStaticText(TXT_WITH_LEN("KDialog"));
	compDwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU;
}

KDialog::~KDialog(){}



KToolWindow::KToolWindow()
{
	compText.AssignStaticText(TXT_WITH_LEN("KTool Window"));
	compDwStyle = WS_OVERLAPPED | WS_SYSMENU;
	compDwExStyle = WS_EX_TOOLWINDOW;
}

KToolWindow::~KToolWindow(){}


// =========== KDirectory.cpp ===========

/*
	RFC - KDirectory.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KDirectory::KDirectory(){}

KDirectory::~KDirectory(){}

bool KDirectory::IsDirExists(const KString& dirName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool KDirectory::CreateDir(const KString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool KDirectory::RemoveDir(const KString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

KString KDirectory::GetModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetParentDir(const KString& filePath)
{
	wchar_t *path = ::_wcsdup(filePath);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	path[0] = 0;
	::GetTempPathW(MAX_PATH + 1, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

KString KDirectory::GetApplicationDataDir(bool isAllUsers)
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	path[0] = 0;
	::SHGetFolderPathW(NULL, isAllUsers ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA, NULL, 0, path);

	return KString(path, KString::FREE_TEXT_WHEN_DONE);
}

// =========== KFile.cpp ===========

/*
	RFC - KFile.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KFile::KFile()
{
	autoCloseHandle = false;
	desiredAccess = KFile::KBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

KFile::KFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool KFile::OpenFile(const KString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool KFile::CloseFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE KFile::GetFileHandle()
{
	return fileHandle;
}

KFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD KFile::ReadFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD KFile::WriteFile(void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool KFile::SetFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool KFile::SetFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::GetFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool KFile::SetFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD KFile::GetFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* KFile::ReadAsData()
{
	const DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool KFile::WriteString(const KString& text, bool isUnicode)
{
	void* buffer = isUnicode ? (void*)(const wchar_t*)text : (void*)(const char*)text;
	const DWORD numberOfBytesToWrite = text.GetLength() * ( isUnicode ? sizeof(wchar_t) : sizeof(char) );

	const DWORD numberOfBytesWritten = this->WriteFile(buffer, numberOfBytesToWrite);

	return (numberOfBytesWritten == numberOfBytesToWrite);
}

KString KFile::ReadAsString(bool isUnicode)
{
	DWORD fileSize = this->GetFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = this->ReadFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (isUnicode)
			{
				return KString((const wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}
			else
			{
				KString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return KString();
}

bool KFile::DeleteFile(const KString& fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool KFile::CopyFile(const KString& sourceFileName, const KString& destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

bool KFile::IsFileExists(const KString& fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

KFile::~KFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}

// =========== KLogger.cpp ===========

/*
	RFC - KLogger.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KLogger::KLogger(DWORD bufferSize)
{
	buffer = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;
	bufferIndex = 0;
	totalEvents = 0;
	totalMills = 0;
	bufferFull = false;
	isFirstCall = true;
}

bool KLogger::WriteNewEvent(unsigned char eventType)
{
	if (!bufferFull)
	{
		if ((bufferIndex + 300) >= bufferSize) // assume each event data is not greater than 300 bytes
		{
			bufferFull = true;
			return false;
		}

		unsigned short secs = 0;
		unsigned short mills = 0;

		if (isFirstCall)
		{
			pCounter.StartCounter();
			isFirstCall = false;
			totalMills = 0;
		}
		else{
			const double deltaMills = pCounter.EndCounter();
			totalMills += (unsigned int)deltaMills;

			secs = (unsigned short)(totalMills/1000);
			mills = (unsigned short)(totalMills % 1000);

			pCounter.StartCounter();
		}

		buffer[bufferIndex] = eventType; // write event type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = secs; // write secs
		bufferIndex += sizeof(unsigned short);

		*((unsigned short*)&buffer[bufferIndex]) = mills; // write mills
		bufferIndex += sizeof(unsigned short);

		totalEvents++;

		return true;
	}
	return false;
}

bool KLogger::EndEvent()
{
	if (!bufferFull)
	{
		buffer[bufferIndex] = EVT_END; // write event end
		bufferIndex += sizeof(unsigned char);

		return true;
	}
	return false;
}

bool KLogger::AddTextParam(const char *text, unsigned char textLength)
{
	if( (textLength < 256) && (!bufferFull) )
	{
		buffer[bufferIndex] = PARAM_STRING; // write param type
		bufferIndex += sizeof(unsigned char);

		buffer[bufferIndex] = textLength; // write data size
		bufferIndex += sizeof(unsigned char);

		for (int i = 0; i < textLength; i++) // write data
		{
			buffer[bufferIndex] = text[i];
			bufferIndex += sizeof(unsigned char);
		}

		return true;
	}
	return false;
}

bool KLogger::AddIntParam(int value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_INT32; // write param type
		bufferIndex += sizeof(unsigned char);

		*((int*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(int);

		return true;
	}
	return false;
}

bool KLogger::AddShortParam(unsigned short value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_SHORT16; // write param type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(unsigned short);

		return true;
	}
	return false;
}

bool KLogger::AddFloatParam(float value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_FLOAT; // write param type
		bufferIndex += sizeof(unsigned char);

		*((float*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(float);

		return true;
	}
	return false;
}
	
bool KLogger::AddDoubleParam(double value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_DOUBLE; // write param type
		bufferIndex += sizeof(unsigned char);

		*((double*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(double);

		return true;
	}
	return false;
}

bool KLogger::IsBufferFull()
{
	return bufferFull;
}

bool KLogger::WriteToFile(const KString &filePath)
{
	KFile file;

	if (KFile::IsFileExists(filePath))
		KFile::DeleteFile(filePath);

	if (file.OpenFile(filePath, KFile::KWRITE))
	{
		file.WriteFile((void*)"RLOG", 4);
		file.WriteFile(&totalEvents, 4);
		file.WriteFile(buffer, bufferIndex);

		return true;
	}

	return false;
}

KLogger::~KLogger()
{
	free(buffer);
}

// =========== KSettingsReader.cpp ===========

/*
	RFC - KSettingsReader.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KSettingsReader::KSettingsReader()
{

}

bool KSettingsReader::OpenFile(const KString& fileName, int formatID)
{
	if (!KFile::IsFileExists(fileName))
		return false;

	if (!settingsFile.OpenFile(fileName, KFile::KREAD))
		return false;

	settingsFile.SetFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.ReadFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void KSettingsReader::ReadData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.ReadFile(buffer, size);
}

KString KSettingsReader::ReadString()
{
	int size = 0;
	settingsFile.ReadFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.ReadFile(buffer, size);

		return KString(buffer, KString::FREE_TEXT_WHEN_DONE);
	}
	else
	{
		return KString();
	}
}

int KSettingsReader::ReadInt()
{
	int value = 0;
	settingsFile.ReadFile(&value, sizeof(int));

	return value;
}

float KSettingsReader::ReadFloat()
{
	float value = 0;
	settingsFile.ReadFile(&value, sizeof(float));

	return value;
}

double KSettingsReader::ReadDouble()
{
	double value = 0;
	settingsFile.ReadFile(&value, sizeof(double));

	return value;
}

bool KSettingsReader::ReadBool()
{
	bool value = 0;
	settingsFile.ReadFile(&value, sizeof(bool));

	return value;
}

KSettingsReader::~KSettingsReader()
{

}

// =========== KSettingsWriter.cpp ===========

/*
	RFC - KSettingsWriter.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KSettingsWriter::KSettingsWriter()
{

}

bool KSettingsWriter::OpenFile(const KString& fileName, int formatID)
{
	if (KFile::IsFileExists(fileName))
		KFile::DeleteFile(fileName);

	if (!settingsFile.OpenFile(fileName, KFile::KWRITE))
		return false;

	settingsFile.SetFilePointerToStart();
	settingsFile.WriteFile(&formatID, sizeof(int));

	return true;
}

void KSettingsWriter::WriteData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.WriteFile(buffer, size);
}

void KSettingsWriter::WriteString(const KString& text)
{
	int size = text.GetLength();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.WriteFile(&size, sizeof(int));

		settingsFile.WriteFile((wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.WriteFile(&size, sizeof(int));
	}
}

void KSettingsWriter::WriteInt(int value)
{
	settingsFile.WriteFile(&value, sizeof(int));
}

void KSettingsWriter::WriteFloat(float value)
{
	settingsFile.WriteFile(&value, sizeof(float));
}

void KSettingsWriter::WriteDouble(double value)
{
	settingsFile.WriteFile(&value, sizeof(double));
}

void KSettingsWriter::WriteBool(bool value)
{
	settingsFile.WriteFile(&value, sizeof(bool));
}

KSettingsWriter::~KSettingsWriter()
{

}

// =========== KInternet.cpp ===========

/*
	RFC - KInternet.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

// mingw does not ship with winhttp. So, this class is not available for mingw compiler.
#ifndef __MINGW32__

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string.h>
#include <stdio.h>

KInternet::KInternet(){}

KInternet::~KInternet(){}

void KInternet::ApplyProxySettings(const wchar_t* url, HINTERNET hInternet)
{
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig;
	WINHTTP_PROXY_INFO proxyInfoTemp, proxyInfo;
	WINHTTP_AUTOPROXY_OPTIONS OptPAC;

	::ZeroMemory(&proxyConfig, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	::ZeroMemory(&proxyInfo, sizeof(WINHTTP_PROXY_INFO));

	if (::WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
	{
		if (proxyConfig.lpszProxy) {
			proxyInfo.lpszProxy = proxyConfig.lpszProxy;
			proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxyInfo.lpszProxyBypass = NULL;
		}

		if (proxyConfig.lpszAutoConfigUrl) {
			// Script proxy pac
			OptPAC.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
			OptPAC.lpszAutoConfigUrl = proxyConfig.lpszAutoConfigUrl;
			OptPAC.dwAutoDetectFlags = 0;
			OptPAC.fAutoLogonIfChallenged = TRUE;
			OptPAC.lpvReserved = 0;
			OptPAC.dwReserved = 0;

			if (::WinHttpGetProxyForUrl(hInternet, url, &OptPAC, &proxyInfoTemp))
				::memcpy(&proxyInfo, &proxyInfoTemp, sizeof(WINHTTP_PROXY_INFO));
		}

		if (proxyConfig.fAutoDetect) {
			// Autodetect proxy
			OptPAC.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
			OptPAC.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
			OptPAC.fAutoLogonIfChallenged = TRUE;
			OptPAC.lpszAutoConfigUrl = NULL;
			OptPAC.lpvReserved = 0;
			OptPAC.dwReserved = 0;

			if (::WinHttpGetProxyForUrl(hInternet, url, &OptPAC, &proxyInfoTemp))
				::memcpy(&proxyInfo, &proxyInfoTemp, sizeof(WINHTTP_PROXY_INFO));
		}

		if (proxyInfo.lpszProxy)
			::WinHttpSetOption(hInternet, WINHTTP_OPTION_PROXY, &proxyInfo, sizeof(WINHTTP_PROXY_INFO));
	}
}

KString KInternet::UrlEncodeString(const KString &text)
{
	if (text.GetLength() == 0)
		return KString();

	KString new_str;
	char c;
	int ic;
	const char* chars = text;
	char bufHex[10];
	int len = text.GetLength();

	for (int i = 0; i < len; i++)
	{
		c = chars[i];
		ic = c;

		if (c == ' ')
		{
			new_str = new_str.AppendStaticText(L"+", 1, true);
		}
		else if (::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{		
			char tmp[] = { c, 0 };
			new_str = new_str.Append(KString(tmp));
		}
		else
		{
			::sprintf(bufHex, "%X", c);

			if (ic < 16)
				new_str = new_str.AppendStaticText(L"%0", 2, true);
			else
				new_str = new_str.AppendStaticText(L"%", 1, true);

			new_str = new_str.Append(KString(bufHex));
		}
	}
	return new_str;
}

KString KInternet::UrlDecodeString(const KString &text)
{
	if (text.GetLength() == 0)
		return KString();

	KString ret;
	const char* str = text;

	char ch;
	int i, ii, len = text.GetLength();

	for (i = 0; i < len; i++)
	{
		if (str[i] != '%')
		{
			if (str[i] == '+')
			{
				ret = ret.AppendStaticText(L" ", 1, true);
			}
			else
			{
				char tmp[] = { str[i], 0 };
				ret = ret.Append(KString(tmp));
			}
		}
		else
		{
			KString sub(text.SubString(i + 1, i + 2));
			::sscanf(sub, "%x", &ii);
			ch = static_cast<char>(ii);

			char tmp[] = { ch, 0 };
			ret = ret.Append(KString(tmp));

			i = i + 2;
		}
	}
	return ret;
}

KString KInternet::PostText(const wchar_t* url,
	const wchar_t* objectName,
	const bool isHttps,
	const char* postData,
	const int postDataLength,
	const bool ignoreCertificateErros,
	const wchar_t* userAgent)
{
	HINTERNET hInternet = 0, hConnect = 0, hRequest = 0;
	BOOL resultOK = FALSE;
	KString receivedText;

	//hInternet = ::WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	hInternet = ::WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_NO_PROXY, 0, WINHTTP_NO_PROXY_BYPASS, 0);

	if (hInternet)
		KInternet::ApplyProxySettings(url, hInternet);

	if (hInternet)
		hConnect = ::WinHttpConnect(hInternet, url, INTERNET_DEFAULT_PORT, 0);

	if (hConnect)
		hRequest = ::WinHttpOpenRequest(hConnect, L"POST", objectName, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, isHttps ? (WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE) : WINHTTP_FLAG_REFRESH);

	if (hRequest)
	{
		if (ignoreCertificateErros)
		{
			DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
			::WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(DWORD));
		}

		resultOK = ::WinHttpSendRequest(hRequest, L"Content-Type: application/x-www-form-urlencoded\r\n", -1, (LPVOID)postData, postDataLength, postDataLength, 0); // Send a request
	}

	if (resultOK)
		resultOK = ::WinHttpReceiveResponse(hRequest, NULL);

	if (resultOK)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;

		do
		{
			dwSize = 0;
			if (::WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				char* outBuffer = new char[dwSize + 1];
				::ZeroMemory(outBuffer, dwSize + 1);

				if (::WinHttpReadData(hRequest, (LPVOID)outBuffer, dwSize, &dwDownloaded))
				{
					receivedText = receivedText + KString(outBuffer);
				}

				delete[] outBuffer;
			}

		} while (dwSize > 0);
	}

	if (hRequest)
		::WinHttpCloseHandle(hRequest);

	if (hConnect)
		::WinHttpCloseHandle(hConnect);

	if (hInternet)
		::WinHttpCloseHandle(hInternet);

	return receivedText;
}

#endif

// =========== KMD5.cpp ===========

/*
	RFC - KMD5.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KMD5::KMD5()
{

}

KString KMD5::GenerateFromString(const KString& text)
{
	if (text.GetLength() == 0)
		return KString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestString((char*)(const char*)text);

	return KString(strMD5);
}

KString KMD5::GenerateFromFile(const KString& fileName)
{
	if (fileName.GetLength() == 0) // empty path
		return KString();

	if (!KFile::IsFileExists(fileName)) // file does not exists
		return KString();

	KFile file;
	file.OpenFile(fileName, KFile::KREAD, false);

	const DWORD fileSize = file.GetFileSize();
	file.CloseFile();

	if (fileSize==0) // empty file
		return KString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestFile((char*)(const char*)fileName);

	return KString(strMD5);
}

KMD5::~KMD5()
{

}

// =========== KSHA1.cpp ===========

/*
	RFC - KSHA1.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KSHA1::KSHA1()
{

}

KString KSHA1::GenerateFromString(const KString& text)
{
	if (text.GetLength() == 0)
		return KString();

	ExtLibs::CSHA1 sha1;
	sha1.Update((const UINT_8*)(const char*)text, text.GetLength());
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return KString(szReport);
}

KString KSHA1::GenerateFromFile(const KString& fileName)
{
	if (fileName.GetLength() == 0) // empty path
		return KString();

	if (!KFile::IsFileExists(fileName)) // file does not exists
		return KString();

	KFile file;
	file.OpenFile(fileName, KFile::KREAD, false);

	const DWORD fileSize = file.GetFileSize();
	file.CloseFile();

	if (fileSize == 0) // empty file
		return KString();

	ExtLibs::CSHA1 sha1;
	sha1.HashFile((const char*)fileName);
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return KString(szReport);
}

KSHA1::~KSHA1()
{

}

// =========== KString.cpp ===========

/*
	RFC - KString.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

#include <stdio.h>

const KString operator+ (const char* const string1, const KString& string2)
{
	KString s(string1);
	return s.Append(string2);
}

const KString operator+ (const wchar_t* const string1, const KString& string2)
{
	KString s(string1);
	return s.Append(string2);
}

const KString operator+ (const KString& string1, const KString& string2)
{
	return string1.Append(string2);
}

KString::KString()
{
	isZeroLength = true;
	stringHolder = 0;
	isStaticText = false;
}

KString::KString(const KString& other)
{
	isZeroLength = other.isZeroLength;

	if (other.isStaticText)
	{
		stringHolder = 0;

		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->AddReference();
		stringHolder = other.stringHolder;

		isStaticText = false;
	}
	else
	{
		stringHolder = 0;
		isStaticText = false;
	}
}

KString::KString(const char* const text, UINT codePage)
{
	isStaticText = false;

	if (text != 0)
	{
		int count = ::MultiByteToWideChar(codePage, 0, text, -1, 0, 0); // get char count with null character
		if (count)
		{
			wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
			if (::MultiByteToWideChar(codePage, 0, text, -1, w_text, count))
			{
				count--; // ignore null character

				stringHolder = new KStringHolder(w_text, count);
				isZeroLength = (count == 0);
				return;
			}
			else
			{
				::free(w_text);
			}
		}
	}

	isZeroLength = true;
	stringHolder = 0;
}

KString::KString(const wchar_t* const text, unsigned char behaviour, int length)
{
	if (text != 0)
	{
		staticTextLength = ((length == -1) ? (int)::wcslen(text) : length);
		if (staticTextLength)
		{
			isZeroLength = false;
			isStaticText = (behaviour == STATIC_TEXT_DO_NOT_FREE);

			if (isStaticText)
			{
				staticText = (wchar_t*)text;
				stringHolder = 0;
				return;
			}

			stringHolder = new KStringHolder(((behaviour == FREE_TEXT_WHEN_DONE) ? (wchar_t*)text : _wcsdup(text)), staticTextLength);
			return;
		}
	}

	isZeroLength = true;
	isStaticText = false;
	stringHolder = 0;
}

KString::KString(const int value, const int radix)
{
	stringHolder = new KStringHolder((wchar_t *)::malloc(33 * sizeof(wchar_t)), 0); // max 32 digits
	::_itow(value, stringHolder->w_text, radix);

	stringHolder->count = (int)::wcslen(stringHolder->w_text);
	isZeroLength = (stringHolder->count == 0);
	isStaticText = false;
}

KString::KString(const float value, const int numDecimals, bool compact)
{
	isStaticText = false;

	// round it to given digits
	char *str_fmtp = (char*)malloc(32);
	char *str_buf = (char*)malloc(64);

	sprintf(str_fmtp, "%%.%df", numDecimals);
	sprintf(str_buf, str_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(str_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (str_buf[len - i] == '0')
				str_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(str_buf) - 1;
		if (str_buf[len] == '.')
			str_buf[len] = 0; // kill it
	}

	int count = ::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, 0, 0); // get char count with null character
	if (count)
	{
		wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
		if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, w_text, count))
		{
			count--; // ignore null character

			stringHolder = new KStringHolder(w_text, count);
			isZeroLength = (count == 0);

			::free(str_buf);
			::free(str_fmtp);
			return;
		}
		else
		{
			::free(w_text);
		}
	}

	::free(str_buf);
	::free(str_fmtp);

	isZeroLength = true;
	stringHolder = 0;
}

const KString& KString::operator= (const KString& other)
{
	if (stringHolder)
		stringHolder->ReleaseReference();

	if (other.isStaticText)
	{
		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->AddReference();
		isStaticText = false;
	}
	else // other is empty
	{
		isStaticText = false;
	}

	stringHolder = other.stringHolder;
	isZeroLength = other.isZeroLength;

	return *this;
}

const KString& KString::operator= (const wchar_t* const other)
{
	isStaticText = false;

	if (stringHolder)
		stringHolder->ReleaseReference();

	if (other != 0)
	{
		const int count = (int)::wcslen(other);
		if (count)
		{
			stringHolder = new KStringHolder(::_wcsdup(other), count);
			isZeroLength = false;
			return *this;
		}
	}

	isZeroLength = true;
	stringHolder = 0;
	return *this;
}

const KString KString::operator+ (const KString& stringToAppend)
{
	return Append(stringToAppend);
}

const KString KString::operator+ (const wchar_t* const textToAppend)
{
	return Append(KString(textToAppend, USE_COPY_OF_TEXT, -1));
}

void KString::ConvertToRefCountedStringIfStatic()const
{
	if (isStaticText)
	{
		isStaticText = false;
		stringHolder = new KStringHolder(::_wcsdup(staticText), staticTextLength);
	}
}

KString::operator const char*()const
{
	if (!isZeroLength)
	{
		this->ConvertToRefCountedStringIfStatic();
		return stringHolder->GetAnsiVersion();
	}
	return "";
}

KString::operator const wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return L"";
	}
}

KString::operator wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return (wchar_t*)L"";
	}
}

const char KString::operator[](const int index)const
{
	if (!isZeroLength)
	{
		this->ConvertToRefCountedStringIfStatic();

		if ((0 <= index) && (index <= (stringHolder->count - 1)))
		{
			return stringHolder->GetAnsiVersion()[index];
		}
	}
	return -1;
}

KString KString::Append(const KString& otherString)const
{
	if (!otherString.isZeroLength)
	{
		if (!this->isZeroLength)
		{
			const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + (otherString.isStaticText ? otherString.staticTextLength : otherString.stringHolder->count);
			wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

			::wcscpy(destText, isStaticText ? staticText : stringHolder->w_text);
			::wcscat(destText, otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text);

			return KString(destText, FREE_TEXT_WHEN_DONE, totalCount);
		}
		else // this string is empty
		{
			return otherString;
		}
	}
	else // other string is empty
	{
		return *this;
	}
}

KString KString::AppendStaticText(const wchar_t* const text, int length, bool appendToEnd)const
{
	if (!this->isZeroLength)
	{
		const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + length;
		wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

		::wcscpy(destText, appendToEnd ? (isStaticText ? staticText : stringHolder->w_text) : text);
		::wcscat(destText, appendToEnd ? text : (isStaticText ? staticText : stringHolder->w_text));

		return KString(destText, FREE_TEXT_WHEN_DONE, totalCount);
	}
	else // this string is empty
	{
		return KString(text, KString::STATIC_TEXT_DO_NOT_FREE, length);
	}
}

void KString::AssignStaticText(const wchar_t* const text, int length)
{
	if (stringHolder)
		stringHolder->ReleaseReference();
	
	stringHolder = 0;
	isZeroLength = false;
	isStaticText = true;
	staticText = (wchar_t*)text;
	staticTextLength = length;
}

KString KString::SubString(int start, int end)const
{
	const int count = this->GetLength();

	if ((0 <= start) && (start <= (count - 1)))
	{
		if ((start < end) && (end <= (count - 1)))
		{
			int size = (end - start) + 1;
			wchar_t* buf = (wchar_t*)::malloc((size + 1) * sizeof(wchar_t));
			wchar_t* src = (isStaticText ? staticText : stringHolder->w_text);
			::wcsncpy(buf, &src[start], size);
			buf[size] = 0;

			return KString(buf, FREE_TEXT_WHEN_DONE, size);
		}
	}
	return KString();
}

bool KString::CompareIgnoreCase(const KString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::_wcsicmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool KString::Compare(const KString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool KString::CompareWithStaticText(const wchar_t* const text)const
{
	if (!this->isZeroLength)
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), text) == 0);

	return false;
}

bool KString::StartsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[0] == character) : (stringHolder->w_text[0] == character));

	return false;
}

bool KString::StartsWithChar(char character)const
{
	if (!this->isZeroLength)
	{
		this->ConvertToRefCountedStringIfStatic();
		return (stringHolder->GetAnsiVersion()[0] == character);
	}
	return false;
}

bool KString::EndsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[staticTextLength - 1] == character) : (stringHolder->w_text[stringHolder->count - 1] == character));

	return false;
}

bool KString::EndsWithChar(char character)const
{
	if (!this->isZeroLength)
	{
		this->ConvertToRefCountedStringIfStatic();
		return (stringHolder->GetAnsiVersion()[stringHolder->count - 1] == character);
	}
	return false;
}

bool KString::IsQuotedString()const
{
	if ((isStaticText && (staticTextLength > 1)) || ((stringHolder != 0) && (stringHolder->count > 1))) // not empty + count greater than 1
		return (StartsWithChar(L'\"') && EndsWithChar(L'\"'));

	return false;
}

wchar_t KString::GetCharAt(int index)const
{
	const int count = this->GetLength();

	if ((0 <= index) && (index <= (count - 1)))
		return (isStaticText ? staticText[index] : stringHolder->w_text[index]);

	return -1;
}

int KString::GetLength()const
{
	return (isStaticText ? staticTextLength : ((stringHolder != 0) ? stringHolder->count : 0));
}

bool KString::IsEmpty()const
{
	return isZeroLength;
}

bool KString::IsNotEmpty()const
{
	return !isZeroLength;
}

int KString::GetIntValue()const
{
	if (isZeroLength)
		return 0;

	return ::_wtoi(isStaticText ? staticText : stringHolder->w_text);
}

KString KString::ToUpperCase()const
{
	if (this->GetLength() == 0)
		return KString();

	KString result((const wchar_t*)*this, KString::USE_COPY_OF_TEXT);
	::CharUpperBuffW((wchar_t*)result, result.GetLength());

	return result;
}

KString KString::ToLowerCase()const
{
	if (this->GetLength() == 0)
		return KString();

	KString result((const wchar_t*)*this, KString::USE_COPY_OF_TEXT);
	::CharLowerBuffW((wchar_t*)result, result.GetLength());

	return result;
}

KString::~KString()
{
	if (stringHolder)
		stringHolder->ReleaseReference();
}

// =========== KStringHolder.cpp ===========

/*
	RFC - KStringHolder.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KStringHolder::KStringHolder(wchar_t *w_text, int count)
{
	refCount = 1;
	a_text = 0;
	this->w_text = w_text;
	this->count = count;

	#ifndef RFC_NO_SAFE_ANSI_STR
	::InitializeCriticalSection(&cs_a_text);
	#endif
}

KStringHolder::~KStringHolder()
{
	#ifndef RFC_NO_SAFE_ANSI_STR
	::DeleteCriticalSection(&cs_a_text);
	#endif
}

void KStringHolder::AddReference()
{
	::InterlockedIncrement(&refCount);
}

void KStringHolder::ReleaseReference()
{
	const LONG res = ::InterlockedDecrement(&refCount);
	if(res == 0)
	{
		if(a_text)
			::free(a_text);

		if(w_text)
			::free(w_text);

		delete this;
	}
}

const char* KStringHolder::GetAnsiVersion(UINT codePage)
{
	#ifndef RFC_NO_SAFE_ANSI_STR
	::EnterCriticalSection(&cs_a_text);
	#endif

	if(a_text)
	{
		#ifndef RFC_NO_SAFE_ANSI_STR
		::LeaveCriticalSection(&cs_a_text);
		#endif
		return a_text;
	}else
	{
		const int length = ::WideCharToMultiByte(codePage, 0, w_text, -1, 0, 0, 0, 0);
		if (length)
		{
			a_text = (char*)::malloc(length);
			if (::WideCharToMultiByte(codePage, 0, w_text, -1, a_text, length, 0, 0))
			{
				#ifndef RFC_NO_SAFE_ANSI_STR
				::LeaveCriticalSection(&cs_a_text);
				#endif
				return a_text;
			}
			::free(a_text);
			a_text = 0;
		}

		#ifndef RFC_NO_SAFE_ANSI_STR
		::LeaveCriticalSection(&cs_a_text);
		#endif
		return 0; // conversion error
	}
}

// =========== KRunnable.cpp ===========

/*
RFC - KRunnable.cpp
Copyright (C) 2013-2019 CrownSoft

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


KRunnable::KRunnable(){}

KRunnable::~KRunnable(){}

void KRunnable::Run(KThread *thread){}

// =========== KThread.cpp ===========

/*
	RFC - KThread.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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



KThread::KThread()
{
	handle = 0; 
	runnable = NULL;
	threadShouldStop = false;
}

void KThread::SetHandle(HANDLE handle)
{
	this->handle = handle;
}

void KThread::SetRunnable(KRunnable *runnable)
{
	this->runnable = runnable;
}

HANDLE KThread::GetHandle()
{
	return handle;
}

KThread::operator HANDLE()const
{
	return handle;
}

bool KThread::ShouldRun()
{
	return !threadShouldStop;
}

void KThread::Run()
{
	if (runnable)
		runnable->Run(this);
}

bool KThread::IsThreadRunning()
{
	if (handle)
	{
		const DWORD result = ::WaitForSingleObject(handle, 0);
		return (result != WAIT_OBJECT_0);
	}

	return false;
}

void KThread::ThreadShouldStop()
{
	threadShouldStop = true;
}

DWORD KThread::WaitUntilThreadFinish(bool pumpMessages)
{
	if (!pumpMessages)
		return ::WaitForSingleObject(handle, INFINITE);
	
	while (true)
	{
		MSG msg;
		while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		// if this thread sent msgs to caller thread before calling following function and after the above while block, those msgs will not be processed until new msg arrived. 
		// if there is no new msgs, there will be a deadlock! that's why we have a timeout! after the timeout, any pending msgs will be processed and continue...
		DWORD dwRet = ::MsgWaitForMultipleObjects(1, &handle, FALSE, 200, QS_ALLINPUT);

		if (dwRet == WAIT_OBJECT_0) // thread finished
		{
			return true;
		}
		else if ((dwRet == (WAIT_OBJECT_0 + 1)) || (dwRet == WAIT_TIMEOUT)) // window message or timeout
		{
			continue;
		}
		else // failure
		{
			break;
		}
	}

	return false;
}

bool KThread::StartThread()
{
	threadShouldStop = false;

	if (handle) // close old handle
	{
		::CloseHandle(handle);
		handle = 0;
	}

	return ::CreateRFCThread(this);
}

void KThread::uSleep(int waitTime)
{
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < ((waitTime * freq) / 1000000));
}

KThread::~KThread()
{
	if (handle)
		::CloseHandle(handle);
}

// =========== KTimer.cpp ===========

/*
	RFC - KTimer.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KTimer::KTimer()
{
	RFC_INIT_VERIFIER;
	resolution = 1000;
	started = false;
	listener = 0;
	timerID = KPlatformUtil::GetInstance()->GenerateTimerID(this);
}

void KTimer::SetInterval(int resolution)
{
	this->resolution = resolution;
}

int KTimer::GetInterval()
{
	return resolution;
}

void KTimer::SetTimerWindow(KWindow *window)
{
	this->window = window;
}

void KTimer::SetTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT KTimer::GetTimerID()
{
	return timerID;
}

void KTimer::StartTimer()
{
	if(started)
		return;

	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			::SetTimer(hwnd, timerID, resolution, 0);
			started = true;
		}
	}
}

void KTimer::StopTimer()
{
	if(window)
	{
		HWND hwnd = window->GetHWND();
		if(hwnd)
		{
			if(started)
				::KillTimer(hwnd, timerID);

			started = false;
		}
	}
}

bool KTimer::IsTimerRunning()
{
	return started;
}

void KTimer::OnTimer()
{
	if(listener)
		listener->OnTimer(this);
}

void KTimer::SetListener(KTimerListener *listener)
{
	this->listener = listener;
}

KTimer::~KTimer()
{
	if(started)
		this->StopTimer();
}

// =========== KTimerListener.cpp ===========

/*
	RFC - KTimerListener.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KTimerListener::KTimerListener(){}

KTimerListener::~KTimerListener(){}

void KTimerListener::OnTimer(KTimer *timer){}



// =========== KPerformanceCounter.cpp ===========

/*
	RFC - KPerformanceCounter.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KPerformanceCounter::KPerformanceCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);

	this->pcFreq = double(li.QuadPart) / 1000.0;
}

void KPerformanceCounter::StartCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	this->counterStart = li.QuadPart;
}

double KPerformanceCounter::EndCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - this->counterStart) / this->pcFreq;
}

KPerformanceCounter::~KPerformanceCounter()
{

}


// =========== KPlatformUtil.cpp ===========

/*
	RFC - KPlatformUtil.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


static const int rfc_InitialMenuItemCount	= 400;
static const int rfc_InitialTimerCount		= 40;
static const int rfc_InitialControlID		= 100;
static const int rfc_InitialMenuItemID		= 30000;
static const int rfc_InitialTimerID			= 1000;

KPlatformUtil* KPlatformUtil::_instance = 0;

KPlatformUtil::KPlatformUtil()
{
	RFC_INIT_VERIFIER;

	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	menuItemList = 0;
	timerList = 0;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::InitializeCriticalSection(&criticalSectionForCount);
	#endif
}

KPlatformUtil* KPlatformUtil::GetInstance()
{
	if(_instance)
		return _instance;
	_instance = new KPlatformUtil();
	return _instance;
}

UINT KPlatformUtil::GenerateControlID()
{
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	++controlCount;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return controlCount + rfc_InitialControlID;
}

UINT KPlatformUtil::GenerateMenuItemID(KMenuItem *menuItem)
{
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	if (menuItemList == 0) // generate on first call
		menuItemList = new KPointerList<KMenuItem*>(rfc_InitialMenuItemCount);

	++menuItemCount;
	menuItemList->AddPointer(menuItem);

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return menuItemCount + rfc_InitialMenuItemID;
}

KMenuItem* KPlatformUtil::GetMenuItemByID(UINT id)
{
	if (menuItemList)
		return menuItemList->GetPointer(id - (rfc_InitialMenuItemID + 1));
	return 0;
}

KString KPlatformUtil::GenerateClassName()
{ 
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'R';
	className[1] = L'F';
	className[2] = L'C';
	className[3] = L'_';

	::_itow((int)KApplication::hInstance, &className[4], 10);

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow(classCount, &className[lastPos + 1], 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	++classCount;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

UINT KPlatformUtil::GenerateTimerID(KTimer *timer)
{
	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::EnterCriticalSection(&criticalSectionForCount);
	#endif

	if (timerList == 0) // generate on first call
		timerList = new KPointerList<KTimer*>(rfc_InitialTimerCount);

	++timerCount;
	timerList->AddPointer(timer);

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::LeaveCriticalSection(&criticalSectionForCount);
	#endif

	return timerCount + rfc_InitialTimerID;
}

KTimer* KPlatformUtil::GetTimerByID(UINT id)
{
	if (timerList)
		return timerList->GetPointer(id - (rfc_InitialTimerID + 1));
	return 0;
}

KPlatformUtil::~KPlatformUtil()
{
	if (menuItemList)
		delete menuItemList;

	if (timerList)
		delete timerList;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	::DeleteCriticalSection(&criticalSectionForCount);
	#endif
}

// =========== KRegistry.cpp ===========

/*
	RFC - KRegistry.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


KRegistry::KRegistry()
{

}

bool KRegistry::CreateKey(HKEY hKeyRoot, const KString& subKey)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	::RegCloseKey(hkey);
	return true;
}

bool KRegistry::DeleteKey(HKEY hKeyRoot, const KString& subKey)
{
	return ::RegDeleteKeyW(hKeyRoot, subKey) == ERROR_SUCCESS ? true : false;
}

bool KRegistry::ReadString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString *result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			if (requiredBytes == 0) // value might be empty
			{
				*result = KString();
			}
			else{
				void* buffer = ::malloc(requiredBytes + 1); // +1 for strings which doesn't have ending null
				::ZeroMemory(buffer, requiredBytes + 1); // ending null might not contain in register

				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)buffer, &requiredBytes);
				*result = KString((wchar_t*)buffer, KString::FREE_TEXT_WHEN_DONE);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::WriteString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const int bCount = (value.GetLength() + 1) * sizeof(wchar_t); // +1 for ending null
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_SZ, (LPBYTE)(const wchar_t*)value, bCount);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::ReadDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD *result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		const LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, &dwType, (LPBYTE)result, &dwSize);
		::RegCloseKey(hkey);

		if (ret == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool KRegistry::WriteDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const DWORD dwSize = sizeof(DWORD);
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_DWORD, (LPBYTE)&value, dwSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool KRegistry::ReadBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void **buffer, DWORD *buffSize)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			*buffSize = requiredBytes;
			if (requiredBytes == 0) // value might be empty
			{
				*buffer = 0;
			}
			else{
				*buffer = ::malloc(requiredBytes);
				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)*buffer, &requiredBytes);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool KRegistry::WriteBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void *buffer, DWORD buffSize)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_BINARY, (LPBYTE)buffer, buffSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

KRegistry::~KRegistry()
{

}