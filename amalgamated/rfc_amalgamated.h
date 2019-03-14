
// ========== Generated With RFC Amalgamator v1.0 ==========

#ifndef _RFC_AMALGAMATED_H_
#define _RFC_AMALGAMATED_H_ 

#define AMALGAMATED_VERSION

#ifndef _CRT_SECURE_NO_WARNINGS 
	#define _CRT_SECURE_NO_WARNINGS 
#endif

// =========== config.h ===========

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef _WIN64
	#define RFC64
	#define RFC_PTR_SIZE 8
	#define RFC_NATIVE_INT __int64
#else
	#define RFC32
	#define RFC_PTR_SIZE 4
	#define RFC_NATIVE_INT int
#endif


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

// =========== KScopedStructPointer.h ===========

/*
	RFC - KScopedStructPointer.h
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

#ifndef _RFC_KSCOPED_STRUCT_POINTER_H_
#define _RFC_KSCOPED_STRUCT_POINTER_H_

#include <malloc.h>
#include <Objbase.h>

template<class StructType>
class KReleaseUsingFree
{
public:
	static void Free(StructType* structPtr)
	{
		::free(structPtr);
	}
};

template<class StructType>
class KReleaseUsingTaskMemFree
{
public:
	static void Free(StructType* memory)
	{
		::CoTaskMemFree(memory);
	}
};

/**
	This class holds a pointer to the struct which is automatically freed when this object goes
	out of scope. 
*/
template<class StructType, class ReleaseMethod = KReleaseUsingFree<StructType>>
class KScopedStructPointer
{
private:
	StructType* structPointer;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedStructPointer()
	{
		structPointer = NULL;
	}

	inline KScopedStructPointer(StructType* structPointer)
	{
		this->structPointer = structPointer;
	}

	KScopedStructPointer(KScopedStructPointer& structPointerToTransferFrom)
	{
		this->structPointer = structPointerToTransferFrom.structPointer;
		structPointerToTransferFrom.structPointer = NULL;
	}

	bool IsNull()
	{
		return (structPointer == NULL);
	}

	/** 
		Removes the current struct pointer from this KScopedStructPointer without freeing it.
		This will return the current struct pointer, and set the KScopedStructPointer to a null pointer.
	*/
	StructType* Detach()
	{ 
		StructType* m = structPointer;
		structPointer = NULL;
		return m; 
	}

	~KScopedStructPointer()
	{
		if (structPointer)
			ReleaseMethod::Free(structPointer);
	}

	/** 
		Changes this KScopedStructPointer to point to a new struct.

		If this KScopedStructPointer already points to a struct, that struct
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedStructPointer& operator= (StructType* const newStructPointer)
	{
		if (structPointer != newStructPointer)
		{
			StructType* const oldStructPointer = structPointer;
			structPointer = newStructPointer;

			if (oldStructPointer)
				ReleaseMethod::Free(oldStructPointer);
		}

		return *this;
	}

	inline StructType** operator&() { return &structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline operator StructType*() const { return structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline StructType& operator*() const { return *structPointer; }

	/** Lets you access properties of the struct that this KScopedStructPointer refers to. */
	inline StructType* operator->() const { return structPointer; }

};

#endif

// =========== KScopedMemoryBlock.h ===========

/*
	RFC - KScopedMemoryBlock.h
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

#ifndef _RFC_KSCOPED_MEMORY_BLOCK_H_
#define _RFC_KSCOPED_MEMORY_BLOCK_H_

#include <malloc.h>

/**
	This class holds a memory which is automatically freed when this object goes
	out of scope.

	Once a memory has been passed to a KScopedMemoryBlock, it will make sure that the memory
	gets freed when the KScopedMemoryBlock is deleted. Using the KScopedMemoryBlock on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	allocated memory.

	A KScopedMemoryBlock can be used in pretty much the same way that you'd use a normal pointer
	to a memory block. If you use the assignment operator to assign a different memory to a
	KScopedMemoryBlock, the old one will be automatically freed.

	If you need to get a memory block out of a KScopedClassPointer without it being freed, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedMemoryBlock<float*> a = (float*)malloc(512 * sizeof(float)); // slow
	KScopedMemoryBlock<float*> b( (float*)malloc(512 * sizeof(float)) ); // fast
	@endcode
*/
template<class T>
class KScopedMemoryBlock
{
private:
	T memoryBlock;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedMemoryBlock()
	{
		memoryBlock = NULL;
	}

	inline KScopedMemoryBlock(T memoryBlock)
	{
		this->memoryBlock = memoryBlock;
	}

	KScopedMemoryBlock(KScopedMemoryBlock& memoryBlockToTransferFrom)
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = NULL;
	}

	bool IsNull()
	{
		return (memoryBlock == NULL);
	}

	/** 
		Removes the current memory block from this KScopedMemoryBlock without freeing it.
		This will return the current memory block, and set the KScopedMemoryBlock to a null pointer.
	*/
	T Detach()
	{ 
		T m = memoryBlock;
		memoryBlock = NULL;
		return m; 
	}

	~KScopedMemoryBlock()
	{
		if (memoryBlock)
			::free(memoryBlock);
	}

	/** 
		Changes this KScopedMemoryBlock to point to a new memory block.

		If this KScopedMemoryBlock already points to a memory, that memory
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedMemoryBlock& operator= (T const newMemoryBlock)
	{
		if (memoryBlock != newMemoryBlock)
		{
			T const oldMemoryBlock = memoryBlock;
			memoryBlock = newMemoryBlock;

			if (oldMemoryBlock)
				::free(oldMemoryBlock);
		}

		return *this;
	}

	/** Returns the memory block that this KScopedMemoryBlock refers to. */
	inline operator T() const { return memoryBlock; }

};

#endif

// =========== KScopedHandle.h ===========

