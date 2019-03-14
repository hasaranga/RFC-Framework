
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

#include "config.h"
#include "text/KString.h"
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