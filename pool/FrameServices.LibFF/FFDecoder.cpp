#include <exception>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <Rational.h>
#include <PixelFormat.h>
#include <FrameHandle.h>

#include "FFDecoder.h"
#include "FrameHandle.h"
#include "common-ffmpeg.h"

using namespace std;
using namespace FrameServices;
using namespace FrameServices::LibFF;

FFDecoder::FFDecoder(const string &filename)
{
    init(filename);
}

FFDecoder::FFDecoder(const string &filename, FrameHandle **handle)
{
    init(filename);
	*handle = &_handle;
}

void FFDecoder::init(const string &filename)
{
    _format_ctx = NULL;
    _codec_ctx = NULL;
    _packet = NULL;

    try
    {
        int rv;
        rv = avformat_open_input(&_format_ctx, filename.c_str(), NULL, NULL);
	    if (rv < 0)
		    throw exception();

	    // Retrieve stream information
        rv = avformat_find_stream_info(_format_ctx, NULL);
	    if (rv < 0)
		    throw exception();

	    av_dump_format(_format_ctx, 0, filename.c_str(), false);

	    // Find the first video stream
	    _video_stream = NULL;
	    for(int it = 0; it < (int)_format_ctx->nb_streams; it++)
        {
		    _video_stream = _format_ctx->streams[it];
		    if (_video_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            {
			    // Get a pointer to the codec context for the video stream
			    _codec_ctx = _video_stream->codec;
			    break;
		    }
	    }

	    if(_video_stream == NULL)
		    throw exception();

	    // Find the decoder for the video stream
	    AVCodec *codec = avcodec_find_decoder(_codec_ctx->codec_id);
	    if (codec == NULL)
		    throw exception();

	    rv = avcodec_open2(_codec_ctx, codec, NULL);
	    if (rv < 0)
		    throw exception();

	    // Init the packet that will be used during decoding
        _packet = (AVPacket *)av_malloc(sizeof(AVPacket));
        if (_packet == NULL)
		    throw exception();

	    av_init_packet(_packet);

	    // Immediately request the first frame
	    if (!RequestNextFrame())
		    throw exception();
    }
    catch (exception &)
    {
        deallocate();
        throw;
    }
}

FFDecoder::~FFDecoder()
{
    deallocate();
}

void FFDecoder::deallocate()
{
	// Close the codec
    if (_codec_ctx != NULL)
	    avcodec_close(_codec_ctx);

	// Close the video file
	if (_format_ctx != NULL)
	    av_close_input_file(_format_ctx);

    // Free the packet
    if (_packet != NULL)
	    av_free(_packet);
}

bool FFDecoder::RequestNextFrame()
{
	while (true)
	{
		int rv = av_read_frame(_format_ctx, _packet);

		switch (rv)
		{
		case AVERROR_EOF:
			return false;
		case 0:
			break;
		default:
			throw exception();
		}
		
		if (_packet->stream_index == _video_stream->index)
			break;
	}

	int got_frame;
    AVFrame frame;
    int rv = avcodec_decode_video2(_codec_ctx, &frame, &got_frame, _packet);
    
    // Packet has to be freed after each read
    av_free_packet(_packet);

	if (rv < 0  && got_frame == 0)
		throw exception();

	// Needed. The pts is not set automatically to the packet
    frame.pts = _codec_ctx->pts_correction_last_pts;

    FillHandleFromAVFrame(&_handle, frame);

	return true;
}

FrameHandle * FFDecoder::GetFrameHandle()
{
    return &_handle;
}

void FFDecoder::getVideoProps(VideoProps &props) const
{
    props.Width = _codec_ctx->width;
    props.Height = _codec_ctx->height;
    props.PixelFormat = Convert(_codec_ctx->pix_fmt);
    props.TimeBase = Convert(_video_stream->time_base);
    if (_video_stream->sample_aspect_ratio.num)
	    props.AspectRatio = Convert(_video_stream->sample_aspect_ratio);
    else
	    props.AspectRatio = Convert(_codec_ctx->sample_aspect_ratio);
    props.InterlacedStyle = _handle.Interlaced ? INTERLACED : PROGRESSIVE;
}

int64_t FFDecoder::GetDuration() const
{
	return _format_ctx->duration;
}

int64_t FFDecoder::GetFrameCount() const
{
    return (int64_t)(_format_ctx->duration / (double)AV_TIME_BASE
        * 1 / av_q2d(_codec_ctx->time_base));
}

int64_t FFDecoder::GetPosition() const
{
    // TODO
    return 0;
}

void FFDecoder::setPosition(int64_t position)
{
    int rv = av_seek_frame(_format_ctx, _video_stream->id, position, 0);

	if (rv < 0)
		throw exception();
}

void FFDecoder::rewind()
{
    // CHECK-ME are there more proper ways? The answer can be: no, this is always
    // supported
    setPosition(0);
}
