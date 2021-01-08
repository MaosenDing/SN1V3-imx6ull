SET(BUILDROOT_PATH /home/dms/linux/IMX6ULL/ALPHA/buildroot-2020.02.3/output/host)
#SET(MYSYSROOT ${BUILDROOT_PATH}/arm-buildroot-linux-gnueabihf/sysroot)
SET(MYSYSROOT ${BUILDROOT_PATH}/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/sysroot)
SET(CMAKE_SYSROOT ${MYSYSROOT})
SET(toolpathprefix ${BUILDROOT_PATH}/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-)


SET(CMAKE_C_COMPILER ${toolpathprefix}gcc)
SET(CMAKE_CXX_COMPILER ${toolpathprefix}g++)
SET(CMAKE_STRIP  ${toolpathprefix}strip)
SET(CMAKE_AR ${toolpathprefix}ar)
SET(CMAKE_LD ${toolpathprefix}ld)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 --sysroot=/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -rdynamic -funwind-tables -mfpu=neon")
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
add_definitions("-g3 -DDEBUG")
add_definitions(-Wall)
add_definitions(-DCORTEX=1)



