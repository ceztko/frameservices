#include <exception>

#include "FrameDecoder.h"

using namespace std;
using namespace FrameServices;

FrameDecoder * FrameDecoder::GetLineDecoder()
{
    return this;
}

void FrameDecoder::SetPosition(int64_t position)
{
    setPosition(position);

    bool ok = RequestNextFrame();
    if (!ok)
        throw exception("Couldn't get frame after seek");

    issueReset();
}

void FrameDecoder::Rewind()
{
    rewind();

    bool ok = RequestNextFrame();
    if (!ok)
        throw exception("Couldn't get frame after seek");

    issueReset();
}

void FrameDecoder::RestoreLine()
{
    clearReset();
}
