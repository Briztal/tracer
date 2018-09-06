
#CORE_INC and CORE_SRCS are defined in the general makefile;

#Add the core folder to the core include dir list;
CORE_INC += -Isrc
CORE_INC += -Isrc/core
CORE_INC += -Isrc/core/hard

#Add general core sources;
CORE_SRCS += core/core.c
CORE_SRCS += core/debug.c

#Add memory management sources;
CORE_SRCS += core/mem/ram.c
CORE_SRCS += core/mem/heap.c

#Add type sources;
CORE_SRCS += core/type/shared_fifo.c

#Add the communication protocol;//TODO CONFIG
CORE_SRCS += core/log/probe.c


#Include the teensy35 platform code;
include src/core/hard/board/teensy35/Makefile


#Build the objects set from sources and reroute to build dir;
CORE_O := $(foreach src, $(CORE_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The general core compilation rule;
$(BUILDDIR)/core/%.o: src/core/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(CORE_INC) -o $@ -c $<


#Define the core archive name;
CORE_AR := build/core/libcore.a

#The core library depends on sys lib, and all core objects mentionned by the core makefile and its subs;
core : $(CORE_O)

	@echo "Creating core archive."

	#Create directory if t doesn't exist;
	@mkdir -p "$(dir $@)"

	#Archive the module to hide symbols;
	$(AR) cr $(CORE_AR) $(CORE_O)



