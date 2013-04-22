#include "Rational.h"

using namespace FrameServices;

Rational::Rational()
{
    Numerator = 0;
    Denominator = 1;
}

Rational::Rational(int numerator, int denominator)
{
    Numerator = numerator;
    Denominator = denominator;
}
