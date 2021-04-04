#include <cmath>
#include <complex>

#include "color-strategy-smooth.h"

namespace mandelbrot
{
    const static double lnP = 0.693;
    const static double lle = 7.847;

    ColorStrategySmooth::ColorStrategySmooth() : ColorStrategy(),
        m_colorIntensity{-0.1275},
        m_temp{}
    {
        mpfr_init2(m_temp, 128);
    }

    ColorStrategySmooth::~ColorStrategySmooth()
    {
        mpfr_clear(m_temp);
    }

    void ColorStrategySmooth::setColorIntensity(double colorIntensity)
    {
        m_colorIntensity = colorIntensity;
    }

    color_t ColorStrategySmooth::getColor(std::complex<double> z, std::complex<double> dZ,
            double scale, int numIterations, int /*maxIterations*/)
    {
        const double modZ = std::abs(z);
        const double absDz = std::abs(dZ);
        const double logModZ = std::log(modZ);

        double V = 1;
        double dist, distScale;
        if (absDz > 0)
        {
            dist = 2.0 * modZ * logModZ / absDz;
            distScale = std::log(dist / scale) / lnP - 1.2;
            
            if (distScale < -8)
                V = 0;
            else if (distScale < 0)
                V = 1.0 + distScale / 8.0;
        }

        double dwell = (double)numIterations - std::log(logModZ) / lnP + lle;
        double q = std::log(std::abs(dwell)) * std::abs(m_colorIntensity);
        double angle;

        if (q < 0.5)
        {
            q = 1.0 - 1.5 * q;
            angle = 1.0 - q;
        }
        else
        {
            q = 1.5 * q - 0.5;
            angle = q;
        }

        double radius = std::sqrt(q);
        if (numIterations % 2 == 1 && m_colorIntensity > 0.0)
        {
            V = 0.85 * V;
            radius = 0.667 * radius;
        }
        double ignore;
        double H = std::modf(angle * 10, &ignore) * 360;
        double S = std::modf(radius, &ignore);
        return hsvToRgba(H, S, V);
    }

    color_t ColorStrategySmooth::getColorPrecise(mpfr_ptr zReal, mpfr_ptr zIm,
                    mpfr_ptr dzReal, mpfr_ptr dzIm, double scale, int numIterations, int /*maxIterations*/)
    {
        // Abs(complex) = sqrt(real^2  + imag^2)
        mpfr_fmma(m_temp, zReal, zReal, zIm, zIm, MPFR_RNDN);
        mpfr_sqrt(m_temp, m_temp, MPFR_RNDN);
        const double modZ = mpfr_get_d(m_temp, MPFR_RNDN);

        mpfr_fmma(m_temp, dzReal, dzReal, dzIm, dzIm, MPFR_RNDN);
        mpfr_sqrt(m_temp, m_temp, MPFR_RNDN);
        const double absDz = mpfr_get_d(m_temp, MPFR_RNDN);

        const double logModZ = std::log(modZ);

        double V = 1;
        double dist, distScale;
        if (absDz > 0)
        {
            dist = 2.0 * modZ * logModZ / absDz;
            distScale = std::log(dist / scale) / lnP - 1.2;

            if (distScale < -8)
                V = 0;
            else if (distScale < 0)
                V = 1.0 + distScale / 8.0;
        }

        double dwell = (double)numIterations - std::log(logModZ) / lnP + lle;
        double q = std::log(std::abs(dwell)) * std::abs(m_colorIntensity);
        double angle;

        if (q < 0.5)
        {
            q = 1.0 - 1.5 * q;
            angle = 1.0 - q;
        }
        else
        {
            q = 1.5 * q - 0.5;
            angle = q;
        }

        double radius = std::sqrt(q);
        if (numIterations % 2 == 1 && m_colorIntensity > 0.0)
        {
            V = 0.85 * V;
            radius = 0.667 * radius;
        }
        double ignore;
        double H = std::modf(angle * 10, &ignore) * 360;
        double S = std::modf(radius, &ignore);
        return hsvToRgba(H, S, V);
    }

    color_t ColorStrategySmooth::hsvToRgba(double hue, double saturation, double value) const
    {
        color_t result;
        result.argb.a = 0xFF;

        int i;
        double r, g, b, z, f, p, q, t, ignore;
        if (saturation <= 0.0)
        {
            value *= 255.0;
            uint8_t vInt = static_cast<uint8_t>(value);
            result.argb.r = vInt;
            result.argb.g = vInt;
            result.argb.b = vInt;
            return result;
        }

        z = hue / 60.0;
        i = static_cast<int>(z);
        f = std::modf(z, &ignore);
        p = value * (1.0 - saturation);
        q = value * (1.0 - saturation * f);
        t = value * (1.0 - saturation * (1.0 - f));

        switch (i)
        {
            case 0:
                r = value;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = value;
                b = p;
                break;
            case 2:
                r = p;
                g = value;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = value;
                break;
            case 4:
                r = t;
                g = p;
                b = value;
                break;
            case 5:
            default:
                r = value;
                g = p;
                b = q;
                break;
        }

        result.argb.r = static_cast<uint8_t>(r * 255.0);
        result.argb.g = static_cast<uint8_t>(g * 255.0);
        result.argb.b = static_cast<uint8_t>(b * 255.0);
        return result;
    }
}
