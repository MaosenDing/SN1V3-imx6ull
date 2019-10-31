cmake_minimum_required(VERSION 3.0)
SET(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)
add_compile_options(-std=c++11)

add_definitions(-mcpu=arm926ej-s -mno-unaligned-access -fno-aggressive-loop-optimizations -fno-omit-frame-pointer -mapcs -rdynamic)

PROJECT(PROJECT_ONE)

link_directories("${PROJECT_SOURCE_DIR}/jpeg")
include_directories("errHandle")
add_executable(main video.cpp)

#添加链接
#libjpeg.so

include_directories("jpeg")
target_link_libraries(main -ljpeg)



#SN1头文件
include_directories("com_inc")

#imageProcess
add_subdirectory(imageProcess 1)

#errHandle
add_subdirectory(errHandle 2)


 