#! /bin/bash

build="tmp"
workPath="/nucroot"



mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath}
make -j8
sudo make install/strip


