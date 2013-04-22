#pragma once

#include <string>

#include <FrameDecoder.h>

#include "defines.h"

#pragma warning(push)
#pragma warning(disable:4251)

struct IGraphBuilder;
struct IMediaControl;
struct IMediaEventEx;
struct IMediaSeeking;
struct IMediaFilter;
struct ISampleGrabber;

namespace FrameServices { namespace DirectShow
{
    class SampleGrabberCB;

    class FRAMESERVICESDIRECTSHOW_API DSDecoder : public FrameDecoder
	{
        friend class SampleGrabberCB;
	private:
        LineStatus _status;
        FrameHandle _handle;
        SampleGrabberCB *_sampleGrabberCB;
	    IGraphBuilder *_graphBuilder;
		ISampleGrabber *_sampleGrabber;
        IMediaFilter * _mediaFilter;
	    IMediaControl *_mediaControl;
        IMediaSeeking *_mediaSeeking;
	    IMediaEventEx *_mediaEvent;
        int _width;
        int _height;
        int _fps;
        void deallocate();
    protected:
        virtual void setPosition(int64_t position);
        virtual void rewind();
        virtual void getVideoProps(VideoProps &props) const;
	public:
        DSDecoder(const std::string &filename);
		virtual ~DSDecoder();
        virtual bool RequestNextFrame();
        virtual FrameHandle * GetFrameHandle();
        virtual int64_t GetDuration() const;
        virtual int64_t GetFrameCount() const;
        virtual int64_t GetPosition() const;
	};
} }
