cmake_minimum_required(VERSION 3.0)

include(compiler.cmake)

option(ENABLE_SHARED_COM "enable shared com" ON)
option(ENABLE_SHARED_ERRHANDLE "enable shared errHandle" ON)
option(ENABLE_SHARED_TIM "enable shared tim" ON)


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


#imageProcess
add_subdirectory(imageProcess)
target_link_libraries(main imageProcess)
#errHandle
add_subdirectory(errHandle)
target_link_libraries(main errHandle)
#sensor
add_subdirectory(sensor)
target_link_libraries(main sensor)
#CRE
add_subdirectory(CRE)
target_link_libraries(main CRE)
#text
add_subdirectory(text)
target_link_libraries(main text)
#datapro
add_subdirectory(datapro)
target_link_libraries(main datapro)
#timework
add_subdirectory(timework)
target_link_libraries(main timework)


target_link_libraries(main imageProcess)
target_link_libraries(main errHandle)
target_link_libraries(main sensor)
target_link_libraries(main CRE)
target_link_libraries(main text)
target_link_libraries(main datapro)
target_link_libraries(main timework)

add_subdirectory(comlib)
target_link_libraries(main comlib)

add_subdirectory(JDsvc)


#install main
INSTALL(TARGETS main
       RUNTIME DESTINATION bin
)
#install scripts
add_subdirectory(workscript)






