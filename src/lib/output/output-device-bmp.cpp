#include <algorithm>
#include <cstring>
#include <fstream>
#include "output-device-bmp.h"

namespace mandelbrot
{
    void OutputDeviceBMP::setFileName(const std::string &fileName)
    {
        m_fileName = fileName;
    }

    void OutputDeviceBMP::setDimensions(int32_t width, int32_t height)
    {
        if (width <= 0 || height <= 0)
            return;

        m_width = width;
        m_height = height;

        m_data.clear();
        m_data.resize(height * width);
        //m_data.reserve(height * width);
    }

    void OutputDeviceBMP::write(int xOffset, int yOffset, std::vector<color_t> &&data)
    {
        const size_t pos = static_cast<size_t>((yOffset * m_width) + xOffset);
        const size_t len = pos + data.size() >= m_data.size() ? m_data.size() - pos : data.size();
        //m_data.insert(m_data.end(), std::make_move_iterator(data.begin()), std::make_move_iterator(data.end()));
        std::copy(data.begin(), data.begin() + len, m_data.begin() + pos);
    }

    void OutputDeviceBMP::flush()
    {
        if (m_fileName.empty() || m_width == 0 || m_height == 0)
            return;

        std::ofstream out { m_fileName, std::ios_base::binary };
        if (!out.is_open())
            return;

        writeHeader(out);
        out.write((const char*)m_data.data(), m_data.size() * BMP_NumChannels);
    }

    void OutputDeviceBMP::writeHeader(std::ofstream &out)
    {
        // Instantiate & build the three header sections of the BMP file
        BitmapFileHeader fileHeader;
        fileHeader.dataOffset = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + sizeof(BitmapColorSpaceHeader);
        fileHeader.fileSize = fileHeader.dataOffset + (m_data.size() * BMP_NumChannels); //(m_height * m_width * BMP_NumChannels);

        BitmapInfoHeader infoHeader;
        infoHeader.infoHeaderSize = sizeof(BitmapInfoHeader) + sizeof(BitmapColorSpaceHeader);
        infoHeader.width = m_width;
        infoHeader.height = m_height;

        BitmapColorSpaceHeader colorSpaceHeader;

        // Save metadata
        out.write((const char*)&fileHeader, sizeof(fileHeader));
        out.write((const char*)&infoHeader, sizeof(infoHeader));
        out.write((const char*)&colorSpaceHeader, sizeof(colorSpaceHeader));
    }
}
