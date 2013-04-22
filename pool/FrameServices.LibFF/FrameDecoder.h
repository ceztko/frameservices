#pragma once

#include <stdint.h>

#include <Rational.h>

#include "defines.h"
#include "FrameProvider.h"

namespace Libav
{
    class LIBAV_API FrameDecoder : public FrameProvider
    {
    public:
        virtual bool IsInterlaced() = 0;
        virtual FrameServices::Rational GetTimeBase() = 0;
		virtual FrameServices::Rational GetAspectRatio() = 0;
        virtual int64_t GetDuration() = 0;
        virtual int64_t GetFrameCount() = 0;
    };
}
