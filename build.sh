#! /bin/bash

build="tmp"
mkdir ${build}
cd ${build}
cmake ..
make -j8
cp main /nucroot









