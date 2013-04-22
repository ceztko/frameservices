#include <cstring>
#include <stdint.h>

#include "FrameHandle.h"

using namespace FrameServices;

FrameHandle::FrameHandle()
{
    memset(Data, 0, sizeof(Data));
    memset(LineSize, 0, sizeof(LineSize));
    Interlaced = false;
    TopFieldFirst = true;
    Pts = 0;
    Width = -1;
    Height =  -1;
    PixelFormat = ANY_PIX_FMT;
}

FrameHandle::FrameHandle(uint8_t* const data[], const int linesize[])
{
    memcpy(Data, data, sizeof(Data));
    memcpy(LineSize, linesize, sizeof(LineSize));
    Interlaced = false;
    TopFieldFirst = true;
    Pts = 0;
    Width = -1;
    Height =  -1;
    PixelFormat = ANY_PIX_FMT;
}

void FrameHandle::SetData(uint8_t* const data[])
{
    memcpy(Data, data, sizeof(Data));
}

void FrameHandle::SetLinesize(const int linesize[])
{
    memcpy(LineSize, linesize, sizeof(LineSize));
}

void FrameHandle::CopyDataTo(uint8_t* dest_data[]) const
{
    memcpy(dest_data, Data, sizeof(Data));
}

void FrameHandle::CopyLineSizeTo(int dest_linesize[]) const
{
    memcpy(dest_linesize, LineSize, sizeof(LineSize));
}