/*
	RFC - KScopedHandle.h
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

#ifndef _RFC_KSCOPED_HANDLE_H_
#define _RFC_KSCOPED_HANDLE_H_

#include <windows.h>

/**
	This class holds a handle which is automatically closed when this object goes
	out of scope.
*/
class KScopedHandle
{
private:
	HANDLE handle;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedHandle()
	{
		handle = 0;
	}

	KScopedHandle(HANDLE handle)
	{
		this->handle = handle;
	}

	HANDLE Detach()
	{
		HANDLE h = handle;
		handle = 0;
		return h;
	}

	KScopedHandle& operator= (HANDLE newHandle)
	{
		if (handle != newHandle)
		{
			HANDLE oldHandle = handle;
			handle = newHandle;

			if (oldHandle)
				::CloseHandle(oldHandle);
		}

		return *this;
	}

	bool IsNull()
	{
		return (handle == 0);
	}

	~KScopedHandle()
	{
		if (handle)
			::CloseHandle(handle);
	}

	inline operator HANDLE() const { return handle; }

	inline HANDLE* operator&() { return &handle; }

};

#endif

// =========== KScopedGdiObject.h ===========

/*
	RFC - KScopedGdiObject.h
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

#ifndef _RFC_KSCOPED_GDI_OBJECT_H_
#define _RFC_KSCOPED_GDI_OBJECT_H_

#include <windows.h>

/**
	This class holds a gdi object which is automatically freed when this object goes
	out of scope.

	If you need to get a gdi object out of a KScopedGdiObject without it being freed, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedGdiObject<HBRUSH> a = ::CreateSolidBrush(RGB(255, 255, 255)); // slow
	KScopedGdiObject<HBRUSH> b(::CreateSolidBrush(RGB(255, 0, 0))); // fast
	@endcode,
*/
template<class T>
class KScopedGdiObject
{
private:
	T gdiObject;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedGdiObject()
	{
		gdiObject = 0;
	}

	inline KScopedGdiObject(T gdiObject)
	{
		this->gdiObject = gdiObject;
	}

	KScopedGdiObject(KScopedGdiObject& gdiObjectToTransferFrom)
	{
		this->gdiObject = gdiObjectToTransferFrom.gdiObject;
		gdiObjectToTransferFrom.gdiObject = 0;
	}

	/** 
		Removes the current gdi object from this KScopedGdiObject without freeing it.
		This will return the current gdi object, and set the KScopedGdiObject to a null value.
	*/
	T Detach()
	{ 
		T g = gdiObject;
		gdiObject = 0;
		return g; 
	}

	~KScopedGdiObject()
	{
		if (gdiObject)
			::DeleteObject(gdiObject);
	}

	/** 
		Changes this KScopedGdiObject to point to a new gdi object.

		If this KScopedGdiObject already points to a gdi object, that object
		will first be freed.

		The object that you pass in may be a zero.
	*/
	KScopedGdiObject& operator= (T const newGdiObject)
	{
		if (gdiObject != newGdiObject)
		{
			T const oldgdiObject = gdiObject;
			gdiObject = newGdiObject;

			if (oldgdiObject)
				::DeleteObject(oldgdiObject);
		}

		return *this;
	}

	/** Returns the gdi object that this KScopedGdiObject refers to. */
	inline operator T() const { return gdiObject; }

};

#endif

// =========== KScopedCriticalSection.h ===========

/*
	RFC - KScopedCriticalSection.h
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

#ifndef _RFC_KSCOPED_CRITICAL_SECTION_H_
#define _RFC_KSCOPED_CRITICAL_SECTION_H_

#include <windows.h>

/**
	This class holds a pointer to CRITICAL_SECTION which is automatically released when this object goes
	out of scope.
*/
class KScopedCriticalSection
{
private:
	CRITICAL_SECTION *criticalSection;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedCriticalSection(CRITICAL_SECTION *criticalSection)
	{
		this->criticalSection = criticalSection;
		::EnterCriticalSection(criticalSection);
	}

	// does not call LeaveCriticalSection
	CRITICAL_SECTION* Detach()
	{ 
		CRITICAL_SECTION *c = criticalSection;
		criticalSection = NULL;
		return c; 
	}

	~KScopedCriticalSection()
	{
		if (criticalSection)
			::LeaveCriticalSection(criticalSection);
	}

	inline operator CRITICAL_SECTION*() const { return criticalSection; }

};

#endif

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

// =========== KScopedClassPointer.h ===========

/*
	RFC - KScopedClassPointer.h
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

#ifndef _RFC_KSCOPED_CLASS_POINTER_H_
#define _RFC_KSCOPED_CLASS_POINTER_H_

/**
	This class holds a pointer which is automatically deleted when this object goes
	out of scope.

	Once a pointer has been passed to a KScopedClassPointer, it will make sure that the pointer
	gets deleted when the KScopedClassPointer is deleted. Using the KScopedClassPointer on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	created objects.

	A KScopedClassPointer can be used in pretty much the same way that you'd use a normal pointer
	to an object. If you use the assignment operator to assign a different object to a
	KScopedClassPointer, the old one will be automatically deleted.

	Important note: The class is designed to hold a pointer to an object, NOT to an array!
	It calls delete on its payload, not delete[], so do not give it an array to hold!

	If you need to get a pointer out of a KScopedClassPointer without it being deleted, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	KScopedClassPointer<MyClass> a = new MyClass(); // slow
	a->myMethod();
	a = new MyClass(); // old object will be deleted
	KScopedClassPointer<MyClass> b( new MyClass() ); // fast
	@endcode

*/
template<class T>
class KScopedClassPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedClassPointer()
	{
		object = 0;
	}

	inline KScopedClassPointer(T* object)
	{
		this->object = object;
	}

	KScopedClassPointer(KScopedClassPointer& objectToTransferFrom)
	{
		this->object = objectToTransferFrom.object;
		objectToTransferFrom.object = 0;
	}

	bool IsNull()
	{
		return (object == NULL);
	}

	/** 
		Removes the current object from this KScopedClassPointer without deleting it.
		This will return the current object, and set the KScopedClassPointer to a null pointer.
	*/
	T* Detach()
	{ 
		T* o = object; 
		object = 0; 
		return o; 
	}

	~KScopedClassPointer()
	{
		if (object)
			delete object;
	}

	/** 
		Changes this KScopedClassPointer to point to a new object.

		If this KScopedClassPointer already points to an object, that object
		will first be deleted.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedClassPointer& operator= (T* const newObject)
	{
		if (object != newObject)
		{
			T* const oldObject = object;
			object = newObject;

			if (oldObject)
				delete oldObject;
		}

		return *this;
	}

	/** Returns the object that this KScopedClassPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this KScopedClassPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this KScopedClassPointer refers to. */
	inline T* operator->() const { return object; }
};

