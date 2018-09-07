
#include the chip makefile;
include src/hard/chip/nxp/kinetis/mk64fx512/Makefile

#Define the tools path for flashing;
TOOLSPATH := $(abspath $(CURDIR)/src/hard/board/teensy35/tool-teensy)

hex: elf
	@echo "[HEX]\tracer.hex"
	@$(SIZE) -A -t -x $(NAME).elf
	@$(OBJCOPY) -O ihex -R .eeprom $(NAME).elf $(NAME).hex

upload: hex
	@$(abspath $(TOOLSPATH))/teensy_loader_cli -mmcu=mk64fx512 -w -v "$(abspath build/tracer.hex)"
	#@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $(NAME).hex)" -path=$(CURDIR) -v -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot


