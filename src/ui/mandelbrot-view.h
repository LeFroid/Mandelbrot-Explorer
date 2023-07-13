#ifndef _MANDELBROT_UI_MANDELBROT_VIEW_H_
#define _MANDELBROT_UI_MANDELBROT_VIEW_H_

#include "mandelbrot.h"
#include "threading/mandelbrot-thread-qt.h"

#include <QPixmap>
#include <QWidget>

class MandelbrotView : public QWidget
{
    Q_OBJECT

public:
    /// Constructs the mandelbrot view with a given parent
    explicit MandelbrotView(QWidget *parent = nullptr);

    int getMaxIterations() const noexcept;
    double getColorIntensity() const noexcept;
    double getScale() const noexcept;

    double getCenterX() const noexcept;
    double getCenterY() const noexcept;

Q_SIGNALS:
    void displayUpdated();

public Q_SLOTS:
    void setColorStrategySmooth(bool enable);
    void setColorStrategyIter(bool enable);
    void setColorStrategyWave(bool enable);

    void setMaxIterations(int maxIterations);

    void setScale(double scale);

    void setColorIntensity(double intensity);

    void saveToFile(const QString &fileName, int colorStrategy);

private Q_SLOTS:
    /// Callback for when the latest image has been passed from the worker thread
    void onImageCreated(const QImage &image, double scale);

    /// Scrolls the mandelbrot image by the given delta. This updates the center X and Y coordinates on the plane
    void scrollImage(int dx, int dy);

protected:
    /// Handles the mouse click event, in order to move around the fractal plane
    void mousePressEvent(QMouseEvent *event) override;

    /// Handles the mouse move event, in order to scroll around the fractal plane
    void mouseMoveEvent(QMouseEvent *event) override;

    /// Determines the offset from a drag operation (if any), and updates the mandelbrot image accordingly
    void mouseReleaseEvent(QMouseEvent *event) override;

    /// Paints the fractal image onto the widget
    void paintEvent(QPaintEvent *event) override;

    /// Handles the resize event
    void resizeEvent(QResizeEvent *event) override;

    /// Handles the wheel event (controls zooming in and out)
    void wheelEvent(QWheelEvent *event) override;

private:
    /// Thread performing the set calculations
    mandelbrot::MandelbrotThreadQt m_thread;

    /// Current mandelbrot image
    QPixmap m_pixmap;

    /// Position used to calculate viewport offset in a drag event
    QPoint m_dragPos;

    /// Offset from dragging pixmap around
    QPoint m_dragOffset;

    /// Scale of the mandelbrot set on the pixmap (can be different than current scale)
    double m_pixmapScale;

    /// Center x coordinate on the mandelbrot plane (the real axis)
    double m_centerX;

    /// Center y coordinate on the mandelbrot plane (the imag axis)
    double m_centerY;

    /// Scale of the fractal
    double m_scale;

    /// Color intensity [only for smooth strategy]
    double m_colorIntensity;

    /// Maximum # of iterations before escaping
    int m_maxIterations;
};

#endif // _MANDELBROT_UI_MANDELBROT_VIEW_H_
