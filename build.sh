#! /bin/bash

build="tmp"
workPath="/nucroot"



mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath} -Dver=$1
cpunum=$(cat /proc/cpuinfo | grep processor | wc -l)
make -j${cpunum}
sudo make install/strip

cd ../update
make clean
./cp.sh
make VERSION=_$1 $2 
cp aim/*.bin ../
make clean

