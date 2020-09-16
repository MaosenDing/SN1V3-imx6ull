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