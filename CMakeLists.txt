cmake_minimum_required(VERSION 3.0)
SET(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)


add_definitions(-mcpu=arm926ej-s -mno-unaligned-access -fno-aggressive-loop-optimizations -fno-omit-frame-pointer -mapcs -rdynamic)

PROJECT(PROJECT_ONE)
#add_subdirectory(mod1 lib)          # 添加一个模块，并且将编译好库文件放置在 build/lib 目录
add_executable(main video.c)
#target_link_libraries(main mod1) 