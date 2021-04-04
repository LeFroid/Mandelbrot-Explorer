#include "color/color-strategy-smooth.h"
#include "color/color-strategy-iteration.h"
#include "color/color-strategy-wavelength.h"
#include "output/output-device-bmp.h"
#include "output/output-device-qt.h"
#include "threading/mandelbrot-thread-qt.h"

#include <array>
#include <memory>
#include <QMutexLocker>

namespace mandelbrot
{
    MandelbrotThreadQt::MandelbrotThreadQt(QObject *parent) :
        QThread(parent),
        m_mutex(),
        m_cv(),
        m_mandelbrotSet(),
        m_renderAgain(false),
        m_quit(false),
        m_discard(false),
        m_maxIterations(0),
        m_outputWidth(0),
        m_outputHeight(0),
        m_centerX(0.0),
        m_centerY(0.0),
        m_scale(0.0),
        m_colorStrategy(nullptr)
    {
        m_mandelbrotSet.setOutputDevice(std::make_unique<OutputDeviceQt>());
    }

    MandelbrotThreadQt::~MandelbrotThreadQt()
    {
        m_mutex.lock();
        m_quit = true;
        m_cv.wakeAll();
        m_mutex.unlock();

        wait();
    }

    void MandelbrotThreadQt::saveToFile(const QString &fileName, int colorStrategy, double colorIntensity) const
    {
        MandelbrotSet temp{};
        temp.setCenter(m_centerX, m_centerY);

        auto outDevice = std::make_unique<OutputDeviceBMP>();
        outDevice->setFileName(fileName.toStdString());
        temp.setOutputDevice(std::move(outDevice));

        temp.setOutputDimensions(m_outputWidth, m_outputHeight);
        temp.setMaxIterations(m_maxIterations);
        temp.setScale(m_scale);

        if (colorStrategy == 0)
        {
            auto colorStrat = std::make_unique<ColorStrategySmooth>();
            colorStrat->setColorIntensity(colorIntensity);
            temp.setColorStrategy(std::move(colorStrat));
        }
        else if (colorStrategy == 1)
            temp.setColorStrategy(std::make_unique<ColorStrategyIteration>());
        else if (colorStrategy == 2)
            temp.setColorStrategy(std::make_unique<ColorStrategyWavelength>());

        temp.render();
    }

    void MandelbrotThreadQt::createImage()
    {
        QMutexLocker lock{&m_mutex};

        m_discard.store(false);

        if (!isRunning())
        {
            start();
        }
        else
        {
            m_renderAgain = true;
            m_cv.wakeOne();
        }
    }

    void MandelbrotThreadQt::discardAny()
    {
        if (!isRunning())
            return;

        m_discard.store(true);
    }

    void MandelbrotThreadQt::setCenter(double x, double y)
    {
        QMutexLocker lock{&m_mutex};
        m_centerX = x;
        m_centerY = y;
    }

    void MandelbrotThreadQt::setColorStrategy(std::unique_ptr<ColorStrategy> colorStrategy)
    {
        QMutexLocker lock{&m_mutex};
        m_colorStrategy = std::move(colorStrategy);
    }

    void MandelbrotThreadQt::setMaxIterations(int maxIterations)
    {
        QMutexLocker lock{&m_mutex};
        m_maxIterations = maxIterations;
    }

    void MandelbrotThreadQt::setOutputDimensions(int width, int height)
    {
        QMutexLocker lock{&m_mutex};
        m_outputWidth = width;
        m_outputHeight = height;
    }

    void MandelbrotThreadQt::setScale(double scale)
    {
        QMutexLocker lock{&m_mutex};
        m_scale = scale;
    }

    void MandelbrotThreadQt::run()
    {
        OutputDeviceQt *outDevice = static_cast<OutputDeviceQt*>(m_mandelbrotSet.getOutputDevice());
        if (!outDevice)
            return;

        while (!m_quit)
        {
            m_mutex.lock();
            const double scale = m_scale;
            m_mandelbrotSet.setMaxIterations(m_maxIterations);
            m_mandelbrotSet.setCenter(m_centerX, m_centerY);
            m_mandelbrotSet.setScale(scale);
            m_mandelbrotSet.setOutputDimensions(m_outputWidth, m_outputHeight);

            if (m_colorStrategy)
            {
                m_mandelbrotSet.setColorStrategy(std::move(m_colorStrategy));
                m_colorStrategy.reset(nullptr);
            }
            m_mutex.unlock();

            m_mandelbrotSet.render();

            if (!m_discard.load())
                emit outputReady(outDevice->getOutput(), scale);
            else
                m_discard.store(false);

            // after calculating the set,
            m_mutex.lock();
            if (!m_renderAgain)
                m_cv.wait(&m_mutex);
            m_renderAgain = false;
            m_mutex.unlock();
        }
    }
}
