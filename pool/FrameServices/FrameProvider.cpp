#include <exception>
#include <sstream>
#include <fstream>
#include <memory>
#include <algorithm>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include "common-ffmpeg.h"
#include "FrameProvider.h"
#include "ExpectedContext.h"
#include "FrameConverter.h"
#include "FrameScaler.h"
#include "FrameProcessor.h"

using namespace std;
using namespace FrameServices;

#define TESTSTREAM(stream, filename) \
if (stream.fail() && !stream.eof()) \
    throw fstream::failure(string("I/O error on ") + filename); \
stream.clear(stream.rdstate() & ~(ios::failbit));

static void SaveFramePPM(const string &filename, uint8_t *data, int width, int height);

FrameProvider::FrameProvider()
{
    _processors = new list<FrameProcessor *>();
}

FrameProvider::~FrameProvider()
{
    delete _processors;
}

FrameProvider * FrameProvider::GetExpectedProvider(ExpectedContext &context,
    const VideoProps &properties)
{
    context.SetContext(this, properties);

    return context.GetExpectedProvider();
}

void FrameProvider::SaveImage(const std::string &filename) const
{
    SaveImage(filename, IMAGE_TYPE_PPM);
}

void FrameProvider::SaveImage(const string &filename, ImageType imageType) const
{
    auto_ptr<FrameScaler> scaler;

    VideoProps props = GetVideoProps();

    FrameHandle *srcHandle = const_cast<FrameProvider *>(this)->GetFrameHandle();
    uint8_t *data;
    if (props.PixelFormat == RGB24_PIX_FMT)
    {
        data = srcHandle->Data[0];
    }
    else
    {
        FrameHandle *scaledHandle;
        scaler.reset(new FrameScaler(props.Width, props.Height, props.PixelFormat,
            props.Width, props.Height, RGB24_PIX_FMT, &scaledHandle));
        scaler->ScaleFrame(*srcHandle);
        data = scaledHandle->Data[0];
    }

    switch (imageType)
    {
        case IMAGE_TYPE_JPEG:
        case IMAGE_TYPE_PNG:
            throw exception("Not implemented");
        case IMAGE_TYPE_PPM:
            SaveFramePPM(filename, data, props.Width, props.Height);
            break;
    }
}

FrameProvider::iterator FrameProvider::begin()
{
    return _processors->begin();
}

FrameProvider::iterator FrameProvider::end()
{
    return _processors->end();
}

const VideoProps & FrameProvider::GetVideoProps() const
{
    if (_status.PropsChanged)
    {
        FrameProvider *thisProvider = const_cast<FrameProvider *>(this);
        getVideoProps(thisProvider->_props);
        thisProvider->_status.PropsChanged = false;
    }

    return _props;
}

const LineStatus & FrameProvider::GetLineStatus() const
{
    return _status;
}

void FrameProvider::addProcessors(FrameProcessor *processor)
{
    _processors->push_back(processor);
}

void FrameProvider::removeProcessors(FrameProcessor *processor)
{
    list<FrameProcessor *>::iterator index =
        find(_processors->begin(), _processors->end(), processor);

    _processors->erase(index);
}

void FrameProvider::issuePropsChanged()
{
    _status.PropsChanged = true;
}

void FrameProvider::issueReset()
{
    issueResetRecursively(this);
}

void FrameProvider::clearReset()
{
    clearResetRecursively(this);
}

void FrameProvider::issueResetRecursively(FrameProvider *provider)
{
    FrameProvider::iterator it = provider->begin();
    FrameProvider::iterator end = provider->end();
    for (; it != end; it++)
        issueResetRecursively(*it);

    provider->_status.Resetted = true;
}

void FrameProvider::clearResetRecursively(FrameProvider *provider)
{
    FrameProvider::iterator it = provider->begin();
    FrameProvider::iterator end = provider->end();
    for (; it != end; it++)
        clearResetRecursively(*it);

    provider->_status.Resetted = false;
}

void SaveFramePPM(const string &filename, uint8_t *data, int width, int height)
{
    ofstream stream;
    stream.open(filename.c_str(), ios_base::binary);
    TESTSTREAM(stream, filename);

    // Prepare header
    stringstream str_stream;
    str_stream << "P6\n" << width << " " << height << "\n255\n";

    // Write header
    string header = str_stream.str();
    stream.write(header.c_str(), header.length());
    TESTSTREAM(stream, filename);

    // Write pixel data
    int linesize = width * 3;
    for (int it = 0; it < height; it++)
    {
        stream.write((char *)&data[it * linesize], linesize);
        TESTSTREAM(stream, filename);
    }
}
