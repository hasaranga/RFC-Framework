
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

template<typename T, size_t MaxSize>
class KFixedStack
{
private:
    T data[MaxSize];
    size_t count = 0;

public:
    // Push element, returns false if stack is full
    bool push(const T& item)
    {
        if (count >= MaxSize) {
            K_ASSERT(false, "KFixedStack overflow!");
            return false; // Stack overflow
        }
        data[count++] = item;
        return true;
    }

    // Pop element, returns false if stack is empty
    bool pop()
    {
        if (count == 0) {
            K_ASSERT(false, "KFixedStack underflow!");
            return false; // Stack underflow
        }
        --count;
        return true;
    }

    // Get top element, returns pointer (nullptr if empty)
    const T* top() const
    {
        return (count > 0) ? &data[count - 1] : nullptr;
    }

    // Non-const version
    T* top()
    {
        return (count > 0) ? &data[count - 1] : nullptr;
    }

    // Check if empty
    bool isEmpty() const
    {
        return count == 0;
    }

    // Get current size
    size_t size() const
    {
        return count;
    }

    // Check if full
    bool isFull() const
    {
        return count >= MaxSize;
    }

    // Clear all elements
    void clear()
    {
        count = 0;
    }
};
