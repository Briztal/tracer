
#include the chip makefile;
include src/hardware/chip/nxp/kinetis/mk64fx512/Makefile

T35_DIR := src/hardware/board/teensy35

#Add the teensy folder to include path;
L_INC += $(T35_DIR)

#configurable options
CFLAGS +=  -DUSB_SERIAL -DLAYOUT_US_ENGLISH

#Build the objects set;
HW_OBJS := $(foreach src, $(HW_SOURCES), $(BUILDDIR)/$(src))
SW_OBJS := $(foreach src, $(SW_SOURCES), $(BUILDDIR)/$(src))

#Define the tools path for flashing;
TOOLSPATH := $(abspath $(CURDIR)/$(T35_DIR)/tool-teensy)

all: hex

build: $(TARGET).elf

hex: $(TARGET).hex

post_compile: $(TARGET).hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

upload: post_compile reboot

$(BUILDDIR)/%.o: %.c
	@echo -e "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"


$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo -e "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LINK_LIBS)

%.hex: %.elf
	@echo -e "[HEX]\t$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
#-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).elf" "$(TARGET).hex"

