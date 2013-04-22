#pragma once

#include <stdint.h>
#include <string>
#include <list>

#include "defines.h"
#include "PixelFormat.h"
#include "FrameHandle.h"
#include "LineStatus.h"
#include "Rational.h"

#include "VideoProps.h"

namespace FrameServices
{
    enum ImageType
    {
        IMAGE_TYPE_JPEG = 0,
        IMAGE_TYPE_PNG,
        IMAGE_TYPE_PPM
    };

    class FrameDecoder;
    class ExpectedContext;
    class FrameProcessor;

    class FRAMESERVICES_API FrameProvider
    {
        friend FrameDecoder;
        friend FrameProcessor;
    public:
        typedef std::list<FrameProcessor *>::iterator iterator;
    private:
        LineStatus _status;
        VideoProps _props;
        std::list<FrameProcessor *> *_processors;
        void addProcessors(FrameProcessor *processor);
        void removeProcessors(FrameProcessor *processor);
        static void issueResetRecursively(FrameProvider *provider);
        static void clearResetRecursively(FrameProvider *provider);
        void clearReset();
    protected:
        void issueReset();
        void issuePropsChanged();
        virtual void getVideoProps(VideoProps &props) const = 0;
    public:
        FrameProvider();
        virtual ~FrameProvider();
        FrameProvider * GetExpectedProvider(ExpectedContext &context,
            const VideoProps &properties);
        void SaveImage(const std::string &filename) const;
        void SaveImage(const std::string &filename, ImageType imageType) const;
        const LineStatus & GetLineStatus() const;
        const VideoProps & GetVideoProps() const;
        iterator begin();
        iterator end();
        virtual bool RequestNextFrame() = 0;
        virtual void RestoreLine() = 0;
        virtual FrameDecoder * GetLineDecoder() = 0;
        virtual FrameHandle * GetFrameHandle() = 0;
    };
}
