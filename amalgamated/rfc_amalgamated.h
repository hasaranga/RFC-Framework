
// ========== Generated With RFC Amalgamator v1.0 ==========

#ifndef _RFC_AMALGAMATED_H_
#define _RFC_AMALGAMATED_H_ 

#define AMALGAMATED_VERSION
#define _CRT_SECURE_NO_WARNINGS

// =========== sha1.h ===========

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


#ifndef ___SHA1_HDR___
#define ___SHA1_HDR___

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined(SHA1_UTILITY_FUNCTIONS) && !defined(SHA1_NO_UTILITY_FUNCTIONS)
#define SHA1_UTILITY_FUNCTIONS
#endif

#include <memory.h> // Required for memset and memcpy

#ifdef SHA1_UTILITY_FUNCTIONS
#include <stdio.h>  // Required for file access and sprintf
#include <string.h> // Required for strcat and strcpy
#endif

#ifdef _MSC_VER
#include <stdlib.h>
#endif


namespace ExtLibs{

	// You can define the endian mode in your files, without modifying the SHA1
	// source files. Just #define SHA1_LITTLE_ENDIAN or #define SHA1_BIG_ENDIAN
	// in your files, before including the SHA1.h header file. If you don't
	// define anything, the class defaults to little endian.
#if !defined(SHA1_LITTLE_ENDIAN) && !defined(SHA1_BIG_ENDIAN)
#define SHA1_LITTLE_ENDIAN
#endif

	// Same here. If you want variable wiping, #define SHA1_WIPE_VARIABLES, if
	// not, #define SHA1_NO_WIPE_VARIABLES. If you don't define anything, it
	// defaults to wiping.
#if !defined(SHA1_WIPE_VARIABLES) && !defined(SHA1_NO_WIPE_VARIABLES)
#define SHA1_WIPE_VARIABLES
#endif

	/////////////////////////////////////////////////////////////////////////////
	// Define 8- and 32-bit variables

#ifndef UINT_32

#ifdef _MSC_VER // Compiling with Microsoft compiler

#define UINT_8  unsigned __int8
#define UINT_32 unsigned __int32

#else // !_MSC_VER

#define UINT_8 unsigned char

#if (ULONG_MAX == 0xFFFFFFFF)
#define UINT_32 unsigned long
#else
#define UINT_32 unsigned int
#endif

#endif // _MSC_VER
#endif // UINT_32

	/////////////////////////////////////////////////////////////////////////////
	// Declare SHA1 workspace

	typedef union
	{
		UINT_8 c[64];
		UINT_32 l[16];
	} SHA1_WORKSPACE_BLOCK;

	class CSHA1
	{
	public:
#ifdef SHA1_UTILITY_FUNCTIONS
		// Two different formats for ReportHash(...)
		enum
		{
			REPORT_HEX = 0,
			REPORT_DIGIT = 1
		};
#endif

		// Constructor and destructor
		CSHA1();
		~CSHA1();

		UINT_32 m_state[5];
		UINT_32 m_count[2];
		UINT_32 m_reserved1[1]; // Memory alignment padding
		UINT_8 m_buffer[64];
		UINT_8 m_digest[20];
		UINT_32 m_reserved2[3]; // Memory alignment padding

		void Reset();

		// Update the hash value
		void Update(const UINT_8* pData, UINT_32 uLen);
#ifdef SHA1_UTILITY_FUNCTIONS
		bool HashFile(const char* szFileName);
#endif

		// Finalize hash and report
		void Final();

		// Report functions: as pre-formatted and raw data
#ifdef SHA1_UTILITY_FUNCTIONS
		void ReportHash(char* szReport, unsigned char uReportType = REPORT_HEX) const;
#endif
		void GetHash(UINT_8* puDest) const;

	private:
		// Private SHA-1 transformation
		void Transform(UINT_32* pState, const UINT_8* pBuffer);

		// Member variables
		UINT_8 m_workspace[64];
		SHA1_WORKSPACE_BLOCK* m_block; // SHA1 pointer to the byte array above
	};

}

#endif

// =========== md5.h ===========

#ifndef _EXT_MD5_H
#define _EXT_MD5_H

	// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
	// rights reserved.

	// License to copy and use this software is granted provided that it
	// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
	// Algorithm" in all material mentioning or referencing this software
	// or this function.
	//
	// License is also granted to make and use derivative works provided
	// that such works are identified as "derived from the RSA Data
	// Security, Inc. MD5 Message-Digest Algorithm" in all material
	// mentioning or referencing the derived work.
	//
	// RSA Data Security, Inc. makes no representations concerning either
	// the merchantability of this software or the suitability of this
	// software for any particular purpose. It is provided "as is"
	// without express or implied warranty of any kind.
	//
	// These notices must be retained in any copies of any part of this
	// documentation and/or software.



	// The original md5 implementation avoids external libraries.
	// This version has dependency on stdio.h for file input and
	// string.h for memcpy.


#include <stdio.h>
#include <string.h>

namespace ExtLibs{

#pragma region MD5 defines
	// Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

	static unsigned char PADDING[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

	// ROTATE_LEFT rotates x left n bits.
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

	// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
	// Rotation is separate from addition to prevent recomputation.
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
		}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
		}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
		}
