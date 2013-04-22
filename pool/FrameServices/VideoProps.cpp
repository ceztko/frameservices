#include "VideoProps.h"

using namespace FrameServices;



VideoProps::VideoProps()
{
    Width = -1;
    Height = -1;
    PixelFormat = ANY_PIX_FMT;
    InterlacedStyle = DONTCARE;
}

bool FrameServices::SourceCompatible(const VideoProps &source,
    const VideoProps &expected)
{
    if (source.Width == expected.Width && source.Height == expected.Height
            && source.PixelFormat == expected.PixelFormat)
        return true;
    else
        return false;
}
