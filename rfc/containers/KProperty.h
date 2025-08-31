
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
#include <functional>

// assignment is not thread safe! the thread which is assigning will run callbacks.
template<typename T>
class KProperty
{
public:
    std::function<void(const T& oldValue, const T& newValue)> onChangeWithOldValue;
    std::function<void(const T& newValue)> onChange;

    KProperty() = default;

    // Perfect forwarding constructor
    template<typename U, typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, KProperty>>>
    KProperty(U&& initial) : value(std::forward<U>(initial)) {}

    // Copy assignment operator
    KProperty& operator=(const T& newValue)
    {
        if (value != newValue) // only trigger if different
        {
            if (onChange)
            {
                value = newValue;
                onChange(value);
            }
            else if (onChangeWithOldValue)
            {
                T oldValue = value;
                value = newValue;
                onChangeWithOldValue(oldValue, value);
            }
        }
        return *this;
    }

    // Move assignment operator
    KProperty& operator=(T&& newValue)
    {
        if (value != newValue) // only trigger if different
        {
            if (onChange)
            {
                value = std::move(newValue);
                onChange(value);
            }
            else if (onChangeWithOldValue)
            {
                T oldValue = std::move(value);
                value = std::move(newValue);
                onChangeWithOldValue(oldValue, value);
            }
        }
        return *this;
    }

    // Copy version of setSilent
    void setSilent(const T& newValue)
    {
        value = newValue;
    }

    // Move version of setSilent
    void setSilent(T&& newValue)
    {
        value = std::move(newValue);
    }

    // Get current value
    const T& get() const { return value; }
    operator const T& () const { return value; } // implicit conversion

private:
    T value{};
};
