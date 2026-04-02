#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
OUTPUT="${SCRIPT_DIR}/pong_native"

"${SCRIPT_DIR}/detect_linux_ui.sh"

if ! command -v nasm >/dev/null 2>&1; then
    echo "Error: nasm is required but was not found in PATH." >&2
    exit 1
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "Error: gcc is required but was not found in PATH." >&2
    exit 1
fi

if ! command -v pkg-config >/dev/null 2>&1; then
    echo "Error: pkg-config is required but was not found in PATH." >&2
    exit 1
fi

mkdir -p "${BUILD_DIR}"

echo "Assembling common/pong.asm"
nasm -f elf64 "${ROOT_DIR}/common/pong.asm" -o "${BUILD_DIR}/pong.o"

echo "Compiling linux/main_sdl.c"
gcc \
    -std=c11 \
    -Wall \
    -Wextra \
    -Wpedantic \
    "${ROOT_DIR}/linux/main_sdl.c" \
    "${BUILD_DIR}/pong.o" \
    -o "${OUTPUT}" \
    $(pkg-config --cflags --libs sdl2)

echo "Built ${OUTPUT}"
