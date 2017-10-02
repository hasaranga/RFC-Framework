
/*
RFC - KString.cpp
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

#include"KString.h"
#include <stdio.h>

const KString operator+ (const char* const string1, const KString& string2)
{
	KString s(string1);
	s = s+string2;
	return s;
}

const KString operator+ (const wchar_t* const string1, const KString& string2)
{
	KString s(string1);
	s = s+string2;
	return s;
}

const KString operator+ (const KString& string1, const KString& string2)
{
	KString s(string1);
	s = s+string2;
	return s;
}

KString::KString()
{
	stringHolder=0;
}

KString::KString(const KString& other)
{
	if(other.stringHolder)
	{
		other.stringHolder->AddReference();
		stringHolder=other.stringHolder;
	}else
	{
		stringHolder=0;
	}
}

KString::KString(const char* const text,UINT codePage)
{

	if (text != 0)
	{
		int count = ::MultiByteToWideChar(codePage, 0, text, -1, 0,0); // get char count with null character
		if (count)
		{
			wchar_t *w_text = (wchar_t *)::malloc(count*sizeof(wchar_t));
			if (::MultiByteToWideChar(codePage, 0, text, -1, w_text,count))
			{
				count--; // ignore null character

				stringHolder=new KStringHolder();
				stringHolder->AddReference();

				stringHolder->w_text=w_text;
				stringHolder->count=count;
				return;
			}else
			{
				::free(w_text);
			}
		}
	}

	stringHolder=0;		
}

KString::KString(const wchar_t* const text)
{

	if (text != 0)
	{
		int count = (int)::wcslen(text);
		if(count)
		{
			stringHolder=new KStringHolder();
			stringHolder->AddReference();

			stringHolder->w_text=(wchar_t*)::malloc((count+1)*sizeof(wchar_t));
			::wcscpy(stringHolder->w_text,text);
			stringHolder->count=count;
			return;
		}
	}

	stringHolder=0;	
}

KString::KString(const int value,const int radix)
{
	stringHolder=new KStringHolder();
	stringHolder->AddReference();

	stringHolder->w_text=(wchar_t *)::malloc(33*sizeof(wchar_t)); // max 32 digits
	::_itow(value,stringHolder->w_text,radix);

	stringHolder->count = (int)::wcslen(stringHolder->w_text);
}

KString::KString(const float value, const int numDecimals, bool compact)
{
	// round it to given digits
	char l_fmtp[32], l_buf[64];
	sprintf(l_fmtp, "%%.%df", numDecimals);
	sprintf(l_buf, l_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(l_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (l_buf[len - i] == '0')
				l_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(l_buf) - 1;
		if (l_buf[len] == '.')
			l_buf[len] = 0; // kill it
	}

	int count = ::MultiByteToWideChar(CP_UTF8, 0, l_buf, -1, 0, 0); // get char count with null character
	if (count)
	{
		wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
		if (::MultiByteToWideChar(CP_UTF8, 0, l_buf, -1, w_text, count))
		{
			count--; // ignore null character

			stringHolder = new KStringHolder();
			stringHolder->AddReference();

			stringHolder->w_text = w_text;
			stringHolder->count = count;

			return;
		}
		else
		{
			::free(w_text);
		}
	}
}

const KString& KString::operator= (const KString& other)
{

	if(stringHolder)
	{
		stringHolder->ReleaseReference();
	}

	if(other.stringHolder)
	{
		other.stringHolder->AddReference();
	}
	
	stringHolder=other.stringHolder;

	return *this;
}

const KString& KString::operator= (const wchar_t* const other)
{

	if(stringHolder)
	{
		stringHolder->ReleaseReference();
	}

	if (other != 0)
	{
		int count = (int)::wcslen(other);
		if(count)
		{
			stringHolder=new KStringHolder();
			stringHolder->AddReference();

			stringHolder->w_text=(wchar_t*)::malloc((count+1)*sizeof(wchar_t));
			::wcscpy(stringHolder->w_text,other);
			stringHolder->count=count;
			return *this;
		}
	}

	stringHolder=0;	
	return *this;
}

const KString KString::operator+ (const KString& stringToAppend)
{
	return Append(stringToAppend);
}

const KString KString::operator+ (const wchar_t* const textToAppend)
{
	return Append(KString(textToAppend));
}

KString::operator const char*()const
{
	if(stringHolder)
	{
		return stringHolder->GetAnsiVersion();
	}else
	{
		return "";
	}
}

KString::operator const wchar_t*()const
{
	if(stringHolder)
	{
		return stringHolder->w_text;
	}else
	{
		return L"";
	}
}

const char KString::operator[](const int index)const
{
	if(stringHolder)
	{
		if((0<=index) && (index<=(stringHolder->count-1)))
		{
			return stringHolder->GetAnsiVersion()[index];
		}
	}
	return -1;
}

KString KString::Append(const KString& otherString)const
{
	if( (otherString.stringHolder!=0) && (otherString.stringHolder->count!=0) )
	{
		KString result;
		result.stringHolder=new KStringHolder();
		result.stringHolder->AddReference();

		int length=otherString.stringHolder->count;
		int count=stringHolder?stringHolder->count:0;

		result.stringHolder->w_text=(wchar_t*)::malloc((length+count+1)*sizeof(wchar_t));
		if(count) // this string is not empty!
		{
			::wcscpy(result.stringHolder->w_text,stringHolder->w_text);
			::wcsncat(result.stringHolder->w_text,otherString.stringHolder->w_text,length);
		}else
		{
			::wcscpy(result.stringHolder->w_text,otherString.stringHolder->w_text);
		}

		result.stringHolder->count=length+count;
		return result;
	}else
	{
		return *this;
	}
}

KString KString::SubString(int start, int end)const
{
	int count=stringHolder?stringHolder->count:0;

	if((0<=start) && (start<=(count-1)))
	{
		if((start<end) && (end<=(count-1)))
		{
			int size=(end-start)+1;
			wchar_t* buf=(wchar_t*)::malloc((size+1)*sizeof(wchar_t));
			::wcsncpy(buf,&stringHolder->w_text[start],size);
			buf[size]=0;

			KString result(buf);
			::free(buf);
			return result;
		}
	}
	return KString();
}

bool KString::EqualsIgnoreCase(const KString& otherString)const
{
	if( (otherString.stringHolder!=0) && (otherString.stringHolder->count!=0) )
	{
		if( (stringHolder!=0) && (stringHolder->count!=0) )
		{
			if(::wcscmp(stringHolder->w_text,otherString.stringHolder->w_text)==0)
			{
				return true;
			}
		}
	}
	return false;
}

bool KString::StartsWithChar(wchar_t character)const
{
	if( (stringHolder!=0) && (stringHolder->count!=0) )
	{
		if(stringHolder->w_text[0]==character)
		{
			return true;
		}
	}
	return false;
}

bool KString::StartsWithChar(char character)const
{
	if( (stringHolder!=0) && (stringHolder->count!=0) )
	{
		if(stringHolder->GetAnsiVersion()[0]==character)
		{
			return true;
		}
	}
	return false;
}

bool KString::EndsWithChar(wchar_t character)const
{
	if( (stringHolder!=0) && (stringHolder->count!=0) )
	{
		if(stringHolder->w_text[stringHolder->count-1]==character)
		{
			return true;
		}
	}
	return false;
}

bool KString::EndsWithChar(char character)const
{
	if( (stringHolder!=0) && (stringHolder->count!=0) )
	{
		if(stringHolder->GetAnsiVersion()[stringHolder->count-1]==character)
		{
			return true;
		}
	}
	return false;
}

bool KString::IsQuotedString()const
{
	if( (stringHolder!=0) && (stringHolder->count>1) )
	{
		if(StartsWithChar(L'\"') && EndsWithChar(L'\"'))
		{
			return true;
		}
	}
	return false;
}

wchar_t KString::GetCharAt(int index)const
{
	int count=stringHolder?stringHolder->count:0;

	if((0<=index) && (index<=(count-1)))
		return stringHolder->w_text[index];
	return -1;
}

int KString::GetLength()const
{
	return stringHolder?stringHolder->count:0;
}

bool KString::IsEmpty()const
{
	if(stringHolder?stringHolder->count:0)
	{
		return false;
	}
	return true;
}

int KString::GetIntValue()const
{
	if(IsEmpty())
	{
		return 0;
	}
	return ::_wtoi(stringHolder->w_text);
}

KString::~KString()
{
	if(stringHolder)
	{
		stringHolder->ReleaseReference();
	}
}

