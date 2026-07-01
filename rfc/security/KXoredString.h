
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

#include "../core/CoreModule.h"
#include <cstddef>

template <typename CharT, std::size_t N, CharT KEY>
struct KXoredString {
	static constexpr std::size_t size = N; // with null char
	CharT data[N]{};

	void decode(CharT(&out)[N]) const {
		const volatile CharT* src = data; // prevents optimization
		for (std::size_t i = 0; i < N; ++i)
			out[i] = src[i] ^ KEY;
	}
};

/*
	creates a compile time xored string. decode function works at runtime.

	example code:
	static constexpr auto xoredMsg = kxored_str<L'\x5A5A'>(L"Hello...\nHow are you?");
	
	wchar_t plain[xoredMsg.size];
	xoredMsg.decode(plain);
	wprintf(plain);

	static constexpr auto xoredMsg2 = kxored_str<'\x5A'>("fine... And you?");

	char plain2[xoredMsg2.size];
	xoredMsg2.decode(plain2);
	printf(plain2);
*/
template <auto KEY, std::size_t N, typename CharT = decltype(KEY)>
consteval KXoredString<CharT, N, KEY> kxored_str(const CharT(&str)[N]) {
	KXoredString<CharT, N, KEY> result{};
	for (std::size_t i = 0; i < N; ++i)
		result.data[i] = str[i] ^ KEY;
	return result;
}

/* 
	defines xored wchar_t array which decode its value at runtime.
	DEF_XORED_VAR(strMsg, L'\x5A5A', L"Hello World");
*/
#define DEF_XORED_VAR(name, key, text) \
    static constexpr auto xored_##name = kxored_str<key>(text); \
    wchar_t name[xored_##name.size]; \
    xored_##name.decode(name);

// compile time 16bit number generator based on time and __COUNTER__ .
template<uint32_t Counter>
constexpr wchar_t kmake_xorkey() {
	constexpr const char* t = __TIME__;
	constexpr uint32_t h1 = (2166136261u ^ (uint8_t)t[0]) * 16777619u;
	constexpr uint32_t h2 = (h1 ^ (uint8_t)t[1]) * 16777619u;
	constexpr uint32_t h3 = (h2 ^ (uint8_t)t[3]) * 16777619u;
	constexpr uint32_t h4 = (h3 ^ Counter) * 16777619u;
	return (wchar_t)(h4 & 0xFFFF);
}

/*
	defines xored KString variable which decode its value at runtime.
    xor key is generated at compile time.
	DEF_XORED_KSTRING(msg, L"Hello World");
*/
#define DEF_XORED_KSTRING(name, text) \
    static constexpr auto xored_##name = kxored_str<kmake_xorkey<__COUNTER__>()>(text); \
    wchar_t array_##name[xored_##name.size]; \
    xored_##name.decode(array_##name); \
	KString name(array_##name, KStringBehaviour::MAKE_A_COPY, xored_##name.size - 1);