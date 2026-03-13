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

LIB_ADI_COMMON = libadi_common.a
ADI_COMMON_SRC = .
ADI_COMMON_ERROR_SRC = ./adi_error
ADI_COMMON_LOGGING_SRC = ./adi_logging
ADI_COMMON_HAL_SRC = ./adi_hal

ARFLAGS = -rv

INC_DIRS := ../common ../platforms ./adi_error ./adi_logging ./adi_hal
CFLAGS += $(addprefix -I,$(INC_DIRS))
CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))
EXT := c

SRC = $(wildcard $(ADI_COMMON_SRC)/*.$(EXT)) \
	$(wildcard $(ADI_COMMON_ERROR_SRC)/*.$(EXT)) \
	$(wildcard $(ADI_COMMON_LOGGING_SRC)/*.$(EXT)) \
	$(wildcard $(ADI_COMMON_HAL_SRC)/*.$(EXT))

SRC_OBJS = $(SRC:.$(EXT)=.o)
#Place .o files in output folder based on build config name
OBJS := $(addprefix $(BINARYDIR)/, $(notdir $(SRC_OBJS)))

all:$(BINARYDIR)/$(LIB_ADI_COMMON)

$(BINARYDIR): 
	$(MKDIR) $(BINARYDIR)
	
$(BINARYDIR)/%.o: $(ADI_COMMON_SRC)/%.$(EXT) |$(BINARYDIR)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BINARYDIR)/%.o: $(ADI_COMMON_ERROR_SRC)/%.$(EXT) |$(BINARYDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINARYDIR)/%.o: $(ADI_COMMON_LOGGING_SRC)/%.$(EXT) |$(BINARYDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINARYDIR)/%.o: $(ADI_COMMON_HAL_SRC)/%.$(EXT) |$(BINARYDIR)
	$(CC) $(CFLAGS) -o $@ -c $<
	
$(BINARYDIR)/$(LIB_ADI_COMMON): $(OBJS) |$(BINARYDIR)
	$(AR) $(ARFLAGS) $(BINARYDIR)/$(LIB_ADI_COMMON) $(OBJS)

clean:
ifeq ($(USE_DEL_TO_CLEAN),1)
	cmd /C if exist $(BINARYDIR) rd /S /Q $(BINARYDIR)
else
	rm -rf ./$(BINARYDIR)
endif

