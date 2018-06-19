
/*
	RFC - KString.h
	Copyright (C) 2013-2018 CrownSoft
  
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

#ifndef _RFC_KSTRING_H_
#define _RFC_KSTRING_H_

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "../config.h"
#include "KStringHolder.h"
#include "../containers/KLeakDetector.h"

#include <windows.h>
#include <string.h>
#include <malloc.h>

/**
	Using a reference-counted internal representation, these strings are fast and efficient.
	KString was optimized to use with unicode strings. So, use unicode strings instead of ansi.
	KString does not support for multiple zero terminated strings.
*/
class RFC_API KString
{
protected:
	KStringHolder *stringHolder;

public:

	enum TextTypes
	{
		STATIC_TEXT_DO_NOT_FREE = 1,
		FREE_TEXT_WHEN_DONE = 2,
		USE_COPY_OF_TEXT = 3,
	};

	/**
		Constructs an empty string
	*/
	KString();

	/**
		Constructs copy of another string
	*/
	KString(const KString& other);

	/**
		Constructs String object using ansi string
	*/
	KString(const char* const text, UINT codePage = CP_UTF8);

	/**
		Constructs String object using unicode string
	*/
	KString(const wchar_t* const text, unsigned char behaviour = USE_COPY_OF_TEXT);

	/**
		Constructs String object using integer
	*/
	KString(const int value, const int radix = 10);

	/**
		Constructs String object using float
		@param compact   removes ending decimal zeros if true
	*/
	KString(const float value, const int numDecimals, bool compact = false);

	/** 
		Replaces this string's contents with another string.
	*/
	const KString& operator= (const KString& other);

	/** 
		Replaces this string's contents with unicode string. 
	*/
	const KString& operator= (const wchar_t* const other);


	/** Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const KString& stringToAppend);

	/** Appends a unicode string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const wchar_t* const textToAppend);
	/**
		Returns ansi version of this string
	*/
	operator const char*()const;

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const;

	/**
		Returns unicode version of this string
	*/
	operator wchar_t*()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const char operator[](const int index)const;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	virtual KString Append(const KString& otherString)const;

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	virtual KString SubString(int start, int end)const;

	/** 
		Case-insensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	virtual bool EqualsIgnoreCase(const KString& otherString)const;

	/**
		Compare first character with given unicode character
	*/
	virtual bool StartsWithChar(wchar_t character)const;
	/**
		Compare first character with given ansi character
	*/
	virtual bool StartsWithChar(char character)const;

	/**
		Compare last character with given unicode character
	*/
	virtual bool EndsWithChar(wchar_t character)const;

	/**
		Compare last character with given ansi character
	*/
	virtual bool EndsWithChar(char character)const;

	/**
		Check if string is quoted or not
	*/
	virtual bool IsQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	virtual wchar_t GetCharAt(int index)const;

	/**
		Returns number of characters in string
	*/
	virtual int GetLength()const;

	/**
		Returns true if string is empty
	*/
	virtual bool IsEmpty()const;

	/**
		Returns value of string
	*/
	virtual int GetIntValue()const;

	virtual ~KString();

private:
	RFC_LEAK_DETECTOR(KString)
};

RFC_API const KString operator+ (const char* const string1, const KString& string2);

RFC_API const KString operator+ (const wchar_t* const string1, const KString& string2);

RFC_API const KString operator+ (const KString& string1, const KString& string2);

#define STATIC_TXT(X) KString(L##X, KString::STATIC_TEXT_DO_NOT_FREE)
#define BUFFER_TXT(X) KString(X, KString::FREE_TEXT_WHEN_DONE)

#endif