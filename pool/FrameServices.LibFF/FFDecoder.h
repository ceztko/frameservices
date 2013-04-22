#pragma once

#include <string>

#include <FrameDecoder.h>
#include <PixelFormat.h>
#include <Rational.h>
#include <FrameHandle.h>
#include <LineStatus.h>

#include "defines.h"
#include "FrameHandle.h"

struct AVPacket;
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;

namespace FrameServices { namespace LibFF
{
    class LIBAV_API FFDecoder : public FrameDecoder
	{
	private:
		AVPacket *_packet;
		FrameHandle _handle;
		AVFormatContext *_format_ctx;
		AVCodecContext *_codec_ctx;
		AVStream *_video_stream;
        void init(const std::string &filename);
        void deallocate();
    protected:
        virtual void setPosition(int64_t position);
        virtual void rewind();
        virtual void getVideoProps(VideoProps &props) const;
	public:
        FFDecoder(const std::string &filename);
        FFDecoder(const std::string &filename, FrameHandle **handle);
		virtual ~FFDecoder();
        virtual bool RequestNextFrame();
        virtual FrameHandle *GetFrameHandle();
        virtual int64_t GetDuration() const;
        virtual int64_t GetFrameCount() const;
        virtual int64_t GetPosition() const;
	};
} }
