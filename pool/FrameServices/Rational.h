#pragma once

#include "defines.h"

namespace FrameServices
{
    struct FRAMESERVICES_API Rational
    {
    public:
        Rational();
        Rational(int numerator, int denominator);
        int Numerator;
        int Denominator;
    };
}