#pragma endregion

	typedef unsigned char BYTE;

	// POINTER defines a generic pointer type
	typedef unsigned char *POINTER;

	// UINT2 defines a two byte word
	typedef unsigned short int UINT2;

	// UINT4 defines a four byte word
	typedef unsigned long int UINT4;


	// convenient object that wraps
	// the C-functions for use in C++ only
	class MD5
	{
	private:
		struct __context_t {
			UINT4 state[4];                                   /* state (ABCD) */
			UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
			unsigned char buffer[64];                         /* input buffer */
		} context;

#pragma region static helper functions
		// The core of the MD5 algorithm is here.
		// MD5 basic transformation. Transforms state based on block.
		static void MD5Transform(UINT4 state[4], unsigned char block[64])
		{
			UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

			Decode(x, block, 64);

			/* Round 1 */
			FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
			FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
			FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
			FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
			FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
			FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
			FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
			FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
			FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
			FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
			FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
			FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
			FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
			FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
			FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
			FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

			/* Round 2 */
			GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
			GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
			GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
			GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
			GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
			GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
			GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
			GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
			GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
			GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
			GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
			GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
			GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
			GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
			GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
			GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

			/* Round 3 */
			HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
			HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
			HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
			HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
			HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
			HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
			HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
			HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
			HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
			HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
			HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
			HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
			HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
			HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
			HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
			HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

			/* Round 4 */
			II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
			II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
			II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
			II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
			II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
			II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
			II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
			II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
			II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
			II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
			II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
			II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
			II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
			II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
			II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
			II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

			state[0] += a;
			state[1] += b;
			state[2] += c;
			state[3] += d;

			// Zeroize sensitive information.
			memset((POINTER)x, 0, sizeof (x));
		}

		// Encodes input (UINT4) into output (unsigned char). Assumes len is
		// a multiple of 4.
		static void Encode(unsigned char *output, UINT4 *input, unsigned int len)
		{
			unsigned int i, j;

			for (i = 0, j = 0; j < len; i++, j += 4) {
				output[j] = (unsigned char)(input[i] & 0xff);
				output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
				output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
				output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
			}
		}

		// Decodes input (unsigned char) into output (UINT4). Assumes len is
		// a multiple of 4.
		static void Decode(UINT4 *output, unsigned char *input, unsigned int len)
		{
			unsigned int i, j;

			for (i = 0, j = 0; j < len; i++, j += 4)
				output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
				(((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
		}
#pragma endregion


	public:
		// MAIN FUNCTIONS
		MD5()
		{
			Init();
		}

		// MD5 initialization. Begins an MD5 operation, writing a new context.
		void Init()
		{
			context.count[0] = context.count[1] = 0;

			// Load magic initialization constants.
			context.state[0] = 0x67452301;
			context.state[1] = 0xefcdab89;
			context.state[2] = 0x98badcfe;
			context.state[3] = 0x10325476;
		}

		// MD5 block update operation. Continues an MD5 message-digest
		// operation, processing another message block, and updating the
		// context.
		void Update(
			unsigned char *input,   // input block
			unsigned int inputLen) // length of input block
		{
			unsigned int i, index, partLen;

			// Compute number of bytes mod 64
			index = (unsigned int)((context.count[0] >> 3) & 0x3F);

			// Update number of bits
			if ((context.count[0] += ((UINT4)inputLen << 3))
				< ((UINT4)inputLen << 3))
				context.count[1]++;
			context.count[1] += ((UINT4)inputLen >> 29);

			partLen = 64 - index;

			// Transform as many times as possible.
			if (inputLen >= partLen) {
				memcpy((POINTER)&context.buffer[index], (POINTER)input, partLen);
				MD5Transform(context.state, context.buffer);

				for (i = partLen; i + 63 < inputLen; i += 64)
					MD5Transform(context.state, &input[i]);

				index = 0;
			}
			else
				i = 0;

			/* Buffer remaining input */
			memcpy((POINTER)&context.buffer[index], (POINTER)&input[i], inputLen - i);
		}

		// MD5 finalization. Ends an MD5 message-digest operation, writing the
		// the message digest and zeroizing the context.
		// Writes to digestRaw
		void Final()
		{
			unsigned char bits[8];
			unsigned int index, padLen;

			// Save number of bits
			Encode(bits, context.count, 8);

			// Pad out to 56 mod 64.
			index = (unsigned int)((context.count[0] >> 3) & 0x3f);
			padLen = (index < 56) ? (56 - index) : (120 - index);
			Update(PADDING, padLen);

			// Append length (before padding)
			Update(bits, 8);

			// Store state in digest
			Encode(digestRaw, context.state, 16);

			// Zeroize sensitive information.
			memset((POINTER)&context, 0, sizeof (context));

			writeToString();
		}

		/// Buffer must be 32+1 (nul) = 33 chars long at least 
		void writeToString()
		{
			int pos;

			for (pos = 0; pos < 16; pos++)
				sprintf(digestChars + (pos * 2), "%02x", digestRaw[pos]);
		}


	public:
		// an MD5 digest is a 16-byte number (32 hex digits)
		BYTE digestRaw[16];

		// This version of the digest is actually
		// a "printf'd" version of the digest.
		char digestChars[33];

		/// Load a file from disk and digest it
		// Digests a file and returns the result.
		char* digestFile(char *filename)
		{
			Init();

			FILE *file;

			int len;
			unsigned char buffer[1024];

			if ((file = fopen(filename, "rb")) == NULL) // file open failed
			{
				digestChars[0] = 0; 
			}
			else
			{
				while (len = (int)fread(buffer, 1, 1024, file))
					Update(buffer, len);
				Final();

				fclose(file);
			}

			return digestChars;
		}

		/// Digests a byte-array already in memory
		char* digestMemory(BYTE *memchunk, int len)
		{
			Init();
			Update(memchunk, len);
			Final();

			return digestChars;
		}
		
		// Digests a string and prints the result.
		char* digestString(char *string)
		{
			Init();
			Update((unsigned char*)string, (unsigned int)strlen(string));
			Final();

			return digestChars;
		}
	};


}

#endif

// =========== config.h ===========

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef AMALGAMATED_VERSION
	#define RFC_API
#else
	#ifdef _MSC_VER // M$ compiler
		#ifdef RFC_STATIC_APP
			#define RFC_API 
		#else
			#ifdef RFC_DLL
				#define RFC_API __declspec(dllexport)
			#else
				#define RFC_API __declspec(dllimport)
				#pragma comment(lib, "rfc026.lib")
			#endif
		#endif
	#else // non M$ compilers must use entire source code or amalgamated version.
		#define RFC_API 
	#endif
#endif


#ifdef _WIN64
	#define RFC64
	#define PTR_SIZE 8
	#define NATIVE_INT __int64
#else
	#define RFC32
	#define PTR_SIZE 4
	#define NATIVE_INT int
#endif


#endif

// =========== KMenuItemListener.h ===========

/*
	RFC - KMenuItemListener.h
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

#ifndef _RFC_KMENUITEMLISTENER_H_
#define _RFC_KMENUITEMLISTENER_H_


class KMenuItem;

class RFC_API KMenuItemListener
{
public:
	KMenuItemListener();

	virtual ~KMenuItemListener();

	virtual void OnMenuItemPress(KMenuItem *menuItem);
};

#endif

// =========== KGridViewListener.h ===========

/*
	RFC - KGridViewListener.h
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

#ifndef _RFC_KGRIDVIEWLISTENER_H_
#define _RFC_KGRIDVIEWLISTENER_H_


class KGridView;

class RFC_API KGridViewListener
{
public:
	KGridViewListener();

	virtual ~KGridViewListener();

	virtual void OnGridViewItemSelect(KGridView *gridView);

	virtual void OnGridViewItemRightClick(KGridView *gridView);

	virtual void OnGridViewItemDoubleClick(KGridView *gridView);
};

#endif

// =========== KListBoxListener.h ===========

/*
	RFC - KListBoxListener.h
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

#ifndef _RFC_KLISTBOXLISTENER_H_
#define _RFC_KLISTBOXLISTENER_H_


class KListBox;

class RFC_API KListBoxListener
{
public:
	KListBoxListener();

	virtual ~KListBoxListener();

	virtual void OnListBoxItemSelect(KListBox *listBox);
};

#endif

// =========== KPointerList.h ===========

/*
	RFC - KPointerList.h
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


#ifndef _RFC_KPOINTERLIST_H_
#define _RFC_KPOINTERLIST_H_

#include <malloc.h>
#include <windows.h>

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma warning(disable:4311)
	#pragma warning(disable:4312)
#endif


/**
	Holds a resizable list of pointers.
	To make all the methods thread-safe, pass true for isThreadSafe parameter of constructor.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	e.g. @code
	KButton btn1;
	KPointerList<KButton*> btnList(1024,true);
	btnList.AddPointer(&btn1);
	btnList.AddPointer(&btn1);
	@endcode
*/
template<class T>
class KPointerList
{
protected:
	int size;
	int roomCount;
	int roomIncrement;
	void* list;

	CRITICAL_SECTION criticalSection;
	bool isThreadSafe;

public:
	/**
		Constructs PointerList object.
		@param roomIncrement initial and reallocation size of internal memory block in DWORDS
		@param isThreadSafe make all the methods thread-safe
	*/
	KPointerList(int roomIncrement = 1024, bool isThreadSafe = false) // 1024*4=4096 = default alignment!
	{
		roomCount = roomIncrement;
		this->roomIncrement = roomIncrement;
		this->isThreadSafe = isThreadSafe;
		size = 0;
		list = ::malloc(roomCount * PTR_SIZE);
		
		if(isThreadSafe)
		{
			::InitializeCriticalSection(&criticalSection);
		}
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool AddPointer(T pointer)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection); // thread safe!
		}

		if(roomCount >= (size + 1) ) // no need reallocation. coz room count is enough!
		{
			*(NATIVE_INT*)((NATIVE_INT)list + (size * PTR_SIZE)) = (NATIVE_INT)pointer;

			size++;
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return true;
		}else // require reallocation!
		{
			roomCount += roomIncrement;
			void* retVal = ::realloc(list, roomCount * PTR_SIZE); 
			if(retVal)
			{
				list = retVal;

				*(NATIVE_INT*)((NATIVE_INT)list + (size * PTR_SIZE)) = (NATIVE_INT)pointer;

				size++;
				if(isThreadSafe)
				{
					::LeaveCriticalSection(&criticalSection);
				}
				return true;
			}else // memory allocation failed!
			{
				if(isThreadSafe)
				{
					::LeaveCriticalSection(&criticalSection);
				}
				return false;
			}
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T GetPointer(int id)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		if( (0 <= id) & (id < size) ) // checks for valid range!
		{	
			T object = (T)(*(NATIVE_INT*)((NATIVE_INT)list + (id * PTR_SIZE)));
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return object;
		}else // out of range!
		{
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return 0;
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T operator[](const int id)
	{
		return GetPointer(id);
	}

	/**
		Replace pointer of given id with new pointer
		@returns false if id is out of range!
	*/
	bool SetPointer(int id, T pointer)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		if( (0 <= id) & (id < size) )
		{			
			*(NATIVE_INT*)((NATIVE_INT)list + (id * PTR_SIZE)) = (NATIVE_INT)pointer;
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return true;
		}else // out of range!
		{
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return false;
		}
	}

	/**
		Remove pointer of given id
		@returns false if id is out of range!
	*/
	bool RemovePointer(int id)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		if( (0 <= id) & (id < size) )
		{	
			int newRoomCount = (((size - 1) / roomIncrement) + 1) * roomIncrement;
			void* newList = ::malloc(newRoomCount * PTR_SIZE);

			for(register int i = 0, j = 0; i < size; i++)
			{
				if(i != id)
				{
					*(NATIVE_INT*)((NATIVE_INT)newList + (j*PTR_SIZE)) = *(NATIVE_INT*)((NATIVE_INT)list + (i * PTR_SIZE));
					j++;
				}	
			}
			::free(list); // free old list!
			list = newList;
			roomCount = newRoomCount;
			size--;
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return true;

		}else // out of range!
		{
			if(isThreadSafe)
			{
				::LeaveCriticalSection(&criticalSection);
			}
			return false;
		}

	}

	/**
		Clears the list!
	*/
	void RemoveAll(bool reallocate = true)// remove all pointers from list!
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		::free(list);
		roomCount = roomIncrement;
		list = reallocate ? ::malloc(roomCount * PTR_SIZE) : 0;
		size = 0;

		if(isThreadSafe)
		{
			::LeaveCriticalSection(&criticalSection);
		}
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list.
	*/
	void DeleteAll(bool reallocate = true)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		for(int i = 0; i < size; i++)
		{
			T object = (T)(*(NATIVE_INT*)((NATIVE_INT)list + (i * PTR_SIZE)));
			delete object;
		}

		::free(list);

		roomCount = roomIncrement;
		list = reallocate ? ::malloc(roomCount * PTR_SIZE) : 0;
		size = 0;

		if(isThreadSafe)
		{
			::LeaveCriticalSection(&criticalSection);
		}
	}

	/**
		Finds the id of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int GetID(T pointer)
	{
		if(isThreadSafe)
		{
			::EnterCriticalSection(&criticalSection);
		}

		for(register int i = 0; i < size; i++)
		{
			if (*(NATIVE_INT*)((NATIVE_INT)list + (i*PTR_SIZE)) == (NATIVE_INT)pointer)
			{
				if(isThreadSafe)
				{
					::LeaveCriticalSection(&criticalSection);
				}
				return i;
			}
		}

		if(isThreadSafe)
		{
			::LeaveCriticalSection(&criticalSection);
		}
		return -1;
	}

	/**
		@returns item count in the list
	*/
	int GetSize()
	{
		return size;
	}

	/** Destructs PointerList object.*/
	~KPointerList()
	{
		if (list)
			::free(list);

		if(isThreadSafe)
		{
			::DeleteCriticalSection(&criticalSection);
		}
	}

};

