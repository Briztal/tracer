
#CORE_INC and CORE_SRCS are defined in the general makefile;

HARD_INC = -Isrc

#Include the teensy35 platform code;
include src/hard/board/teensy35/Makefile


#Build the objects set from sources and reroute to build dir;
HARD_OBJS := $(foreach src, $(HARD_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The hard compilation rule;
$(BUILDDIR)/hard/%.o: src/hard/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(HARD_INC) -o $@ -c $<


#The core library depends on sys lib, and all core objects mentionned by the core makefile and its subs;
core : $(HARD_OBJS)

	@echo "Creating core archive."

	#Create directory if t doesn't exist;
	@mkdir -p "$(dir $@)"

	#Archive the module to hide symbols;
	#TODO $(AR) cr $(CORE_AR) $(HARD_OBJS)



