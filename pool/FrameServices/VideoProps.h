#pragma once

#include "defines.h"
#include "Rational.h"
#include "PixelFormat.h"

namespace FrameServices
{
    enum FRAMESERVICES_API InterlacedStyle
    {
        DONTCARE = 0,
        INTERLACED,
        PROGRESSIVE
    };

    struct FRAMESERVICES_API VideoProps
    {
    public:
        int Width;
        int Height;
        PixelFormat PixelFormat;
        InterlacedStyle InterlacedStyle;
        Rational TimeBase;
        Rational AspectRatio;

        VideoProps();
    };

    FRAMESERVICES_API bool SourceCompatible(const VideoProps &source,
        const VideoProps &expected);
}
