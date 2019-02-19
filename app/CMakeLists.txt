cmake_minimum_required(VERSION 3.4.1)

# general compiler options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -Wall")

#
set(${CMAKE_INCLUDE_CURRENT_DIR} on)

#
if (ANDROID)
    # global android native app glue
    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
endif (ANDROID)

# subdirectory
add_subdirectory(./common)
add_subdirectory(./util)
add_subdirectory(./src/main/cpp)
