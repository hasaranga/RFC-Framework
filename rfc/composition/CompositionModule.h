
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

// How to use Composition module with project:
// install Microsoft.Windows.CppWinRT nuget package
// then install Microsoft.WindowsAppSDK.Foundation nuget package
// right click on solution then "unload project". it will allow to edit vcxproj file.
// add following to inside of <PropertyGroup Label="Globals"> group
//		<WindowsPackageType>None</WindowsPackageType>
//		<WindowsAppSDKSelfContained>true</WindowsAppSDKSelfContained>
// use precompiled headers to reduce compile time. (pch.h and pch.cpp)

#pragma once

#include <winrt/base.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Microsoft.ui.interop.h> // GetWindowIdFromWindow
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <DispatcherQueue.h>

#include "KComposition.h"
#include "KCompositionWindow.h"
#include "KAcrylicBackdrop.h"
#include "KMicaBackdrop.h"

