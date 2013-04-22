#include "AutoEncoder.h"

using namespace std;
using namespace FrameServices;
using namespace FrameServices::LibFF;

static void setExpectedProps(VideoProps &props)
{
    props.PixelFormat = YUV420P_PIX_FMT;
}

static VideoProps encoderProps(const FrameProvider &provider, const Rational &timeBase)
{
    VideoProps ret = provider.GetVideoProps();
    ret.TimeBase = timeBase;
    return ret;
}

AutoEncoder::AutoEncoder(const std::string &filename, FrameProvider *srcFrameProvider,
    const Rational &timeBase, bool rawvideo)
    : FrameProcessor(&srcFrameProvider, &setExpectedProps),
    _encoder(filename, encoderProps(*srcFrameProvider, timeBase), rawvideo)
{
    _nextPts = 0;

    _encoder.AppendFrame(*_sourceHandle);

    _nextPts++;

    _enabled = true;
}

FrameHandle * AutoEncoder::GetFrameHandle()
{
    return _sourceProvider->GetFrameHandle();
}

bool AutoEncoder::requestNextFrame()
 {
    if(!_sourceProvider->RequestNextFrame())
        return false;

    if (_enabled)
        encodeFrame();

    return true;
}

void AutoEncoder::getVideoProps(VideoProps &props) const
{
    // VideoProps are left intact
}

bool AutoEncoder::IsEnabled() const
{
    return _enabled;
}

void AutoEncoder::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

bool AutoEncoder::restoreLine()
{
    if (_enabled)
        encodeFrame();
    
    return true;
}

void AutoEncoder::encodeFrame()
{
    _encoder.AppendFrame(*_sourceHandle, _nextPts);
    _nextPts++;
}
