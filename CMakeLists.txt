cmake_minimum_required(VERSION 3.1.0)

project(Mandelbrot-Set VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(ENABLE_QT "Enable QT GUI" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -msse2 -msse3 -msse4")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

if (ENABLE_QT)
    set(QT_VERSION_MIN "5.9.0")
    find_package(Qt5 ${QT_VERSION_MIN} REQUIRED COMPONENTS Core Gui Widgets)
endif()

find_package(MPFR REQUIRED)
if(NOT MPFR_FOUND)
    message(FATAL_ERROR "Could not find MPFR!")
endif()

find_package(GMP REQUIRED)
if(NOT GMP_FOUND)
    message(FATAL_ERROR "Could not find GMP!")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/src/lib
    ${GMP_INCLUDES}
    ${MPFR_INCLUDES}
)

add_subdirectory(src)
