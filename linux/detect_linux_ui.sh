#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

echo "Linux environment detection for pong-native"
echo "Repository root: ${ROOT_DIR}"
echo

if [[ -r /etc/os-release ]]; then
    # shellcheck disable=SC1091
    . /etc/os-release
    echo "Distro: ${PRETTY_NAME:-unknown}"
else
    echo "Distro: unavailable (/etc/os-release not found)"
fi

echo "XDG_CURRENT_DESKTOP: ${XDG_CURRENT_DESKTOP:-unset}"
echo "DESKTOP_SESSION: ${DESKTOP_SESSION:-unset}"
echo "XDG_SESSION_TYPE: ${XDG_SESSION_TYPE:-unset}"
echo

echo "Framework checks:"

if pkg-config --exists sdl2; then
    echo "  SDL2: found via pkg-config"
    SDL2_FOUND=1
else
    echo "  SDL2: missing"
    SDL2_FOUND=0
fi

if pkg-config --exists gtk4; then
    echo "  GTK4: found"
elif pkg-config --exists gtk+-3.0; then
    echo "  GTK3: found"
else
    echo "  GTK: not detected"
fi

if pkg-config --exists Qt6Widgets; then
    echo "  Qt6 Widgets: found"
elif pkg-config --exists Qt5Widgets; then
    echo "  Qt5 Widgets: found"
else
    echo "  Qt Widgets: not detected"
fi

echo
echo "Selection rationale:"
echo "  Desktop shell alone does not determine the best game framework."
echo "  This project prefers SDL2 on Linux when available because it is"
echo "  practical for games and provides windowing, input, and rendering"
echo "  with less platform-specific overhead than a general UI toolkit."

if [[ "${SDL2_FOUND}" -eq 1 ]]; then
    echo
    echo "Chosen Linux framework: SDL2"
    exit 0
fi

echo
echo "Failure: SDL2 development files are required for the Linux build."
echo "Install SDL2 development packages and ensure pkg-config can resolve 'sdl2'."
echo "Examples:"
echo "  Debian/Ubuntu: sudo apt install libsdl2-dev pkg-config"
echo "  Fedora:        sudo dnf install SDL2-devel pkgconf-pkg-config"
echo "  Arch:          sudo pacman -S sdl2 pkgconf"
exit 1
