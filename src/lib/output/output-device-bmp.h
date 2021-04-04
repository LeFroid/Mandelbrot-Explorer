#ifndef _MANDELBROT_LIB_OUTPUT_DEVICE_BMP_H_
#define _MANDELBROT_LIB_OUTPUT_DEVICE_BMP_H_

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "color/color.h"
#include "output/output-device.h"

namespace mandelbrot
{

/**
 * @class OutputDeviceBMP
 * @brief Represents a handle to a BMP-formatted file, in which
 *        the output of a mandelbrot set calculation will be written.
 */
class OutputDeviceBMP final : public OutputDevice
{
public:
    void setFileName(const std::string &fileName);

    /**
     * @brief Sets the dimensions of the file
     * @param width Width of the file, in pixels
     * @param height Height of the file, in pixels
     */
    void setDimensions(int32_t width, int32_t height) override;

    void write(int xOffset, int yOffset, std::vector<color_t> &&data) override;

    void flush() override;

private:
    void writeHeader(std::ofstream &out);

private:
    std::string m_fileName;

    std::vector<color_t> m_data;

    int32_t m_width;
    int32_t m_height;
};

#pragma pack(push, 1)

struct BitmapFileHeader
{
    uint16_t id { 0x4D42 };
    uint32_t fileSize { 0 };
    uint32_t reservedSpace { 0 };
    uint32_t dataOffset { 0 };
};

struct BitmapInfoHeader
{
    uint32_t infoHeaderSize { 0 };

    int32_t width { 0 };
    int32_t height { 0 };

    uint16_t planes { 1 };

    // 24 bits for RGB, or 32 for RGBA
    uint16_t bitsPerPixel { 32 }; 

    uint32_t compression { 3 };
    uint32_t rawImageSize { 0 };

    int32_t horizontalResolution { 0 };
    int32_t verticalResolution { 0 };

    // Number of colors in the palette, defaults to 2^n when set to 0
    uint32_t colorCount { 0 };
    
    // The number of important colors used, or 0 when every color is important; generally ignored
    uint32_t importantColorCount { 0 };
};

struct BitmapColorSpaceHeader
{
    uint32_t redColorMask   { 0x00FF0000 };
    uint32_t greenColorMask { 0x0000FF00 };
    uint32_t blueColorMask  { 0x000000FF };
    uint32_t alphaColorMask { 0xFF000000 };

    // "sRGB"
    uint32_t colorMaskIdentifier { 0x73524742 };

    uint32_t reserved[16] { 0 };
};

#pragma pack(pop)

static constexpr uint16_t BMP_NumChannels = 4;

}

#endif // _MANDELBROT_LIB_OUTPUT_DEVICE_BMP_H_

