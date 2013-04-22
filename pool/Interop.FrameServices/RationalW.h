#pragma once

#include <Rational.h>

using namespace System;
using namespace FrameServices;

#pragma make_public(::Rational)

namespace Interop { namespace FrameServices
{
    public value struct Rational
    {
    public:
        int Numerator;
        int Denominator;
        static initonly Rational Default;
        static Rational()
        {
            Default.Numerator = 0;
            Default.Denominator = 1;
        }
        Rational(int numerator, int denominator)
        {
            Numerator = numerator;
            Denominator = denominator;
        }
        Rational(const ::Rational &rational)
        {
            Numerator = rational.Numerator;
            Denominator = rational.Denominator;
        }
        operator ::Rational()
        {
            return ::Rational(Numerator, Denominator);
        }
    };
} }