#endif

// =========== KTrackBarListener.h ===========

/*
	RFC - KTrackBarListener.h
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

#ifndef _RFC_KTRACKBARLISTENER_H_
#define _RFC_KTRACKBARLISTENER_H_


class KTrackBar;

class RFC_API KTrackBarListener
{
public:
	KTrackBarListener();

	virtual ~KTrackBarListener();

	virtual void OnTrackBarChange(KTrackBar *trackBar);
};

#endif

// =========== KButtonListener.h ===========

/*
	RFC - KButtonListener.h
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

#ifndef _RFC_KBUTTONLISTENER_H_
#define _RFC_KBUTTONLISTENER_H_


class KButton;

class RFC_API KButtonListener
{
public:
	KButtonListener();

	virtual ~KButtonListener();

	virtual void OnButtonPress(KButton *button);
};

#endif

// =========== KStringHolder.h ===========

/*
	RFC - KString.h
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

#ifndef _RFC_KSTRINGHOLDER_H_
#define _RFC_KSTRINGHOLDER_H_

#include <windows.h>


/**
	This class holds reference counted string.
*/
class RFC_API KStringHolder
{
	volatile LONG refCount;
	char *a_text; // ansi version
	CRITICAL_SECTION cs_a_text; // to guard ansi string creation
	bool isStaticText; // do not free w_text if true

public:
	wchar_t *w_text; // unicode version
	int count; // character count

	KStringHolder(bool isStaticText = false);

	~KStringHolder();

	/**
		Make sure to call this method if you contruct new KStringHolder or keep reference to another KStringHolder object.
	*/
	void AddReference();

	/**
		Make sure to call this method if you clear reference to KStringHolder object. it will release allocated memory for string.
	*/
	void ReleaseReference();

	/**
		ANSI version available only when needed.
	*/
	const char* GetAnsiVersion(UINT codePage = CP_UTF8);
};

#endif

// =========== KComboBoxListener.h ===========

/*
	RFC - KComboBoxListener.h
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

#ifndef _RFC_KCOMBOBOXLISTENER_H_
#define _RFC_KCOMBOBOXLISTENER_H_


class KComboBox;

class RFC_API KComboBoxListener
{
public:
	KComboBoxListener();

	virtual ~KComboBoxListener();

	virtual void OnComboBoxItemSelect(KComboBox *comboBox);
};

#endif

// =========== KPerformanceCounter.h ===========

/*
	RFC - KPerformanceCounter.h
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

#ifndef _RFC_KPERFORMANCECOUNTER_H_
#define _RFC_KPERFORMANCECOUNTER_H_

#include <windows.h>

class RFC_API KPerformanceCounter
{
protected:
	double pcFreq;
	__int64 counterStart;

public:
	KPerformanceCounter();

	virtual void StartCounter();

	/**
		returns delta time(milliseconds) between StartCounter and EndCounter calls.
	*/
	virtual double EndCounter();

	virtual ~KPerformanceCounter();

};

