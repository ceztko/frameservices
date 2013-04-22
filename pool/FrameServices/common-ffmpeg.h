#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "PixelFormat.h"
#include "Rational.h"
#include "FrameHandle.h"

namespace FrameServices
{
    inline ::PixelFormat Convert(FrameServices::PixelFormat pixelFormat)
    {
        switch(pixelFormat)
        {
        case BGRA_PIX_FMT:
            return PIX_FMT_BGRA;
        case RGBA_PIX_FMT:
            return PIX_FMT_RGBA;
        case ABGR_PIX_FMT:
            return PIX_FMT_ABGR;
        case ARGB_PIX_FMT:
            return PIX_FMT_ARGB;
        case BGR24_PIX_FMT:
            return PIX_FMT_BGR24;
        case RGB24_PIX_FMT:
            return PIX_FMT_RGB24;
        case GRAY8_PIX_FMT:
            return PIX_FMT_GRAY8;
        case YUV420P_PIX_FMT:
            return PIX_FMT_YUV420P;
        default:
            return PIX_FMT_NONE;
        }
    }

    inline FrameServices::PixelFormat Convert(::PixelFormat pixelFormat)
    {
        switch(pixelFormat)
        {
       case PIX_FMT_BGRA:
            return BGRA_PIX_FMT;
        case PIX_FMT_RGBA:
            return RGBA_PIX_FMT;
        case PIX_FMT_ABGR:
            return ABGR_PIX_FMT;
        case PIX_FMT_ARGB:
            return ARGB_PIX_FMT;
        case PIX_FMT_BGR24:
            return BGR24_PIX_FMT;
        case PIX_FMT_RGB24:
            return RGB24_PIX_FMT;
        case PIX_FMT_GRAY8:
            return GRAY8_PIX_FMT;
        case PIX_FMT_YUV420P:
            return YUV420P_PIX_FMT;
        default:
            return ANY_PIX_FMT;
        }
    }

    inline Rational Convert(const AVRational &rational)
    {
        Rational ret(rational.num, rational.den);
        return ret;
    }

    inline AVRational Convert(const Rational &rational)
    {
        AVRational ret;
        ret.num = rational.Numerator;
        ret.den = rational.Denominator;
        return ret;
    }

    inline void FillHandleFromAVFrame(FrameHandle *handle, const AVFrame &frame)
    {
        handle->SetData(frame.data);
        handle->SetLinesize(frame.linesize);

        handle->Width = frame.width;
        handle->Height = frame.height;
        handle->PixelFormat = Convert((::PixelFormat)frame.format);
        handle->Pts = frame.pts;
        handle->Interlaced = frame.interlaced_frame == 1 ? true : false;
        handle->TopFieldFirst = frame.top_field_first == 1 ? true : false;
    }

    inline void FillAVFrameFromHandle(AVFrame *frame, const FrameHandle &handle)
    {
        avcodec_get_frame_defaults(frame);

        handle.CopyDataTo(frame->data);
        handle.CopyLineSizeTo(frame->linesize);

        frame->width = handle.Width;
        frame->height = handle.Height;
        frame->format = (int)Convert(handle.PixelFormat);
        frame->pts = handle.Pts;
        frame->interlaced_frame = handle.Interlaced ? 1 : 0;
        frame->top_field_first = handle.TopFieldFirst ? 1 : 0;
    }

    inline FrameHandle HandleFromAVFrame(const AVFrame &frame)
    {
        FrameHandle handle(frame.data, frame.linesize);

        handle.Width = frame.width;
        handle.Height = frame.height;
        handle.PixelFormat = Convert((::PixelFormat)frame.format);
        handle.Pts = frame.pts;
        handle.Interlaced = frame.interlaced_frame == 1 ? true : false;
        handle.TopFieldFirst = frame.top_field_first == 1 ? true : false;

        return handle;
    }

    inline AVFrame AVFrameFromHandle(const FrameHandle &handle)
    {
        AVFrame frame;
        avcodec_get_frame_defaults(&frame);

        handle.CopyDataTo(frame.data);
        handle.CopyLineSizeTo(frame.linesize);

        frame.width = handle.Width;
        frame.height = handle.Height;
        frame.format = (int)Convert(handle.PixelFormat);
        frame.pts = handle.Pts;
        frame.interlaced_frame = handle.Interlaced ? 1 : 0;
        frame.top_field_first = handle.TopFieldFirst ? 1 : 0;

        return frame;
    }

    inline AVPicture AVPictureFromHandle(const FrameHandle &handle)
    {
        AVPicture picture;

        handle.CopyDataTo(picture.data);
        handle.CopyLineSizeTo(picture.linesize);

        return picture;
    }
}
