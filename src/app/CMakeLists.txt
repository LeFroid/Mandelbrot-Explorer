include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (ENABLE_QT)
    include_directories(${CMAKE_SOURCE_DIR}/src/ui)

    set(mandelbrot-qt_src
        app-qt.cpp
    )
    add_executable(mandelbrot-qt ${mandelbrot-qt_src})
    target_link_libraries(mandelbrot-qt
        mandelbrot-lib
        mandelbrot-ui
        Threads::Threads
        ${MPFR_LIBRARIES}
        ${GMP_LIBRARIES}
    )
    install(TARGETS mandelbrot-qt DESTINATION bin)
endif()

add_executable(mandelbrot-bmp app-bmp.cpp)
target_link_libraries(mandelbrot-bmp
    mandelbrot-lib
    Threads::Threads
    ${MPFR_LIBRARIES}
    ${GMP_LIBRARIES}
)
install(TARGETS mandelbrot-bmp DESTINATION bin)
