#pragma once

#include <stdint.h>

#include "defines.h"
#include "FrameProvider.h"

namespace FrameServices
{
    class FRAMESERVICES_API FrameDecoder : public FrameProvider
    {
    protected:
        virtual void setPosition(int64_t position) = 0;
        virtual void rewind() = 0;
    public:
        virtual FrameDecoder * GetLineDecoder();
        virtual void SetPosition(int64_t position);
        virtual void Rewind();
        virtual void RestoreLine();
        virtual int64_t GetPosition() const = 0;
        virtual int64_t GetDuration() const = 0;
        virtual int64_t GetFrameCount() const = 0;
    };
}
