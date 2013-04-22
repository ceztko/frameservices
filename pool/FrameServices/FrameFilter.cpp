#include <exception>

extern "C" {
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/avcodec.h>
#include <libavfilter/buffersink.h>
}

#include "FrameFilter.h"
#include "common-ffmpeg.h"

using namespace std;
using namespace FrameServices;

FrameFilter::FrameFilter(const std::string &filter_description,
    FrameProvider *src_frame_provider)
    : FrameProcessor(src_frame_provider), _filter_description(filter_description)
{
    try
    {
        init(src_frame_provider);
    }
    catch (exception &)
    {
        this->~FrameFilter();
        throw;
    }
}

FrameFilter::~FrameFilter()
{
    cleanFilter();

    if (_dest_buffer != NULL)
        delete _dest_buffer;
}

void FrameFilter::init(FrameProvider *src_frame_provider)
{
    reviseFilter();

    // Internal filter of libav allocate each time a different buffer (Sigh...).
    // Allocate another buffer that will have a fixed location
    _dest_buffer = new DataBuffer(&_destHandle, _destHandle.Width, _destHandle.Height,
        _destHandle.PixelFormat);

	// Immediately request the first frame
	if (!requestNextFrame())
		throw exception();
}

void FrameFilter::reviseFilter()
{
    _dest_buffer = NULL;

	_filter_graph = avfilter_graph_alloc();
    if (_filter_graph == NULL)
		throw exception();

	AVFilter *buffer_filter = avfilter_get_by_name("buffer");
	if (buffer_filter == NULL)
        throw exception();

    VideoProps sourceProps = _sourceProvider->GetVideoProps();

	char args[256];
    snprintf(args, sizeof(args), "%d:%d:%d:%d:%d:%d:%d",
        sourceProps.Width, sourceProps.Height,
        Convert(sourceProps.PixelFormat),
        sourceProps.TimeBase.Numerator, sourceProps.TimeBase.Denominator,
        sourceProps.AspectRatio.Numerator, sourceProps.AspectRatio.Denominator);

	int rv;
	
	// Buffer video source: the decoded frames from the codec will be inserted here
	rv = avfilter_graph_create_filter(&_buffer_filter_ctx, buffer_filter, "src", args,
		NULL, _filter_graph);
	if (rv < 0)
        throw exception();

	AVFilter *sink_filter = avfilter_get_by_name("buffersink");
	if (sink_filter == NULL)
        throw exception();

    enum ::PixelFormat destPixFmts[] = { Convert(sourceProps.PixelFormat),
        PIX_FMT_NONE };
    AVBufferSinkParams buffersinkParms;
    buffersinkParms.pixel_fmts = destPixFmts;

	// Null video sink: to terminate the filter chain
	rv = avfilter_graph_create_filter(&_sink_filter_ctx, sink_filter, "out",
		NULL, &buffersinkParms, _filter_graph);
	if (rv < 0)
        throw exception();

	// Endpoints for the filter graph

	AVFilterInOut *buffer_filter_outputs = avfilter_inout_alloc();
	if (buffer_filter_outputs == NULL)
        throw exception();

	buffer_filter_outputs->name = av_strdup("in");
	buffer_filter_outputs->filter_ctx = _buffer_filter_ctx;
	buffer_filter_outputs->pad_idx = 0;
	buffer_filter_outputs->next = NULL;

	AVFilterInOut *sink_filter_inputs = avfilter_inout_alloc();
	if (sink_filter_inputs == NULL)
        throw exception();

	sink_filter_inputs->name = av_strdup("out");
	sink_filter_inputs->filter_ctx = _sink_filter_ctx;
	sink_filter_inputs->pad_idx = 0;
	sink_filter_inputs->next = NULL;

	rv = avfilter_graph_parse(_filter_graph, _filter_description.c_str(),
		&sink_filter_inputs, &buffer_filter_outputs, NULL);
	if (rv < 0)
        throw exception();

	rv = avfilter_graph_config(_filter_graph, NULL);
	if (rv < 0)	
        throw exception();

    _output_link = _sink_filter_ctx->inputs[0];

    AVFrame frame = AVFrameFromHandle(*_sourceHandle);

    // Immediately load the first frame
	rv = av_vsrc_buffer_add_frame(_buffer_filter_ctx, &frame,
        AV_VSRC_BUF_FLAG_OVERWRITE);
	if (rv < 0)
        throw exception();

    _slice_start_pts = _sourceHandle->Pts;
    _next_filtered_pts = _sourceHandle->Pts;
    _cached_frames_before_output = 1;

    // Set handle
    _destHandle.Width = _output_link->w;
    _destHandle.Height = _output_link->h;
    _destHandle.PixelFormat = Convert((::PixelFormat)_output_link->format);
}

