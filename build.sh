#!/bin/zsh
set -e

xtensa-esp32-elf-gcc -nostdlib -T linker.ld main.c -o main.elf
esptool --chip esp32 elf2image --flash-mode dio --flash-freq 40m --flash-size 4MB main.elf
esptool --chip esp32 write-flash 0x1000 main.bin

