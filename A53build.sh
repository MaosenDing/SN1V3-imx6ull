#! /bin/bash

build="tmp"
workPath="/a53root"



ver=`date +%Y-%m-%d`
installword='install/strip'
while getopts "v:d" arg
do
	case $arg in
		v)
			ver=$OPTARG
			;;
		d)
			installword="install"
			;;
		?)
			echo 'unknown argument'
			;;
	esac
done


mkdir ${build}
cd ${build}
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath} -Dver=${ver} -DCMAKE_TOOLCHAIN_FILE=../compiler.cmake -DCPU=A53
cpunum=$(cat /proc/cpuinfo | grep processor | wc -l)
make -j${cpunum}
sudo make $installword

cd ../update
make clean
./cp.sh
make VERSION=_${ver}
cp aim/*.bin ../
make clean


