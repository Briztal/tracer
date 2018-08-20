
#include the chip makefile;
include src/core/chip/nxp/kinetis/mk64fx512/Makefile

T35_DIR := src/core/board/teensy35

#Add the teensy folder to include path;
COMPIL_INC += -I$(T35_DIR)

#configurable options
CFLAGS += -DUSB_SERIAL -DLAYOUT_US_ENGLISH

#Define the tools path for flashing;
TOOLSPATH := $(abspath $(CURDIR)/$(T35_DIR)/tool-teensy)


post_compile: hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $(NAME).hex)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

upload: post_compile reboot

