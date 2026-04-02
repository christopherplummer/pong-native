#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY="${SCRIPT_DIR}/pong_native"

"${SCRIPT_DIR}/detect_linux_ui.sh"

if [[ ! -x "${BINARY}" ]]; then
    echo "Linux binary not found. Building first."
    "${SCRIPT_DIR}/build.sh"
fi

exec "${BINARY}"
