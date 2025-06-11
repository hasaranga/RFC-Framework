
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

#include "KStaticAllocator.h"
#include "../core/CoreModule.h"

// Static member definitions
char KStaticAllocator::memory_pool[KStaticAllocator::POOL_SIZE];
std::atomic<size_t> KStaticAllocator::current_offset{ 0 };

void* KStaticAllocator::allocate(size_t size, size_t alignment)
{
    size_t old_offset, new_offset;

    do {
        old_offset = current_offset.load(std::memory_order_acquire);

        // Align the offset
        size_t aligned_offset = (old_offset + alignment - 1) & ~(alignment - 1);

        if (aligned_offset + size > POOL_SIZE)
        {
            K_ASSERT(false, "KStaticAllocator KSTATIC_POOL_SIZE not enough!");
            return nullptr; // Pool exhausted
        }

        new_offset = aligned_offset + size;

    } while (!current_offset.compare_exchange_weak(
        old_offset, new_offset,
        std::memory_order_release,
        std::memory_order_acquire));

    // Calculate the actual aligned offset for the successful allocation
    size_t final_aligned_offset = (old_offset + alignment - 1) & ~(alignment - 1);
    return memory_pool + final_aligned_offset;
}

void KStaticAllocator::reset()
{
    current_offset.store(0, std::memory_order_release);
}