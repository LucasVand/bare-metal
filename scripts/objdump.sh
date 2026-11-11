#!/usr/bin/env bash
EXE="$1"

if [ ! -f "$EXE" ]; then
    echo "Error: ELF file not found at $EXE."
    exit 1
fi

# Force read to pull directly from the physical terminal
if [ -z "$FUNC" ]; then
    read -p "Enter function name to disassemble: " FUNC < /dev/tty
fi

# Double check they didn't just hit enter
if [ -z "$FUNC" ]; then
    echo "Error: No function name provided."
    exit 1
fi


# Run objdump, demangle symbols (-C), disassemble all (-d), 
# and use sed to extract from '<function_name>:' until the next empty line
ASM_OUTPUT=$(xtensa-esp32-elf-objdump -C -d "$EXE" | sed -n "/<$FUNC>:/,/^$/p")

if [ -z "$ASM_OUTPUT" ]; then
    echo "Error: Function '$FUNC' not found in ELF."
    echo "Check spelling, or ensure it wasn't aggressively inlined by the compiler (-O2/-O3)."
    exit 1
fi

echo "$ASM_OUTPUT"
