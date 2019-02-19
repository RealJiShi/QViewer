cmake_minimum_required(VERSION 3.4.1)

set(OPENGL_LIB GLESv3)

include_directories(../3rd_party/glm)
include_directories(./)

aux_source_directory(./ SRC)
add_library(gl-util SHARED ${SRC})

target_link_libraries(gl-util ${OPENGL_LIB} log)
