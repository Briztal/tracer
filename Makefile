

##TODO ----------------------- GENERAL MAKEFILE -----------------------

TARGET = main

BUILDDIR = $(abspath $(CURDIR)/build)


##TODO ----------------------- Architecture MAKEFILE -----------------------

COMPILERPATH ?= /usr/bin

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size


##TODO ----------------------- Processor MAKEFILE -----------------------



CPUARCH = cortex-m4


#Compiler options for C;
CFLAGS = -Wall -g -Os -mcpu=$(CPUARCH) -mthumb -MMD $(OPTIONS) -I.

# linker options
LDFLAGS = -Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=$(CPUARCH) -mthumb -T$(MCU_LD)

LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -T$(LDSCRIPT)

CFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

##TODO ----------------------- Chip MAKEFILE -----------------------


LDSCRIPT = rlink.ld

CFLAGS += -D__MK64FX512__


##TODO ----------------------- Board MAKEFILE -----------------------



# configurable options
OPTIONS = -DF_CPU=120000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE

TOOLSPATH = $(abspath $(CURDIR)/tool-teensy)

# additional libraries to link
LIBS = -lm


# include paths for libraries TODO
L_INC :=

##SOURCES := $(C_FILES:.c=.o)

SOURCES := src/arch/processors/arm/arm_vx/arm_v7m/arm_v7m.o
SOURCES += src/arch/processors/arm/arm_vx/arm_v7m/nvic.o

OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))



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

#
#$(BUILDDIR)/%.o: %.cpp
#	@echo -e "[CXX]\t$<"
#	@mkdir -p "$(dir $@)"
#	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"


$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo -e "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

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

