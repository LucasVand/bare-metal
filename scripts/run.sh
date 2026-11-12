#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
cd "$ROOT_DIR"

BUILD_DIR="builddir"
BIN_FILE="${BUILD_DIR}/main.bin"
FLASH_FILE="${BUILD_DIR}/flash.bin"
FLASH_OFFSET=4096

if ! command -v qemu-system-xtensa >/dev/null 2>&1; then
  echo "Error: qemu-system-xtensa not found in PATH." >&2
  exit 1
fi

if [[ ! -f "$BIN_FILE" ]]; then
  echo "Error: $BIN_FILE not found. Run: ninja -C $BUILD_DIR" >&2
  exit 1
fi

python3 - "$BIN_FILE" "$FLASH_FILE" "$FLASH_OFFSET" <<'PY'
import sys

bin_path, flash_path, offset = sys.argv[1], sys.argv[2], int(sys.argv[3])
with open(bin_path, "rb") as f:
    data = f.read()

image = bytearray(4 * 1024 * 1024)
image[offset:offset + len(data)] = data
with open(flash_path, "wb") as f:
    f.write(image)
PY

exec qemu-system-xtensa \
  -nographic \
  -machine esp32 \
  -drive "file=${FLASH_FILE},if=mtd,format=raw" \
  "$@"
