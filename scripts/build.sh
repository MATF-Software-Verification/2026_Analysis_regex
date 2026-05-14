#!/bin/bash
set -e

# mkdir -p build
# cd build
cmake --preset=linux-system
make -j