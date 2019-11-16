cmake_minimum_required(VERSION 3.0)
SET(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-mcpu=arm926ej-s -mno-unaligned-access -fno-aggressive-loop-optimizations -fno-omit-frame-pointer -mapcs -rdynamic)
add_definitions(-Wall)

PROJECT(PROJECT_ONE)

include_directories(${PROJECT_SOURCE_DIR})

#添加链接
#libjpeg.so

include_directories("jpeg")
link_directories("${PROJECT_SOURCE_DIR}/jpeg")
#SN1头文件
include_directories("com_inc")
#glog
include_directories("glog")
link_directories("${PROJECT_SOURCE_DIR}/glog")





#主目录编译文件
add_executable(main aim.cpp)

#imageProcess
add_subdirectory(imageProcess)
target_link_libraries(main imageProcess)
#errHandle
add_subdirectory(errHandle)
target_link_libraries(main errHandle)
#errHandle
add_subdirectory(sensor)
target_link_libraries(main sensor)


#添加外部动态链接库 必须放后面 不然链接不过
target_link_libraries(main -lglog)
target_link_libraries(main -ljpeg)

