#include "FFDecoderW.h"

namespace Interop { namespace FrameServices { namespace LibFF
{
    public ref class FFDecoderFactory : DecoderFactory
    {
    public:
        virtual FrameDecoder ^ GetDecoder(String^ filename) override sealed
        {
            return gcnew FFDecoder(filename);
        }
    };
} } }
