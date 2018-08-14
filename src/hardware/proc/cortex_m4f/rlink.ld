MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 512K
	RAM  (rwx) : ORIGIN = 0x1FFF0000, LENGTH = 192K
}


SECTIONS {

    . = 0;

    .vectors :
    {
        KEEP(*(.vectors))
    } > FLASH


    /*
     * Flash must be configurated carefully; The flash driver will contain a flash config section that will be inserted
     *  here.
     */

    . = 0x400;
    .flashconfig :
    {
        KEEP(*(.flashconfig*))
    } > FLASH


    . = ALIGN(4);

    .text :
    {

        *(.text*)

        . = ALIGN(4);

        _start_rodata = .;
        *(.rodata*)
        _end_rodata = .;

    } > FLASH


    . = ALIGN(4);

    .data :
    {
        _start_data = .;
        *(.data*)
        _end_data = .;
    } > RAM

    .bss :
    {
        _start_bss = .;
        *(.bss*)
        _end_bss = .;
    } > RAM

    . = ALIGN(4);

    _start_stack = .;


    PROVIDE(_end_stack = ORIGIN(sram) + LENGTH(sram));
}

_end = .;