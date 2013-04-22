#include <exception>

extern "C" {
#include <libswscale/swscale.h>
}

#include "FrameScaler.h"
#include "common-ffmpeg.h"

using namespace std;
using namespace FrameServices;

FrameScaler::FrameScaler(int src_width, int src_height, FrameServices::PixelFormat src_pix_fmt,
	int dst_width, int dst_height, FrameServices::PixelFormat dst_pix_fmt,
    FrameHandle **handle) : _buffer(&_handle, dst_width, dst_height, dst_pix_fmt)
{
    _src_height = src_height;

	_img_convert_ctx = sws_getCachedContext(NULL,
        src_width, src_height, Convert(src_pix_fmt),
		dst_width, dst_height, Convert(dst_pix_fmt),
        SWS_BICUBIC, NULL, NULL, NULL);
	if (_img_convert_ctx == NULL)
		throw exception();

	*handle = &_handle;
}

FrameScaler::~FrameScaler()
{
    sws_freeContext(_img_convert_ctx);
}

void FrameScaler::ScaleFrame(const FrameHandle &src_frame, int src_slice_y, int src_slice_height)
{
    AVPicture source = AVPictureFromHandle(src_frame);
    AVPicture dest = AVPictureFromHandle(_handle);

    sws_scale(_img_convert_ctx, source.data, source.linesize,
        src_slice_y, src_slice_height, dest.data, dest.linesize);
}

void FrameScaler::ScaleFrame(const FrameHandle &src_frame)
{
    ScaleFrame(src_frame, 0, _src_height);
}
