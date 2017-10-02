
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
				#pragma comment(lib,"rfc026.lib")
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