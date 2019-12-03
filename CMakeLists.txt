cmake_minimum_required(VERSION 3.0)

include(compiler.cmake)

option(ENABLE_SHARED_COM "enable shared com" ON)



option(CMAKE_VERBOSE_MAKEFILE "make verbose=1" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-mcpu=arm926ej-s -mapcs -rdynamic)
add_definitions(-mno-unaligned-access -fno-aggressive-loop-optimizations -fno-omit-frame-pointer)
add_definitions("-O2")
add_definitions("-g3")
add_definitions(-Wall)

PROJECT(PROJECT_ONE)

include_directories(${PROJECT_SOURCE_DIR})


#SN1头文件
include_directories("com_inc")
include_directories("comlib/errHandle")

#通用lib
include_directories("${PROJECT_SOURCE_DIR}/comlib")


add_subdirectory(comlib)
link_libraries(comlib)

add_subdirectory(capmain)
add_subdirectory(JDsvc)



#install scripts
add_subdirectory(workscript)






