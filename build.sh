#! /bin/bash

build="tmp"
workPath="/nucroot"


if [ $# -lt 1 ] ; then
	ver="default80"
else
	ver=$1
fi

mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath} -Dver=${ver}
cpunum=$(cat /proc/cpuinfo | grep processor | wc -l)
make -j${cpunum}
#sudo make install/strip
sudo make install
cd ../update
make clean
./cp.sh
make VERSION=_${ver} $2 
cp aim/*.bin ../
make clean


