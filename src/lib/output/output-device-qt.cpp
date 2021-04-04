#include "output/output-device-qt.h"

#include <cstring>
#include <QColor>

namespace mandelbrot
{
    void OutputDeviceQt::setDimensions(int32_t width, int32_t height)
    {
        if (width <= 0 || height <= 0)
            return;

        m_width = width;
        m_height = height;
        m_data.clear();
        m_data.resize(m_width * m_height);
        m_image = QImage(width, height, QImage::Format_ARGB32);
    }

    void OutputDeviceQt::write(int xOffset, int yOffset, std::vector<color_t> &&data)
    {
        const size_t pos = static_cast<size_t>((yOffset * m_width) + xOffset);
        const size_t len = pos + data.size() >= m_data.size() ? m_data.size() - pos : data.size();
        std::copy(data.begin(), data.begin() + len, m_data.begin() + pos);
        /*
        if ((yOffset * m_height) + xOffset + static_cast<int>(data.size()) > m_width * m_height)
            return;

        uint32_t *handle = ((uint32_t*)m_image.scanLine(yOffset)) + xOffset;

        for (color_t pixel : data)
        {
            *handle++ = pixel.raw;
        }*/
    }

    void OutputDeviceQt::flush()
    {
        for (int32_t y = 0; y < m_height; ++y)
        {
            color_t *dataHandle = m_data.data() + (y * m_width);
            uint32_t *handle = (uint32_t*)m_image.scanLine(y);
            for (int32_t x = 0; x < m_width; ++x)
                *handle++ = (dataHandle++)->raw;
                //*handle++ = m_data.at((y * m_width) + x).raw;
        }

        //uint32_t *handle = (uint32_t*)m_image.scanLine(0);
        //std::memcpy(handle, m_data.data(), m_data.size());
    }

    const QImage &OutputDeviceQt::getOutput() const
    {
        return m_image;
    }
}
