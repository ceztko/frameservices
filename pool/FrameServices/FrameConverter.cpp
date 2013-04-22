#include <exception>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include "common-ffmpeg.h"
#include "FrameConverter.h"

using namespace std;
using namespace FrameServices;

FrameConverter::FrameConverter(FrameProvider *source, const VideoProps &outProps)
    : FrameProcessor(source),
    _dataBuffer(&_destHandle, outProps),
    _outProps(outProps)
{
    _img_convert_ctx = NULL;

    try
    {
        init();
    }
    catch (exception &)
    {
        deallocate();
        throw;
    }
}

FrameConverter::FrameConverter(FrameProvider *source, const FrameProcessor &destination)
    : FrameProcessor(source),
    _dataBuffer(&_destHandle, destination.GetExpectedVideoProps()),
    _outProps(destination.GetExpectedVideoProps())
{
    try
    {
        VideoProps destVideoProps = destination.GetExpectedVideoProps();
        init();
    }
    catch (exception &)
    {
        deallocate();
        throw;
    }
}

void FrameConverter::init()
{
    VideoProps sourceProps = _sourceProvider->GetVideoProps();

    // CHECK-ME: this is a workaround, best is having FrameProcessor (or FrameProvider
    // directly) having always a FrameHandle private copy, doing a default copy of
    // source handle. See TODO
    _destHandle.Width = _outProps.Width;
    _destHandle.Height = _outProps.Height;
    _destHandle.PixelFormat = _outProps.PixelFormat;

    _sourceHeight = sourceProps.Height;

    _img_convert_ctx = sws_getCachedContext(NULL,
        sourceProps.Width, sourceProps.Height,
        Convert(sourceProps.PixelFormat), _outProps.Width, _outProps.Height,
        Convert(_outProps.PixelFormat), SWS_BICUBIC, NULL, NULL, NULL);
    if (_img_convert_ctx == NULL)
	    throw exception();

    // Convert immediately the current frame available from the source
    convertCurrentFrame();
}

void FrameConverter::deallocate()
{
    if (_img_convert_ctx != NULL)
	    sws_freeContext(_img_convert_ctx);
}

FrameConverter::~FrameConverter()
{
    sws_freeContext(_img_convert_ctx);
}

void FrameConverter::convertCurrentFrame()
{
    AVFrame source = AVFrameFromHandle(*_sourceHandle);
    AVFrame dest = AVFrameFromHandle(_destHandle);

    sws_scale(_img_convert_ctx, source.data, source.linesize,
        0, _sourceHeight, dest.data, dest.linesize);

    _destHandle.Pts = _sourceHandle->Pts;
}

FrameHandle * FrameConverter::GetFrameHandle()
{
    return &_destHandle;
}

bool FrameConverter::requestNextFrame()
 {
    if(!_sourceProvider->RequestNextFrame())
        return false;

    convertCurrentFrame();

    return true;
}

void FrameConverter::getVideoProps(VideoProps &props) const
{
    props.Width = _outProps.Width;
    props.Height = _outProps.Height;
    props.PixelFormat = _outProps.PixelFormat;
}

bool FrameConverter::restoreLine()
{
    convertCurrentFrame();

    return true;
}
