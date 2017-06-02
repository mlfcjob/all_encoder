# all_encoder

host: fedora or linux
target: linux win32 win64


prepare:
@ you need to install cmake
@ you need to install mingw-32 or mingw64
 target win32:
         yum install mingw32-gcc.x86_64
         yum install mingw32-gcc-c++.x86_64
 target win64:
         yum install mingw64-gcc.x86_64
         yum install mingw64-gcc-c++

compile:

1.If You need a program on linux:

./build.sh linux

2.If you need a program on win32:

./build.sh win32

3.If you need a program on win64

./build.sh win64