void FrameFilter::cleanFilter()
{
    if (_filter_graph != NULL)
	    avfilter_graph_free(&_filter_graph);
}

bool FrameFilter::requestNextFrame()
{
	int rv = avfilter_poll_frame(_output_link);

	if (rv < 0)
		throw exception();

    while (rv == 0)
	{
		if (!_sourceProvider->RequestNextFrame())
			return false;

        AVFrame frame = AVFrameFromHandle(*_sourceHandle);

		rv = av_vsrc_buffer_add_frame(_buffer_filter_ctx, &frame,
            AV_VSRC_BUF_FLAG_OVERWRITE);
		if (rv < 0)
			throw exception();

        _slice_stop_pts = _sourceHandle->Pts;
        _cached_frames_before_output++;

		rv = avfilter_poll_frame(_output_link);
		if (rv < 0)
			throw exception();
	}

    int ready_frames = rv;


	AVFilterBufferRef *filter_buffer;
    av_buffersink_get_buffer_ref(_sink_filter_ctx, &filter_buffer, 0);

	if (filter_buffer == NULL)
		throw exception();

    AVPicture temp_filtered_picture;
    memcpy(temp_filtered_picture.data, filter_buffer->data,
        sizeof(temp_filtered_picture.data));
	memcpy(temp_filtered_picture.linesize, filter_buffer->linesize,
        sizeof(temp_filtered_picture.linesize));

    AVPicture dest = AVPictureFromHandle(_destHandle);

    // The filtered buffer in ffmpeg is volatile, copy it to the fixed one
    av_picture_copy(&dest, &temp_filtered_picture,
        (::PixelFormat)_output_link->format, _output_link->w, _output_link->h);

    // Unlink ref buffer
    avfilter_unref_buffer(filter_buffer);

    _destHandle.Interlaced = filter_buffer->video->interlaced ==  1 ? true : false;
    _destHandle.TopFieldFirst = filter_buffer->video->top_field_first ==  1 ? true : false;
    _destHandle.Pts = _next_filtered_pts;

    uint64_t filtered_frames_pts_step = (uint64_t) ceil (
        (_slice_stop_pts - _slice_start_pts) / (double)_cached_frames_before_output);
    _next_filtered_pts += filtered_frames_pts_step;

    ready_frames--;

    if (ready_frames == 0)
    {
        _slice_start_pts = _sourceHandle->Pts;
        _cached_frames_before_output = 0;
    }

	return true;
}

FrameHandle * FrameFilter::GetFrameHandle()
{
    return &_destHandle;
}

void FrameFilter::getVideoProps(VideoProps &props) const
{
    props.Width = _output_link->w;
    props.Height = _output_link->h;
    props.PixelFormat = Convert((::PixelFormat)_output_link->format);

    // CHECK-ME: Hack. It would be cool to have ffmpeg to detect the correct timebase
    if (_filter_description == "yadif=1:-1")
    {
        props.TimeBase.Numerator = _output_link->time_base.num;
        props.TimeBase.Denominator = _output_link->time_base.den * 2;
    }
    else
    {
        props.TimeBase.Numerator = _output_link->time_base.num;
        props.TimeBase.Denominator = _output_link->time_base.den;
    }

    props.AspectRatio.Numerator = _output_link->sample_aspect_ratio.num;
    props.AspectRatio.Denominator = _output_link->sample_aspect_ratio.den;

    props.InterlacedStyle = _destHandle.Interlaced ? INTERLACED : PROGRESSIVE;
}

bool FrameFilter::restoreLine()
{
    cleanFilter();
    reviseFilter();
    return requestNextFrame();
}