#endif

// =========== KThread.h ===========

/*
	RFC - KThread.h
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

#ifndef _RFC_KTHREAD_H_
#define _RFC_KTHREAD_H_

#include <windows.h>

/**
	Encapsulates a thread.

	Subclasses derive from KThread and implement the Run() method, in which they
	do their business. The thread can then be started with the StartThread() method
	and controlled with various other methods.

	Run() method implementation might be like this
	@code
	virtual void Run()
	{
		while(!threadShouldStop)
		{
			// your code goes here...
		}
		isThreadRunning=false;	
	}
	@endcode

*/
class RFC_API KThread
{
protected:
	HANDLE handle;
	volatile bool isThreadRunning;
	volatile bool threadShouldStop;

public:
	KThread();

	/**
		Sets thread handle.
	*/
	virtual void SetHandle(HANDLE handle);

	/**
		Returns handle of the thread
	*/
	virtual HANDLE GetHandle();

	/**
		Override this method in your class.
	*/
	virtual void Run();

	/**
		Starts thread
	*/
	virtual bool StartThread();

	/**
		Another thread can signal this thread should stop. 
	*/
	virtual void ThreadShouldStop();

	/**
		@returns true if thread is still running
	*/
	virtual bool IsThreadRunning();

	/**
		Calling thread is not return until this thread finish.
	*/
	virtual void WaitUntilThreadFinish();

	/**
		Sleeps calling thread to given micro seconds.
	*/
	static void uSleep(int waitTime);

	virtual ~KThread();
};

#endif

// =========== KTimerListener.h ===========

/*
	RFC - KTimerListener.h
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

#ifndef _RFC_KTIMER_LISTENER_H_
#define _RFC_KTIMER_LISTENER_H_


class KTimer;

class RFC_API KTimerListener
{
public:
	KTimerListener();

	virtual ~KTimerListener();

	virtual void OnTimer(KTimer *timer);
};

#endif

// =========== KString.h ===========

/*
	RFC - KString.h
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

#ifndef _RFC_KSTRING_H_
#define _RFC_KSTRING_H_

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif


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
		Returns unicode version of this string
	*/
	operator const wchar_t*()const;

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
};

RFC_API const KString operator+ (const char* const string1, const KString& string2);

RFC_API const KString operator+ (const wchar_t* const string1, const KString& string2);

RFC_API const KString operator+ (const KString& string1, const KString& string2);

#define STATIC_TXT(X) KString(L##X, KString::STATIC_TEXT_DO_NOT_FREE)
#define BUFFER_TXT(X) KString(X, KString::FREE_TEXT_WHEN_DONE)

#endif

// =========== KRegistry.h ===========

/*
    RFC - KRegistry.h
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

#ifndef _RFC_KREGISTRY_H_
#define _RFC_KREGISTRY_H_

#include<windows.h>

class RFC_API KRegistry
{

public:
	KRegistry();

	// returns true on success or if the key already exists.
	static bool CreateKey(HKEY hKeyRoot, const KString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool DeleteKey(HKEY hKeyRoot, const KString& subKey);

	static bool ReadString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString *result);

	static bool WriteString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value);

	static bool ReadDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD *result);

	static bool WriteDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void *buffer;
		DWORD bufferSize;
		if(KRegistry::ReadBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool ReadBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void **buffer, DWORD *buffSize);

	static bool WriteBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void *buffer, DWORD buffSize);

	virtual ~KRegistry();

};

#endif

// =========== KApplication.h ===========

/*
	RFC - KApplication.h
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

#ifndef _RFC_KAPPLICATION_H_
#define _RFC_KAPPLICATION_H_


/**
	Derive your application object from this class!
	Override Main method and put your application code there.
	Use START_RFC_APPLICATION macro somewhere in a cpp file to declare an instance of this class.

	e.g. @code
	class MyApp : public KApplication
	{
		public:
			MyApp(){}
			~MyApp(){}

			virtual int Main(KString **argv,int argc)
			{
				// your app code goes here...
				return 0;
			}
	};

	START_RFC_APPLICATION(MyApp)
	@endcode
*/
class RFC_API KApplication
{
public:

	/** 
		Constructs an KApplication object.
	*/
	KApplication();

	/** 
		Called when the application starts.
		Put your application code here and if you create a window, 
		then make sure to call DoMessagePump method before you return.

		@param argv array of command-line arguments! access them like this KString* arg1=argv[0];
		@param argc number of arguments
	*/
	virtual int Main(KString **argv, int argc);

	/** 
		Destructs an Application object.
	*/
	virtual ~KApplication();
};

#endif

// =========== KLogger.h ===========

/*
	RFC - KLogger.h
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

#ifndef _RFC_KLOGGER_H_
#define _RFC_KLOGGER_H_

#include <windows.h>

/**
	Super fast logging class for logging within a (audio)loop. (not thread safe)
	Writes logging data into ram & dumps data into file when needed.
	You can use this class instead of OutputDebugString API.(OutputDebugString is too slow & ETW is too complex?)
	Use Log Viewer tool to view generated log file.

	Log File Format:
		file header:				'R' 'L' 'O' 'G'
		event count:				int32					; (event count)
		event start packet format:	byte|short16|short16	; (event type|secs|mills)
		event param number format:	byte|data				; (param type|data)
		event param string format:	byte|byte|data			; (param type|data size[max 255]|data)
		event end packet format:	byte					; (EVT_END)
*/
class KLogger
{
protected:
	DWORD bufferSize;
	DWORD bufferIndex;
	unsigned int totalMills;
	unsigned int totalEvents;
	char *buffer;
	bool bufferFull, isFirstCall;
	KPerformanceCounter pCounter;

public:

	enum ByteSizes
	{
		SZ_MEGABYTE = 1024 * 1024,
		SZ_KILOBYTE = 1024,
	};

	enum EventTypes
	{
		EVT_END = 0,
		EVT_INFORMATION = 1,
		EVT_WARNING = 2,
		EVT_ERROR = 3,
	};

	enum ParamTypes
	{
		// skipped value zero. because parser will fail to recognize EVT_END.
		PARAM_STRING = 1,
		PARAM_INT32 = 2,
		PARAM_SHORT16 = 3,
		PARAM_FLOAT = 4,
		PARAM_DOUBLE = 5,
	};

	KLogger(DWORD bufferSize = (SZ_MEGABYTE * 10));

	virtual bool WriteNewEvent(unsigned char eventType = EVT_INFORMATION);

	virtual bool EndEvent();

	/**
		textLength is number of chars. max value is 255.
	*/
	virtual bool AddTextParam(const char *text, unsigned char textLength);

	virtual bool AddIntParam(int value);

	virtual bool AddShortParam(unsigned short value);

	virtual bool AddFloatParam(float value);
	
	virtual bool AddDoubleParam(double value);

	virtual bool IsBufferFull();

	virtual bool WriteToFile(const KString &filePath);

	virtual ~KLogger();
};

#endif

// =========== KMenuItem.h ===========

