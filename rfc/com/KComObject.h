
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
/// Base class that provides standard IUnknown implementation for COM interfaces
/// Handles reference counting and QueryInterface for single interface implementations
/// 
/// Usage: Derive your class using CRTP (Curiously Recurring Template Pattern)
/// Example:
/// class MyAnimationHandler : public KComObject<MyAnimationHandler, IUIAnimationTimerEventHandler>
/// {
/// public:
///     HRESULT STDMETHODCALLTYPE OnPreUpdate() override { return S_OK; }
///     HRESULT STDMETHODCALLTYPE OnPostUpdate() override { return S_OK; }
///     HRESULT STDMETHODCALLTYPE OnRenderingTooSlow(UINT32 fps) override { return S_OK; }
/// };
/// 
/// Template Parameters:
/// - T: Your derived class name (for proper delete)
/// - Interface: The COM interface you're implementing
/// </summary>
template<class T, class Interface>
class KComObject : public Interface
{
public:
    /// <summary>Constructor - initializes reference count to 1</summary>
    KComObject() noexcept : m_refCount(1) {}

    /// <summary>Virtual destructor for proper cleanup</summary>
    virtual ~KComObject() noexcept = default;

    // IUnknown implementation

    /// <summary>Increments the reference count using Windows interlocked functions</summary>
    /// <returns>New reference count</returns>
    ULONG STDMETHODCALLTYPE AddRef() noexcept override
    {
        return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
    }

    /// <summary>Decrements reference count using Windows interlocked functions and deletes object when it reaches zero</summary>
    /// <returns>New reference count</returns>
    ULONG STDMETHODCALLTYPE Release() noexcept override
    {
        ULONG count = static_cast<ULONG>(InterlockedDecrement(&m_refCount));
        if (count == 0)
        {
            delete static_cast<T*>(this);
        }
        return count;
    }

    /// <summary>QueryInterface implementation for single interface</summary>
    /// <param name="riid">Interface ID being requested</param>
    /// <param name="ppvObject">Output pointer to receive the interface</param>
    /// <returns>S_OK if interface is supported, E_NOINTERFACE otherwise</returns>
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) noexcept override
    {
        if (!ppvObject)
            return E_POINTER;

        *ppvObject = nullptr;

        if (riid == __uuidof(IUnknown) || riid == __uuidof(Interface))
        {
            *ppvObject = static_cast<Interface*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

protected:
    /// <summary>Gets current reference count (for debugging)</summary>
    /// <returns>Current reference count</returns>
    ULONG getRefCount() const noexcept { return static_cast<ULONG>(m_refCount); }

private:
    LONG m_refCount;  ///< Reference counter for use with Windows interlocked functions
};
