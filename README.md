# Mandelbrot-Explorer
Interactive and modular Mandelbrot set renderer

Licensed under GPLv3

## Features

*   Multiple cross-platform frontends - CLI and a Qt GUI
*   Customizable coloring algorithms, plug-and-play at runtime
*   Near infinite zooming
*   Multithreaded rendering
*   Lightweight

## Building

The project can be built using the cmake build system, by either importing the root CMakeLists file into your IDE of choice or performing the following commands from a console:

```console
$ git clone https://github.com/LeFroid/Mandelbrot-Explorer.git mandelbrot-explorer
$ mkdir -p mandelbrot-explorer/build && cd mandelbrot-explorer/build
$ cmake .. [-ENABLE_QT=(ON|OFF)]
$ make
```

## Dependencies

*   Modern C++ compiler, supporting c++17 or greater
*   CMake version 3.1.0 or greater
*   Qt version 5.9.0 or greater for the GUI
*   GMP
*   MPFR


