
/*
    Copyright (C) 2013-2025 CrownSoft

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
#include <stdio.h>

// shows current thread stack usage.
class KStackInfo
{
    static void _printUsage(size_t used, size_t total)
    {
        char buf[256];
        sprintf_s(buf, sizeof(buf),
            "Stack: %zu/%zu bytes (%.1f%% used, %.1f MB total)\n",
            used, total, (double)used / total * 100.0, total / (1024.0 * 1024.0));

        ::OutputDebugStringA(buf);
    }

public:
    static size_t getTotalStackSize()
    {
        ULONG_PTR low, high;
        ::GetCurrentThreadStackLimits(&low, &high);
        return high - low;
    }

    static size_t getCurrentStackUsage()
    {
        ULONG_PTR low, high;
        ::GetCurrentThreadStackLimits(&low, &high);

        volatile char dummy;
        void* currentSP = (void*)&dummy;

        return high - (ULONG_PTR)currentSP;
    }

    static void showStackInfo()
    {    
        size_t total = KStackInfo::getTotalStackSize();
        size_t used = KStackInfo::getCurrentStackUsage();

        KStackInfo::_printUsage(used, total);
    }
};
