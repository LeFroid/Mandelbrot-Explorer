#include "color-strategy-iteration.h"

namespace mandelbrot
{
    color_t ColorStrategyIteration::getColor(std::complex<double> /*z*/,
                std::complex<double> /*dZ*/,
                double /*scale*/,
                int numIterations,
                int maxIterations)
    {
        const double n = static_cast<double>(numIterations) / static_cast<double>(maxIterations);
        const double n2 = 1.0 - n;
        int r = static_cast<int>(9 * n2 * n * n * n * 255);
        int g = static_cast<int>(15 * n2 * n2 * n * n * 255);
        int b = static_cast<int>(8.5 * n2 * n2 * n2 * n * 255);

        color_t result;
        result.argb.a = 0xFF;
        result.argb.r = static_cast<uint8_t>(r);
        result.argb.g = static_cast<uint8_t>(g);
        result.argb.b = static_cast<uint8_t>(b);
        return result;
    }

    color_t ColorStrategyIteration::getColorPrecise(mpfr_ptr /*zReal*/,
                    mpfr_ptr /*zIm*/,
                    mpfr_ptr /*dzReal*/,
                    mpfr_ptr /*dzIm*/,
                    double /*scale*/,
                    int numIterations,
                    int maxIterations)
    {
        const double n = static_cast<double>(numIterations) / static_cast<double>(maxIterations);
        const double n2 = 1.0 - n;
        int r = static_cast<int>(9 * n2 * n * n * n * 255);
        int g = static_cast<int>(15 * n2 * n2 * n * n * 255);
        int b = static_cast<int>(8.5 * n2 * n2 * n2 * n * 255);

        color_t result;
        result.argb.a = 0xFF;
        result.argb.r = static_cast<uint8_t>(r);
        result.argb.g = static_cast<uint8_t>(g);
        result.argb.b = static_cast<uint8_t>(b);
        return result;
    }
}
