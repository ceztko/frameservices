#pragma once

#include "defines.h"

namespace FrameServices
{
    enum FRAMESERVICES_API PixelFormat
    {
        ANY_PIX_FMT = 0,
        YUV420P_PIX_FMT,
        RGB24_PIX_FMT,
        BGR24_PIX_FMT,
        GRAY8_PIX_FMT,
        ARGB_PIX_FMT,
        ABGR_PIX_FMT,
        BGRA_PIX_FMT,
        RGBA_PIX_FMT
    };
}
