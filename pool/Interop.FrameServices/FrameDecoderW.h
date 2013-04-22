#pragma once

#include <FrameDecoder.h>

#include "FrameProviderW.h"

using namespace System;
using namespace FrameServices;

#pragma make_public(::FrameDecoder)

namespace Interop { namespace FrameServices
{
    public ref class FrameDecoder abstract : FrameProvider
    {
    public:
        void Rewind()
        {
            Assert();
            ::FrameDecoder *wrapped = this;
            return wrapped->Rewind();
        }
        virtual property FrameDecoder ^ LineDecoder
        {
            FrameDecoder ^ get() override sealed
            {
                return this;
            }
        }
        property int64_t Duration
        {
            int64_t get()
            {
                Assert();
                ::FrameDecoder *wrapped = this;
                return wrapped->GetDuration();
            }
        }
        property int64_t FrameCount
        {
            int64_t get()
            {
                Assert();
                ::FrameDecoder *wrapped = this;
                return wrapped->GetFrameCount();
            }
        }
        property int64_t Position
        {
            int64_t get()
            {
                Assert();
                ::FrameDecoder *wrapped = this;
                return wrapped->GetPosition();
            }
            void set(int64_t value)
            {
                Assert();
                ::FrameDecoder *wrapped = this;
                wrapped->SetPosition(value);
            }
        }
        virtual operator ::FrameDecoder *() abstract;
    };
} }
