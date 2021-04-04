#ifndef _MANDELBROT_LIB_OUTPUT_DEVICE_H_
#define _MANDELBROT_LIB_OUTPUT_DEVICE_H_

#include <cstdint>
#include <vector>

#include "color/color.h"

namespace mandelbrot
{

class OutputDevice
{
public:
    virtual void setDimensions(int32_t width, int32_t height) = 0;
    virtual void write(int xOffset, int yOffset, std::vector<color_t> &&data) = 0;
    virtual void flush() = 0;
};

}

#endif // _MANDELBROT_LIB_OUTPUT_DEVICE_H_

