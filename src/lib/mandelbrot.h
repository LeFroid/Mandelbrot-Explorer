#ifndef _MANDELBROT_LIB_MANDELBROT_H_
#define _MANDELBROT_LIB_MANDELBROT_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "color/color.h"
#include "color/color-strategy.h"
#include "output/output-device.h"
#include "threading/thread-pool.h"

namespace mandelbrot
{

class MandelbrotSet
{
public:
    explicit MandelbrotSet();
    ~MandelbrotSet();

    /**
     * @brief Calculates the Mandelbrot set at current scale and offset, feeding
     *        the output into the current output device. If the color strategy or
     *        output device are invalid, no calculations will be made.
     */
    void render();

    /**
     * @brief Sets the center coordinates on the Mandelbrot plane (not the output device)
     * @param x Center position on the real portion of the plane (horizontal)
     * @param y Center position on the imaginary portion of the plane (vertical)
     */
    void setCenter(double x, double y);

    /**
     * @brief Sets the coloring method to render items in and out of the mandelbrot
     *        set at runtime.
     * @param colorStrategy An implementation of the \ref ColorStrategy interface
     */
    void setColorStrategy(std::unique_ptr<ColorStrategy> colorStrategy);

    /**
     * @brief Sets the maximum number of times to iterate the Mandelbrot function z -> z^2 + c
     *        before assuming any given point does indeed belong to the set.
     * @param maxIterations Maximum number of iterations
     */
    void setMaxIterations(int maxIterations);

    /**
     * @brief Returns the implementation of the output device
     * @return Pointer to the output device implementation, or nullptr if not set
     */
    OutputDevice *getOutputDevice() const noexcept;

    /**
     * @brief Sets the device to which the mandelbrot data set will be written.
     * @param outputDevice An implementation of the \ref OutputDevice interface
     */
    void setOutputDevice(std::unique_ptr<OutputDevice> outputDevice);

    /**
     * @brief Sets the real dimensions of the set that will be rendered to the output device
     * @param width Real screen width, pixels
     * @param height Real screen height, pixels
     */
    void setOutputDimensions(int width, int height);

    /**
     * @brief Sets the scale, or "zoom" factor in which the points in the Mandelbrot set
     *        will be calculated
     * @param Scale factor. This should be a very small fractional value for good results.
     */
    void setScale(double scale);

private:
    /// Renders a portion of the mandelbrot set, from startRow to startRow + numRows
    void renderSection(int startRow, int numRows, const double xOffset, const double yOffset);

    /// Renders a portion of the mandelbrot set, from startRow to startRow + numRows, at deep zoom levels
    /// using MPFR for precision
    void renderSectionPrecise(int startRow, int numRows, const double xOffset, const double yOffset);

private:
    int m_maxIterations;

    int m_outputWidth;

    int m_outputHeight;

    double m_centerX;

    double m_centerY;

    double m_scale;

    std::unique_ptr<ColorStrategy> m_colorStrategy;

    std::unique_ptr<OutputDevice> m_outputDevice;

    ThreadPool m_threadPool;

    std::mutex m_mutex;

    std::condition_variable m_cv;

    std::atomic_int m_threadsComplete;
};

}

#endif // _MANDELBROT_LIB_MANDELBROT_H_

