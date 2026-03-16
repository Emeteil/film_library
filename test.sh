#!/bin/bash

echo "Building tests..."

mkdir -p build
cd build
cmake ..
make -j$(nproc) film_library_tests

if [ $? -eq 0 ]; then
    echo "Running tests..."
    ./tests/film_library_tests
else
    echo "Test build failed."
    exit 1
fi