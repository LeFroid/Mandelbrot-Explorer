#ifndef _MANDELBROT_LIB_COLOR_STRATEGY_H_
#define _MANDELBROT_LIB_COLOR_STRATEGY_H_

#include <complex>
#include <mpfr.h>

#include "color/color.h"

namespace mandelbrot
{

class ColorStrategy
{
public:
    /**
     * @brief Determines the color for a value outside of the Mandelbrot set.
     * @param z The value of the function "z => z^2 + c"
     * @param dZ The derivative of z
     * @param scale The scale of the fractal
     * @param numIterations The number of iterations taken before z breached the "in the set" limit
     * @param maxIterations The maximum number of iterations before assuming that z is within the set.
     * @return A color for the given z value
     */
    virtual color_t getColor(
                std::complex<double> z,
                std::complex<double> dZ,
                double scale,
                int numIterations,
                int maxIterations) = 0;

    /**
     * @brief Determines the color for a value outside of the Mandelbrot set.
     * @param zReal The real portion of the z output from the function "z => z^2 + c"
     * @param zIm The imaginary portion of the z output from the function "z => z^2 + c"
     * @param dzReal The real portion of the derivative of z
     * @param dzIm The imaginary portion of the derivative of z
     * @param scale The scale of the fractal
     * @param numIterations The number of iterations taken before z breached the "in the set" limit
     * @param maxIterations The maximum number of iterations before assuming that z is within the set.
     * @return A color for the given z value
     */
    virtual color_t getColorPrecise(
                mpfr_ptr zReal,
                mpfr_ptr zIm,
                mpfr_ptr dzReal,
                mpfr_ptr dzIm,
                double scale,
                int numIterations,
                int maxIterations) = 0;

    /// Returns the color that will be rendered for a pixel within the Mandelbrot set
    /// This is usually black or white
    virtual color_t getColorInSet() = 0;
};

}

#endif // _MANDELBROT_LIB_COLOR_STRATEGY_H_

