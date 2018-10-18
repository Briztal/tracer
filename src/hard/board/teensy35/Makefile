
#include the chip makefile;
include src/hard/chip/nxp/kinetis/mk64fx512/Makefile

#define the hardware rule. Depends on the mk64fx512 rule;
hard: mk64fx512


#Define the tools path for flashing;
TOOLSPATH := $(abspath $(CURDIR)/src/hard/board/teensy35/tool-teensy)

hex: elf
	@echo "[HEX]\tracer.hex"
	@$(OBJCOPY) -O ihex -R .eeprom $(NAME).elf $(NAME).hex

upload: hex
	@$(abspath $(TOOLSPATH))/teensy_loader_cli -mmcu=mk64fx512 -w -v "$(abspath $(NAME).hex)"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot


