#ifndef _MANDELBROT_LIB_OUTPUT_DEVICE_QT_H_
#define _MANDELBROT_LIB_OUTPUT_DEVICE_QT_H_

#include <vector>

#include "color/color.h"
#include "output/output-device.h"

#include <QImage>

namespace mandelbrot
{

class OutputDeviceQt final : public OutputDevice
{
public:
    void setDimensions(int32_t width, int32_t height) override;

    void write(int xOffset, int yOffset, std::vector<color_t> &&data) override;

    void flush() override;

    const QImage &getOutput() const;

private:
    int32_t m_width;
    int32_t m_height;

    std::vector<color_t> m_data;

    /// Output device, encapsulated by this class
    QImage m_image;
};

}

#endif // _MANDELBROT_LIB_OUTPUT_DEVICE_QT_H_
