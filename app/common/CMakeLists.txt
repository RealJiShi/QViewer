cmake_minimum_required(VERSION 3.4.1)

# set opengl es 3.0
set(OPENGL_LIB GLESv3)

# include
include_directories(./)
if (ANDROID)
    include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
endif(ANDROID)

# src
aux_source_directory(./src SRC)
add_library(common SHARED ${SRC})

# library
target_link_libraries(common ${OPENGL_LIB} android native_app_glue EGL log)

