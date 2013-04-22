#pragma once

#include "defines.h"
#include "FrameProvider.h"

namespace FrameServices
{
    typedef void (*SetExpectedProps)(VideoProps &props);

    void leaveProviderProps(VideoProps &props);

    class FRAMESERVICES_API FrameProcessor : public FrameProvider
    {
    private:
        VideoProps _expectedProps;
        static void restoreLineRecursive(FrameProcessor *processor);
    protected:
        FrameProvider *_sourceProvider;
        FrameHandle *_sourceHandle;
        virtual bool requestNextFrame() = 0;
        virtual bool restoreLine() = 0;
    public:
        FrameProcessor(FrameProvider **sourceProvider,
            SetExpectedProps setExpectedProps);
        FrameProcessor(FrameProvider *sourceProvider);
        ~FrameProcessor();
        FrameProvider * GetSourceProvider() const;
        const VideoProps & GetExpectedVideoProps() const;
        virtual bool RequestNextFrame();                      /* Sealed */ 
        virtual void RestoreLine();                           /* Sealed */
        virtual FrameDecoder * GetLineDecoder();
    };
}
