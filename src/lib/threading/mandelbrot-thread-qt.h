#ifndef _MANDELBROT_LIB_MANDELBROT_THREAD_QT_H_
#define _MANDELBROT_LIB_MANDELBROT_THREAD_QT_H_

#include <atomic>
#include <QImage>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "mandelbrot.h"

namespace mandelbrot
{

class MandelbrotThreadQt : public QThread
{
    Q_OBJECT

public:
    /// Constructs the mandelbrot thread with an optional parent
    explicit MandelbrotThreadQt(QObject *parent = nullptr);

    /// Destructor
    ~MandelbrotThreadQt();

    /// Saves the current mandelbrot image to a file with the given filename
    void saveToFile(const QString &fileName, int colorStrategy, double colorIntensity) const;

    /// Renders the mandelbrot set with the current parameters
    void createImage();

    /// Discards any images that are still being calculated/rendered
    void discardAny();

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

protected:
    /// Entry point in the worker thread. Invokes \ref MandelbrotSet::render() and emits
    /// the outputReady signal with the contents of the \ref OutputDeviceQt
    void run() override;

Q_SIGNALS:
    /// Emitted when the mandelbrot image has finished rendering
    void outputReady(const QImage &image, double scale);

private:
    /// Synchronization object
    QMutex m_mutex;

    /// Condition variable
    QWaitCondition m_cv;

    /// Does the actual work in this thread
    MandelbrotSet m_mandelbrotSet;

    /// Flag indicating whether or not the set needs to be re-rendered after the initial job
    bool m_renderAgain;

    /// Flag indicating whether or not the thread needs to stop working
    bool m_quit;

    /// Flag indicating whether or not the current image should be discarded
    std::atomic_bool m_discard;

    // Below parameters are queued for the run() routine
    int m_maxIterations;
    int m_outputWidth;
    int m_outputHeight;
    double m_centerX;
    double m_centerY;
    double m_scale;

    std::unique_ptr<ColorStrategy> m_colorStrategy;
};

}

#endif // _MANDELBROT_LIB_MANDELBROT_THREAD_QT_H_