#endif

// =========== KLeakDetector.h ===========

/*
	RFC - KLeakDetector.h
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

#ifndef _RFC_KLEAK_DETECTOR_H_
#define _RFC_KLEAK_DETECTOR_H_

#include <windows.h>
#include <string.h>  
#include <stdlib.h>  

/**
	Embedding an instance of this class inside another class can be used as a
	low-overhead way of detecting leaked instances.

	This class keeps an internal static count of the number of instances that are
	active, so that when the app is shutdown and the static destructors are called,
	it can check whether there are any left-over instances that may have been leaked.

	To use it, simply declare RFC_LEAK_DETECTOR(YourClassName) inside a private section
	of the class declaration.

	(ref: LeakedObjectDetector class of JUCE)

	@code
	class MyClass
	{
	public:
		MyClass();
		void method1();

	private:
		RFC_LEAK_DETECTOR (MyClass)
	};
	@endcode
*/
template <class T>
class KLeakDetector
{
public:
	KLeakDetector()
	{
		::InterlockedIncrement(&GetCounter().numObjects);
	}

	KLeakDetector(const KLeakDetector&)
	{
		::InterlockedIncrement(&GetCounter().numObjects);
	}

	~KLeakDetector()
	{
		::InterlockedDecrement(&GetCounter().numObjects);
	}

private:
	class LeakCounter
	{
	public:
		LeakCounter()
		{
			numObjects = 0;
		}

		~LeakCounter()
		{
			if (numObjects > 0) // If you hit this, then you've leaked one or more objects of the type specified by the template parameter
			{
				char textBuffer[128];
				::strcpy(textBuffer, "Leaked objects detected : ");

				char intBuffer[16];
				::_itoa((int)numObjects, intBuffer, 10);

				::strcat(textBuffer, intBuffer);
				::strcat(textBuffer, " instance(s) of class ");
				::strcat(textBuffer, GetLeakedClassName());

				::MessageBoxA(0, textBuffer, "Warning", MB_ICONWARNING);
			}
		}

		volatile long numObjects;
	};

	static const char* GetLeakedClassName()
	{
		return T::rfc_GetLeakedClassName();
	}

	static LeakCounter& GetCounter()
	{
		static LeakCounter counter;
		return counter;
	}
};

