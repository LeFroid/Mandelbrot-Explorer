include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(mandelbrot-ui_src
    mandelbrot-view.cpp
    window.cpp
)

qt5_wrap_ui(mandelbrot-ui_files
    window.ui
)

set(mandelbrot-ui_deps
    ${mandelbrot-ui_src}
    ${mandelbrot-ui_files}
)

add_library(mandelbrot-ui STATIC ${mandelbrot-ui_deps})
target_link_libraries(mandelbrot-ui
    mandelbrot-lib
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)
