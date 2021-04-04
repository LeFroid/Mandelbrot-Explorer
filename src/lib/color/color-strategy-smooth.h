#ifndef _MANDELBROT_LIB_COLOR_STRATEGY_SMOOTH_H_
#define _MANDELBROT_LIB_COLOR_STRATEGY_SMOOTH_H_

#include <mpfr.h>
#include "color/color-strategy.h"

namespace mandelbrot
{

class ColorStrategySmooth final : public ColorStrategy
{
public:
    /// Constructor
    ColorStrategySmooth();

    /// Destructor
    ~ColorStrategySmooth();

    /**
     * @brief Determines the color for a value outside of the Mandelbrot set.
     * @param z The value of the function "z => z^2 + c"
     * @param dZ The derivative of z
     * @param scale The scale of the fractal
     * @param numIterations The number of iterations taken before z breached the "in the set" limit
     * @param maxIterations The maximum number of iterations before assuming that z is within the set.
     * @return A color for the given z value
     */
    color_t getColor(
                std::complex<double> z,
                std::complex<double> dZ,
                double scale,
                int numIterations,
                int maxIterations) override;

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
    color_t getColorPrecise(
                    mpfr_ptr zReal,
                    mpfr_ptr zIm,
                    mpfr_ptr dzReal,
                    mpfr_ptr dzIm,
                    double scale,
                    int numIterations,
                    int maxIterations) override;

    /// Returns the color that will be rendered for a pixel within the Mandelbrot set
    /// This is usually black or white
    color_t getColorInSet() override { return color_t{0xFF000000}; }

    void setColorIntensity(double colorIntensity);

private:
    /// Translates the given HSV values into a standard RGBA value
    color_t hsvToRgba(double hue, double saturation, double value) const;

    /// Color intensity factor
    double m_colorIntensity{-0.1275};

    /// Temporary variable for precise (MPFR) floating point calculations
    mpfr_t m_temp;
};

}

#endif // _MANDELBROT_LIB_COLOR_STRATEGY_SMOOTH_H_

