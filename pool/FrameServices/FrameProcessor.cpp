#include <exception>

#include "FrameProcessor.h"
#include "FrameConverter.h"
#include "AutoFrameConverter.h"
#include "LineStatus.h"

using namespace FrameServices;
using namespace std;

FrameProcessor::FrameProcessor(FrameProvider **sourceProvider,
        SetExpectedProps setExpectedProps)
    : _expectedProps((*sourceProvider)->GetVideoProps())
{
    // Set actual expected props 
    setExpectedProps(_expectedProps);

    VideoProps sourceProps = (*sourceProvider)->GetVideoProps();
    if (SourceCompatible(sourceProps, _expectedProps))
    {
        _sourceProvider = *sourceProvider;
    }
    else
    {
        _sourceProvider = new AutoFrameConverter(*sourceProvider, *this);
        *sourceProvider = _sourceProvider;
    }

    _sourceHandle = _sourceProvider->GetFrameHandle();
    _sourceProvider->addProcessors(this);

    // By default set video properties same as the actual source provider
    _props = _sourceProvider->GetVideoProps();
}

FrameProcessor::FrameProcessor(FrameProvider *sourceProvider)
    : _expectedProps(sourceProvider->GetVideoProps())
{
    _sourceProvider = sourceProvider;
    _sourceHandle = _sourceProvider->GetFrameHandle();
    sourceProvider->addProcessors(this);

    // By default set video properties same as the actual source provider
    _props = _sourceProvider->GetVideoProps();
}

FrameProcessor::~FrameProcessor()
{
    _sourceProvider->removeProcessors(this);
    if (dynamic_cast<AutoFrameConverter *>(_sourceProvider))
        delete _sourceProvider;
}

FrameDecoder * FrameProcessor::GetLineDecoder()
{
    return _sourceProvider->GetLineDecoder();
}

const VideoProps & FrameProcessor::GetExpectedVideoProps() const
{
    return _expectedProps;
}

bool FrameProcessor::RequestNextFrame()
{
    const LineStatus &status = GetLineStatus();

    if (status.Resetted)
        throw exception("Line has not been restored");

    return requestNextFrame();
}

void FrameProcessor::RestoreLine()
{
    restoreLineRecursive(this);
}

void FrameProcessor::restoreLineRecursive(FrameProcessor *processor)
{
    FrameProvider *currentSource = processor->_sourceProvider;
    if (currentSource != NULL)
    {
        FrameProcessor *sourceProcessor = dynamic_cast<FrameProcessor *>(currentSource);
        if (sourceProcessor != NULL)
            restoreLineRecursive(sourceProcessor);
        else
            currentSource->RestoreLine();
    }
    bool success = processor->restoreLine();
    if (!success)
        throw exception("Unable to restore FrameProcessor");
}
