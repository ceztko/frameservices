#include "SampleGrabberCB.h"

using namespace FrameServices::DirectShow;

#define FLIPREMOVEMEASSOONASPOSSIBLE

SampleGrabberCB::SampleGrabberCB(DSDecoder *dsDecoder)
{
    _dsDecoder = dsDecoder;
}

// Fake out any COM ref counting
STDMETHODIMP_(ULONG) SampleGrabberCB::AddRef() { return 2; }
STDMETHODIMP_(ULONG) SampleGrabberCB::Release() { return 1; }

// Fake out any COM QI'ing
STDMETHODIMP SampleGrabberCB::QueryInterface(REFIID riid, void ** ppv)
{
    if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
    {
        *ppv = (void *) static_cast<ISampleGrabberCB*>(this);
        return NOERROR;
    }    

    return E_NOINTERFACE;
}

// We don't implement this one
STDMETHODIMP SampleGrabberCB::SampleCB(double sampleTime, IMediaSample * sample)
{
    return 0;
}

STDMETHODIMP SampleGrabberCB::BufferCB(double sampleTime, BYTE *buffer,
                                        long bufferSize)
{
#ifdef FLIPREMOVEMEASSOONASPOSSIBLE
    int width = _dsDecoder->_width;
    int height = _dsDecoder->_height;
    for (int it = 0; it < height; it++)
    {
        int offsetSource = (height - 1 - it) * width * 3;
        int offsetDestination = it * width * 3;
        memcpy(&_dsDecoder->_handle.Data[0][offsetDestination], &buffer[offsetSource], width * 3);
    }
#else
    memcpy(_dsDecoder->_handle.Data[0], buffer, bufferSize);
#endif

    // Pause immediately the media
    _dsDecoder->_mediaControl->Pause();

    return -1;
}
