#include <cmath>

#include "color-strategy-wavelength.h"

namespace mandelbrot
{
    static constexpr double logBase = 1.44269504089;
    static constexpr double logHalfBase = -1.0;

    ColorStrategyWavelength::ColorStrategyWavelength() :
        m_colorMap{}
    {
        for (int i = 0; i < 512; ++i)
        {
            color_t c { 0xFF000000 };
            auto r = std::sin(0.2 * i) * 127.5 + 127.5,
                 g = std::sin(0.2 * i + 2) * 127.5 + 127.5,
                 b = std::sin(0.2 * i + 4) * 127.5 + 127.5;
            c.argb.r = static_cast<uint8_t>(r);
            c.argb.g = static_cast<uint8_t>(g);
            c.argb.b = static_cast<uint8_t>(b);
            m_colorMap[i] = c;
        }
    }

    color_t ColorStrategyWavelength::getColor(
                std::complex<double> z,
                std::complex<double> /*dZ*/,
                double /*scale*/,
                int numIterations,
                int maxIterations)
    {
        double n = 5.0 + numIterations - logHalfBase - std::log(std::log(std::norm(z))) * logBase;
        double normalized = n / static_cast<double>(maxIterations);
        return m_colorMap[static_cast<int>(normalized * 511)];
    }

    color_t ColorStrategyWavelength::getColorPrecise(
                    mpfr_ptr zReal,
                    mpfr_ptr zIm,
                    mpfr_ptr temp,
                    mpfr_ptr temp2,
                    double /*scale*/,
                    int numIterations,
                    int maxIterations)
    {
        //norm(z) = zReal * zReal + zIm * zIm
        mpfr_sqr(temp, zReal, MPFR_RNDN);
        mpfr_sqr(temp2, zIm, MPFR_RNDN);
        mpfr_add(temp, temp, temp2, MPFR_RNDN);

        double n = 5.0 + numIterations - logHalfBase - std::log(std::log(mpfr_get_d(temp, MPFR_RNDN))) * logBase;
        double normalized = n / static_cast<double>(maxIterations);
        return m_colorMap[static_cast<int>(normalized * 511)];
    }
}

