#! /bin/bash

build="tmp"
mkdir ${build}
cd ${build}
cmake ..
make -j8

workPath="/nucroot"

cp ../workscript/* ${workPath}
cp main ${workPath}