/*
	RFC - KMenuItem.h
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

#ifndef _RFC_KMENUITEM_H_
#define _RFC_KMENUITEM_H_

#include <windows.h>

class KMenuItemListener;

class RFC_API KMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	KMenuItemListener *listener;
	KString itemText;
	bool enabled;
	bool checked;

public:
	KMenuItem();

	virtual void AddToMenu(HMENU hMenu);

	virtual bool IsChecked();

	virtual void SetCheckedState(bool state);

	virtual bool IsEnabled();

	virtual void SetEnabled(bool state);

	virtual void SetText(const KString& text);

	virtual KString GetText();

	virtual UINT GetItemID();

	virtual HMENU GetMenuHandle();

	virtual void SetListener(KMenuItemListener *listener);

	virtual KMenuItemListener* GetListener();

	virtual void OnPress();

	virtual ~KMenuItem();
};

#endif

// =========== KCursor.h ===========

/*
	RFC - KCursor.h
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

#ifndef _RFC_KCURSOR_H_
#define _RFC_KCURSOR_H_

#include <windows.h>

/**
	Can be use to load cursor from file or resource.
*/
class RFC_API KCursor
{
protected:
	HCURSOR hCursor;
	HINSTANCE appHInstance;

public:
	KCursor();

	/**
		Loads cursor from resource
		@param resourceID resource ID of cursor file
		@returns false if cursor load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads cursor from file
		@param filePath path to cursor file
		@returns false if cursor load fails
	*/
	bool LoadFromFile(const KString& filePath);

	/**
		Returns cursor handle
	*/
	HCURSOR GetHandle();

	virtual ~KCursor();
};

#endif

// =========== KBitmap.h ===========

/*
	RFC - KBitmap.h
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

#ifndef _RFC_KBITMAP_H_
#define _RFC_KBITMAP_H_

#include <windows.h>

/**
	Can be use to load bitmap image from file or resource.
*/
class RFC_API KBitmap
{
protected:
	HBITMAP hBitmap;
	HINSTANCE appHInstance;

public:
	KBitmap();

	/**
		Loads bitmap image from resource
		@param resourceID resource ID of image
		@returns false if image load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads bitmap image from file
		@param filePath path to image
		@returns false if image load fails
	*/
	bool LoadFromFile(const KString& filePath);

	void DrawOnHDC(HDC hdc, int x, int y, int width, int height);

	/**
		Returns bitmap handle
	*/
	HBITMAP GetHandle();

	virtual ~KBitmap();
};

#endif


// =========== KFont.h ===========

/*
	RFC - KFont.h
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

#ifndef _RFC_KFONT_H_
#define _RFC_KFONT_H_

#include <windows.h>

/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class RFC_API KFont
{
private:
	static KFont* defaultInstance;

protected:
	HFONT hFont;
	bool customFont;

public:
	/**
		Constructs default system Font object.
	*/
	KFont();

	/**
		Constructs custom font object.
	*/
	KFont(const KString& face, int size = 14, bool bold = false, bool italic = false, bool antiAliased=false);

	/**
		If you want to use system default font, then use this static method. Do not delete returned object!
	*/
	static KFont* GetDefaultFont();

	/**
		Loads font from a file. make sure to call RemoveFont when done.
	*/
	static bool LoadFont(const KString& path);

	static void RemoveFont(const KString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT GetFontHandle();

	virtual ~KFont();
};

#endif

// =========== KIcon.h ===========

/*
	RFC -KIcon.h
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

#ifndef _RFC_KICON_H_
#define _RFC_KICON_H_

#include <windows.h>

/**
	Can be use to load icon from file or resource.
*/
class RFC_API KIcon
{
protected:
	HICON hIcon;
	HINSTANCE appHInstance;

public:
	KIcon();

	/**
		Loads icon from resource
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool LoadFromResource(WORD resourceID);

	/**
		Loads icon from file
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool LoadFromFile(const KString& filePath);

	/**
		Returns icon handle
	*/
	HICON GetHandle();

	virtual ~KIcon();
};

#endif

// =========== KMD5.h ===========

/*
	RFC - KMD5.h
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

#ifndef _RFC_KMD5_H_
#define _RFC_KMD5_H_


class RFC_API KMD5
{
public:
	KMD5();

	static KString GenerateFromString(const KString& text);

	static KString GenerateFromFile(const KString& fileName);

	virtual ~KMD5();
};

#endif

// =========== KDirectory.h ===========

/*
	RFC - KDirectory.h
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

#ifndef _RFC_KDIRECTORY_H_
#define _RFC_KDIRECTORY_H_

#include <windows.h>
#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class RFC_API KDirectory
{
public:
	KDirectory();

	static bool IsDirExists(const KString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool CreateDir(const KString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool RemoveDir(const KString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static KString GetModuleDir(HMODULE hModule);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static KString GetTempDir();

	/**
		returns the the Application Data directory. if isAllUsers is true this function will return dir shared across all users.
		returns empty string on error.
	*/
	static KString GetApplicationDataDir(bool isAllUsers = false);

	virtual ~KDirectory();
};

#endif

// =========== KSHA1.h ===========

/*
	RFC - KSHA1.h
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

#ifndef _RFC_KSHA1_H_
#define _RFC_KSHA1_H_


class RFC_API KSHA1
{
public:
	KSHA1();

	static KString GenerateFromString(const KString& text);

	static KString GenerateFromFile(const KString& fileName);

	virtual ~KSHA1();
};

#endif

// =========== KFile.h ===========

/*
	RFC - KFile.h
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

#ifndef _RFC_KFILE_H_
#define _RFC_KFILE_H_

#include <windows.h>

/**
	Can be use to read/write data from a file easily.
*/
class RFC_API KFile
{
protected:
	KString fileName;
	HANDLE fileHandle;
	bool autoCloseHandle;
	DWORD desiredAccess;

public:
	KFile();

	/** 
		Used in file opening, to specify whether to open as read or write or both.
	*/
	enum FileAccessTypes
	{
		KREAD = GENERIC_READ,
		KWRITE = GENERIC_WRITE,
		KBOTH = GENERIC_READ | GENERIC_WRITE,
	};

