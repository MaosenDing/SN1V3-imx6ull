
IF (CPU STREQUAL "arm9")

SET(toolpathprefix /share/nuc/buildroot-2019.02.1/output/host/bin/arm-buildroot-linux-gnueabi-)


SET(CMAKE_C_COMPILER ${toolpathprefix}gcc)
SET(CMAKE_CXX_COMPILER ${toolpathprefix}g++)
SET(CMAKE_STRIP  ${toolpathprefix}strip)
SET(CMAKE_AR ${toolpathprefix}ar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-mcpu=arm926ej-s -mapcs -rdynamic)
add_definitions(-mno-unaligned-access -fno-aggressive-loop-optimizations -fno-omit-frame-pointer)
add_definitions("-O2")
add_definitions("-g3")
add_definitions(-Wall)

ENDIF()

IF (CPU STREQUAL "A7")
SET(toolpathprefix /share/imx/buildroot-2020.02.3/output/host/bin/arm-buildroot-linux-gnueabihf-)

SET(CMAKE_C_COMPILER ${toolpathprefix}gcc)
SET(CMAKE_CXX_COMPILER ${toolpathprefix}g++)
SET(CMAKE_STRIP  ${toolpathprefix}strip)
SET(CMAKE_AR ${toolpathprefix}ar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-mcpu=cortex-a7 -mapcs -rdynamic)
add_definitions(-mfpu=neon-vfpv4)
add_definitions("-O2")
add_definitions("-g3")
add_definitions(-Wall)
add_definitions(-DCORTEX=1)

ENDIF()
