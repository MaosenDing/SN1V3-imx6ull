
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


IF (CPU STREQUAL "A53")
#SET(MYSYSROOT /share/a53/buildroot-2020.02.3/output/host/arm-buildroot-linux-gnueabihf/sysroot)
#SET(CMAKE_SYSROOT ${MYSYSROOT})
#SET(CMAKE_FIND_ROOT_PATH /opt/fsl-imx-xwayland/4.14-sumo)

SET(toolpathprefix /share/a53/buildroot-2020.02.3/output/host/bin/arm-buildroot-linux-gnueabihf-)


SET(CMAKE_C_COMPILER ${toolpathprefix}gcc)
SET(CMAKE_CXX_COMPILER ${toolpathprefix}g++)
SET(CMAKE_STRIP  ${toolpathprefix}strip)
SET(CMAKE_AR ${toolpathprefix}ar)
SET(CMAKE_LD ${toolpathprefix}ld)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 --sysroot=/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
#set(ENABLE_EXPORTS on)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME "Linux")
#add_definitions("--sysroot=${MYSYSROOT}")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${MYSYSROOT}" CACHE INTERNAL "" FORCE)

#set(CMAKE_FIND_ROOT_PATH ${MYSYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


add_definitions(-mcpu=cortex-a53)
add_definitions("-O2")
add_definitions("-g3")
add_definitions(-Wall)
add_definitions(-DCORTEX=1)

ENDIF()





