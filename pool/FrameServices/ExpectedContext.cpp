#include "ExpectedContext.h"
#include "FrameFilter.h"
#include <algorithm>

using namespace FrameServices;
using namespace std;

#define FILTER_DESC "yadif=1:-1"

struct delete_item
{
    template <typename T>
    void operator()(const T& item) const
    {
        delete item;
    }
};

ExpectedContext::ExpectedContext()
{
    _provider = NULL;
    _converters = new list<FrameProvider *>();
}

ExpectedContext::ExpectedContext(const ExpectedContext &context)
{
    _provider = context._provider;
    _converters = new list<FrameProvider *>(*(context._converters));
}

ExpectedContext::~ExpectedContext()
{
    clearConverters();

    delete _converters;
}

void ExpectedContext::clearConverters()
{
    for_each(_converters->begin(), _converters->end(), delete_item());
    _converters->clear();
}

void ExpectedContext::SetContext(FrameProvider *provider,
    const VideoProps &expectedProps)
{
    if (_converters->size() != 0)
        clearConverters();

    _provider = provider;

    VideoProps sourceProps = provider->GetVideoProps();
    FrameProvider *current = provider;

    if (expectedProps.InterlacedStyle == PROGRESSIVE
        && sourceProps.InterlacedStyle == INTERLACED)
    {
        current = new FrameFilter(FILTER_DESC, current);
        _converters->push_front(current);
    }

    if (!SourceCompatible(sourceProps, expectedProps))
    {
        current = new FrameConverter(current, expectedProps);
        _converters->push_front(current);
    }
}

FrameProvider * ExpectedContext::GetProvider()
{
    return _provider;
}

FrameProvider * ExpectedContext::GetExpectedProvider()
{
    if (_converters->size() != 0)
        return _converters->front();

    return _provider;
}
