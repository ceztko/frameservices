#include <exception>
#include <atlstr.h>
#include <cmath>

#include "stdafx.h"

#include "DSDecoder.h"
#include "SampleGrabberCB.h"

using namespace std;
using namespace FrameServices;
using namespace FrameServices::DirectShow;

#define N_CHANNELS 3

static HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir,
    IPin **ppPin);
static HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
static HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, 
    IBaseFilter *pDest);

template <class T>
static void SafeRelease(T **ppT);

DSDecoder::DSDecoder(const std::string &filename)
{
    _sampleGrabberCB = NULL;
	_sampleGrabber = NULL;
    _graphBuilder = NULL;
    _mediaEvent = NULL;
    _mediaSeeking = NULL;
    _mediaControl = NULL;
    _mediaFilter = NULL;
    _graphBuilder = NULL;

	IBaseFilter *sourceFilter = NULL;
	IBaseFilter *grabberFilter = NULL;
	IEnumPins *enumPins = NULL;
	IBaseFilter *nullFilter = NULL;

    // Convert utf-8 std::string to wide-char string
    CA2W filenameW(filename.c_str(), CP_UTF8);

    try
    {
        _sampleGrabberCB = new SampleGrabberCB(this);

        HRESULT hr;

	    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&_graphBuilder));
	    if (FAILED(hr))
            throw exception();

	    hr = _graphBuilder->QueryInterface(IID_PPV_ARGS(&_mediaFilter));
	    if (FAILED(hr))
            throw exception();

	    hr = _graphBuilder->QueryInterface(IID_PPV_ARGS(&_mediaControl));
	    if (FAILED(hr))
            throw exception();

        hr = _graphBuilder->QueryInterface(IID_PPV_ARGS(&_mediaSeeking));
	    if (FAILED(hr))
            throw exception();

        hr = _graphBuilder->QueryInterface(IID_PPV_ARGS(&_mediaEvent));
	    if (FAILED(hr))
            throw exception();

	    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&grabberFilter));
	    if (FAILED(hr))
            throw exception();

	    hr = _graphBuilder->AddFilter(grabberFilter, L"Sample Grabber");
	    if (FAILED(hr))
            throw exception();

	    hr = grabberFilter->QueryInterface(IID_PPV_ARGS(&_sampleGrabber));
	    if (FAILED(hr))
            throw exception();
    	
        // Set the desidered media type for the grabber
	    AM_MEDIA_TYPE mediaType;
	    ZeroMemory(&mediaType, sizeof(AM_MEDIA_TYPE));
	    mediaType.majortype = MEDIATYPE_Video;
	    mediaType.subtype = MEDIASUBTYPE_RGB24;
	    hr = _sampleGrabber->SetMediaType(&mediaType);
	    if (FAILED(hr))
            throw exception();
    	
        hr = _graphBuilder->AddSourceFilter(filenameW, L"Source", &sourceFilter);
	    if (FAILED(hr))
            throw exception();
    	
	    hr = sourceFilter->EnumPins(&enumPins);
	    if (FAILED(hr))
            throw exception();

        // Have no idea what this does
        IPin *pin;
        while (S_OK == enumPins->Next(1, &pin, NULL))
        {
            hr = ConnectFilters(_graphBuilder, pin, grabberFilter);
            SafeRelease(&pin);
            if (SUCCEEDED(hr))
                break;
        }

        if (FAILED(hr))
            throw exception();

        // Prepare a null filter termination
        hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&nullFilter));
	    if (FAILED(hr))
            throw exception();

	    hr = _graphBuilder->AddFilter(nullFilter, L"Null Filter");
	    if (FAILED(hr))
            throw exception();

	    hr = ConnectFilters(_graphBuilder, grabberFilter, nullFilter);
	    if (FAILED(hr))
            throw exception();

	    hr = _sampleGrabber->SetOneShot(FALSE);
	    if (FAILED(hr))
            throw exception();

	    hr = _sampleGrabber->SetBufferSamples(FALSE);
	    if (FAILED(hr))
            throw exception();

	    hr = _sampleGrabber->SetCallback(_sampleGrabberCB, 1);
	    if (FAILED(hr))
            throw exception();

	    hr = _sampleGrabber->GetConnectedMediaType(&mediaType);
	    if (FAILED(hr))
            throw exception();

        // If couldn't find a video stream
        if ((mediaType.formattype == FORMAT_VideoInfo) && (mediaType.pbFormat != NULL))
        {
            VIDEOINFOHEADER *videoInfoHeader = (VIDEOINFOHEADER*)mediaType.pbFormat;
            _width = videoInfoHeader->bmiHeader.biWidth;
            _height = videoInfoHeader->bmiHeader.biHeight;
            _fps = (int)(10e6  / videoInfoHeader->AvgTimePerFrame);
        }
        else 
            throw exception();

        // Remove the default syncronization to video time-base: decode as fast as possible
        hr = _mediaFilter->SetSyncSource(NULL);
        if (hr != S_OK)
            throw exception();

        _handle.LineSize[0] = _width * N_CHANNELS;
        _handle.Data[0] = (uint8_t *)malloc(_width * _height * N_CHANNELS);

        // Immediately request the first frame
         RequestNextFrame();
    }
    catch (exception &)
    {
        SafeRelease(&grabberFilter);
        SafeRelease(&sourceFilter);
        SafeRelease(&enumPins);
        SafeRelease(&nullFilter);

        deallocate();

        throw;
    }
}

