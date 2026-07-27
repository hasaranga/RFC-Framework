
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

#include <winrt/base.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <windows.ui.composition.interop.h>
#include "KComposition.h"
#include "../gui/GUIModule.h"
#include <type_traits> // std::is_base_of

// adds a DesktopWindowTarget + root ContainerVisual to a window, backed by KComposition::compositor.
// create/destroy can be called as many times as needed (in order) - each create rebuilds the
// target and root visual, each destroy nulls them back out.
// =====================================================
// usage:
//   class MyWindow : public KCompositionWindow<KFrame>
//   {
//   public:
//       void addSomething()
//       {
//           auto sprite = KComposition::compositor.CreateSpriteVisual();
//           sprite.Brush(KComposition::compositor.CreateColorBrush(winrt::Windows::UI::Colors::DodgerBlue()));
// 			 sprite.RelativeSizeAdjustment({ 1.0f, 1.0f });
//			 sprite.Offset({ 0.0f, 0.0f, 0.0f });
//           rootVisual().Children().InsertAtTop(sprite);
//       }
//   };
//
//   MyWindow window;
//   window.create();
//   window.addSomething();
//	=====================================================
// 
// marker base so other mixins (e.g. KAcrylicBackdrop) can require "T must be a KCompositionWindow<...>"
class KCompositionWindowBase {};

// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KCompositionWindow : public T, public KCompositionWindowBase
{
protected:
	winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget compWindowTarget{ nullptr };
	winrt::Windows::UI::Composition::ContainerVisual compRootVisual{ nullptr };

public:
	template<typename... Args>
	KCompositionWindow(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KCompositionWindow() noexcept = default;

	// add child visuals to this to draw on the window.
	winrt::Windows::UI::Composition::ContainerVisual& rootVisual() noexcept { return compRootVisual; }

	virtual bool create(bool requireInitialMessages = false) noexcept override
	{
		if (!__super::create(requireInitialMessages))
			return false;

		namespace abi = ABI::Windows::UI::Composition::Desktop;

		auto interop = KComposition::compositor.as<abi::ICompositorDesktopInterop>();

		compWindowTarget = nullptr; // release any previous target before writing a new one below
		winrt::check_hresult(interop->CreateDesktopWindowTarget(T::compHWND, false,
			reinterpret_cast<abi::IDesktopWindowTarget**>(winrt::put_abi(compWindowTarget))));

		compRootVisual = KComposition::compositor.CreateContainerVisual();
		compRootVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });
		compRootVisual.Offset({ 0.0f, 0.0f, 0.0f });
		compWindowTarget.Root(compRootVisual);

		return true;
	}

	virtual void onDestroy() noexcept override
	{
		compRootVisual = nullptr;
		compWindowTarget = nullptr;

		__super::onDestroy();
	}
};
