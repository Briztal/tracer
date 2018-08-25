
/* Include memory link script. It will be searched in -L paths. */
INCLUDE memory_map.ld

/*
 * The linking script will provide the following constants :
 *  - _data_lma     :   the load address of the data region, in FLASG
 *  - _data_vma     :   the lowest address of the data region in RAM;
 *  - _end_data     :   the highest address of the data region in RAM;
 *  - _bss_vma      :   the lowest address of the bss region in RAM;
 *  - _end_bss      :   the highest address of the bss region in RAM;
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
        KEEP(*(.flashconfig))
    } > FLASH


    /*The text section contains the program itself;*/
    .text : {
        . = ALIGN(4);
        *(.text)
    } > FLASH


    /*The rodata section contains constant data (const, strings, ...);*/
    .rodata : {
        . = ALIGN(4);
        *(.rodata*)
    } > FLASH


    /*The libc init section. Junk crap LOL;*/
    .cinit : {
        . = ALIGN(4);
        *(.init*)
        *(.fini*)
    } > FLASH

    /*The lma of the data section, after all previous sections;*/
    _data_lma = .;


    /*The data section contains globals that we must initialise. Its LMA will be at _data_lma;*/
    .data : AT (_data_lma) {
        . = ALIGN(4);
        _data_vma = .;
        *(.data*)
        _end_data = .;
    } > RAM


    /*The bss section contains globals that must be initialised to 0 at init*/
    .bss : {
        . = ALIGN(4);
        _bss_vma = .;
        *(.bss*)
        _end_bss = .;
    } > RAM


    . = ALIGN(4);

    /*The lowest accessible RAM address;*/
    _ram_lowest = .;

    /*The highest accessible RAM address;*/
    PROVIDE(_ram_highest = ORIGIN(RAM) + LENGTH(RAM));
}
