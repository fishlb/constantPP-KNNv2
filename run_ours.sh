#!/bin/bash
# ==============================================================================
# Script to run our scheme
# ==============================================================================
echo "================================="
echo "Building constantPP-KNN Project"
echo "================================="

mkdir -p build
cd build || exit

cmake ..
make -j

echo ""
echo "================================="
echo "Build complete."
echo "Launching program..."
echo "================================="

./main