	/** 
		If the file does not exist, it will be created.
	*/
	KFile(const KString& fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	/** 
		If the file does not exist, it will be created.
	*/
	virtual bool OpenFile(const KString& fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	virtual bool CloseFile();

	virtual HANDLE GetFileHandle();

	/** 
		fills given buffer and returns number of bytes read.
	*/
	virtual DWORD ReadFile(void* buffer, DWORD numberOfBytesToRead);

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use GetFileSize method. return value will be null on read error.
	*/
	virtual void* ReadAsData();

	virtual KString ReadAsString(bool isUnicode = true);

	/**
		returns number of bytes written.
	*/
	virtual DWORD WriteFile(void* buffer, DWORD numberOfBytesToWrite);

	virtual bool WriteString(const KString& text, bool isUnicode = true);

	virtual bool SetFilePointerToStart();

	/**
		moves file pointer to given distance from start.
	*/
	virtual bool SetFilePointerTo(DWORD distance);

	virtual bool SetFilePointerToEnd();

	/**
		returns zero on error
	*/
	virtual DWORD GetFileSize();

	static bool DeleteFile(const KString& fileName);

	static bool IsFileExists(const KString& fileName);

	static bool CopyFile(const KString& sourceFileName, const KString& destFileName);

	virtual ~KFile();
};

#endif

// =========== KComponent.h ===========

/*
	RFC - KComponent.h
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

#ifndef _RFC_KCOMPONENT_H_
#define _RFC_KCOMPONENT_H_


#include <windows.h>

/**
	Base class of all W32 gui objects.
*/
class RFC_API KComponent
{
protected:
	KString compClassName;
	KString compText;
	HWND compHWND;
	HWND compParentHWND;
	DWORD compDwStyle;
	DWORD compDwExStyle;
	UINT compCtlID;
	int compX;
	int compY;
	int compWidth;
	int compHeight;
	bool compVisible;
	bool compEnabled;
	bool isRegistered;
	KFont *compFont;
	KCursor *cursor;

public:
	WNDCLASSEXW wc;

	KComponent();

	/**
		Called after hotplugged into a given HWND.
	*/
	virtual void OnHotPlug();

	/**
		HotPlugs given HWND. this method does not update current compFont and cursor variables.
		Set fetchInfo to true if you want to acquire all the information about this HWND. (width, height, position etc...)
		Set fetchInfo to false if you only need to receive events. (button click etc...)
	*/
	virtual void HotPlugInto(HWND component, bool fetchInfo = true, bool subClassWindowProc = false);

	/**
		Sets mouse cursor of this component.
	*/
	virtual void SetMouseCursor(KCursor *cursor);

	/**
		@returns autogenerated unique class name for this component
	*/
	virtual KString GetComponentClassName();

	/**
		Registers the class name and creates the component. 
		top level windows & owner-drawn controls must set subClassWindowProc to true.
		Otherwise WindowProc will be disabled & you will not receive WM_MEASUREITEM like messages into the EventProc.
		@returns false if registration failed or component creation failed.
	*/
	virtual bool CreateComponent(bool subClassWindowProc = true);

	/**
		Handles internal window messages. (subclassed window proc)
		Important: Pass unprocessed messages to parent if you override this method.
	*/
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
		Receives messages like WM_COMMAND, WM_NOTIFY, WM_DRAWITEM from the parent window. (if it belongs to this component)
		Pass unprocessed messages to parent if you override this method.
		@returns true if msg processed.
	*/
	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	/**
		Identifier of the child component which can be used with WM_MEASUREITEM like messages.
		@returns zero for top level windows
	*/
	virtual UINT GetControlID();

	/**
		Sets font of this component
	*/
	virtual void SetFont(KFont *compFont);

	/**
		Returns font of this component
	*/
	virtual KFont* GetFont();

	/**
		Returns caption of this component
	*/
	virtual KString GetText();

	/**
		Sets caption of this component
	*/
	virtual void SetText(const KString& compText);

	virtual void SetHWND(HWND compHWND);

	/**
		Returns HWND of this component
	*/
	virtual HWND GetHWND();

	/**
		Changes parent of this component
	*/
	virtual void SetParentHWND(HWND compParentHWND);

	/**
		Returns parent of this component
	*/
	virtual HWND GetParentHWND();

	/**
		Returns style of this component
	*/
	virtual DWORD GetStyle();

	/**
		Sets style of this component
	*/
	virtual void SetStyle(DWORD compStyle);

	/**
		Returns exstyle of this component
	*/
	virtual DWORD GetExStyle();

	/**
		Sets exstyle of this component
	*/
	virtual void SetExStyle(DWORD compExStyle);

	/**
		Returns x position of this component which is relative to parent component.
	*/
	virtual int GetX();

	/**
		Returns y position of this component which is relative to parent component.
	*/
	virtual int GetY();

	/**
		Returns width of the component.
	*/
	virtual int GetWidth();

	/**
		Returns height of the component.
	*/
	virtual int GetHeight();

	/**
		Sets width and height of the component.
	*/
	virtual void SetSize(int compWidth, int compHeight);

	/**
		Sets x and y position of the component. x and y are relative to parent component
	*/
	virtual void SetPosition(int compX, int compY);

	/**
		Sets visible state of the component
	*/
	virtual void SetVisible(bool state);

	/**
		Returns visible state of the component
	*/
	virtual bool IsVisible();

	/**
		Returns the component is ready for user input or not
	*/
	virtual bool IsEnabled();

	/**
		Sets component's user input reading state
	*/
	virtual void SetEnabled(bool state);

	/**
		Brings component to front
	*/
	virtual void BringToFront();

	/**
		Grabs keyboard focus into this component
	*/
	virtual void SetKeyboardFocus();

	/**
		Repaints the component
	*/
	virtual void Repaint();

	virtual ~KComponent();

};

#endif

// =========== KProgressBar.h ===========

/*
	RFC - KProgressBar.h
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

#ifndef _RFC_KPROGRESSAR_H_
#define _RFC_KPROGRESSAR_H_


class RFC_API KProgressBar : public KComponent
{
protected:
	int value;

public:
	KProgressBar(bool smooth=true, bool vertical=false);

	virtual int GetValue();

	virtual void SetValue(int value);

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual ~KProgressBar();
};


#endif

// =========== KGridView.h ===========

/*
	RFC - KGridView.h
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

#ifndef _RFC_KGRIDVIEW_H_
#define _RFC_KGRIDVIEW_H_


class KGridViewListener;

class RFC_API KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;
	KGridViewListener *listener;

public:
	KGridView(bool sortItems = false);

	virtual void SetListener(KGridViewListener *listener);

	virtual KGridViewListener* GetListener();

	virtual void InsertRecord(KString **columnsData);

	virtual void InsertRecordTo(int rowIndex, KString **columnsData);

	virtual KString GetRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int GetSelectedRow();

	virtual void RemoveRecordAt(int rowIndex);

	virtual void RemoveAll();

	virtual void UpdateRecordAt(int rowIndex, int columnIndex, const KString& text);

	virtual void SetColumnWidth(int columnIndex, int columnWidth);

	virtual int GetColumnWidth(int columnIndex);

	virtual void CreateColumn(const KString& text, int columnWidth = 100);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual void OnItemSelect();

	virtual void OnItemRightClick();

	virtual void OnItemDoubleClick();

	virtual ~KGridView();
};

#endif

// =========== KLabel.h ===========

/*
	RFC - KLabel.h
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

#ifndef _RFC_KLABEL_H_
#define _RFC_KLABEL_H_


class RFC_API KLabel : public KComponent
{
public:
	KLabel();

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual ~KLabel();
};

#endif

// =========== KTextBox.h ===========

/*
	RFC - KTextBox.h
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

#ifndef _RFC_KTEXTBOX_H_
#define _RFC_KTEXTBOX_H_


class RFC_API KTextBox : public KComponent
{
public:
	KTextBox(bool readOnly = false);

	virtual KString GetText();

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual ~KTextBox();
};

#endif

// =========== KTrackBar.h ===========

/*
	RFC - KTrackBar.h
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

#ifndef _RFC_KTRACKBAR_H_
#define _RFC_KTRACKBAR_H_


class KTrackBarListener;

class RFC_API KTrackBar : public KComponent
{
protected:
	int rangeMin,rangeMax,value;
	KTrackBarListener *listener;

public:
	KTrackBar(bool showTicks = false, bool vertical = false);

	/**
		Range between 0 to 100
	*/
	virtual void SetRange(int min, int max);

	virtual void SetValue(int value);

	virtual int GetValue();

	virtual void SetListener(KTrackBarListener *listener);

	virtual void OnChange();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual ~KTrackBar();
};


#endif


// =========== KSettingsWriter.h ===========

/*
	RFC - KSettingsWriter.h
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

#ifndef _RFC_KSETTINGSWRITER_H_
#define _RFC_KSETTINGSWRITER_H_

#include <windows.h>

/**
	High performance configuration writing class.
*/
class RFC_API KSettingsWriter
{
protected:
	KFile settingsFile;

public:
	KSettingsWriter();

