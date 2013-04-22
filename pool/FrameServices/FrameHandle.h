#pragma once

#include <stdint.h>

#include "defines.h"
#include "PixelFormat.h"

namespace FrameServices
{
    // Interchange handle for frame buffers
    struct FRAMESERVICES_API FrameHandle
    {
    public:
        uint8_t * Data[4];
        int LineSize[4];
        int Width;
        int Height;
        PixelFormat PixelFormat;
        bool Interlaced;
        bool TopFieldFirst;
        int64_t Pts;
        FrameHandle();
        FrameHandle(uint8_t* const data[], const int linesize[]);
        void SetData(uint8_t* const data[]);
        void SetLinesize(const int linesize[]);
        void CopyDataTo(uint8_t* data[]) const;
        void CopyLineSizeTo(int linesize[]) const;
    };
}
