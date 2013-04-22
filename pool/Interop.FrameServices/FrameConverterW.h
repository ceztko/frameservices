#pragma once

#include <FrameConverter.h>

#include "FrameProcessorW.h"

using namespace System;
using namespace FrameServices;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

#pragma make_public(::FrameConverter)

namespace Interop { namespace FrameServices
{
    public ref class FrameConverter : FrameProcessor
    {
    private:
        ::FrameConverter *_converter;
    public:
        FrameConverter(FrameProvider ^source, VideoProps outVideoProps)
            : FrameProcessor(source)
        {
            _converter = new ::FrameConverter(source, outVideoProps);
        }
        ~FrameConverter()
        {
            if (this->IsDisposed)
                return;

            delete _converter;
        }
        virtual operator ::FrameProvider *() override sealed
        {
            return _converter;
        }
        virtual operator ::FrameProcessor *() override sealed
        {
            return _converter;
        }
        operator ::FrameConverter *()
        {
            return _converter;
        }
    };
} }
