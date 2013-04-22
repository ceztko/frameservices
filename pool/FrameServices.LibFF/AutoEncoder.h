#pragma once

#include <FrameProcessor.h>
#include <Rational.h>

#include "defines.h"
#include "FFEncoder.h"

namespace FrameServices { namespace LibFF
{
    class LIBAV_API AutoEncoder : public FrameProcessor
	{
    private:
        FFEncoder _encoder;
        int64_t _nextPts;
        bool _enabled;
        void encodeFrame();
    protected:
        virtual bool restoreLine();
        virtual bool requestNextFrame();
        virtual void getVideoProps(VideoProps &props) const;
	public:
        AutoEncoder(const std::string &filename, FrameProvider *srcFrameProvider,
            const Rational &timeBase, bool rawvideo);
        virtual FrameHandle * GetFrameHandle();
        bool IsEnabled() const;
        void SetEnabled(bool enabled);
    };
} }
