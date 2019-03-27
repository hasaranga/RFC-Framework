
/*
	RFC - KString.h
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

#ifndef _RFC_KSTRING_H_
#define _RFC_KSTRING_H_

#ifdef _MSC_VER
	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE
	#endif
#endif

#include "../config.h"
#include "KStringHolder.h"
#include "../containers/KLeakDetector.h"

#include <windows.h>
#include <string.h>
#include <malloc.h>

/**
	Using a reference-counted internal representation, these strings are fast and efficient. <BR>
	KString was optimized to use with unicode strings. So, use unicode strings instead of ansi. <BR>
	KString does not support for multiple zero terminated strings. <BR>

	Optimization tips: <BR>
	use CONST_TXT macro when using statically typed text. <BR>
	use constructor instead of assignment (if can). <BR>
	use "Append" method instead of "+" operator. <BR>
	use "AppendStaticText" method instead of "Append" if you are using statically typed text. <BR>
	use "CompareWithStaticText" method instead of "Compare" if you are comparing statically typed text. <BR>
	define RFC_NO_SAFE_ANSI_STR if your app is not casting KString to ansi string within multiple threads. <BR>

	@code
	KString result1 = str1 + L"1234"; // slow
	KString result2 = str1 + CONST_TXT("1234"); // fast
	KString result3( str1 + CONST_TXT("1234") ); // more fast
	KString result4( str1.Append(CONST_TXT("1234")) ); // bit more fast
	KString result5( str1.AppendStaticText(TXT_WITH_LEN("1234")) ); // that's all you can have ;-)
	@endcode
*/
class KString
{
protected:
	mutable KStringHolder *stringHolder; // for empty string: stringHolder=0 && isStaticText=false
	bool isZeroLength; // true if empty string or staticText, stringHolder are zero length
	mutable bool isStaticText; // staticText & staticTextLength are valid only if this field is true. stringHolder can be zero even this filed is false.
	wchar_t *staticText;
	int staticTextLength;

	void ConvertToRefCountedStringIfStatic()const; // generates StringHolder object from static text

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
	KString(const wchar_t* const text, unsigned char behaviour = USE_COPY_OF_TEXT, int length = -1);

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


	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const KString& stringToAppend);

	/** 
		Appends a unicode string at the end of this one.
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
	KString Append(const KString& otherString)const;

	/**
		Appends a statically typed string to begining or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to begining if false
		@returns			the concatenated string
	*/
	KString AppendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void AssignStaticText(const wchar_t* const text, int length);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	KString SubString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool CompareIgnoreCase(const KString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool Compare(const KString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool CompareWithStaticText(const wchar_t* const text)const;

	/**
		Compare first character with given unicode character
	*/
	bool StartsWithChar(wchar_t character)const;

	/**
		Compare first character with given ansi character
	*/
	bool StartsWithChar(char character)const;

	/**
		Compare last character with given unicode character
	*/
	bool EndsWithChar(wchar_t character)const;

	/**
		Compare last character with given ansi character
	*/
	bool EndsWithChar(char character)const;

	/**
		Check if string is quoted or not
	*/
	bool IsQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t GetCharAt(int index)const;

	/**
		Returns number of characters in string
	*/
	int GetLength()const;

	/**
		Returns true if string is empty
	*/
	bool IsEmpty()const;

	bool IsNotEmpty()const;

	/**
		Returns value of string
	*/
	int GetIntValue()const;

	/** 
		Returns an upper-case version of this string.
	*/
	KString ToUpperCase()const;

	/** 
		Returns an lower-case version of this string. 
	*/
	KString ToLowerCase()const;

	virtual ~KString();

private:
	RFC_LEAK_DETECTOR(KString)
};

const KString operator+ (const char* const string1, const KString& string2);

const KString operator+ (const wchar_t* const string1, const KString& string2);

const KString operator+ (const KString& string1, const KString& string2);

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) KString(L##X, KString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X))

// do not make a copy + do not free + calculate length
#define STATIC_TXT(X) KString(L##X, KString::STATIC_TEXT_DO_NOT_FREE, -1)

// do not make a copy + free when done + calculate length
#define BUFFER_TXT(X) KString(X, KString::FREE_TEXT_WHEN_DONE, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, KString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)

#endif