#!/usr/bin/env bash
set -euo pipefail

# Usage: ./build.sh [BuildType]
# Default: Debug
# Requires: git, cmake, gcc/g++

BUILD_TYPE="${1:-Debug}"

SCRIPT_DIR="$(cd -- "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
PROJECT_ROOT="${SCRIPT_DIR}"

echo "Updating repository..."
git -C "${PROJECT_ROOT}" pull --rebase

echo "Configuring CMake (${BUILD_TYPE})..."
cmake -S "${PROJECT_ROOT}" \
      -B "${PROJECT_ROOT}/build" \
      -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo "Building project..."
cmake --build "${PROJECT_ROOT}/build" --config "${BUILD_TYPE}"

echo "Done. Binaries are in ${PROJECT_ROOT}/build/bin"
