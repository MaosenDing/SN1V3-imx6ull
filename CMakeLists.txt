cmake_minimum_required(VERSION 3.0)

option(ENABLE_SHARED_COM "enable shared com" ON)
option(CMAKE_VERBOSE_MAKEFILE "make verbose=1" OFF)

add_definitions(-DVERSION_MDC="${ver}")
message("using VERSION =${ver}")

PROJECT(PROJECT_ONE)

include_directories(${PROJECT_SOURCE_DIR})

#SN1头文件
include_directories("com_inc")
include_directories("comlib/errHandle")

#通用lib
include_directories("${PROJECT_SOURCE_DIR}/comlib")


add_subdirectory(comlib)


add_subdirectory(capmain)
add_subdirectory(JDsvc)
add_subdirectory(wifiSVC)
add_subdirectory(daemon)

#测试用
IF (DEFINED  TABLE_TEST)
add_subdirectory(tableTestFile)
ENDIF()



#install scripts
add_subdirectory(workscript)
#update config file
add_subdirectory(config_file)





