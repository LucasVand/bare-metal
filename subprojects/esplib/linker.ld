
ENTRY(call_start_cpu0)

MEMORY {
  sram_1 (RX) : ORIGIN = 0x40080000, LENGTH = 0x30000
  sram_2 (RW) : ORIGIN = 0x3FFC0000, LENGTH = 0x10000
}

SECTIONS {
  .text : {
    *(.literal .text .literal.* .text.*)
    . = ALIGN(4);
  } > sram_1

  /* Force the location counter to align to the next available spot in sram_2 */
  .rodata : {
    . = ALIGN(4);
    *(.rodata .rodata.*)
    . = ALIGN(4);
  } > sram_2

  .data : {
    . = ALIGN(4);
    *(.data .data.*)
    *(.sdata .sdata.*)
    . = ALIGN(4);
  } > sram_2

  /* Explicitly position .bss right after .data in sram_2 */
  .bss (NOLOAD) : {
    . = ALIGN(4);
    __vbss_start = .;
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    . = ALIGN(4);
    __vbss_end = .;
  } > sram_2
  __bss_start = __vbss_start; 
  __bss_size = __vbss_end - __vbss_start;
}

