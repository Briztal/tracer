
#The util sources;
UTIL_SRCS :=

#Add util sources;
UTIL_SRCS += util/string.c
UTIL_SRCS += util/type.c

#Build the objects set from sources and reroute to build dir;
UTIL_OBJS := $(foreach src, $(UTIL_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The general core object build rule : depends on the related source file, located in the src directory;
#	It changes the directory to the build dir;

$(BUILDDIR)/util/%.o: src/util/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<


#The core library depends on all core objects mentionned by the core makefile and its subs;
util : $(UTIL_OBJS)
