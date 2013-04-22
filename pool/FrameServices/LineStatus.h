#pragma once

#include "defines.h"

namespace FrameServices
{
    struct FRAMESERVICES_API LineStatus
    {
    public:
        bool Resetted;
        bool PropsChanged;
        LineStatus();
    };
}
