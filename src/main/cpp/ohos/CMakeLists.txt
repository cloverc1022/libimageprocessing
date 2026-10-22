cmake_minimum_required(VERSION 3.18)

project(imageprocessing)

get_filename_component(CPP_ROOT ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

if(NOT DEFINED OHOS_ARCH)
    set(OHOS_ARCH arm64-v8a)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CPP_ROOT}/output/ohos/release/${OHOS_ARCH})

set(X264_ROOT ${CPP_ROOT}/x264/${OHOS_ARCH})
if(NOT EXISTS "${X264_ROOT}/lib/libx264.a")
    message(FATAL_ERROR
            "Missing OHOS libx264.a at ${X264_ROOT}/lib/libx264.a\n"
            "Run scripts/build_x264_ohos.ps1 or copy the OHOS static library there.")
endif()

include_directories(
        ${CPP_ROOT}/include
        ${CPP_ROOT}/libyuv/include
        ${X264_ROOT}/include)

link_directories(${X264_ROOT}/lib)

add_subdirectory(${CPP_ROOT}/libyuv ${CMAKE_CURRENT_BINARY_DIR}/libyuv)

add_compile_definitions(BUILD_FOR_OHOS=1 ST_NO_EXCEPTION_HANDLING)

add_library(${PROJECT_NAME} SHARED
        ${CPP_ROOT}/imageprocessing_napi.cpp
        ${CPP_ROOT}/imageprocessing_native.cpp
        ${CPP_ROOT}/VideoChannel.cpp)

target_link_libraries(${PROJECT_NAME}
        yuv
        x264
        ace_napi.z
        hilog_ndk.z
        c++_shared
        -Wl,--no-undefined)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -fvisibility=hidden -Os)
