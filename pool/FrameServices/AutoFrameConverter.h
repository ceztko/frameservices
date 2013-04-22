#pragma once

#include "FrameConverter.h"

namespace FrameServices
{
    class AutoFrameConverter : public FrameConverter
    {
    public:
        AutoFrameConverter(FrameProvider *source, const FrameProcessor &destination);
    };
}
