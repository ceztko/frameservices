#pragma once

#include <string>

#include <stdint.h>

#include "defines.h"
#include "FrameProvider.h"
#include "FrameProcessor.h"
#include "Rational.h"
#include "FrameHandle.h"
#include "LineStatus.h"
#include "DataBuffer.h"

struct AVFilterGraph;
struct AVFilterContext;
struct AVFilterContext;
struct AVFilterLink;

namespace FrameServices
{
    class FRAMESERVICES_API FrameFilter : public FrameProcessor
	{
	private:
        AVFilterGraph *_filter_graph;
		AVFilterContext *_buffer_filter_ctx;
		AVFilterContext *_sink_filter_ctx;
        AVFilterLink *_output_link;
        std::string _filter_description;
		FrameHandle _destHandle;
        DataBuffer *_dest_buffer;
        uint64_t _slice_start_pts;
        uint64_t _slice_stop_pts;
        int _cached_frames_before_output;
        uint64_t _next_filtered_pts;
	    void init(FrameProvider *src_frame_provider);
        void reviseFilter();
        void cleanFilter();
        void setVideoProps();
    protected:
        virtual bool restoreLine();
        virtual bool requestNextFrame();
        virtual void getVideoProps(VideoProps &props) const;
	public:
		FrameFilter(const std::string &filter_description,
            FrameProvider *src_frame_provider);
		~FrameFilter();
        virtual FrameHandle * GetFrameHandle();
	};
}
