#include <math.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <Rational.h>

#include "FFEncoder.h"
#include "common-ffmpeg.h"
#include "FrameHandle.h"

#define CODEC_COMPRESSED CODEC_ID_MPEG4
#define COMPRESSED_PIX_FMT PIX_FMT_YUV420P
#define BITRATE 5000000
#define GOPSIZE 12
#define VIDEO_STREAM_ID 0
#define OUTPUT_BUFFER_SIZE 200000

using namespace std;
using namespace FrameServices;
using namespace FrameServices::LibFF;

FFEncoder::FFEncoder(const string &filename, const VideoProps &props,
	bool rawvideo)
{
	_format_ctx = NULL;
    _output_buffer = NULL;
    _codec_ctx = NULL;
    _format_ctx = NULL;
    
    try
    {
	    int rv;

	    _output_format = av_guess_format(NULL, filename.c_str(), NULL);
	    if (_output_format == NULL)
		    throw exception();

	    _format_ctx = avformat_alloc_context();
	    if (_format_ctx == NULL)
		    throw exception();

	    _format_ctx->oformat = _output_format;
	    snprintf(_format_ctx->filename, sizeof(_format_ctx->filename), "%s",
		    filename.c_str());

	    _stream = av_new_stream(_format_ctx, VIDEO_STREAM_ID);
	    if (_stream == NULL)
		    throw exception();

	    _codec_ctx = _stream->codec;
	    _codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;

	    // resolution must be a multiple of two
	    _codec_ctx->width = props.Width;
	    _codec_ctx->height = props.Height;
	    // time base: this is the fundamental unit of time (in seconds) in terms
	    // of which frame timestamps are represented. for fixed-fps content,
	    // timebase should be 1/framerate and timestamp increments should be
	    // identically 1.
	    _codec_ctx->time_base = Convert(props.TimeBase);

		if (rawvideo)
		{
			_codec_ctx->codec_id = CODEC_ID_RAWVIDEO;
            _codec_ctx->pix_fmt = Convert(props.PixelFormat);
            _output_format->flags |= AVFMT_RAWPICTURE;
		}
		else
		{
			_codec_ctx->codec_id = CODEC_COMPRESSED;
			_codec_ctx->pix_fmt = COMPRESSED_PIX_FMT;
			_codec_ctx->bit_rate = BITRATE;
			_codec_ctx->gop_size = GOPSIZE; // emit one intra frame every GOPSIZE frames at most
		}

	    // some formats want stream headers to be separate
	    if(_output_format->flags & AVFMT_GLOBALHEADER)
		    _codec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;

	    // find the video encoder
	    AVCodec *codec = avcodec_find_encoder(_codec_ctx->codec_id);
	    if (codec == NULL)
		    throw exception();

	    // open the codec
	    rv = avcodec_open2(_codec_ctx, codec, NULL);
	    if (rv < 0)
		    throw exception();

	    _output_buffer = NULL;
	    if (!(_output_format->flags & AVFMT_RAWPICTURE)) {
		    // Allocate output buffer
		    // WARNING: API change will be done
		    // buffers passed into lav* can be allocated any way you prefer,
		    // as long as they're aligned enough for the architecture, and
		    // they're freed appropriately (such as using av_free for buffers
		    // allocated with av_malloc)
		    _output_buffer = (uint8_t *)av_malloc(OUTPUT_BUFFER_SIZE);
		    if (_output_buffer == NULL)
		        throw exception();
	    }

	    av_dump_format(_format_ctx, 0, filename.c_str(), 1);

	    // open the output file, if needed
	    if (!(_output_format->flags & AVFMT_NOFILE)) {
		    rv = avio_open(&_format_ctx->pb, filename.c_str(), URL_WRONLY);
		    if (rv < 0)
		        throw exception();
        }

	    rv = avformat_write_header(_format_ctx, NULL);
	    if (rv < 0)
	        throw exception();
    }
    catch (exception &)
    {
        deallocate();
        throw;
    }
}

FFEncoder::~FFEncoder()
{
    deallocate();
}

void FFEncoder::deallocate()
{
    if (_format_ctx != NULL)
	    av_write_trailer(_format_ctx);

	if (_output_buffer != NULL)
	    av_free(_output_buffer);

	if (_codec_ctx != NULL)
	    avcodec_close(_codec_ctx);

	if (_format_ctx != NULL && !(_output_format->flags & AVFMT_NOFILE))
    {
		// close the output file
		avio_close(_format_ctx->pb);
	}

	if (_format_ctx != NULL)
	    avformat_free_context(_format_ctx);
}

void FFEncoder::AppendFrame(const FrameHandle &handle)
{
    AVFrame frame = AVFrameFromHandle(handle);
    AppendFrame(&frame);
}

void FFEncoder::AppendFrame(const FrameHandle &handle, int64_t pts)
{
    AVFrame frame = AVFrameFromHandle(handle);
    frame.pts = pts;
    AppendFrame(&frame);
}

void FFEncoder::AppendFrame(AVFrame *frame)
{
	int rv = 0;

	int out_size = 0;
	if (_output_format->flags & AVFMT_RAWPICTURE)
    {
		// Raw video case. The API will change slightly in the near future for that
		AVPacket packet;
		av_init_packet(&packet);

		if (_codec_ctx->coded_frame->pts != AV_NOPTS_VALUE)
			packet.pts= av_rescale_q(_codec_ctx->coded_frame->pts,
			_codec_ctx->time_base, _stream->time_base);

		packet.flags |= AV_PKT_FLAG_KEY;
		packet.stream_index = VIDEO_STREAM_ID;

        if (_codec_ctx->codec_id == CODEC_ID_RAWVIDEO)
        {
            packet.data = (uint8_t *)frame->data[0];
            packet.size = avpicture_get_size((::PixelFormat)frame->format,
                frame->width, frame->height);
        }
        else // Raw images (JPG, PNG, ...) flow format
        {
            packet.data = (uint8_t *)frame;
            packet.size = sizeof(AVPicture);
        }

		rv = av_interleaved_write_frame(_format_ctx, &packet);
	}
    else
    {
		// encode the image
		out_size = avcodec_encode_video(_codec_ctx, _output_buffer,
			OUTPUT_BUFFER_SIZE, frame);
		// if zero size, it means the image was buffered
		if (out_size > 0) {
			AVPacket packet;
			av_init_packet(&packet);

			if (_codec_ctx->coded_frame->pts != AV_NOPTS_VALUE)
				packet.pts= av_rescale_q(_codec_ctx->coded_frame->pts,
				_codec_ctx->time_base, _stream->time_base);
			if(_codec_ctx->coded_frame->key_frame)
				packet.flags |= AV_PKT_FLAG_KEY;
			packet.stream_index= VIDEO_STREAM_ID;
			packet.data= _output_buffer;
			packet.size= out_size;

			// write the compressed frame in the media file
			rv = av_interleaved_write_frame(_format_ctx, &packet);
		}
	}

	if (rv < 0 || out_size < 0)
		throw exception();
}


int FFEncoder::GetExpectedWidth() const
{
    return _codec_ctx->width;
}

int FFEncoder::GetExpectedHeight() const
{
    return _codec_ctx->height;
}

FrameServices::PixelFormat FFEncoder::GetExpectedPixelFormat() const
{
    return YUV420P_PIX_FMT;
}

Rational FFEncoder::GetTimeBase() const
{
    return Rational(_codec_ctx->time_base.num, _codec_ctx->time_base.den);
}

