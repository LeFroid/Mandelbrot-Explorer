#include "mandelbrot.h"

#include <functional>
#include <thread>
#include <utility>

#include <mpfr.h>

#include <iostream>

static mpfr_t mpLim;

namespace mandelbrot
{
    static constexpr int NumThreads = 4;

    typedef void (MandelbrotSet::*MandelbrotPtr)(int, int, const double, const double);

    MandelbrotSet::MandelbrotSet() :
        m_maxIterations(0),
        m_outputWidth(0),
        m_outputHeight(0),
        m_centerX(0.0),
        m_centerY(0.0),
        m_scale(0.0),
        m_colorStrategy(nullptr),
        m_outputDevice(nullptr),
        m_threadPool(NumThreads),
        m_mutex(),
        m_cv(),
        m_threadsComplete(0)
    {
        mpfr_init2(mpLim, 128);
        mpfr_set_d(mpLim, 4.0, MPFR_RNDN);
    }

    MandelbrotSet::~MandelbrotSet()
    {
        mpfr_clear(mpLim);
    }

    void MandelbrotSet::render()
    {
        if (!m_colorStrategy
                || !m_outputDevice
                || m_maxIterations == 0
                || m_outputWidth <= 0
                || m_outputHeight <= 4)
            return;

        const double yOffset = (-1.0 * static_cast<double>(m_outputHeight)) / 2.0;
        const double xOffset = (-1.0 * static_cast<double>(m_outputWidth)) / 2.0;

        m_threadsComplete.store(0);

        MandelbrotPtr renderCallback = m_scale < 1e-16 ? &MandelbrotSet::renderSectionPrecise : &MandelbrotSet::renderSection;

        // split work among each thread
        const int rowsPerThread = m_outputHeight / NumThreads;
        for (int i = 0; i < NumThreads; ++i)
        {
            int rowsToProcess = rowsPerThread;
            if (i + 1 == NumThreads)
                rowsToProcess += (m_outputHeight % NumThreads);
            m_threadPool.post(std::bind(renderCallback, this, i * rowsPerThread, rowsToProcess, xOffset, yOffset));
        }

        {
            std::unique_lock lock{m_mutex};
            m_cv.wait(lock, [this](){
                return m_threadsComplete == NumThreads;
            });
        }

        m_outputDevice->flush();
    }

    void MandelbrotSet::renderSection(int startRow, int numRows, const double xOffset, const double yOffset)
    {
        constexpr double Limit = 4.0;
        const int endIdx = std::min(m_outputHeight, startRow + numRows);

        for (int y = startRow; y < endIdx; ++y)
        {
            double cIm = m_centerY + m_scale * (y + yOffset);

            std::vector<color_t> rowColors;
            rowColors.reserve(m_outputWidth);

            for (int x = 0; x < m_outputWidth; ++x)
            {
                double cRe = m_centerX + m_scale * (x + xOffset);

                double zRe = 0.0,
                       zIm = 0.0,
                       zRe2 = 0.0,
                       zIm2 = 0.0,
                       dzRe = 0.0,
                       dzIm = 0.0,
                       dzTemp = 0.0;
                int numIterations = 0;
                do
                {
                    ++numIterations;

                    // Derivative of z
                    dzTemp = 2.0 * (zRe * dzRe - dzIm * zIm) + 1.0;
                    dzIm = 2.0 * (zIm * dzRe + zRe * dzIm);
                    dzRe = dzTemp;

                    dzTemp = zRe + zIm;
                    zIm = (dzTemp * dzTemp) - zRe2 - zIm2;
                    zIm += cIm;
                    zRe = zRe2 - zIm2 + cRe;
                    zRe2 = zRe * zRe;
                    zIm2 = zIm * zIm;

                    if ((zRe2 + zIm2) > Limit)
                        break;

                } while (numIterations < m_maxIterations);

                /*
                int numIterations = 0;
                do
                {
                    ++numIterations;

                    //zDerivative
                    nextDzRe = 2.0 * (zRe * dzRe - dzIm * zIm) + 1.0;
                    nextDzIm = 2.0 * (zIm * dzRe + zRe * dzIm);
                    dzRe = nextDzRe;
                    dzIm = nextDzIm;

                    //z
                    nextZRe = zRe2 - zIm2 + cRe;
                    nextZIm = 2.0 * (zIm * zRe) + cIm;
                    zRe = nextZRe;
                    zIm = nextZIm;

                    zRe2 = zRe * zRe;
                    zIm2 = zIm * zIm;
                    if ((zRe2 + zIm2) > Limit)
                        break;
                } while (numIterations < m_maxIterations);
                */

                if (numIterations < m_maxIterations)
                {
                    rowColors.emplace_back(m_colorStrategy->getColor(
                                std::complex<double>(zRe, zIm),
                                std::complex<double>(dzRe, dzIm),
                                m_scale,
                                numIterations,
                                m_maxIterations));
                }
                else
                {
                    rowColors.emplace_back(m_colorStrategy->getColorInSet());
                }
            }

            m_outputDevice->write(0, y, std::move(rowColors));
        }

        m_threadsComplete++;
        m_cv.notify_one();
    }

