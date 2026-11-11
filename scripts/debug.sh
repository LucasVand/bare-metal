
#!/usr/bin/env bash
set -euo pipefail

# --- Configuration ---
BUILD_DIR="builddir"
BIN_FILE="${BUILD_DIR}/main.bin"
ELF_FILE="${BUILD_DIR}/main.elf"
FLASH_FILE="${BUILD_DIR}/flash.bin"
FLASH_OFFSET=4096          # 0x1000 (standard ESP32 bootloader/bare-metal offset)
GDB_PORT=1234

ROM_FILE="/usr/local/share/qemu/esp32-v3-rom.bin"

# 1. Verify build artifacts exist
if [[ ! -f "$BIN_FILE" || ! -f "$ELF_FILE" ]]; then
    echo "Error: Missing $BIN_FILE or $ELF_FILE. Build your project first."
    exit 1
fi

# 2. Create a 4 MB zero-padded flash image and write main.bin at 0x1000
echo "[1/3] Creating 4 MB flash image at $FLASH_FILE..."
dd if=/dev/zero of="$FLASH_FILE" bs=1M count=4 status=none
dd if="$BIN_FILE" of="$FLASH_FILE" bs=1 seek="$FLASH_OFFSET" conv=notrunc status=none

# 3. Start QEMU in the background and ensure it closes when this script exits
echo "[2/3] Starting qemu-system-xtensa on GDB port $GDB_PORT..."
qemu-system-xtensa \
    -nographic \
    -machine esp32 \
    -drive file="$FLASH_FILE",if=mtd,format=raw \
    -gdb tcp::"$GDB_PORT" \
    -S \
    > "${BUILD_DIR}/qemu_uart.log" 2>&1 &

QEMU_PID=$!
trap 'kill "$QEMU_PID" 2>/dev/null || true' EXIT

# Give QEMU a brief moment to bind the TCP port
sleep 0.3

# 4. Launch GDB, connect to QEMU, and set a breakpoint at main
echo "[3/3] Launching xtensa-esp32-elf-gdb..."
echo "      (UART output is logged to ${BUILD_DIR}/qemu_uart.log)"
TERM=xterm-256color xtensa-esp32-elf-gdb "$ELF_FILE" \
    -ex "target remote :$GDB_PORT" \
