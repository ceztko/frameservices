#include <string.h>
#include <exception>

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "DataBuffer.h"
#include "common-ffmpeg.h"

using namespace std;
using namespace FrameServices;

void DataBuffer::init(FrameHandle *handle, int width, int heigth,
    FrameServices::PixelFormat pix_fmt)
{
    _handle = handle;

    AVPicture picture;
    int rv = avpicture_alloc(&picture, Convert(pix_fmt), width, heigth);
	if (rv < 0)
		throw exception();

    _handle->SetData(picture.data);
    _handle->SetLinesize(picture.linesize);
}

DataBuffer::DataBuffer(FrameHandle *handle)
{
    _handle = handle;

    AVPicture picture;
    int rv = avpicture_alloc(&picture, Convert(handle->PixelFormat),
        handle->Width, handle->Height);
	if (rv < 0)
		throw exception();

    _handle->SetData(picture.data);
    _handle->SetLinesize(picture.linesize);
}

DataBuffer::DataBuffer(FrameHandle *handle, const VideoProps &properties)
{
    init(handle, properties.Width, properties.Height, properties.PixelFormat);
}

DataBuffer::DataBuffer(FrameHandle *handle, int width, int heigth,
    FrameServices::PixelFormat pix_fmt)
{
    init(handle, width, heigth, pix_fmt);
}

DataBuffer::~DataBuffer()
{
    AVPicture picture;
    _handle->CopyDataTo(picture.data);
    _handle->CopyLineSizeTo(picture.linesize);

	avpicture_free(&picture);
}