void DSDecoder::deallocate()
{
	// the following seems to be needed to avoid crashes in some unstopped graphs
	_sampleGrabber->SetCallback(NULL, 1);

	SafeRelease(&_sampleGrabber);
    SafeRelease(&_mediaEvent);
    SafeRelease(&_mediaSeeking);
    SafeRelease(&_mediaControl);
    SafeRelease(&_mediaFilter);
    SafeRelease(&_graphBuilder);

    if (_handle.Data[0] != NULL)
        free(_handle.Data[0]);

    if (_sampleGrabberCB != NULL)
        delete _sampleGrabberCB;
}

DSDecoder::~DSDecoder()
{
    deallocate();
}

bool DSDecoder::RequestNextFrame()
{
    HRESULT hr = _mediaControl->Run(); // Run the graph.

    if (FAILED(hr))
        throw exception();

    long evCode = -1;
    _mediaEvent->WaitForCompletion(INFINITE, &evCode);

    // Detected end of the stream
    if (evCode == EC_COMPLETE)
        return false;

	return true;
}

void DSDecoder::rewind()
{
    setPosition(0);
}

FrameHandle * DSDecoder::GetFrameHandle()
{
    return &_handle;
}

void DSDecoder::getVideoProps(VideoProps &props) const
{
    props.Width = _width;
    props.Height = _height;
    props.PixelFormat = BGR24_PIX_FMT;
    props.TimeBase = Rational(1, _fps);
}

int64_t DSDecoder::GetPosition() const
{
    throw exception();
}

void DSDecoder::setPosition(int64_t position)
{
    HRESULT	hr = _mediaSeeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning,
        NULL, AM_SEEKING_NoPositioning);

	if (FAILED(hr))
        throw exception();
}

int64_t DSDecoder::GetDuration() const
{
    // CHECK-ME
    throw exception();
}

int64_t DSDecoder::GetFrameCount() const
{
    // CHECK-ME
    int64_t duration;
    _mediaSeeking->GetDuration(&duration);
    return duration / pow(10.0, 6.0) * _fps;
}

HRESULT GetUnconnectedPin(
    IBaseFilter *pFilter,   // Pointer to the filter.
    PIN_DIRECTION PinDir,   // Direction of the pin to find.
    IPin **ppPin)           // Receives a pointer to the pin.
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;
            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // Did not find a matching pin.
    return E_FAIL;
}

HRESULT ConnectFilters(
    IGraphBuilder *pGraph, // Filter Graph Manager.
    IPin *pOut,            // Output pin on the upstream filter.
    IBaseFilter *pDest)    // Downstream filter.
{
    if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // Find an input pin on the downstream filter.
    IPin *pIn = 0;
    HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (FAILED(hr))
    {
        return hr;
    }
    // Try to connect them.
    hr = pGraph->Connect(pOut, pIn);
    pIn->Release();
    return hr;
}


HRESULT ConnectFilters(
    IGraphBuilder *pGraph, 
    IBaseFilter *pSrc, 
    IBaseFilter *pDest)
{
    if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // Find an output pin on the first filter.
    IPin *pOut = 0;
    HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (FAILED(hr)) 
    {
        return hr;
    }
    hr = ConnectFilters(pGraph, pOut, pDest);
    pOut->Release();
    return hr;
}

template <class T>
void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}
