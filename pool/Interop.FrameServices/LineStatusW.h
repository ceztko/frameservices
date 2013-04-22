#pragma once

#include <LineStatus.h>

using namespace System;
using namespace FrameServices;

#pragma make_public(::LineStatus)

namespace Interop { namespace FrameServices
{
    public value struct LineStatus
    {
    public:
        static initonly LineStatus Default;
        static LineStatus()
        {
            Default.Resetted = false;
            Default.PropsChanged = true;
        }
        LineStatus(const ::LineStatus &status)
        {
            Resetted = status.Resetted;
            PropsChanged = status.PropsChanged;
        }
        bool Resetted;
        bool PropsChanged;
        operator ::LineStatus()
        {
            ::LineStatus ret;
            ret.Resetted = Resetted;
            ret.PropsChanged = PropsChanged;
            return ret;
        }
    };
} }
