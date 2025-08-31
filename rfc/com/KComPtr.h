
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

#include <windows.h>
#include <unknwn.h>

/// <summary>
/// Smart pointer class for COM interfaces with automatic reference counting
/// Similar to CComPtr but with lowercase method names and additional "as" method
/// </summary>
template<class T>
class KComPtr
{
public:
    /// <summary>Default constructor - initializes with nullptr</summary>
    KComPtr() noexcept : m_ptr(nullptr) {}

    /// <summary>Constructor from raw pointer - calls AddRef if not null</summary>
    /// <param name="ptr">Raw COM interface pointer</param>
    KComPtr(T* ptr) noexcept : m_ptr(ptr)
    {
        if (m_ptr)
            m_ptr->AddRef();
    }

    /// <summary>Copy constructor - calls AddRef on the copied pointer</summary>
    /// <param name="other">Another KComPtr of same type</param>
    KComPtr(const KComPtr<T>& other) noexcept : m_ptr(other.m_ptr)
    {
        if (m_ptr)
            m_ptr->AddRef();
    }

    /// <summary>Copy constructor from different type - uses QueryInterface</summary>
    /// <param name="other">KComPtr of different but compatible type</param>
    template<class U>
    KComPtr(const KComPtr<U>& other) noexcept : m_ptr(nullptr)
    {
        if (other.get())
        {
            other.get()->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&m_ptr));
        }
    }

    /// <summary>Move constructor - transfers ownership without AddRef/Release</summary>
    /// <param name="other">KComPtr to move from</param>
    KComPtr(KComPtr<T>&& other) noexcept : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    /// <summary>Move constructor from different type - uses QueryInterface then resets source</summary>
    /// <param name="other">KComPtr of different type to move from</param>
    template<class U>
    KComPtr(KComPtr<U>&& other) noexcept : m_ptr(nullptr)
    {
        if (other.get())
        {
            other.get()->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&m_ptr));
            other.reset();
        }
    }

    /// <summary>Destructor - calls Release if pointer is not null</summary>
    ~KComPtr() noexcept
    {
        if (m_ptr)
            m_ptr->Release();
    }

    /// <summary>Assignment from raw pointer - handles AddRef/Release correctly</summary>
    /// <param name="ptr">Raw COM interface pointer</param>
    /// <returns>Reference to this KComPtr</returns>
    KComPtr<T>& operator=(T* ptr) noexcept
    {
        if (m_ptr != ptr)
        {
            if (ptr)
                ptr->AddRef();
            if (m_ptr)
                m_ptr->Release();
            m_ptr = ptr;
        }
        return *this;
    }

    /// <summary>Copy assignment operator - handles AddRef/Release correctly</summary>
    /// <param name="other">Another KComPtr of same type</param>
    /// <returns>Reference to this KComPtr</returns>
    KComPtr<T>& operator=(const KComPtr<T>& other) noexcept
    {
        if (this != &other)
        {
            if (other.m_ptr)
                other.m_ptr->AddRef();
            if (m_ptr)
                m_ptr->Release();
            m_ptr = other.m_ptr;
        }
        return *this;
    }

    /// <summary>Copy assignment from different type - uses QueryInterface</summary>
    /// <param name="other">KComPtr of different but compatible type</param>
    /// <returns>Reference to this KComPtr</returns>
    template<class U>
    KComPtr<T>& operator=(const KComPtr<U>& other) noexcept
    {
        if (m_ptr)
            m_ptr->Release();
        m_ptr = nullptr;

        if (other.get())
        {
            other.get()->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&m_ptr));
        }
        return *this;
    }

    /// <summary>Move assignment operator - transfers ownership without AddRef/Release</summary>
    /// <param name="other">KComPtr to move from</param>
    /// <returns>Reference to this KComPtr</returns>
    KComPtr<T>& operator=(KComPtr<T>&& other) noexcept
    {
        if (this != &other)
        {
            if (m_ptr)
                m_ptr->Release();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    /// <summary>Move assignment from different type - uses QueryInterface then resets source</summary>
    /// <param name="other">KComPtr of different type to move from</param>
    /// <returns>Reference to this KComPtr</returns>
    template<class U>
    KComPtr<T>& operator=(KComPtr<U>&& other) noexcept
    {
        if (m_ptr)
            m_ptr->Release();
        m_ptr = nullptr;

        if (other.get())
        {
            other.get()->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&m_ptr));
            other.reset();
        }
        return *this;
    }

    /// <summary>Arrow operator for calling methods on the COM interface</summary>
    /// <returns>Raw pointer to the COM interface</returns>
    T* operator->() const noexcept
    {
        return m_ptr;
    }

    /// <summary>Dereference operator for accessing the COM interface</summary>
    /// <returns>Reference to the COM interface</returns>
    T& operator*() const noexcept
    {
        return *m_ptr;
    }

    /// <summary>Logical NOT operator - returns true if pointer is null</summary>
    /// <returns>True if the pointer is null, false otherwise</returns>
    bool operator!() const noexcept
    {
        return m_ptr == nullptr;
    }

    /// <summary>Equality comparison with raw pointer</summary>
    /// <param name="ptr">Raw pointer to compare with</param>
    /// <returns>True if pointers are equal</returns>
    bool operator==(T* ptr) const noexcept
    {
        return m_ptr == ptr;
    }

    /// <summary>Inequality comparison with raw pointer</summary>
    /// <param name="ptr">Raw pointer to compare with</param>
    /// <returns>True if pointers are not equal</returns>
    bool operator!=(T* ptr) const noexcept
    {
        return m_ptr != ptr;
    }

    /// <summary>Less than comparison for use in containers</summary>
    /// <param name="other">Another KComPtr to compare with</param>
    /// <returns>True if this pointer is less than other pointer</returns>
    bool operator<(const KComPtr<T>& other) const noexcept
    {
        return m_ptr < other.m_ptr;
    }

    // Core methods
    T* get() const noexcept
    {
        return m_ptr;
    }

    T** getAddressOf() noexcept
    {
        return &m_ptr;
    }

    T* const* getAddressOf() const noexcept
    {
        return &m_ptr;
    }

    void reset() noexcept
    {
        if (m_ptr)
        {
            m_ptr->Release();
            m_ptr = nullptr;
        }
    }

    void reset(T* ptr) noexcept
    {
        if (m_ptr != ptr)
        {
            if (m_ptr)
                m_ptr->Release();
            m_ptr = ptr;
        }
    }

    T* detach() noexcept
    {
        T* ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }

    void attach(T* ptr) noexcept
    {
        if (m_ptr)
            m_ptr->Release();
        m_ptr = ptr;
    }

    void swap(KComPtr<T>& other) noexcept
    {
        T* temp = m_ptr;
        m_ptr = other.m_ptr;
        other.m_ptr = temp;
    }

    // QueryInterface wrapper - the "as" method
    template<class U>
    KComPtr<U> as() const noexcept
    {
        KComPtr<U> result;
        if (m_ptr)
        {
            m_ptr->QueryInterface(__uuidof(U), reinterpret_cast<void**>(result.getAddressOf()));
        }
        return result;
    }

    // Alternative QueryInterface method
    template<class U>
    HRESULT queryInterface(KComPtr<U>& result) const noexcept
    {
        result.reset();
        if (!m_ptr)
            return E_POINTER;

        return m_ptr->QueryInterface(__uuidof(U), reinterpret_cast<void**>(result.getAddressOf()));
    }

    // For use with functions that return interface pointers
    T** releaseAndGetAddressOf() noexcept
    {
        reset();
        return &m_ptr;
    }

    // Conversion operator for QueryInterface - returns void**
    operator void** () noexcept
    {
        reset(); // Always reset before receiving new interface
        return reinterpret_cast<void**>(&m_ptr);
    }

    // Alternative explicit method for QueryInterface parameter
    void** asVoidPP() noexcept
    {
        reset();
        return reinterpret_cast<void**>(&m_ptr);
    }

    operator T* () noexcept
    {
        return m_ptr;
    }

private:
    T* m_ptr;
};


// Global comparison operators
template<class T, class U>
bool operator==(const KComPtr<T>& left, const KComPtr<U>& right) noexcept
{
    return left.get() == right.get();
}

template<class T, class U>
bool operator!=(const KComPtr<T>& left, const KComPtr<U>& right) noexcept
{
    return left.get() != right.get();
}

template<class T>
bool operator==(const KComPtr<T>& left, std::nullptr_t) noexcept
{
    return left.get() == nullptr;
}

template<class T>
bool operator!=(const KComPtr<T>& left, std::nullptr_t) noexcept
{
    return left.get() != nullptr;
}

template<class T>
bool operator==(std::nullptr_t, const KComPtr<T>& right) noexcept
{
    return nullptr == right.get();
}

template<class T>
bool operator!=(std::nullptr_t, const KComPtr<T>& right) noexcept
{
    return nullptr != right.get();
}