
#include the chip makefile;
include src/hard/chip/kinetis/chips/mk64fx512/mk64fx512.mk


#Define the tools path for flashing;
TOOLSPATH := $(abspath $(CURDIR)/src/hard/board/teensy35/tool-teensy)

hex: elf
	@$(OBJCOPY) -O ihex -R .eeprom $(NAME).elf $(NAME).hex

upload: hex
	@$(abspath $(TOOLSPATH))/teensy_loader_cli -mmcu=mk64fx512 -w -v "$(abspath $(NAME).hex)"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot


