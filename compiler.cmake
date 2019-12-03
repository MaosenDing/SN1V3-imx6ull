
SET(toolpathprefix /share/nuc/buildroot-2019.02.1/output/host/bin/arm-buildroot-linux-gnueabi-)


SET(CMAKE_C_COMPILER ${toolpathprefix}gcc)
SET(CMAKE_CXX_COMPILER ${toolpathprefix}g++)
SET(CMAKE_STRIP  ${toolpathprefix}strip)
SET(CMAKE_AR ${toolpathprefix}ar)