    void MandelbrotSet::renderSectionPrecise(int startRow, int numRows, const double xOffset, const double yOffset)
    {
        const int endIdx = std::min(m_outputHeight, startRow + numRows);

        mpfr_t zI, zI2, zR, zR2, dzI, dzR, dzTmp, cIm, cRe;
        mpfr_inits2(128, zI, zI2, zR, zR2, dzI, dzR, dzTmp, cIm, cRe, (mpfr_ptr)0);

        for (int y = startRow; y < endIdx; ++y)
        {
            mpfr_set_zero(cIm, 0);
            mpfr_add_si(cIm, cIm, y, MPFR_RNDN);
            mpfr_add_d(cIm, cIm, yOffset, MPFR_RNDN);
            mpfr_mul_d(cIm, cIm, m_scale, MPFR_RNDN);
            mpfr_add_d(cIm, cIm, m_centerY, MPFR_RNDN);

            std::vector<color_t> rowColors;
            rowColors.reserve(m_outputWidth);

            for (int x = 0; x < m_outputWidth; ++x)
            {
                mpfr_set_zero(cRe, 0);
                mpfr_add_si(cRe, cRe, x, MPFR_RNDN);
                mpfr_add_d(cRe, cRe, xOffset, MPFR_RNDN);
                mpfr_mul_d(cRe, cRe, m_scale, MPFR_RNDN);
                mpfr_add_d(cRe, cRe, m_centerX, MPFR_RNDN);

                mpfr_set_zero(zI, 0);
                mpfr_set_zero(zI2, 0);
                mpfr_set_zero(zR, 0);
                mpfr_set_zero(zR2, 0);
                mpfr_set_zero(dzI, 0);
                mpfr_set_zero(dzR, 0);
                mpfr_set_zero(dzTmp, 0);

                int numIterations = 0;
                do
                {
                    ++numIterations;

                    // Derivative of z
                    // dzTmp = (zR * dzR) - (dzI * zI)
                    mpfr_fmms(dzTmp, zR, dzR, dzI, zI, MPFR_RNDN);
                    // dzR = 2.0 * ((zR * dzR) - (dzI * zI)) + 1.0;
                    mpfr_mul_si(dzTmp, dzTmp, 2, MPFR_RNDN);

                    // dzI = 2.0 * (zI * dzR + zR * dzI);
                    mpfr_fmma(dzI, zI, dzR, zR, dzI, MPFR_RNDN);
                    mpfr_mul_d(dzI, dzI, 2.0, MPFR_RNDN);
                    mpfr_add_si(dzR, dzTmp, 1, MPFR_RNDN);

                    //zI = ((zR+zI)^2) - zR2 - zI2 + cI
                    mpfr_add(dzTmp, zR, zI, MPFR_RNDN);
                    mpfr_sqr(zI, dzTmp, MPFR_RNDN);
                    mpfr_sub(zI, zI, zR2, MPFR_RNDN);
                    mpfr_sub(zI, zI, zI2, MPFR_RNDN);
                    mpfr_add(zI, zI, cIm, MPFR_RNDN);

                    //zR = zR2 - zI2 + cR
                    mpfr_sub(zR, zR2, zI2, MPFR_RNDN);
                    mpfr_add(zR, zR, cRe, MPFR_RNDN);

                    mpfr_sqr(zR2, zR, MPFR_RNDN);
                    mpfr_sqr(zI2, zI, MPFR_RNDN);

                    mpfr_add(dzTmp, zR2, zI2, MPFR_RNDN);
                    if (mpfr_greater_p(dzTmp, mpLim))
                        break;
                } while (numIterations < m_maxIterations);

                if (numIterations < m_maxIterations)
                {
                    rowColors.emplace_back(m_colorStrategy->getColorPrecise(
                                zR, zI,
                                dzR, dzI,
                                m_scale,
                                numIterations,
                                m_maxIterations));
                }
                else
                {
                    rowColors.emplace_back(m_colorStrategy->getColorInSet());
                }
            }

            m_outputDevice->write(0, y, std::move(rowColors));
        }

        mpfr_clears(zI, zI2, zR, zR2, dzI, dzR, dzTmp, cIm, cRe, (mpfr_ptr)0);

        m_threadsComplete++;
        m_cv.notify_one();
    }

    void MandelbrotSet::setCenter(double x, double y)
    {
        m_centerX = x;
        m_centerY = y;
    }

    void MandelbrotSet::setColorStrategy(std::unique_ptr<ColorStrategy> colorStrategy)
    {
        m_colorStrategy = std::move(colorStrategy);
    }

    void MandelbrotSet::setMaxIterations(int maxIterations)
    {
        m_maxIterations = maxIterations;
    }

    OutputDevice *MandelbrotSet::getOutputDevice() const noexcept
    {
        return m_outputDevice.get();
    }

    void MandelbrotSet::setOutputDevice(std::unique_ptr<OutputDevice> outputDevice)
    {
        m_outputDevice = std::move(outputDevice);
    }

    void MandelbrotSet::setOutputDimensions(int width, int height)
    {
        m_outputWidth = width;
        m_outputHeight = height;

        if (m_outputDevice)
            m_outputDevice->setDimensions(int32_t{width}, int32_t{height});
    }

    void MandelbrotSet::setScale(double scale)
    {
        m_scale = scale;
    }
}

