#pragma once

#include <PixelFormat.h>

using namespace System;
using namespace System::Drawing;
using namespace FrameServices;

namespace Interop { namespace FrameServices
{
    public ref class Conversion
    {
    public:
        static Imaging::PixelFormat FromNative(::PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
            case GRAY8_PIX_FMT:
                return Imaging::PixelFormat::Format8bppIndexed;
            case BGR24_PIX_FMT:
                return Imaging::PixelFormat::Format24bppRgb;
            case BGRA_PIX_FMT:
                return Imaging::PixelFormat::Format32bppPArgb;
            default:
                return Imaging::PixelFormat::DontCare;
            }
        }

        static ::PixelFormat ToNative(Imaging::PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
            case Imaging::PixelFormat::Format8bppIndexed:
                return GRAY8_PIX_FMT;
            case Imaging::PixelFormat::Format24bppRgb:
                return BGR24_PIX_FMT;
            case Imaging::PixelFormat::Format32bppPArgb:
                return BGRA_PIX_FMT;
            default:
                return ANY_PIX_FMT;
            }
        }
    };
} }
