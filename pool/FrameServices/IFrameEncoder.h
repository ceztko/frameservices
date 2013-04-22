#pragma once

#include <stdint.h>

#include "defines.h"
#include "PixelFormat.h"
#include "FrameHandle.h"
#include "Rational.h"

namespace FrameServices
{
    class FRAMESERVICES_API IFrameEncoder
    {
    public:
        ~IFrameEncoder();
        virtual void AppendFrame(const FrameHandle &frame) = 0;
        virtual void AppendFrame(const FrameHandle &frame, int64_t pts) = 0;
        virtual int GetExpectedWidth() const = 0;
        virtual int GetExpectedHeight() const = 0;
        virtual PixelFormat GetExpectedPixelFormat() const = 0;
        virtual Rational GetTimeBase() const = 0;
    };
}
