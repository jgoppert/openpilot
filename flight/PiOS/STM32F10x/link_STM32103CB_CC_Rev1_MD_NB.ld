MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x00020000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00005000
}

_estack = 0x20004FF0;

/* Section Definitions */
SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector .isr_vector.*))
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.rodata .rodata* .gnu.linkonce.r.*)
    } > FLASH

    /* init sections */
    .initcalluavobj.init :
    {
        . = ALIGN(4);
	__uavobj_initcall_start = .;
        KEEP(*(.initcalluavobj.init))
	. = ALIGN(4);
	__uavobj_initcall_end   = .;
    } >FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

    . = ALIGN(4);
    _etext = .;
    _sidata = .;

    .data : AT (_etext)
    {
        _sdata = .;
        *(.data .data.*)
        . = ALIGN(4);
        _edata = . ;
    } > SRAM

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        _sbss = . ;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } > SRAM

    . = ALIGN(4);
    _end = . ;
}
