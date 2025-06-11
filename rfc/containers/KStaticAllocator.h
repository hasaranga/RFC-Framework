
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

#include <cstddef>
#include <new>
#include <atomic>

#ifndef KSTATIC_POOL_SIZE
    #define KSTATIC_POOL_SIZE 520
#endif

// thread-safe static allocation. (Lock-free)
class KStaticAllocator
{
private:
    static constexpr size_t POOL_SIZE = KSTATIC_POOL_SIZE; // 1MB pool
    static char memory_pool[POOL_SIZE];
    static std::atomic<size_t> current_offset;

public:
    // once allocated, returned buffer will stay until the application exit.
    // returns nullptr if KSTATIC_POOL_SIZE is not enough.
    static void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    
    static void reset();
};

