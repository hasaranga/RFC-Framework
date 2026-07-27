
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

#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.ui.interop.h> // GetWindowIdFromWindow
#include "KCompositionWindow.h"
#include <type_traits> // std::is_base_of

// adds a MicaController backdrop to a composition window. Windows 11 22H2+ only -
// silently does nothing on older systems (IsSupported() check).
// IsInputActive is kept in sync with WM_ACTIVATE so the backdrop dims/brightens like the OS default.
// =====================================================
// usage:
//   class MyWindow : public KMicaBackdrop<KCompositionWindow<KFrame>>
//   {
//   public:
//       MyWindow() { setMicaKind(winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind::BaseAlt); }
//   };
//
//   MyWindow window;
//   window.create();
//	=====================================================
//
// T must be derived from KCompositionWindow<...> (needs compWindowTarget + compHWND)
template <class T,
	typename = typename std::enable_if<std::is_base_of<KCompositionWindowBase, T>::value>::type>
class KMicaBackdrop : public T
{
protected:
	winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController compMicaController{ nullptr };
	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration compBackdropConfig{ nullptr };
	winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind compMicaKind =
		winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind::Base;

public:
	template<typename... Args>
	KMicaBackdrop(Args&&... args) noexcept : T(std::forward<Args>(args)...) {}

	virtual ~KMicaBackdrop() noexcept = default;

	// can be called before create (sets initial kind) or after create (updates the live controller).
	virtual void setMicaKind(winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind kind) noexcept
	{
		compMicaKind = kind;

		if (compMicaController)
			compMicaController.Kind(compMicaKind);
	}

	virtual bool create(bool requireInitialMessages = false) noexcept override
	{
		if (!__super::create(requireInitialMessages))
			return false;

		namespace MUCSB = winrt::Microsoft::UI::Composition::SystemBackdrops;

		if (!MUCSB::MicaController::IsSupported())
			return true; // Windows 11 22H2+ only

		compBackdropConfig = MUCSB::SystemBackdropConfiguration();
		compBackdropConfig.IsInputActive(true);

		compMicaController = MUCSB::MicaController();
		compMicaController.Kind(compMicaKind);
		compMicaController.SetSystemBackdropConfiguration(compBackdropConfig);

		winrt::Microsoft::UI::WindowId windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(T::compHWND);
		compMicaController.SetTarget(windowId, T::compWindowTarget);

		return true;
	}

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override
	{
		if (msg == WM_ACTIVATE && compBackdropConfig)
			compBackdropConfig.IsInputActive(LOWORD(wParam) != WA_INACTIVE);

		return T::windowProc(hwnd, msg, wParam, lParam);
	}

	virtual void onDestroy() noexcept override
	{
		if (compMicaController)
			compMicaController.Close();

		compMicaController = nullptr;
		compBackdropConfig = nullptr;

		__super::onDestroy();
	}
};
