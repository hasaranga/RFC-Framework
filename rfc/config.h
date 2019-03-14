
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