#ifdef _DEBUG
	#define RFC_LEAK_DETECTOR(Class) \
			friend class KLeakDetector<Class>; \
			KLeakDetector<Class> rfc_leakDetector; \
			static const char* rfc_GetLeakedClassName() { return #Class; }
#else 
	#define RFC_LEAK_DETECTOR(Class)
#endif

#endif

// =========== KScopedComPointer.h ===========

/*
	RFC - KScopedComPointer.h
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

#ifndef _RFC_KSCOPED_COM_POINTER_H_
#define _RFC_KSCOPED_COM_POINTER_H_

/**
	This class holds a COM pointer which is automatically released when this object goes
	out of scope.
*/
template<class T>
class KScopedComPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedComPointer()
	{
		object = NULL;
	}

	inline KScopedComPointer(T* object)
	{
		this->object = object;

		if (this->object)
			this->object->AddRef();
	}

	KScopedComPointer(KScopedComPointer& objectToTransferFrom)
	{
		object = objectToTransferFrom.object;

		if (object)
			object->AddRef();
	}

	bool IsNull()
	{
		return (object == NULL);
	}

	/** 
		Removes the current COM object from this KScopedComPointer without releasing it.
		This will return the current object, and set the KScopedComPointer to a null pointer.
	*/
	T* Detach()
	{ 
		T* o = object; 
		object = NULL; 
		return o; 
	}

	~KScopedComPointer()
	{
		if (object)
			object->Release();

		object = NULL;
	}

	inline T** operator&() { return &object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this KScopedComPointer refers to. */
	inline T* operator->() const { return object; }

};

#endif

// =========== KMenuItemListener.h ===========

/*
	RFC - KMenuItemListener.h
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

#ifndef _RFC_KMENUITEMLISTENER_H_
#define _RFC_KMENUITEMLISTENER_H_


class KMenuItem;

class KMenuItemListener
{
public:
	KMenuItemListener();

	virtual ~KMenuItemListener();

	virtual void OnMenuItemPress(KMenuItem *menuItem);
};

#endif

// =========== KTrackBarListener.h ===========

/*
	RFC - KTrackBarListener.h
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

#ifndef _RFC_KTRACKBARLISTENER_H_
#define _RFC_KTRACKBARLISTENER_H_


class KTrackBar;

class KTrackBarListener
{
public:
	KTrackBarListener();

	virtual ~KTrackBarListener();

	virtual void OnTrackBarChange(KTrackBar *trackBar);
};

#endif

// =========== KTimerListener.h ===========

/*
	RFC - KTimerListener.h
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

#ifndef _RFC_KTIMER_LISTENER_H_
#define _RFC_KTIMER_LISTENER_H_


class KTimer;

class KTimerListener
{
public:
	KTimerListener();

	virtual ~KTimerListener();

	virtual void OnTimer(KTimer *timer);
};

#endif

// =========== KGridViewListener.h ===========

/*
	RFC - KGridViewListener.h
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

#ifndef _RFC_KGRIDVIEWLISTENER_H_
#define _RFC_KGRIDVIEWLISTENER_H_


class KGridView;

class KGridViewListener
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

#ifndef _RFC_KLISTBOXLISTENER_H_
#define _RFC_KLISTBOXLISTENER_H_


class KListBox;

class KListBoxListener
{
public:
	KListBoxListener();

	virtual ~KListBoxListener();

	virtual void OnListBoxItemSelect(KListBox *listBox);

	virtual void OnListBoxItemDoubleClick(KListBox *listBox);
};

#endif

// =========== KButtonListener.h ===========

/*
	RFC - KButtonListener.h
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

#ifndef _RFC_KBUTTONLISTENER_H_
#define _RFC_KBUTTONLISTENER_H_


class KButton;

class KButtonListener
{
public:
	KButtonListener();

	virtual ~KButtonListener();

	virtual void OnButtonPress(KButton *button);
};

#endif

// =========== KComboBoxListener.h ===========

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

#ifndef _RFC_KCOMBOBOXLISTENER_H_
#define _RFC_KCOMBOBOXLISTENER_H_


class KComboBox;

class KComboBoxListener
{
public:
	KComboBoxListener();

	virtual ~KComboBoxListener();

	virtual void OnComboBoxItemSelect(KComboBox *comboBox);
};

#endif

// =========== KStringHolder.h ===========

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

#ifndef _RFC_KSTRINGHOLDER_H_
#define _RFC_KSTRINGHOLDER_H_

#include <windows.h>

/**
	This class holds reference counted string.
*/
class KStringHolder
{
	volatile LONG refCount;
	char *a_text; // ansi version

	#ifndef RFC_NO_SAFE_ANSI_STR
	CRITICAL_SECTION cs_a_text; // to guard ansi string creation
	#endif

public:
	wchar_t *w_text; // unicode version
	int count; // character count

	KStringHolder(wchar_t *w_text, int count);

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

private:
	RFC_LEAK_DETECTOR(KStringHolder)
};

#endif

// =========== KRunnable.h ===========

/*
RFC - KRunnable.h
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

#ifndef _RFC_KRUNNABLE_H_
#define _RFC_KRUNNABLE_H_


class KThread;

class KRunnable
{
public:
	KRunnable();

	virtual ~KRunnable();

	virtual void Run(KThread *thread);

private:
	RFC_LEAK_DETECTOR(KRunnable)
};

#endif

// =========== KPerformanceCounter.h ===========

/*
	RFC - KPerformanceCounter.h
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

#ifndef _RFC_KPERFORMANCECOUNTER_H_
#define _RFC_KPERFORMANCECOUNTER_H_

#include <windows.h>

class KPerformanceCounter
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

private:
	RFC_LEAK_DETECTOR(KPerformanceCounter)
};

#endif

// =========== KPointerList.h ===========

/*
	RFC - KPointerList.h
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
	T* list;

	CRITICAL_SECTION criticalSection;
	volatile bool isThreadSafe;

public:
	/**
		Constructs PointerList object.
		@param roomIncrement initial and reallocation size of internal memory block in DWORDS
		@param isThreadSafe make all the methods thread-safe
	*/
	KPointerList(const int roomIncrement = 1024, const bool isThreadSafe = false) // 1024*4=4096 = default alignment!
	{
		roomCount = roomIncrement;
		this->roomIncrement = roomIncrement;
		this->isThreadSafe = isThreadSafe;
		size = 0;
		list = (T*)::malloc(roomCount * RFC_PTR_SIZE);
		
		if(isThreadSafe)
			::InitializeCriticalSection(&criticalSection);
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool AddPointer(T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection); // thread safe!

		if(roomCount >= (size + 1) ) // no need reallocation. coz room count is enough!
		{
			list[size] = pointer;
			size++;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;
		}
		else // require reallocation!
		{
			roomCount += roomIncrement;
			void* retVal = ::realloc((void*)list, roomCount * RFC_PTR_SIZE);
			if(retVal)
			{
				list = (T*)retVal;
				list[size] = pointer;
				size++;

				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return true;
			}
			else // memory allocation failed!
			{
				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return false;
			}
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T GetPointer(const int id)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) ) // checks for valid range!
		{	
			T object = list[id];

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return object;
		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return NULL;
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
	bool SetPointer(const int id, T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) )
		{	
			list[id] = pointer;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;
		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return false;
		}
	}

	/**
		Remove pointer of given id
		@returns false if id is out of range!
	*/
	bool RemovePointer(const int id)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) )
		{	
			const int newRoomCount = (((size - 1) / roomIncrement) + 1) * roomIncrement;
			T* newList = (T*)::malloc(newRoomCount * RFC_PTR_SIZE);

			for(int i = 0, j = 0; i < size; i++)
			{
				if(i != id)
				{
					newList[j] = list[i];
					j++;
				}	
			}
			::free((void*)list); // free old list!
			list = newList;
			roomCount = newRoomCount;
			size--;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;

		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return false;
		}

	}

	/**
		Clears the list!
	*/
	void RemoveAll(bool reallocate = true)// remove all pointers from list!
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		::free((void*)list);
		roomCount = roomIncrement;
		list = reallocate ? (T*)::malloc(roomCount * RFC_PTR_SIZE) : NULL;
		size = 0;

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list.
	*/
	void DeleteAll(bool reallocate = true)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		for(int i = 0; i < size; i++)
		{
			T object = list[i];
			delete object;
		}

		::free((void*)list);

		roomCount = roomIncrement;
		list = reallocate ? (T*)::malloc(roomCount * RFC_PTR_SIZE) : NULL;
		size = 0;

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	/**
		Finds the id of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int GetID(T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		for(int i = 0; i < size; i++)
		{
			if (list[i] == pointer)
			{
				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return i;
			}
		}

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);

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
			::free((void*)list);

		if(isThreadSafe)
			::DeleteCriticalSection(&criticalSection);
	}

private:
	RFC_LEAK_DETECTOR(KPointerList)
};

#endif

// =========== KGraphics.h ===========

/*
	RFC -KGraphics.h
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

#ifndef _RFC_KGRAPHICS_H_
#define _RFC_KGRAPHICS_H_

#include <windows.h>

class KGraphics
{
public:
	KGraphics();

	virtual ~KGraphics();

	static void Draw3dVLine(HDC hdc, int startX, int startY, int height);

	static void Draw3dHLine(HDC hdc, int startX, int startY, int width);

	static void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color);

	static void FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color);

	static RECT CalculateTextSize(const wchar_t *text, HFONT hFont);

private:
	RFC_LEAK_DETECTOR(KGraphics)
};

#endif

// =========== KString.h ===========

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
	virtual KString Append(const KString& otherString)const;

	/**
		Appends a statically typed string to begining or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to begining if false
		@returns			the concatenated string
	*/
	virtual KString AppendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	virtual void AssignStaticText(const wchar_t* const text, int length);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	virtual KString SubString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	virtual bool CompareIgnoreCase(const KString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	virtual bool Compare(const KString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	virtual bool CompareWithStaticText(const wchar_t* const text)const;

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

	virtual bool IsNotEmpty()const;

	/**
		Returns value of string
	*/
	virtual int GetIntValue()const;

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

// =========== KThread.h ===========

/*
	RFC - KThread.h
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

#ifndef _RFC_KTHREAD_H_
#define _RFC_KTHREAD_H_

#include <windows.h>

/**
	Encapsulates a thread.

	Method1:
	Subclasses derive from KThread and implement the Run() method, in which they
	do their business. The thread can then be started with the StartThread() method
	and controlled with various other methods.

	Run() method implementation might be like this
	@code
	virtual void Run()
	{
		while(ShouldRun())
		{
			// your code goes here...
		}
	}
	@endcode

	Method2:
	Subclasses derive from KRunnable and implement the Run method. 

	Run method implementation might be like this
	@code
	virtual void Run(KThread *thread)
	{
		while(thread->ShouldRun())
		{
			// your code goes here...
		}
	}
	@endcode
*/
class KThread
{
protected:
	HANDLE handle;
	volatile bool threadShouldStop;
	KRunnable *runnable;

public:
	KThread();

	/**
		Sets thread handle.
	*/
	virtual void SetHandle(HANDLE handle);

	/**
		Sets runnable object for this thread.
	*/
	virtual void SetRunnable(KRunnable *runnable);

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
		@returns true if thread should run
	*/
	virtual bool ShouldRun();

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

private:
	RFC_LEAK_DETECTOR(KThread)
};

#endif

// =========== KLogger.h ===========

/*
	RFC - KLogger.h
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

private:
	RFC_LEAK_DETECTOR(KLogger)
};

#endif

// =========== KDirectory.h ===========

/*
	RFC - KDirectory.h
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

#ifndef _RFC_KDIRECTORY_H_
#define _RFC_KDIRECTORY_H_

#include <windows.h>
#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class KDirectory
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

private:
	RFC_LEAK_DETECTOR(KDirectory)
};

#endif

// =========== KMD5.h ===========

/*
	RFC - KMD5.h
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

#ifndef _RFC_KMD5_H_
#define _RFC_KMD5_H_


class KMD5
{
public:
	KMD5();

	static KString GenerateFromString(const KString& text);

	static KString GenerateFromFile(const KString& fileName);

	virtual ~KMD5();

private:
	RFC_LEAK_DETECTOR(KMD5)
};

#endif

// =========== KFile.h ===========

/*
	RFC - KFile.h
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

#ifndef _RFC_KFILE_H_
#define _RFC_KFILE_H_

#include <windows.h>

/**
	Can be use to read/write data from a file easily.
*/
class KFile
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
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	virtual bool SetFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN);

	virtual DWORD GetFilePointerPosition();

	virtual bool SetFilePointerToEnd();

	/**
		returns zero on error
	*/
	virtual DWORD GetFileSize();

	static bool DeleteFile(const KString& fileName);

	static bool IsFileExists(const KString& fileName);

	static bool CopyFile(const KString& sourceFileName, const KString& destFileName);

	virtual ~KFile();

private:
	RFC_LEAK_DETECTOR(KFile)
};

