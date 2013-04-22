#pragma once

#include "defines.h"
#include "FrameProcessor.h"
#include "DataBuffer.h"

struct SwsContext;
struct AVFrame;

namespace FrameServices
{
    typedef void (*BufferProvider)(const FrameHandle &handle, void *opaque);

    class FRAMESERVICES_API FrameConverter : public FrameProcessor
    {
    private:
        FrameHandle _destHandle;
        DataBuffer _dataBuffer;
		SwsContext *_img_convert_ctx;
        VideoProps _outProps;
        int _sourceHeight;
        void init();
        void convertCurrentFrame();
        void deallocate();
    protected:
        FrameConverter(FrameProvider *source, const FrameProcessor &destination);
        virtual bool restoreLine();
        virtual bool requestNextFrame();
    public:
        FrameConverter(FrameProvider *source, const VideoProps &outProps);
        ~FrameConverter();
        virtual FrameHandle * GetFrameHandle();
        void getVideoProps(VideoProps &props) const;
    };
}
