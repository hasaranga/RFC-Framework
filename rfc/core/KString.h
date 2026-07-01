/*
	Copyright (C) 2013-2026 CrownSoft
  
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

#include "KRefCountedMemory.h"
#include "KLeakDetector.h"
#include "KAssert.h"

#include <windows.h>
#include <string.h>
#include <malloc.h>
#include <functional>

/*
	DO_NOT_FREE: supplied pointer is a static string literal(always available). it will not freed on destroy.
	FREE_ON_DESTROY: supplied pointer is a heap memory created using malloc. it will be freed on destroy.
	MAKE_A_COPY: make a copy of supplied string. if string length is larger than 11, heap memory will be allocated.
*/
enum class KStringBehaviour { DO_NOT_FREE,
	FREE_ON_DESTROY, 
	MAKE_A_COPY
};

// define RFC_NO_CHECK_ARRAY_AS_LITERAL if you want to disable the check for array passed as string literal at debug mode.
#ifndef RFC_NO_CHECK_ARRAY_AS_LITERAL
#define RFC_CHECK_ARRAY_AS_LITERAL(literal,N) K_ASSERT(wcslen(literal) == (N - 1), "array used as a string literal. please use the array with KString constructor that accepts a behavior parameter.");
#else
#define RFC_CHECK_ARRAY_AS_LITERAL(literal,N) 
#endif

enum class KStringBufferType : unsigned char { StaticText, SSOText, HeapText };

/**
	Using a reference-counted internal representation for long strings and Small String Optimization (SSO) for short strings.
	KString was optimized to use with unicode strings. So, use unicode strings instead of ansi.
	KString does not support for multiple zero terminated strings.
	wchar_t buff[32];
	KString txt(buff); should not be used. it will treat as literal.
	KString txt(buff, KStringBehaviour::MAKE_A_COPY); correct!
	STR_COPY(buff); correct!

	SSO Implementation:
	- Strings with 11 characters or fewer are stored directly in the object (Small String Optimization)
	- Longer strings use the heap with KRefCountedMemory mechanism
	- Static text references just use pointer assignment

	Optimization tips:
	use unicode strings instead of ansi.
	try to use strings with length less than 12.
	L"hello"_st is same as CONST_TXT("hello") or KString(L"hello",KStringBehaviour::DO_NOT_FREE,5)
	use "CompareWithStaticText" method instead of "Compare" if you are comparing statically typed text.

*/
class KString
{
public:
	// SSO buffer size: can fit up to 11 wchar_t characters + null terminator in 24 bytes
	static const int SSO_BUFFER_SIZE = 12;

protected:
	// we try to make sizeof KString to be 32 bytes for better cache align.

	// you can use either data.ssoBuffer or data.staticText or data.refCountedMem. 
	// can use only one at a time. selected by the bufferType.
	union {
		KRefCountedMemory<wchar_t*>* refCountedMem;
		const wchar_t* staticText;
		wchar_t ssoBuffer[SSO_BUFFER_SIZE]; // for small strings
	} data; 

	int characterCount; // character count (empty string has zero characterCount)
	KStringBufferType bufferType;

	inline void markAsEmptyString() noexcept;

	void initFromLiteral(const wchar_t* literal, size_t N) noexcept;
	void assignFromLiteral(const wchar_t* literal, size_t N) noexcept;
	void copyFromOther(const KString& other) noexcept;
public:

	/**
		Constructs an empty string
	*/
	KString() noexcept;

	/**
		Constructs copy of another string.
		Same performance as move. lightweight!
	*/
	KString(const KString& other) noexcept;

	/**
		Move constructor. Same performance as copy. other string will be cleared.
	*/
	KString(KString&& other) noexcept;

	/**
		Constructs String object using ansi string
	*/
	KString(const char* const text, UINT codePage = CP_UTF8) noexcept;

