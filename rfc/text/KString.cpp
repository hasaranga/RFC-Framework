
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

#include "KString.h"
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

KString::~KString()
{
	if (stringHolder)
		stringHolder->ReleaseReference();
}