	virtual bool OpenFile(const KString& fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	virtual void WriteData(DWORD size, void *buffer);

	virtual void WriteString(const KString& text);

	virtual void WriteInt(int value);

	virtual void WriteFloat(float value);

	virtual void WriteDouble(double value);

	virtual void WriteBool(bool value);

	virtual ~KSettingsWriter();
};

#endif

// =========== KSettingsReader.h ===========

/*
	RFC - KSettingsReader.h
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

#ifndef _RFC_KSETTINGSREADER_H_
#define _RFC_KSETTINGSREADER_H_

#include <windows.h>

/**
	High performance configuration reading class.
*/
class RFC_API KSettingsReader
{
protected:
	KFile settingsFile;

public:
	KSettingsReader();

	virtual bool OpenFile(const KString& fileName, int formatID);

	/**
		read struct, array or whatever...
	*/
	virtual void ReadData(DWORD size, void *buffer);

	virtual KString ReadString();

	virtual int ReadInt();

	virtual float ReadFloat();

	virtual double ReadDouble();

	virtual bool ReadBool();

	virtual ~KSettingsReader();
};

#endif

// =========== KButton.h ===========

/*
	RFC - KButton.h
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

#ifndef _RFC_KBUTTON_H_
#define _RFC_KBUTTON_H_


class KButtonListener;

class RFC_API KButton : public KComponent
{
protected:
	KButtonListener *listener;

public:
	KButton();

	virtual void SetListener(KButtonListener *listener);

	virtual KButtonListener* GetListener();

	virtual void OnPress();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual ~KButton();
};

#endif

// =========== KCheckBox.h ===========

/*
	RFC - KCheckBox.h
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

#ifndef _RFC_KCHECKBOX_H_
#define _RFC_KCHECKBOX_H_


class RFC_API KCheckBox : public KButton
{
protected:
	bool checked;

public:
	KCheckBox();

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual void OnPress();

	virtual bool IsChecked();

	virtual void SetCheckedState(bool state);

	virtual ~KCheckBox();
};

#endif

// =========== KWindow.h ===========

/*
	RFC - KWindow.h
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

#ifndef _RFC_KWINDOW_H_
#define _RFC_KWINDOW_H_


class RFC_API KWindow : public KComponent
{
public:
	KWindow();

	virtual void Flash();

	virtual void SetIcon(KIcon *icon);

	virtual void Destroy();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual void CenterScreen();

	/**
		set subClassWindowProc value to true if component is owner-drawn.
		Otherwise WindowProc will be disabled & you will not receive WM_MEASUREITEM like messages into the EventProc.
	*/
	virtual bool AddComponent(KComponent *component, bool subClassWindowProc = false);

	virtual bool SetClientAreaSize(int width, int height);

	virtual bool GetClientAreaSize(int *width, int *height);

	virtual void OnMoved();

	virtual void OnResized();

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual ~KWindow();
};

#endif

// =========== KTimer.h ===========

/*
	RFC - KTimer.h
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

#ifndef _RFC_KTIMER_H_
#define _RFC_KTIMER_H_

#include <windows.h>

/**
	Encapsulates a timer.

	The timer can be started with the StartTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling SetTimerWindow method.
*/
class RFC_API KTimer
{
protected:
	UINT timerID;
	int resolution;
	bool started;
	KWindow *window;
	KTimerListener *listener;

public:

	KTimer();

	/**
		@param resolution timer interval
	*/
	virtual void SetInterval(int resolution);

	virtual int GetInterval();

	/**
		Call this method before you start timer
	*/
	virtual void SetTimerWindow(KWindow *window);

	virtual void SetTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	virtual UINT GetTimerID();

	/**
		Starts timer
	*/
	virtual void StartTimer();

	/**
		Stops the timer. You can restart it by calling StartTimer() method.
	*/
	virtual void StopTimer();

	virtual void SetListener(KTimerListener *listener);

	virtual bool IsTimerRunning();

	virtual void OnTimer();

	virtual ~KTimer();
};

#endif

// =========== KListBox.h ===========

/*
	RFC - KListBox.h
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

#ifndef _RFC_KLISTBOX_H_
#define _RFC_KLISTBOX_H_


class KListBoxListener;

class RFC_API KListBox : public KComponent
{
protected:
	KPointerList<KString*> *stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

	KListBoxListener *listener;

public:
	KListBox(bool multipleSelection=false, bool sort=false, bool vscroll=true);

	virtual void SetListener(KListBoxListener *listener);

	virtual void AddItem(const KString& text);

	virtual void RemoveItem(int index);

	virtual void RemoveItem(const KString& text);

	virtual int GetItemIndex(const KString& text);

	virtual int GetItemCount();

	virtual int GetSelectedItemIndex();

	virtual KString GetSelectedItem();

	virtual int GetSelectedItems(int* itemArray, int itemCountInArray);

	virtual void ClearList();

	virtual void SelectItem(int index);

	virtual void SelectItems(int start, int end);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual void OnItemSelect();

	virtual ~KListBox();
};

#endif

// =========== KComboBox.h ===========

/*
	RFC - KComboBox.h
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

#ifndef _RFC_KCOMBOBOX_H_
#define _RFC_KCOMBOBOX_H_



class KComboBoxListener;

class RFC_API KComboBox : public KComponent
{
protected:
	KPointerList<KString*> *stringList;
	int selectedItemIndex;
	KComboBoxListener *listener;

public:
	KComboBox(bool sort=false);

	virtual void AddItem(const KString& text);

	virtual void RemoveItem(int index);

	virtual void RemoveItem(const KString& text);

	virtual int GetItemIndex(const KString& text);

	virtual int GetItemCount();

	virtual int GetSelectedItemIndex();

	virtual KString GetSelectedItem();

	virtual void ClearList();

	virtual void SelectItem(int index);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool subClassWindowProc = false);

	virtual void SetListener(KComboBoxListener *listener);

	virtual void OnItemSelect();

	virtual ~KComboBox();
};

#endif

// =========== KCommonDialogBox.h ===========

/*
	RFC - KCommonDialogBox.h
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

#ifndef _RFC_KCOMMONDIALOGBOX_H_
#define _RFC_KCOMMONDIALOGBOX_H_


class RFC_API KCommonDialogBox
{
public:
	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
	*/
	static bool ShowOpenFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName);

	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
	*/
	static bool ShowSaveFileDialog(KWindow *window, const KString& title, const wchar_t* filter, KString *fileName);
};

#endif

// =========== KMenu.h ===========

