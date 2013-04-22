#pragma once

#include <list>

#include "defines.h"
#include "FrameConverter.h"
#include "VideoProps.h"

namespace FrameServices
{
    class FRAMESERVICES_API ExpectedContext
    {
        friend class FrameProvider;
    private:
        FrameProvider *_provider;
        std::list<FrameProvider *> *_converters;
        void SetContext(FrameProvider *provider, const VideoProps &expectedProps);
        void clearConverters();
    public:
        ExpectedContext();
        ExpectedContext(const ExpectedContext &context);
        ~ExpectedContext();
        FrameProvider * GetProvider();
        FrameProvider * GetExpectedProvider();
    };
}
