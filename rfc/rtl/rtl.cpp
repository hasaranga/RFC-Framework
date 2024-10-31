
#include "rtl.h"

static const int rtl_InitialControlID = 100;

TIDGenerator* TIDGenerator::_instance = 0;

TIDGenerator::TIDGenerator()
{
	classCount = 0;
	controlCount = 0;
}

TIDGenerator* TIDGenerator::GetInstance()
{
	if (_instance)
		return _instance;
	_instance = new TIDGenerator();
	return _instance;
}

UINT TIDGenerator::GenerateControlID()
{
	++controlCount;
	return controlCount + rtl_InitialControlID;
}

KString TIDGenerator::GenerateClassName()
{
	wchar_t* className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'R';
	className[1] = L'T';
	className[2] = L'L';
	className[3] = L'_';

	int hInstance32 = HandleToLong(KApplication::hInstance);
	if (hInstance32 < 0)
		hInstance32 *= -1;

	::_itow_s(hInstance32, &className[4], 28, 10); // 28 is buffer size

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow_s(classCount, &className[lastPos + 1], 32 - (lastPos + 1), 10);

	++classCount;

	return KString(className, KString::FREE_TEXT_WHEN_DONE);
}

TIDGenerator::~TIDGenerator()
{

}