/*
	RFC - KMenu.h
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

#ifndef _RFC_KMENU_H_
#define _RFC_KMENU_H_


class RFC_API KMenu
{
protected:
	HMENU hMenu;

public:
	KMenu();

	virtual void AddMenuItem(KMenuItem *menuItem);

	virtual void AddSubMenu(const KString& text, KMenu *menu);

	virtual void AddSeperator();

	virtual HMENU GetMenuHandle();

	virtual void PopUpMenu(KWindow *window);

	virtual ~KMenu();
};

#endif

// =========== KMenuBar.h ===========

/*
	RFC - KMenuBar.h
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

#ifndef _RFC_KMENUBAR_H_
#define _RFC_KMENUBAR_H_


class RFC_API KMenuBar
{
protected:
	HMENU hMenu;

public:
	KMenuBar();

	virtual void AddMenu(const KString& text, KMenu *menu);

	virtual void AddToWindow(KWindow *window);

	virtual ~KMenuBar();
};


#endif

// =========== KPlatformUtil.h ===========

/*
	RFC - KPlatformUtil.h
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

#ifndef _RFC_KPLATFORMUTIL_H_
#define _RFC_KPLATFORMUTIL_H_

#include <windows.h>
#include <stdio.h>

/**
	Singleton class which can be use to get HINSTANCE of your application.
	e.g. @code
	HINSTANCE hInstance=PlatformUtil::GetInstance()->GetAppInstance();
	@endcode
*/
class RFC_API KPlatformUtil
{
private:

	static KPlatformUtil *_instance;

	KPlatformUtil();

protected:
	HINSTANCE hInstance;
	CRITICAL_SECTION g_csCount;
	int classCount;
	int timerCount;
	int controlCount;
	UINT menuItemCount;

	KPointerList<KMenuItem*> *menuItemList;
	KPointerList<KTimer*> *timerList;

public:

	static KPlatformUtil* GetInstance();

	void SetAppHInstance(HINSTANCE hInstance);

	HINSTANCE GetAppHInstance();

	UINT GenerateControlID();

	KString GenerateClassName();
	UINT GenerateMenuItemID(KMenuItem *menuItem);
	KMenuItem* GetMenuItemByID(UINT id);

	UINT GenerateTimerID(KTimer *timer);
	KTimer* GetTimerByID(UINT id);

	~KPlatformUtil();

};


#endif

// =========== KNumericField.h ===========

/*
	RFC - KNumericField.h
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

#ifndef _RFC_KNUMERICFIELD_H_
#define _RFC_KNUMERICFIELD_H_


class RFC_API KNumericField : public KTextBox
{
public:
	KNumericField();

	virtual ~KNumericField();
};

#endif

// =========== KPasswordBox.h ===========

/*
	RFC - KPasswordBox.h
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

#ifndef _RFC_KPASSWORDBOX_H_
#define _RFC_KPASSWORDBOX_H_


class RFC_API KPasswordBox : public KTextBox
{
protected:
	char pwdChar;

public:
	KPasswordBox(bool readOnly=false);

	virtual void SetPasswordChar(const char pwdChar);

	virtual char GetPasswordChar();

	virtual bool CreateComponent();

	virtual ~KPasswordBox();
};

#endif

// =========== KPushButton.h ===========

/*
	RFC - KPushButton.h
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

#ifndef _RFC_KPUSHBUTTON_H_
#define _RFC_KPUSHBUTTON_H_


class RFC_API KPushButton : public KCheckBox
{
public:
	KPushButton();

	virtual ~KPushButton();
};

#endif

// =========== KGroupBox.h ===========

/*
	RFC - KGroupBox.h
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

#ifndef _RFC_KGROUPBOX_H_
#define _RFC_KGROUPBOX_H_


class RFC_API KGroupBox : public KButton
{
public:
	KGroupBox();

	virtual ~KGroupBox();
};

#endif

// =========== KWindowTypes.h ===========

/*
	RFC - KWindowTypes.h
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

#ifndef _RFC_KWINDOWTYPES_H_
#define _RFC_KWINDOWTYPES_H_


class RFC_API KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual ~KHotPluggedDialog();
};

class RFC_API KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow();

	virtual ~KOverlappedWindow();
};

class RFC_API KFrame : public KWindow
{
public:
	KFrame();

	virtual ~KFrame();
};

class RFC_API KDialog : public KWindow
{
public:
	KDialog();

	virtual ~KDialog();
};

class RFC_API KToolWindow : public KWindow
{
public:
	KToolWindow();

	virtual ~KToolWindow();
};

#endif

// =========== KTextArea.h ===========

/*
	RFC - KTextArea.h
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

#ifndef _RFC_KTEXTAREA_H_
#define _RFC_KTEXTAREA_H_


class RFC_API KTextArea : public KTextBox
{
public:
	KTextArea(bool autoScroll = false, bool readOnly = false);

	virtual ~KTextArea();
};


#endif

// =========== KRadioButton.h ===========

/*
	RFC - KRadioButton.h
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

#ifndef _RFC_KRADIOBUTTON_H_
#define _RFC_KRADIOBUTTON_H_


class RFC_API KRadioButton : public KCheckBox
{
public:
	KRadioButton();

	virtual ~KRadioButton();
};

#endif

// =========== rfc.h ===========

/*
	RFC Framework v0.2.6
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

#ifndef _RFC_H_
#define _RFC_H_

#include <windows.h>
#include <commctrl.h>

#ifdef _MSC_VER
#pragma comment(lib, "Comctl32.lib")
#endif




RFC_API LRESULT CALLBACK GlobalWnd_Proc(HWND,UINT,WPARAM,LPARAM);
RFC_API INT_PTR CALLBACK GlobalDlg_Proc(HWND, UINT, WPARAM, LPARAM);
RFC_API DWORD WINAPI GlobalThread_Proc(LPVOID);

/**
	top level windows & owner-drawn controls must set subClassWindowProc to true.
*/
RFC_API HWND CreateRFCComponent(KComponent* component, bool subClassWindowProc);
RFC_API bool CreateRFCThread(KThread* thread);

RFC_API void DoMessagePump(bool handleTabKey=true);

RFC_API void InitRFC(HINSTANCE);
RFC_API void DeInitRFC();

RFC_API int HotPlugAndRunDialogBox(WORD resourceID,HWND parentHwnd,KComponent* component);
RFC_API HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);

#define START_RFC_APPLICATION(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitRFC(hInstance);\
	int argc = 0;\
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc);\
	KString **str_argv = (KString**)::malloc(argc * PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new KString(args[i], KString::STATIC_TEXT_DO_NOT_FREE);}\
	AppClass* application = new AppClass();\
	int retVal = application->Main(str_argv, argc);\
	delete application;\
	::DeInitRFC();\
	for(int i = 0; i < argc; i++){delete str_argv[i];}\
	::free((void*)str_argv);\
	::GlobalFree(args);\
	return retVal;\
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitRFC(hInstance);\
	AppClass* application = new AppClass();\
	int retVal = application->Main(0, 0);\
	delete application;\
	::DeInitRFC();\
	return retVal;\
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef RFC_DLL
		#ifndef RFC_NO_MANIFEST
			#ifdef RFC64
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#else
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#endif
		#endif
	#endif
#endif

#define BEGIN_KMSG_HANDLER \
	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
	{\
	switch(msg)\
	{

#define ON_KMSG(_KMsg,_KMsgHandler) \
	case _KMsg: return _KMsgHandler(wParam,lParam);

#define END_KMSG_HANDLER(_KComponentParentClass) \
	default: return _KComponentParentClass::WindowProc(hwnd,msg,wParam,lParam); \
	}\
	}

#ifndef RFC_NO_UNICODE_STR
	#define T(stringLiteral) KString(L##stringLiteral)
#else
	#define T(stringLiteral) KString(stringLiteral)
#endif

#define KFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

class RFC_API InternalDefinitions
{
public:
	static const wchar_t* RFCPropText_Object;
	static const wchar_t* RFCPropText_OldProc;
};

#endif

#endif

