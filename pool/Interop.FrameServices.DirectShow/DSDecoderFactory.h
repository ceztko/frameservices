#include "DSDecoderW.h"

namespace Interop { namespace FrameServices { namespace DirectShow
{
    public ref class DSDecoderFactory : DecoderFactory
    {
    public:
        virtual FrameDecoder ^ GetDecoder(String^ filename) override sealed
        {
            return gcnew DSDecoder(filename);
        }
    };
} } }
