#! /bin/bash

build="tmp"
workPath="/nucroot"



mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath} $1
cpunum=$(cat /proc/cpuinfo | grep processor | wc -l)
make -j${cpunum}
sudo make install/strip


