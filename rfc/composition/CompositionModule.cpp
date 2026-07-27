
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

#include "CompositionModule.h"

class RFC_CompositionModule
{
public:
	static winrt::Windows::System::DispatcherQueueController dispatcherQueueController;

	static void createDispatcherQueueForCurrentThread()
	{
		namespace abi = ABI::Windows::System;

		if (dispatcherQueueController == nullptr)
		{
			DispatcherQueueOptions options
			{
				sizeof(DispatcherQueueOptions), /* dwSize */
				DQTYPE_THREAD_CURRENT,          /* threadType */
				DQTAT_COM_NONE                  /* apartmentType. valid only if threadType is DQTYPE_THREAD_DEDICATED */
			};

			winrt::check_hresult(CreateDispatcherQueueController(options,
				reinterpret_cast<abi::IDispatcherQueueController**>(winrt::put_abi(RFC_CompositionModule::dispatcherQueueController))));
		}
	}

	static bool rfcModuleInit() noexcept
	{
		winrt::init_apartment(winrt::apartment_type::single_threaded);
		RFC_CompositionModule::createDispatcherQueueForCurrentThread();
		KComposition::compositor = winrt::Windows::UI::Composition::Compositor();
		return true;
	}

	static void rfcModuleFree() noexcept
	{
		KComposition::compositor = nullptr;

		if (dispatcherQueueController)
			dispatcherQueueController.ShutdownQueueAsync();

		dispatcherQueueController = nullptr;
	}
};

winrt::Windows::System::DispatcherQueueController RFC_CompositionModule::dispatcherQueueController{ nullptr };

REGISTER_RFC_MODULE(4, RFC_CompositionModule)