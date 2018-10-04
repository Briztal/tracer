
/*
 * The teensy35 has 512Kb of flash, and 192K of RAM;
 */

MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 512K
	RAM  (rwx) : ORIGIN = 0x1FFF0000, LENGTH = 192K
}


