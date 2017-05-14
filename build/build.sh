#!/bin/bash

CMAKE_INSTALL=cmake_install.cmake
CMAKE_CACHE=CMakeCache.txt
CMAKE_PATH=..

make uninstall
rm -rf *cmake* *CMake* *Make* lib
cmake $CMAKE_PATH
make -j8
make install
