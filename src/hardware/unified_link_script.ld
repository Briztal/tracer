
/* Include memory link script. It will be searched in -L paths. */
INCLUDE memory_map.ld

/*
 * The linking script will provide the following constants :
 *  - _start_rodata :   the start of the rodata region in FLASH. .rodata must be copied in RAM at init at @_start_data.
 *  - _end_rodata   :   the end of the rodata region in flasg;
 *  - _start_data   :   the start of the region where to copy rodata, in RAM;
 *  - _start_bss    :   the start of the bss region, to initialise with zeros;
 *  - _end_bss      :   the end of the bss region, to initialise with zeros;
 *  - _start_stack  :   the lowest address of the stack;
 *  - _end_stack    :   the highest address of the stack;
 */

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
    } > RAM

    .bss :
    {
        _start_bss = .;
        *(.bss*)
        _end_bss = .;
    } > RAM

    . = ALIGN(4);

    _start_stack = .;


    PROVIDE(_end_stack = ORIGIN(RAM) + LENGTH(RAM));
}