#endif

// =========== KApplication.h ===========

/*
	RFC - KApplication.h
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

#ifndef _RFC_KAPPLICATION_H_
#define _RFC_KAPPLICATION_H_

#include <windows.h>

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

			int Main(KString **argv,int argc)
			{
				// your app code goes here...
				return 0;
			}
	};

	START_RFC_APPLICATION(MyApp)
	@endcode
*/
class KApplication
{
public:

	/**
		Use this field to get HINSTANCE of your application.
		(This field will set when calling InitRFC function.)
	*/
	static HINSTANCE hInstance;

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
		Return false if your application is single instance only.
		Single instance applications must implement "GetApplicationID" method.
	*/
	virtual bool AllowMultipleInstances();

	/**
		This method will be called if the application is single instance only and another instance is already running.
		("Main" method will not be called.)
	*/
	virtual int AnotherInstanceIsRunning(KString **argv, int argc);

	/**
		Unique id of your application which is limited to MAX_PATH characters.
		Single instance applications must implement this method.
	*/
	virtual const wchar_t* GetApplicationID();

	/** 
		Destructs an Application object.
	*/
	virtual ~KApplication();

private:
	RFC_LEAK_DETECTOR(KApplication)
};

#endif

// =========== KSHA1.h ===========

/*
	RFC - KSHA1.h
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

#ifndef _RFC_KSHA1_H_
#define _RFC_KSHA1_H_


class KSHA1
{
public:
	KSHA1();

	static KString GenerateFromString(const KString& text);

	static KString GenerateFromFile(const KString& fileName);

	virtual ~KSHA1();

private:
	RFC_LEAK_DETECTOR(KSHA1)
};

#endif

// =========== KSettingsReader.h ===========

/*
	RFC - KSettingsReader.h
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

#ifndef _RFC_KSETTINGSREADER_H_
#define _RFC_KSETTINGSREADER_H_

#include <windows.h>

/**
	High performance configuration reading class.
*/
class KSettingsReader
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