	/**
		Constructs String object using unicode string literal
	*/
	template<size_t N>
	KString(const wchar_t(&literal)[N]) noexcept
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literal, N);
		initFromLiteral(literal, N);
	}

	/**
		Constructs String object using unicode string pointer
	*/
	KString(const wchar_t* const text, KStringBehaviour behaviour, int length = -1) noexcept;

	/**
		Constructs String object using integer
	*/
	KString(const int value, const int radix = 10) noexcept;

	/**
		Constructs String object using float
		@param compact   removes ending decimal zeros if true
	*/
	KString(const float value, const int numDecimals, bool compact = false) noexcept;

	/** 
		Replaces this string's contents with another string.
	*/
	const KString& operator= (const KString& other) noexcept;

	// Move assignment. clears other string.
	KString& operator= (KString&& other) noexcept;

	/**
		Replaces this string's contents with static unicode string literal.
	*/
	template<size_t N>
	const KString& operator= (const wchar_t(&literal)[N]) noexcept
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literal, N);
		assignFromLiteral(literal, N);
		return *this;
	}

	// compare with other string
	bool operator==(const KString& other) const noexcept;

	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const KString& stringToAppend) noexcept;

	/**
		Appends a unicode string literal at the end of this one.
		@returns     the concatenated string
	*/
	template<size_t N>
	const KString operator+ (const wchar_t(&literalToAppend)[N]) noexcept
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literalToAppend, N);
		return appendStaticText(literalToAppend, (int)N - 1);
	}

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const noexcept;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const wchar_t operator[](const int index)const noexcept;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	KString append(const KString& otherString)const noexcept;

	/**
		Appends a statically typed string to beginning or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to beginning if false
		@returns			the concatenated string
	*/
	KString appendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const noexcept;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void assignStaticText(const wchar_t* const text, int length) noexcept;

	// clears the content of the string.
	void clear() noexcept;

	// the string automatically clears and converted to SSOText when you call accessRawSSOBuffer.
	// sso buffer size is KString::SSO_BUFFER_SIZE in wchars.
	void accessRawSSOBuffer(wchar_t** ssoBuffer, int** ppLength) noexcept;

	/**
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be returned.

		@param start   the index of the start of the substring needed (inclusive)
		@param end     the index of the end of the substring needed (inclusive)
		@return        substring containing characters from start to end (both inclusive)

		Example: "Hello".subString(1, 3) returns "ell"
	*/
	KString subString(int start, int end)const noexcept;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool compareIgnoreCase(const KString& otherString)const noexcept;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool compare(const KString& otherString)const noexcept;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool compareWithStaticText(const wchar_t* const text)const noexcept;

	/**
		Compare first character with given unicode character
	*/
	bool startsWithChar(wchar_t character)const noexcept;

	/**
		Compare last character with given unicode character
	*/
	bool endsWithChar(wchar_t character)const noexcept;

	/**
		Check if string is quoted or not
	*/
	bool isQuotedString()const noexcept;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t getCharAt(int index)const noexcept;

	KStringBufferType getBufferType()const noexcept;

	/**
		Returns number of characters in string
	*/
	int length()const noexcept;

	/**
		Returns true if string is empty
	*/
	bool isEmpty()const noexcept;

	bool isNotEmpty()const noexcept;

	/**
		Returns value of string
	*/
	int getIntValue()const noexcept;

	/** 
		Returns an upper-case version of this string.
	*/
	KString toUpperCase()const noexcept;

	/** 
		Returns an lower-case version of this string. 
	*/
	KString toLowerCase()const noexcept;

	// trim from both sides.
	KString trim() const noexcept;

	// separate a string by newlines with index. remaining content will be also passed at the end.
	void splitLines(bool ignoreEmptyLines, std::function<void(int, const KString&)> func) const noexcept;

	// free the returned buffer when done.
	static char* toUTF8String(const wchar_t* text) noexcept;

	// free the returned buffer when done.
	static wchar_t* toUnicodeString(const char* utf8Text) noexcept;

	~KString() noexcept;

private:
	/**
		Returns pointer to the actual string data regardless of storage type
	*/
	const wchar_t* getStringPtr() const noexcept;

	RFC_LEAK_DETECTOR(KString)
};

// static text literal operator
namespace kstring_literals {
	KString operator"" _st(const wchar_t* str, size_t len) noexcept;
}

using namespace kstring_literals;

const KString operator+ (const char* const string1, const KString& string2) noexcept;

const KString operator+ (const wchar_t* const string1, const KString& string2) noexcept;

const KString operator+ (const KString& string1, const KString& string2) noexcept;

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// X is a literal without L prefix. do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) KString(L##X, KStringBehaviour::DO_NOT_FREE, LEN_UNI_STR(L##X))

// X is a unicode variable or array. make a copy + calculate length
#define STR_COPY(X) KString(X, KStringBehaviour::MAKE_A_COPY, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, KStringBehaviour::DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)
