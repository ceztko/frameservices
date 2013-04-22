#include "FrameProviderW.h"
#include "FrameDecoderW.h"
#include "FrameProcessorW.h"
#include "FrameConverterW.h"
#include "FrameFilterW.h"

#define FILTER_DESC "yadif=1:-1"

namespace Interop { namespace FrameServices
{
    void FrameProvider::Close()
    {
        List<FrameProcessor ^> processorsToDelete =
            gcnew List<FrameProcessor ^>(_processors);

        for each(FrameProcessor ^processor in processorsToDelete)
            processor->Close();

        delete this;
    }

    void FrameProvider::CloseLine()
    {
        this->LineDecoder->Close();
    }

    FrameProvider ^ FrameProvider::GetExpectedProvider(VideoProps expected)
    {
        FrameProvider ^current = this;

        VideoProps props = this->Properties;

        if (expected.InterlacedStyle == InterlacedStyle::PROGRESSIVE
                && props.InterlacedStyle == InterlacedStyle::INTERLACED)
            current = gcnew FrameFilter(FILTER_DESC, current);

        bool sourceCompatible = isSourceCompatible(props, expected);
        if (!sourceCompatible)
            current = gcnew FrameConverter(current, expected);

        return current;
    }
} }
