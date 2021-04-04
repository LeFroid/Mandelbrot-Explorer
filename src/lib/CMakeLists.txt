include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(mandelbrot_lib_src
    color/color-strategy-iteration.cpp
    color/color-strategy-smooth.cpp
    color/color-strategy-wavelength.cpp
    output/output-device-bmp.cpp
    threading/thread-pool.cpp
    mandelbrot.cpp
)

if (ENABLE_QT)
  set(mandelbrot_lib_src ${mandelbrot_lib_src} output/output-device-qt.cpp threading/mandelbrot-thread-qt.cpp)
endif()

add_library(mandelbrot-lib STATIC ${mandelbrot_lib_src})

if (ENABLE_QT)
    target_link_libraries(mandelbrot-lib Qt5::Core Qt5::Gui)
endif()

