#! /bin/bash

build="tmp"
workPath="/nucroot"



mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath}
make install -j8
make install/strip
#cp ../workscript/* ${workPath}
#cp main ${workPath}
#file main
#arm-linux-gnueabi-strip ${workPath}/main
#file ${workPath}/main








