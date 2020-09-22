#! /bin/bash

build="tmp"
workPath="/nucroot"


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
cmake .. -DCMAKE_INSTALL_PREFIX=${workPath} -Dver=${ver} -DCMAKE_TOOLCHAIN_FILE=../compiler.arm9.cmake
cpunum=$(cat /proc/cpuinfo | grep processor | wc -l)
make -j${cpunum}
make $installword

cd ../update
make clean
./cp.sh
make VERSION=_${ver} $2 
cp aim/*.bin ../
make clean


