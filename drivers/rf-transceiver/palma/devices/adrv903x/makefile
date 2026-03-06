##########################################################
# Include build toolchain from top level build config
ifeq ($(CONFIGFILE),)
error:
	$(error Please specify a config file name)
endif

CONFIGURATION_FLAGS_FILE := $(CONFIGFILE)

include $(CONFIGURATION_FLAGS_FILE)

ifeq ($(BINARYDIR),)
error:
	$(error Invalid configuration, please check your inputs)
endif

# Shared library nomenclature: https://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html 
SO_LIBNAME = adi_adrv903x
SO_VER = 1
SO_MINOR = 0
SO_RELEASE = 0
# linkername e.g. libfoo.so
SO_LINKERNAME = lib$(SO_LIBNAME).so
# vername e.g. libfoo.so.x
SO_SONAME = $(SO_LINKERNAME).$(SO_VER)
# realname e.g. libfoo.so.x.y.z
SO_REALNAME = $(SO_SONAME).$(SO_MINOR).$(SO_RELEASE)
API_VER = $(SO_VER).$(SO_MINOR).$(SO_RELEASE)

LIB_ADI_ADRV903X_DEVICE_SO = $(SO_LINKERNAME)
LIB_ADI_ADRV903X_DEVICE = libadi_adrv903x.a

ADI_ADRV903X_SRC = ./public/src
ADI_ADRV903X_PRIVATE_SRC = ./private/src
ADI_ADRV903X_PRIVATE_BF_SRC = ./private/bf
ADI_PLATFORM_SRC = ../../platforms
ADI_COMMON_SRC = ../../common

ARFLAGS = -rv
INC_DIRS := ./public/include ../../platforms/ ../../common ../../common/adi_error ../../common/adi_logging ../../common/adi_hal
CFLAGS += $(addprefix -I,$(INC_DIRS))
CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))
EXT := c

SRC = $(wildcard $(ADI_ADRV903X_PRIVATE_SRC)/*.$(EXT)) \
	$(wildcard $(ADI_ADRV903X_SRC)/*.$(EXT))

SRC_OBJS = $(SRC:.$(EXT)=.o)
#Place .o files in output folder based on build config name
OBJS := $(addprefix $(BINARYDIR)/, $(notdir $(SRC_OBJS)))

BF_SRC = $(wildcard $(ADI_ADRV903X_PRIVATE_BF_SRC)/*.c)
BF_OBJS = $(BF_SRC:.c=.o)
BF_OBJS := $(addprefix $(ADI_ADRV903X_PRIVATE_BF_SRC)/$(BINARYDIR)/, $(notdir $(BF_OBJS)))

#adi_platform_lib
all: adi_common_lib \
	adi_platform_lib \
	adi_adrv903x_private_bf \
	$(BINARYDIR)/$(LIB_ADI_ADRV903X_DEVICE) \
	$(BINARYDIR)/$(LIB_ADI_ADRV903X_DEVICE_SO)

$(BINARYDIR): 
	$(MKDIR) $(BINARYDIR)

$(BINARYDIR)/%.o: $(ADI_ADRV903X_PRIVATE_SRC)/%.$(EXT) |$(BINARYDIR)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BINARYDIR)/%.o: $(ADI_ADRV903X_SRC)/%.$(EXT) |$(BINARYDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINARYDIR)/$(LIB_ADI_ADRV903X_DEVICE): $(OBJS) |$(BINARYDIR)
	$(AR) $(ARFLAGS) $(BINARYDIR)/$(LIB_ADI_ADRV903X_DEVICE) $(OBJS)

$(BINARYDIR)/$(LIB_ADI_ADRV903X_DEVICE_SO): $(OBJS) |$(BINARYDIR)
	$(CC) -shared -Wl,-soname,$(SO_SONAME) -o $(BINARYDIR)/$(SO_REALNAME) $(OBJS) $(BF_OBJS) -lc 

adi_adrv903x_private_bf:
	$(MAKE) -C $(ADI_ADRV903X_PRIVATE_BF_SRC)
	
adi_platform_lib:
	$(MAKE) -C $(ADI_PLATFORM_SRC)

adi_common_lib:
	$(MAKE) -C $(ADI_COMMON_SRC)

clean:
ifeq ($(USE_DEL_TO_CLEAN),1)
	cmd /C if exist $(BINARYDIR) rd /S /Q $(BINARYDIR)
else
	rm -rf ./$(BINARYDIR)
endif
	$(MAKE) -C $(ADI_ADRV903X_PRIVATE_BF_SRC) clean	
	$(MAKE) -C $(ADI_COMMON_SRC) clean
	$(MAKE) -C $(ADI_PLATFORM_SRC) clean
