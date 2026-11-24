#!/usr/bin/env bash
set -euo pipefail

# --- Configuration ---
BUILD_DIR="builddir"
BIN_FILE="${BUILD_DIR}/main.bin"
ELF_FILE="${BUILD_DIR}/main.elf"
FLASH_FILE="${BUILD_DIR}/flash.bin"
FLASH_OFFSET=0             # C6 ROM loads the image from flash offset 0x0
GDB_PORT=1234
EMU_LOG="${BUILD_DIR}/emu.log"

if command -v riscv32-esp-elf-gdb >/dev/null 2>&1; then
  GDB="riscv32-esp-elf-gdb"
elif [[ -x "$HOME/.espressif/v6.1/tools/tools/riscv32-esp-elf-gdb/17.1_20260402/riscv32-esp-elf-gdb/bin/riscv32-esp-elf-gdb" ]]; then
  GDB="$HOME/.espressif/v6.1/tools/tools/riscv32-esp-elf-gdb/17.1_20260402/riscv32-esp-elf-gdb/bin/riscv32-esp-elf-gdb"
else
  echo "Error: riscv32-esp-elf-gdb not found." >&2
  exit 1
fi

# 1. Verify build artifacts exist
if [[ ! -f "$BIN_FILE" || ! -f "$ELF_FILE" ]]; then
    echo "Error: Missing $BIN_FILE or $ELF_FILE. Build your project first."
    exit 1
fi

# 2. Create a 4 MB zero-padded flash image and write main.bin at 0x1000
echo "[1/3] Creating 4 MB flash image at $FLASH_FILE..."
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

# 3. Start esp-emu in the background, halted at the reset vector until GDB
#    connects (--gdb-halt); it stays halted until you 'continue' in GDB.
echo "[2/3] Starting esp-emu (esp32c6) with GDB stub on port $GDB_PORT..."
esp-emu \
    --chip esp32c6 \
    --firmware "$FLASH_FILE" \
    --gdb-halt \
    --gdb "$GDB_PORT" \
    > "$EMU_LOG" 2>&1 &

EMU_PID=$!
trap 'kill "$EMU_PID" 2>/dev/null || true' EXIT

# Wait for the GDB stub to bind the TCP port
sleep 0.5

# 4. Launch GDB and connect to esp-emu
echo "[3/3] Launching riscv32-esp-elf-gdb..."
echo "      (UART output is logged to $EMU_LOG)"
cgdb -d "$GDB" -- "$ELF_FILE" -ex "target remote :$GDB_PORT"
