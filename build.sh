#! /bin/bash

build="_build"
mkdir ${build}
cd ${build}
cmake ..
make -j8
cp main /nucroot









