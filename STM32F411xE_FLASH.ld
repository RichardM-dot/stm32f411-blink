/*
 ******************************************************************************
 * @file    STM32F411xE_FLASH.ld
 * @brief   GNU ld linker script for STM32F411xE (Cortex-M4F).
 *
 *          Memory map (STM32F411xE datasheet):
 *            - Flash : 512 KB @ 0x08000000
 *            - SRAM  : 128 KB @ 0x20000000
 *
 *          Symbols consumed by startup_stm32f411xe.s:
 *            _estack, _sidata, _sdata, _edata, _sbss, _ebss
 ******************************************************************************
 */

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack (grows down from end of SRAM) */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Minimum heap/stack sizes reserved by ._user_heap_stack (checked at link
   time only; increase if your application uses more). */
_Min_Heap_Size  = 0x200;   /* 512 B  */
_Min_Stack_Size = 0x400;   /* 1 KB   */

MEMORY
{
  RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 128K
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
}

SECTIONS
{
  /* Vector table: must be the very first thing in Flash */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))   /* Startup vector table */
    . = ALIGN(4);
  } >FLASH

  /* Program code and constant data */
  .text :
  {
    . = ALIGN(4);
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;              /* global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* Used by the startup to initialize .data: LMA in FLASH, VMA in RAM */
  _sidata = LOADADDR(.data);

  /* Initialized data sections go into RAM, load image copied from FLASH */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;              /* create a global symbol at data start */
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    *(.RamFunc)              /* functions copied to and run from RAM */
    *(.RamFunc*)
    . = ALIGN(4);
    _edata = .;              /* define a global symbol at data end */
  } >RAM AT> FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    _sbss = .;               /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;               /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* Heap and stack sanity check: fails the link if RAM is exhausted */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0       : { *(.ARM.attributes) }
  .ARM.attributes 0       : { *(.ARM.attributes) }
  .debug_info     0       : { *(.debug_info .debug_info.* .gnu.linkonce.wi.*) }
  .debug_abbrev   0       : { *(.debug_abbrev*) }
  .debug_line     0       : { *(.debug_line*) }
  .debug_frame    0       : { *(.debug_frame*) }
  .debug_str      0       : { *(.debug_str*) }
  .debug_loc      0       : { *(.debug_loc*) }
  .debug_macinfo  0       : { *(.debug_macinfo*) }
  .note.gnu.arm.ident 0   : { KEEP (*(.note.gnu.arm.ident)) }
}
