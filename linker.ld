ENTRY(call_start_cpu0)

MEMORY {
  sram_1 (RX) : ORIGIN = 0x40080000, LENGTH = 0x20000
  sram_2 (RW) : ORIGIN = 0x3FFC0000, LENGTH = 0x20000
}

SECTIONS {
  .text : {
    *(.literal .text .literal.* .text.*)
  } > sram_1
    . = ALIGN(4);

  /* Put text strings and constants in safe Data RAM */
  .rodata : {
    *(.rodata .rodata.*)
  } > sram_2
  . = ALIGN(4);

  /* Put global initialized variables in Data RAM */
  .data : {
    *(.data .data.*)
  } > sram_2
}
