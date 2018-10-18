/*
  unified_linker_script.ld Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
 * Linking constants are listed and described in kernel/hard.h:
 */


/* Include memory link script. It will be searched in -L paths. */
INCLUDE memory_map.ld


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

    /*Declare the program section in the flash;;*/
    .text : {
        *(.text)
    } > FLASH


    /*The embedded modules section contains kernel modules to load at init;*/
    .kernel_modules : {

        __prmod_min = .;
        KEEP(*(.kernel_proc_modules))
        __prmod_max = .;

        __smod_min = .;
        KEEP(*(.kernel_system_modules))
        __smod_max = .;

        __pemod_min = .;
        KEEP(*(.kernel_peripheral_modules))
        __pemod_max = .;

        __kmod_min = .;
        KEEP(*(.kernel_kernel_modules))
        __kmod_max = .;

        __umod_min = .;
        KEEP(*(.kernel_user_modules))
        __umod_max = .;

     } > FLASH


    /*The rodata section contains constant data (const, strings, ...);*/
    .rodata : {
        *(.rodata*)
    } > FLASH


    /*The min lma of the data section, after all previous sections;*/
    __data_lma_min = .;

    /*The data section contains globals that we must initialise. Its LMA will be at __data_lma;*/
    .data : AT (__data_lma_min) {
        __data_vma_min = .;
        *(.data*)
        __data_vma_max = .;
    } > RAM


    /*The bss section contains globals that must be initialised to 0 at init*/
    .bss : {
        __bss_vma_min = .;
        *(.bss*)
        __bss_vma_max = .;
    } > RAM


    . = ALIGN(4);

    /*The lowest accessible RAM address;*/
    __ram_min = .;

    /*The highest accessible RAM address;*/
    PROVIDE(__ram_max = ORIGIN(RAM) + LENGTH(RAM));
}
