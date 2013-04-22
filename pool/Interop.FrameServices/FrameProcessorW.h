#pragma once

#include <FrameProcessor.h>

#include "FrameProviderW.h"

#pragma make_public(::FrameProcessor)

namespace Interop { namespace FrameServices
{
    public ref class FrameProcessor abstract : FrameProvider
    {
    private:
        FrameProvider ^_sourceProvider;
    public:
        FrameProcessor(FrameProvider ^sourceProvider)
        {
            _sourceProvider = sourceProvider;
            sourceProvider->AddProcessor(this);
        }
        ~FrameProcessor()
        {
            if (this->IsDisposed)
                return;

            _sourceProvider->RemoveProcessor(this);
        }
        virtual property FrameDecoder ^ LineDecoder
        {
            FrameDecoder ^ get() override sealed
            {
                return _sourceProvider->LineDecoder;
            }
        }
        property FrameProvider ^ SourceProvider
        {
            FrameProvider ^ get()
            {
                return _sourceProvider;
            }
        }
        property VideoProps ExpectedVideoProps
        {
            VideoProps get()
            {
                Assert();
                ::FrameProcessor *wrapped = this;
                return VideoProps(wrapped->GetExpectedVideoProps());
            }
        }
        virtual operator ::FrameProcessor *() abstract;
    };
} }
