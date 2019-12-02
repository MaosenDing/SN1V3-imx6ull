cmake_minimum_required(VERSION 3.0)

include(compiler.cmake)

option(ENABLE_SHARED_COM "enable shared com" ON)
#option(ENABLE_SHARED_ERRHANDLE "enable shared errHandle" ON)
#option(ENABLE_SHARED_TIM "enable shared tim" ON)


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
include_directories("errHandle")

#通用lib
include_directories("${PROJECT_SOURCE_DIR}/comlib")


#主目录编译文件
add_executable(main aim.cpp)

#sensor
add_subdirectory(sensor)
target_link_libraries(main sensor)


add_subdirectory(comlib)
target_link_libraries(main comlib)

add_subdirectory(JDsvc)


#install main
INSTALL(TARGETS main
       RUNTIME DESTINATION bin
)
#install scripts
add_subdirectory(workscript)






