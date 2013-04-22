#pragma once

#include <string>

struct AVOutputFormat;
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;

#include <Rational.h>
#include <IFrameEncoder.h>
#include <VideoProps.h>
#include <FrameHandle.h>

#include "defines.h"

namespace FrameServices { namespace LibFF
{
    class LIBAV_API FFEncoder : public IFrameEncoder
	{
	private:
		AVOutputFormat *_output_format;
		AVFormatContext *_format_ctx;
		AVCodecContext *_codec_ctx;
		AVStream *_stream;
		uint8_t *_output_buffer;
        void deallocate();
		void AppendFrame(AVFrame *frame);
	public:
        FFEncoder(const std::string &filename, const VideoProps &props,
			bool rawvideo);
		~FFEncoder();
		virtual void AppendFrame(const FrameHandle &frame);
		virtual void AppendFrame(const FrameHandle &frame, int64_t pts);
        virtual int GetExpectedWidth() const;
        virtual int GetExpectedHeight() const;
        virtual Rational GetTimeBase() const;
        virtual PixelFormat GetExpectedPixelFormat() const;
	};
} }
