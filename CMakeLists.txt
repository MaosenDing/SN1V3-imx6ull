cmake_minimum_required(VERSION 3.0)
SET(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)


add_definitions(-mcpu=arm926ej-s -mno-unaligned-access -fno-aggressive-loop-optimizations -fno-omit-frame-pointer -mapcs -rdynamic)

PROJECT(PROJECT_ONE)
//添加库目录
//libjpeg
link_directories("${PROJECT_SOURCE_DIR}/jpeg")





add_executable(main video.c)
//添加链接
//libjpeg.so
target_link_libraries(main -ljpeg)





 