private:
	RFC_LEAK_DETECTOR(KSettingsReader)
};

#endif

// =========== KBitmap.h ===========

/*
	RFC - KBitmap.h
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

#ifndef _RFC_KBITMAP_H_
#define _RFC_KBITMAP_H_

#include <windows.h>

/**
	Can be use to load bitmap image from file or resource.
*/
class KBitmap
{
protected:
	HBITMAP hBitmap;

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

private:
	RFC_LEAK_DETECTOR(KBitmap)
};

#endif


// =========== KFont.h ===========

/*
	RFC - KFont.h
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

#ifndef _RFC_KFONT_H_
#define _RFC_KFONT_H_

#include <windows.h>

/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class KFont
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

private:
	RFC_LEAK_DETECTOR(KFont)
};

#endif

// =========== KCursor.h ===========

/*
	RFC - KCursor.h
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

#ifndef _RFC_KCURSOR_H_
#define _RFC_KCURSOR_H_

#include <windows.h>

/**
	Can be use to load cursor from file or resource.
*/
class KCursor
{
protected:
	HCURSOR hCursor;

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

private:
	RFC_LEAK_DETECTOR(KCursor)
};

#endif

// =========== KMenuItem.h ===========

/*
	RFC - KMenuItem.h
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

#ifndef _RFC_KMENUITEM_H_
#define _RFC_KMENUITEM_H_

#include <windows.h>

class KMenuItemListener;

class KMenuItem
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

private:
	RFC_LEAK_DETECTOR(KMenuItem)
};

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

class KRegistry
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

// =========== KIcon.h ===========

/*
	RFC -KIcon.h
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

#ifndef _RFC_KICON_H_
#define _RFC_KICON_H_

#include <windows.h>

/**
	Can be use to load icon from file or resource.
*/
class KIcon
{
protected:
	HICON hIcon;

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

private:
	RFC_LEAK_DETECTOR(KIcon)
};

#endif

// =========== KComponent.h ===========

/*
	RFC - KComponent.h
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

#ifndef _RFC_KCOMPONENT_H_
#define _RFC_KCOMPONENT_H_


#include <windows.h>

/**
	Base class of all W32 gui objects.
	define "RFC_SINGLE_THREAD_COMP_CREATION" if your app does not create components within multiple threads.
*/
class KComponent
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

	/**
		Constructs a standard win32 component.
		@param generateWindowClassDetails	set to false if you are not registering window class and using standard class name like BUTTON, STATIC etc... wc member is invalid if generateWindowClassDetails is false.
	*/
	KComponent(bool generateWindowClassDetails);

	/**
		Called after hotplugged into a given HWND.
	*/
	virtual void OnHotPlug();

	/**
		HotPlugs given HWND. this method does not update current compFont and cursor variables.
		Set fetchInfo to true if you want to acquire all the information about this HWND. (width, height, position etc...)
		Set fetchInfo to false if you just need to receive events. (button click etc...)
	*/
	virtual void HotPlugInto(HWND component, bool fetchInfo = true);

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
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		@returns false if registration failed or component creation failed.
	*/
	virtual bool CreateComponent(bool requireInitialMessages = false);

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

private:
	RFC_LEAK_DETECTOR(KComponent)
};

#endif

// =========== KLabel.h ===========

/*
	RFC - KLabel.h
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

#ifndef _RFC_KLABEL_H_
#define _RFC_KLABEL_H_


class KLabel : public KComponent
{
public:
	KLabel();

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual ~KLabel();
};

#endif

// =========== KProgressBar.h ===========

/*
	RFC - KProgressBar.h
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

#ifndef _RFC_KPROGRESSAR_H_
#define _RFC_KPROGRESSAR_H_


class KProgressBar : public KComponent
{
protected:
	int value;

public:
	KProgressBar(bool smooth=true, bool vertical=false);

	virtual int GetValue();

	virtual void SetValue(int value);

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual ~KProgressBar();
};


#endif

// =========== KTextBox.h ===========

/*
	RFC - KTextBox.h
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

#ifndef _RFC_KTEXTBOX_H_
#define _RFC_KTEXTBOX_H_


class KTextBox : public KComponent
{
public:
	KTextBox(bool readOnly = false);

	virtual KString GetText();

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual ~KTextBox();
};

#endif

// =========== KTrackBar.h ===========

/*
	RFC - KTrackBar.h
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

#ifndef _RFC_KTRACKBAR_H_
#define _RFC_KTRACKBAR_H_


class KTrackBarListener;

class KTrackBar : public KComponent
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

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual ~KTrackBar();
};


#endif


// =========== KTextArea.h ===========

/*
	RFC - KTextArea.h
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

#ifndef _RFC_KTEXTAREA_H_
#define _RFC_KTEXTAREA_H_


class KTextArea : public KTextBox
{
public:
	KTextArea(bool autoScroll = false, bool readOnly = false);

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual ~KTextArea();
};


#endif

// =========== KButton.h ===========

/*
	RFC - KButton.h
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

#ifndef _RFC_KBUTTON_H_
#define _RFC_KBUTTON_H_


class KButtonListener;

class KButton : public KComponent
{
protected:
	KButtonListener *listener;

public:
	KButton();

	virtual void SetListener(KButtonListener *listener);

	virtual KButtonListener* GetListener();

	virtual void OnPress();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual ~KButton();
};

#endif

// =========== KCheckBox.h ===========

/*
	RFC - KCheckBox.h
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

#ifndef _RFC_KCHECKBOX_H_
#define _RFC_KCHECKBOX_H_


class KCheckBox : public KButton
{
protected:
	bool checked;

public:
	KCheckBox();

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void OnPress();

	virtual bool IsChecked();

	virtual void SetCheckedState(bool state);

	virtual ~KCheckBox();
};

#endif

// =========== KGridView.h ===========

/*
	RFC - KGridView.h
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

#ifndef _RFC_KGRIDVIEW_H_
#define _RFC_KGRIDVIEW_H_


class KGridViewListener;

class KGridView : public KComponent
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

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void OnItemSelect();

	virtual void OnItemRightClick();

	virtual void OnItemDoubleClick();

	virtual ~KGridView();
};

#endif

// =========== KSettingsWriter.h ===========

/*
	RFC - KSettingsWriter.h
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

#ifndef _RFC_KSETTINGSWRITER_H_
#define _RFC_KSETTINGSWRITER_H_

#include <windows.h>

/**
	High performance configuration writing class.
*/
class KSettingsWriter
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

