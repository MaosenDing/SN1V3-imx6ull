SET(BUILDROOT_PATH /share/nuc/buildroot-2019.02.1/output/host)
SET(MYSYSROOT ${BUILDROOT_PATH}/arm-buildroot-linux-gnueab/sysroot)
SET(CMAKE_SYSROOT ${MYSYSROOT})
SET(toolpathprefix ${BUILDROOT_PATH}/bin/arm-buildroot-linux-gnueabi-)




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


