IDF_TOOLS=~/.espressif/v6.1/tools

export ESP_IDF_VERSION=6.1.0
export IDF_TOOLS_PATH=$IDF_TOOLS
export ESP_ROM_ELF_DIR=$IDF_TOOLS/tools/esp-rom-elfs/20241011
export OPENOCD_SCRIPTS=$IDF_TOOLS/tools/openocd-esp32/v0.12.0-esp32-20260703/openocd-esp32/share/openocd/scripts

# xtensa toolchain (xtensa-esp32-elf-gcc, etc.)
export PATH=$IDF_TOOLS/tools/xtensa-esp-elf/esp-15.2.0_20251204/xtensa-esp-elf/bin:$PATH

# xtensa gdb
export PATH=$IDF_TOOLS/tools/xtensa-esp-elf-gdb/17.1_20260402/xtensa-esp-elf-gdb/bin:$PATH

# esp32 ULP assembler
export PATH=$IDF_TOOLS/tools/esp32ulp-elf/2.38_20240113/esp32ulp-elf/bin:$PATH

# clang/clangd
export PATH=$IDF_TOOLS/esp-clang/bin:$PATH

# openocd
export PATH=$IDF_TOOLS/tools/openocd-esp32/v0.12.0-esp32-20260703/openocd-esp32/bin:$PATH

# esptool/espefuse/espsecure (venv bin, scripts hardcode their shebang so no activate needed)
export PATH=$IDF_TOOLS/python/bin:$PATH
