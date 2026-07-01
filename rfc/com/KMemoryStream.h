
#pragma once

#include "../core/CoreModule.h"

// To use read only memory as IStream.
class KMemoryStream : public IStream
{
private:
    LONG m_refCount;
    const BYTE* m_data;
    ULONG m_size;
    ULONG m_position;

public:
    KMemoryStream(const void* data, ULONG size)
        : m_refCount(1), m_data((const BYTE*)data), m_size(size), m_position(0) {
    }

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IStream) || riid == __uuidof(ISequentialStream))
        {
            *ppvObject = this;
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_refCount); }
    STDMETHODIMP_(ULONG) Release() override
    {
        ULONG count = InterlockedDecrement(&m_refCount);
        if (count == 0)
            delete this;
        return count;
    }

    // ISequentialStream methods
    STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead) override
    {
        if (!pv)
            return STG_E_INVALIDPOINTER;

        ULONG bytesToRead = min(cb, m_size - m_position);
        if (bytesToRead > 0)
        {
            memcpy(pv, m_data + m_position, bytesToRead);
            m_position += bytesToRead;
        }

        if (pcbRead)
            *pcbRead = bytesToRead;

        return (bytesToRead == cb) ? S_OK : S_FALSE;
    }

    STDMETHODIMP Write(const void*, ULONG, ULONG*) override { return STG_E_ACCESSDENIED; }

    // IStream methods (minimal implementation)
    STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition) override
    {
        LONG newPos = m_position;
        switch (dwOrigin)
        {
        case STREAM_SEEK_SET: newPos = (LONG)dlibMove.QuadPart; break;
        case STREAM_SEEK_CUR: newPos += (LONG)dlibMove.QuadPart; break;
        case STREAM_SEEK_END: newPos = m_size + (LONG)dlibMove.QuadPart; break;
        default: return STG_E_INVALIDFUNCTION;
        }

        if (newPos < 0 || (ULONG)newPos > m_size)
            return STG_E_INVALIDFUNCTION;

        m_position = newPos;

        if (plibNewPosition)
            plibNewPosition->QuadPart = m_position;

        return S_OK;
    }

    STDMETHODIMP SetSize(ULARGE_INTEGER) override { return STG_E_ACCESSDENIED; }
    STDMETHODIMP CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*) override { return E_NOTIMPL; }
    STDMETHODIMP Commit(DWORD) override { return S_OK; }
    STDMETHODIMP Revert() override { return S_OK; }
    STDMETHODIMP LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) override { return E_NOTIMPL; }
    STDMETHODIMP UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) override { return E_NOTIMPL; }

    STDMETHODIMP Stat(STATSTG* pstatstg, DWORD) override
    {
        if (!pstatstg)
            return STG_E_INVALIDPOINTER;

        ZeroMemory(pstatstg, sizeof(STATSTG));
        pstatstg->type = STGTY_STREAM;
        pstatstg->cbSize.QuadPart = m_size;

        return S_OK;
    }

    STDMETHODIMP Clone(IStream**) override { return E_NOTIMPL; }
};