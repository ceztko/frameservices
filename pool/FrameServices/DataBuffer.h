#pragma once

#include "PixelFormat.h"

#include "defines.h"
#include "FrameHandle.h"
#include "VideoProps.h"

namespace FrameServices
{
    class FRAMESERVICES_API DataBuffer
    {
    private:
        FrameHandle *_handle;
        void init(FrameHandle *handle, int width, int heigth, PixelFormat pix_fmt); // Deprecated
    public:
        DataBuffer(FrameHandle *handle);
        DataBuffer(FrameHandle *handle, const VideoProps &properties); // Deprecated
        DataBuffer(FrameHandle *handle, int width, int heigth, PixelFormat pix_fmt); // Deprecated
        ~DataBuffer();
    };
}
