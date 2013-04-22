#pragma once

#include "FrameDecoderW.h"

using namespace System;
using namespace NET::Persistence;

namespace Interop { namespace FrameServices
{
    public ref class DecoderFactory abstract : IPersistableItem
    {
    public:
        virtual FrameDecoder ^ GetDecoder(String ^filename) abstract;
        virtual void Serialize(SerializerStream ^writer)
        {
        }
        virtual void Deserialize(DeserializerStream ^reader)
        {
        }
        virtual void SetInitList(array<Object ^> ^%args)
        {
        }
        virtual void Revise(ReviseMode mode)
        {
        }
    };
} }
