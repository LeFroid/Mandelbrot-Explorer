#ifndef _MANDELBROT_LIB_COLOR_H_
#define _MANDELBROT_LIB_COLOR_H_

#include <cstdint>

namespace mandelbrot
{
    typedef union colorTag
    {
        uint32_t raw;
        struct argbTag
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        } argb;

    } color_t;
    // using color_t = uint32_t;
}

#endif // _MANDELBROT_LIB_COLOR_H_

