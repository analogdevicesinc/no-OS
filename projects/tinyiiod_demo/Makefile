include ../../tools/scripts/generic_variables.mk

CFLAGS += -DHEAP_SIZE=0x10000 -DSTACK_SIZE=0x4000
USER_INCLUDES = /home/dari/Work/libiio/include /home/dari/Work/libiio /home/dari/Work/libiio/build 
#/home/dari/Work/libxml2/include /home/dari/Work/libxml2/build
EXTRA_LIBS = libtinyiiod.a libiio.a iiod-responder.a 
#libxml2.a 
EXTRA_LIBS_PATHS = /home/dari/Work/libiio/build/tinyiiod /home/dari/Work/libiio/build 
#/home/dari/Work/libxml2/build

include src.mk

include ../../tools/scripts/generic.mk
