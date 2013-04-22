#pragma once

#include <PixelFormat.h>

#include "defines.h"
#include "Frame.h"

namespace Libav
{
    class LIBAV_API FrameProvider
    {
    public:
        virtual ~FrameProvider();
        virtual bool RequestNextFrame() = 0;
        virtual Frame * GetFrameHandle() = 0;
		virtual FrameServices::PixelFormat GetPixelFormat() = 0;
		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;
    };
}
