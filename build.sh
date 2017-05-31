#!/bin/bash
cd ./build


if [ $1 == "win32" ]
then
echo "Target windows 32 lib."
cmake -DCMAKE_TOOLCHAIN_FILE=../toolChain_win32.cmake ..
fi

if [ $1 == "win64" ]
then
echo "Target windows 64 lib"
cmake -DCMAKE_TOOLCHAIN_FILE=../toolChain_win64.cmake ..
fi

if [ $1 == "linux" ]
then
echo "Target linux lib."
cmake ..
fi

make -j8
make install

