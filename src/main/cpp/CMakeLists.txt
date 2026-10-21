cmake_minimum_required(VERSION 3.18.1)

project("imageprocessing")

include_directories(${CMAKE_SOURCE_DIR}/x264/${ANDROID_ABI}/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/x264/${ANDROID_ABI}/lib")

include_directories(${CMAKE_SOURCE_DIR}/libyuv/include)

add_subdirectory(libyuv)

add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        imageprocessing.cpp
        VideoChannel.cpp
        )


target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        yuv
        x264
        android
        log)