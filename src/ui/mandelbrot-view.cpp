#include "mandelbrot-view.h"
#include "color/color-strategy.h"
#include "color/color-strategy-smooth.h"
#include "color/color-strategy-iteration.h"
#include "color/color-strategy-wavelength.h"

#include <cmath>

#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QWheelEvent>

#include <QDebug>

static constexpr double DefaultCenterX = -0.637011;
static constexpr double DefaultCenterY = -0.0395159;
static constexpr double DefaultScale   = 0.00403897;
static constexpr int    DefaultMaxIter = 400;

//set initial color strategy to smooth, after that, update worker thread via signal-slot binding
//managed by the main window class

MandelbrotView::MandelbrotView(QWidget *parent) :
    QWidget(parent),
    m_thread(),
    m_pixmap(),
    m_dragPos(),
    m_dragOffset(),
    m_pixmapScale(DefaultScale),
    m_centerX(DefaultCenterX),
    m_centerY(DefaultCenterY),
    m_scale(DefaultScale),
    m_colorIntensity(-0.1275),
    m_maxIterations(DefaultMaxIter)
{
    m_thread.setCenter(m_centerX, m_centerY);
    m_thread.setColorStrategy(std::make_unique<mandelbrot::ColorStrategySmooth>());
    m_thread.setMaxIterations(m_maxIterations);
    m_thread.setScale(m_scale);

    connect(&m_thread, &mandelbrot::MandelbrotThreadQt::outputReady, this, &MandelbrotView::onImageCreated);
}

int MandelbrotView::getMaxIterations() const noexcept
{
    return m_maxIterations;
}

double MandelbrotView::getColorIntensity() const noexcept
{
    return m_colorIntensity;
}

void MandelbrotView::saveToFile(const QString &fileName, int colorStrategy)
{
    if (!m_pixmap.save(fileName))
        m_thread.saveToFile(fileName, colorStrategy, m_colorIntensity);
}

void MandelbrotView::setColorIntensity(double intensity)
{
    m_colorIntensity = intensity;
    std::unique_ptr<mandelbrot::ColorStrategySmooth> smoothStrategy =
            std::make_unique<mandelbrot::ColorStrategySmooth>();
    smoothStrategy->setColorIntensity(intensity);
    m_thread.setColorStrategy(std::move(smoothStrategy));
    m_thread.createImage();
}

void MandelbrotView::setColorStrategySmooth(bool enable)
{
    if (!enable)
        return;

    setColorIntensity(m_colorIntensity);
}

void MandelbrotView::setColorStrategyIter(bool enable)
{
    if (!enable)
        return;

    m_thread.setColorStrategy(std::make_unique<mandelbrot::ColorStrategyIteration>());
    m_thread.createImage();
}

void MandelbrotView::setColorStrategyWave(bool enable)
{
    if (!enable)
        return;

    m_thread.setColorStrategy(std::make_unique<mandelbrot::ColorStrategyWavelength>());
    m_thread.createImage();
}

void MandelbrotView::setMaxIterations(int maxIterations)
{
    if (maxIterations == m_maxIterations)
        return;

    m_maxIterations = maxIterations;
    m_thread.setMaxIterations(maxIterations);
    m_thread.createImage();
}

void MandelbrotView::setScale(double scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    m_thread.discardAny();
    update();
    m_thread.setScale(m_scale);
    m_thread.createImage();
}

void MandelbrotView::onImageCreated(const QImage &image, double scale)
{
    m_pixmap = QPixmap::fromImage(image);
    m_pixmapScale = scale;
    m_dragOffset = QPoint();
    update();
}

void MandelbrotView::scrollImage(int dx, int dy)
{
    m_centerX += dx * m_scale;
    m_centerY += dy * m_scale;
    m_thread.discardAny();
    update();
    m_thread.setCenter(m_centerX, m_centerY);
    m_thread.createImage();
}

void MandelbrotView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragPos = event->pos();
}

void MandelbrotView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        m_dragOffset += event->pos() - m_dragPos;
        m_dragPos = event->pos();
        update();
    }
}

void MandelbrotView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    m_dragOffset += event->pos() - m_dragPos;
    m_dragPos = QPoint();

    scrollImage((width() - m_pixmap.width()) / 2 - m_dragOffset.x(),
                (height() - m_pixmap.height()) / 2 - m_dragOffset.y());
}

void MandelbrotView::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (m_pixmap.isNull())
        return;

    if (m_pixmapScale != m_scale)
    {
        double zoomRatio = m_pixmapScale / m_scale;
        int newWidth = static_cast<int>(m_pixmap.width() * zoomRatio);
        int newHeight = static_cast<int>(m_pixmap.height() * zoomRatio);
        int newX = m_dragOffset.x() + (m_pixmap.width() - newWidth) / 2;
        int newY = m_dragOffset.y() + (m_pixmap.height() - newHeight) / 2;

        painter.save();
        painter.translate(newX, newY);
        painter.scale(zoomRatio, zoomRatio);

        QRectF exposed = painter.transform().inverted().mapRect(rect()).adjusted(-1, -1, 1, 1);
        painter.drawPixmap(exposed, m_pixmap, exposed);
        painter.restore();
    }
    else
        painter.drawPixmap(m_dragOffset, m_pixmap);
}

void MandelbrotView::resizeEvent(QResizeEvent *event)
{
    m_thread.setOutputDimensions(event->size().width(), event->size().height());
    m_thread.createImage();
}

void MandelbrotView::wheelEvent(QWheelEvent *event)
{
    const QPoint degrees = event->angleDelta() / 8;
    const double numSteps = static_cast<double>(degrees.y()) / 15.0;
    setScale(m_scale * std::pow(0.8, numSteps));
    event->accept();
}
