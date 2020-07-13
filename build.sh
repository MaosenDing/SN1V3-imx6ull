#! /bin/bash

build="tmp"
workPath="/nucroot"


if [ $# -lt 1 ] ; then
	ver=`date +%Y-%m-%d`
else
	ver=$1
fi

mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath} -Dver=${ver} -DCMAKE_TOOLCHAIN_FILE=../compiler.cmake
cpunum=$(cat /proc/cpuinfo | grep processor | wc -l)
make -j${cpunum}
sudo make install/strip

cd ../update
make clean
./cp.sh
make VERSION=_${ver} $2 
cp aim/*.bin ../
make clean


