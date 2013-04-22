#pragma once

#include "defines.h"
#include "Rational.h"
#include "PixelFormat.h"
#include "FrameHandle.h"
#include "DataBuffer.h"

struct SwsContext;
struct AVFrame;

namespace FrameServices
{
    class FRAMESERVICES_API FrameScaler
	{
	private:
		SwsContext *_img_convert_ctx;
        int _src_height;
		FrameHandle _handle;
        DataBuffer _buffer;
	public:
        FrameScaler(int src_width, int src_height, PixelFormat src_pix_fmt,
			int dst_width, int dst_height, PixelFormat dst_pix_fmt, FrameHandle **handle);
		~FrameScaler();
		void ScaleFrame(const FrameHandle &src_frame, int src_slice_y, int src_slice_height);
        void ScaleFrame(const FrameHandle &src_frame);
	};
}
