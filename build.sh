#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
GENERATOR="Ninja"
CONFIG="Debug"  # Change to Release if needed

usage() {
    echo "Usage: $0 [build|clean]"
    exit 1
}

if [ $# -ne 1 ]; then
    usage
fi

COMMAND="$1"

case "$COMMAND" in
    clean)
        echo "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
        ;;
    build)
        echo "Configuring project..."
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake -G "$GENERATOR" \
              -DCMAKE_BUILD_TYPE="$CONFIG" \
              -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
              ..
        echo "Building project..."
        cmake --build . -- -j$(nproc)
        ;;
    *)
        usage
        ;;
esac
