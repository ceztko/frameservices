#include "AutoFrameConverter.h"
#include "FrameConverter.h"

using namespace FrameServices;

AutoFrameConverter::AutoFrameConverter(FrameProvider *source,
    const FrameProcessor &destination) : FrameConverter(source, destination) { }
