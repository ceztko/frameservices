#pragma once

#include <VideoProps.h>

#include "Conversion.h"
#include "RationalW.h"

using namespace System;
using namespace System::Drawing;
using namespace FrameServices;

#pragma make_public(::VideoProps)

namespace Interop { namespace FrameServices
{
    public enum class InterlacedStyle
    {
        DONTCARE = 0,
        INTERLACED,
        PROGRESSIVE
    };

    public value struct VideoProps
    {
    public:
        int Width;
        int Height;
        Imaging::PixelFormat PixelFormat;
        InterlacedStyle InterlacedStyle;
        Rational TimeBase;
        Rational AspectRatio;
        static initonly VideoProps Default;
        static VideoProps()
        {
            Default.Width = -1;
            Default.Height = -1;
            Default.PixelFormat = Imaging::PixelFormat::DontCare;
            Default.InterlacedStyle = InterlacedStyle::DONTCARE;
            Default.TimeBase = Rational::Default;
            Default.AspectRatio = Rational::Default;
        }
        VideoProps(const ::VideoProps &properties)
        {
            Width = properties.Width;
            Height = properties.Height;
            PixelFormat = Conversion::FromNative(properties.PixelFormat);
            InterlacedStyle = (Interop::FrameServices::InterlacedStyle)properties.InterlacedStyle;
            TimeBase = Rational(properties.TimeBase);
            AspectRatio = Rational(properties.AspectRatio);
        }
        operator ::VideoProps()
        {
            ::VideoProps ret;
            ret.Height = Height;
            ret.Width = Width;
            ret.PixelFormat = Conversion::ToNative(PixelFormat);
            ret.InterlacedStyle = (::InterlacedStyle)InterlacedStyle;
            ret.TimeBase = TimeBase.operator ::Rational();
            ret.AspectRatio = AspectRatio.operator ::Rational();
            return ret;
        }
    };
} }