private:
	RFC_LEAK_DETECTOR(KSettingsWriter)
};

#endif

// =========== KWindow.h ===========

/*
	RFC - KWindow.h
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

#ifndef _RFC_KWINDOW_H_
#define _RFC_KWINDOW_H_


class KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;

public:
	KWindow();

	virtual void Flash();

	virtual void SetIcon(KIcon *icon);

	virtual void Destroy();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual void CenterScreen();

	/**
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool AddComponent(KComponent *component, bool requireInitialMessages = false);

	virtual bool SetClientAreaSize(int width, int height);

	virtual bool GetClientAreaSize(int *width, int *height);

	virtual void OnMoved();

	virtual void OnResized();

	virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual ~KWindow();
};

#endif

// =========== KListBox.h ===========

/*
	RFC - KListBox.h
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

#ifndef _RFC_KLISTBOX_H_
#define _RFC_KLISTBOX_H_


class KListBoxListener;

class KListBox : public KComponent
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

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void OnItemSelect();

	virtual void OnItemDoubleClick();

	virtual ~KListBox();
};

#endif

// =========== KCommonDialogBox.h ===========

/*
	RFC - KCommonDialogBox.h
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

#ifndef _RFC_KCOMMONDIALOGBOX_H_
#define _RFC_KCOMMONDIALOGBOX_H_


class KCommonDialogBox
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

// =========== KComboBox.h ===========

/*
	RFC - KComboBox.h
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

#ifndef _RFC_KCOMBOBOX_H_
#define _RFC_KCOMBOBOX_H_



class KComboBoxListener;

class KComboBox : public KComponent
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

	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void SetListener(KComboBoxListener *listener);

	virtual void OnItemSelect();

	virtual ~KComboBox();
};

#endif

// =========== KMenu.h ===========

/*
	RFC - KMenu.h
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

#ifndef _RFC_KMENU_H_
#define _RFC_KMENU_H_


class KMenu
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

private:
	RFC_LEAK_DETECTOR(KMenu)
};

#endif

// =========== KTimer.h ===========

/*
	RFC - KTimer.h
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

#ifndef _RFC_KTIMER_H_
#define _RFC_KTIMER_H_

#include <windows.h>

/**
	Encapsulates a timer.

	The timer can be started with the StartTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling SetTimerWindow method.
*/
class KTimer
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

private:
	RFC_LEAK_DETECTOR(KTimer)
};

#endif

// =========== KMenuBar.h ===========

/*
	RFC - KMenuBar.h
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

#ifndef _RFC_KMENUBAR_H_
#define _RFC_KMENUBAR_H_


class KMenuBar
{
protected:
	HMENU hMenu;

public:
	KMenuBar();

	virtual void AddMenu(const KString& text, KMenu *menu);

	virtual void AddToWindow(KWindow *window);

	virtual ~KMenuBar();

private:
	RFC_LEAK_DETECTOR(KMenuBar)
};


#endif

// =========== KPlatformUtil.h ===========

/*
	RFC - KPlatformUtil.h
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

#ifndef _RFC_KPLATFORMUTIL_H_
#define _RFC_KPLATFORMUTIL_H_

#include <windows.h>
#include <stdio.h>

/**
	Singleton class which can be use to generate class names, timer ids etc...
	define "RFC_SINGLE_THREAD_COMP_CREATION" if your app does not creating components/menu items/timers
	within multiple threads.
	(this class is for internal use)
*/
class KPlatformUtil
{
private:
	RFC_LEAK_DETECTOR(KPlatformUtil)

	static KPlatformUtil *_instance;
	KPlatformUtil();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	KPointerList<KMenuItem*> *menuItemList;
	KPointerList<KTimer*> *timerList;

	#ifndef RFC_SINGLE_THREAD_COMP_CREATION
	CRITICAL_SECTION criticalSectionForCount;
	#endif

public:

	static KPlatformUtil* GetInstance();

	UINT GenerateControlID();

	// KApplication:hInstance must be valid before calling this method
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

#ifndef _RFC_KNUMERICFIELD_H_
#define _RFC_KNUMERICFIELD_H_


class KNumericField : public KTextBox
{
public:
	KNumericField();

	virtual ~KNumericField();
};

#endif

// =========== KPasswordBox.h ===========

/*
	RFC - KPasswordBox.h
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

#ifndef _RFC_KPASSWORDBOX_H_
#define _RFC_KPASSWORDBOX_H_


class KPasswordBox : public KTextBox
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

#ifndef _RFC_KPUSHBUTTON_H_
#define _RFC_KPUSHBUTTON_H_


class KPushButton : public KCheckBox
{
public:
	KPushButton();

	virtual ~KPushButton();
};

#endif

// =========== KRadioButton.h ===========

/*
	RFC - KRadioButton.h
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

#ifndef _RFC_KRADIOBUTTON_H_
#define _RFC_KRADIOBUTTON_H_


class KRadioButton : public KCheckBox
{
public:
	KRadioButton();

	virtual ~KRadioButton();
};

#endif

// =========== KToolTip.h ===========

/*
	RFC - KToolTip.h
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

#ifndef _RFC_KTOOLTIP_H_
#define _RFC_KTOOLTIP_H_


class KToolTip : public KComponent
{
protected:
	HWND attachedCompHWND;

public:
	KToolTip();

	virtual ~KToolTip();

	/**
		parentWindow must be created before you call this method.
		attachedComponent must be added to a window before you call this method.
		do not attach same tooltip into multiple components.
	*/
	virtual void AttachToComponent(KWindow *parentWindow, KComponent *attachedComponent);

