ifeq ($(OS), Windows_NT)
remove_fun = del /S /Q $(subst /,\,$1)
else
remove_fun = rm -rf $(1)
endif

# Local variables
TINYIIOD_VERSION_MAJOR = 0
TINYIIOD_VERSION_MINOR = 1
TINYIIOD_VERSION = $(TINYIIOD_VERSION_MAJOR).$(TINYIIOD_VERSION_MINOR)

BUFF_SIZE ?= 0x1000
STD_TYPES ?= true

# Build settings
IIO_DEFS = -D TINYIIOD_VERSION_MAJOR=$(TINYIIOD_VERSION_MAJOR)		\
	   -D TINYIIOD_VERSION_MINOR=$(TINYIIOD_VERSION_MINOR)		\
	   -D TINYIIOD_VERSION_GIT=0x$(shell git rev-parse --short HEAD)\
	   -D IIOD_BUFFER_SIZE=$(BUFF_SIZE)

ifeq (true,$(strip $(STD_TYPES)))
IIO_DEFS += -D_USE_STD_INT_TYPES
endif

IIO_DIR		= $(realpath .)
TINYIIOD_DIR	= $(IIO_DIR)/libtinyiiod

LIB_NAME	= libiio.a

SRCS = iio.c $(TINYIIOD_DIR)/parser.c $(TINYIIOD_DIR)/tinyiiod.c

OBJS = $(SRCS:.c=.o)

CFLAGS += $(IIO_DEFS) \
	  -I"$(TINYIIOD_DIR)"\
	  -I"$(NO-OS)/include"

.PHONEY = all clean

#Init libtinyiiod submodule if not initialized
ifeq ($(wildcard $(TINYIIOD_DIR)/.git),)
all:
	git submodule update --init --remote -- $(TINYIIOD_DIR)
	$(MAKE) $(LIB_NAME)
else
all: $(LIB_NAME)
endif

$(LIB_NAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	-$(call remove_fun,$(OBJS) $(LIB_NAME))