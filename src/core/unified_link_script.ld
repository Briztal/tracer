
/* Include memory link script. It will be searched in -L paths. */
INCLUDE memory_map.ld

/*
 * The linking script will provide the following constants :
 *  - _start_rodata :   the start of the rodata region in FLASH. .rodata must be copied in RAM at init at @_start_data.
 *  - _end_rodata   :   the end of the rodata region in flasg;
 *  - _start_data   :   the start of the region where to copy rodata, in RAM;
 *  - _start_bss    :   the start of the bss region, to initialise with zeros;
 *  - _end_bss      :   the end of the bss region, to initialise with zeros;
 *  - _ram_lowest   :   the lowest bound of the ram's available zone;
 *  - _ram_highest  :   the highest bound of the ram's available zone;
 */

/*TODO : */


SECTIONS {

    . = 0;
    .vectors : {
        KEEP(*(.vectors))
    } > FLASH


    /*
     * Flash must be configurated carefully; The flash driver will contain a flash config section that will be inserted
     *  here.
     */

    . = 0x400;
    .flashconfig : {
        KEEP(*(.flashconfig*))
    } > FLASH


    /*The text section contains the program itself;*/
    . = ALIGN(4);
    .text : {
        *(.text*)
    } > FLASH


    /*The rodata section contains globals init values;*/
    . = ALIGN(4);
    .rodata : {
        _start_rodata = .;
        *(.rodata*)
        _end_rodata = .;
    } > FLASH


    /*The bss section contains globals that must be initialised to 0 at init*/
    . = ALIGN(4);
    .data :
    {
        _start_data = .;
        *(.data*)
    } > RAM


    /*The bss section contains globals that must be initialised to 0 at init*/
    . = ALIGN(4);
    .bss : {
        _start_bss = .;
        *(.bss*)
        _end_bss = .;
    } > RAM


    . = ALIGN(4);

    /*The lowest accessible RAM address;*/
    _ram_lowest = .;

    /*The highest accessible RAM address;*/
    PROVIDE(_ram_highest = ORIGIN(RAM) + LENGTH(RAM));
}
