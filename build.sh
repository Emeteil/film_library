#!/bin/bash

echo -e "Building project..."

mkdir -p build
cd build

cmake ..
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo -e "Build successful!"
else
    echo "Build failed."
    exit 1
fi