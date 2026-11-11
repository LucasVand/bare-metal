#!/usr/bin/env bash

# Meson passed the ELF path as the first argument ($1)
EXE="$1"

# Check if the file actually exists before running
if [ ! -f "$EXE" ]; then
    echo "Error: ELF file not found at $EXE. Did you build it first?"
    exit 1
fi

# Prompt for the address if it wasn't passed via environment variable 'A'
if [ -z "$A" ]; then
    read -p "Enter memory address: " A < /dev/tty
fi

# Run the ESP32 specific tool
xtensa-esp32-elf-addr2line -pfia -e "$EXE" "$A"

