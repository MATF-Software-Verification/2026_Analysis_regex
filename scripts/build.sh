#!/bin/bash
set -e

mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=/c/vcpkg/scripts/buildsystems/vcpkg.cmake ..
make -j