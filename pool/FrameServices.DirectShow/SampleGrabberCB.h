#pragma once

#include "stdafx.h"
#include "DSDecoder.h"

namespace FrameServices { namespace DirectShow
{
    class SampleGrabberCB : public ISampleGrabberCB 
    {
    private:
	    DSDecoder *_dsDecoder;
    public:
	    SampleGrabberCB(DSDecoder *dsDecoder);
        virtual STDMETHODIMP_(ULONG) AddRef();
        virtual STDMETHODIMP_(ULONG) Release();
        virtual STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
        virtual STDMETHODIMP SampleCB(double sampleTime, IMediaSample *sample );
        virtual STDMETHODIMP BufferCB(double dblSampleTime, BYTE * buffer,
            long bufferSize);
    };
} }