	/**
		calling this method has no effect.
	*/
	virtual bool CreateComponent(bool requireInitialMessages = false);

	virtual void SetText(const KString& compText);
};

#endif


// =========== KGroupBox.h ===========

/*
	RFC - KGroupBox.h
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

#ifndef _RFC_KGROUPBOX_H_
#define _RFC_KGROUPBOX_H_


class KGroupBox : public KButton
{
public:
	KGroupBox();

	virtual ~KGroupBox();
};

#endif

// =========== KWindowTypes.h ===========

/*
	RFC - KWindowTypes.h
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

#ifndef _RFC_KWINDOWTYPES_H_
#define _RFC_KWINDOWTYPES_H_


class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void OnClose();

	virtual void OnDestroy();

	virtual ~KHotPluggedDialog();
};

class KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow();

	virtual ~KOverlappedWindow();
};

class KFrame : public KWindow
{
public:
	KFrame();

	virtual ~KFrame();
};

class KDialog : public KWindow
{
public:
	KDialog();

	virtual ~KDialog();
};

class KToolWindow : public KWindow
{
public:
	KToolWindow();

	virtual ~KToolWindow();
};

#endif

// =========== KGlyphButton.h ===========

/*
	RFC - KGlyphButton.h
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

#ifndef _RFC_KGLYPHBUTTON_H_
#define _RFC_KGLYPHBUTTON_H_


class KGlyphButton : public KButton
{
protected:
	KFont *glyphFont;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KGlyphButton();

	virtual ~KGlyphButton();

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);

};


#endif

// =========== KMenuButton.h ===========

/*
	RFC - KMenuButton.h
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

#ifndef _RFC_KMENUBUTTON_H_
#define _RFC_KMENUBUTTON_H_


class KMenuButton : public KButton
{
protected:
	KFont *arrowFont;
	KFont *glyphFont;
	KMenu *buttonMenu;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KMenuButton();

	virtual ~KMenuButton();

	virtual void SetMenu(KMenu *buttonMenu);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void SetGlyph(const wchar_t *glyphChar, KFont *glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual void OnPress();

	virtual bool EventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result);
};


#endif

// =========== rfc.h ===========

/*
	RFC Framework v0.2.6
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

#ifndef _RFC_H_
#define _RFC_H_

#include <windows.h>
#include <commctrl.h>

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
	#include <assert.h>
#endif

#ifdef _MSC_VER
	#pragma comment(lib, "Comctl32.lib")
#endif



LRESULT CALLBACK GlobalWnd_Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GlobalDlg_Proc(HWND, UINT, WPARAM, LPARAM);
unsigned __stdcall GlobalThread_Proc(void*);

/**
	set requireInitialMessages to true to receive initial messages lke WM_CREATE... (installs a hook)
	define "RFC_SINGLE_THREAD_COMP_CREATION" if your app does not create components within multiple threads.
*/
HWND CreateRFCComponent(KComponent* component, bool requireInitialMessages);
bool CreateRFCThread(KThread* thread);

void DoMessagePump(bool handleTabKey = true);

/**
	Important: hInstance is current module HINSTANCE.
	If you are in EXE, then hInstance is HINSTANCE provided by WinMain.
	If you are in DLL, then hInstance is HINSTANCE provided by DllMain or HMODULE of the DLL.
	If you are in Console app, then pass zero.
*/
void InitRFC(HINSTANCE hInstance);
void DeInitRFC();

/** 
	hwnd can be window, custom control, dialog or common control.
	hwnd will be subclassed if it common control or dialog.
*/
void AttachRFCPropertiesToHWND(HWND hwnd, KComponent* component);

int HotPlugAndRunDialogBox(WORD resourceID,HWND parentHwnd,KComponent* component);
HWND HotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);

#define START_RFC_APPLICATION(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitRFC(hInstance); \
	int argc = 0; \
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc); \
	KString **str_argv = (KString**)::malloc(argc * PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new KString(args[i], KString::STATIC_TEXT_DO_NOT_FREE);} \
	AppClass* application = new AppClass(); \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(str_argv, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->AnotherInstanceIsRunning(str_argv, argc); \
		}else{ \
			retVal = application->Main(str_argv, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	for(int i = 0; i < argc; i++){delete str_argv[i];} \
	::DeInitRFC(); \
	::free((void*)str_argv); \
	::GlobalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitRFC(hInstance); \
	AppClass* application = new AppClass(); \
	int retVal = 0; \
	if (application->AllowMultipleInstances()){ \
		retVal = application->Main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->GetApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->AnotherInstanceIsRunning(0, 0); \
		}else{ \
			retVal = application->Main(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	::DeInitRFC(); \
	return retVal; \
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef RFC_NO_MANIFEST
		#ifdef RFC64
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#else
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
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

#define KFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

// generates filter text for KFILE_FILTER("Text Files", "txt") as follows: L"Text Files\0*.txt\0"
#define KFILE_FILTER(desc, ext) L##desc L"\0*." L##ext L"\0"

class InternalDefinitions
{
public:
	static ATOM RFCPropAtom_Component;
	static ATOM RFCPropAtom_OldProc;
};

#ifdef _DEBUG
	#ifdef _MSC_VER
		#define RFC_INIT_VERIFIER _ASSERT_EXPR((KApplication::hInstance != 0), L"##### RFC Framework used before being initialized! Did you forget to call the InitRFC function? Or did you declared RFC class as a global variable? #####")
	#else
		#define RFC_INIT_VERIFIER assert((KApplication::hInstance != 0) && "##### RFC Framework used before being initialized! Did you forget to call the InitRFC function? Or did you declared RFC class as a global variable? #####")
	#endif
#else
	#define RFC_INIT_VERIFIER
#endif

#endif

#endif

