ifeq ($(OS), Windows_NT)
remove_fun = del /S /Q $(subst /,\,$1)
else
remove_fun = rm -rf $(1)
endif

OBJS = source/ff.o source/ffsystem.o source/ffunicode.o adi_diskio.o

CFLAGS += -Isource

.PHONEY = all clean

all: libfatfs.a

libfatfs.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	-$(call remove_fun,$(OBJS) libfatfs.a)
