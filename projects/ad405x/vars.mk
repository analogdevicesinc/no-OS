DEV_TYPE_=$(strip $(DEV_TYPE))
DEV_TYPE_VALID_SPI=AD4050 AD4052 AD4056 AD4058
DEV_TYPE_VALID_I3C=AD4060 AD4062
INSTANCE_ID_=$(strip $(INSTANCE_ID))
INSTANCE_ID_VALID=0 1 2 3 4 5 6 7

ifeq ($(DEV_TYPE_), $(filter $(DEV_TYPE_), $(DEV_TYPE_VALID_SPI)))
CFLAGS += -DAD405X_SPI
AD405X_SPI = 1
else ifeq ($(DEV_TYPE_), $(filter $(DEV_TYPE_), $(DEV_TYPE_VALID_I3C)))
AD405X_I3C = 1
CFLAGS += -DAD405X_I3C
ifeq ($(INSTANCE_ID_), $(filter $(INSTANCE_ID_), $(INSTANCE_ID_VALID)))
CFLAGS += -DAD405X_INSTANCE_ID=$(INSTANCE_ID_)
else
$(error Parameter INSTANCE_ID $(INSTANCE_ID_) is not valid. Valid values are $(INSTANCE_ID_VALID))
endif
else
$(error Parameter DEV_TYPE $(DEV_TYPE_) is not valid. Valid values are $(DEV_TYPE_VALID_SPI) $(DEV_TYPE_VALID_I3C))
endif
CFLAGS += -DAD405X_DEV_TYPE=ID_$(DEV_TYPE_)
CFLAGS += -D$(shell echo $(basename $(notdir $(strip $(HARDWARE)))) | tr '[:lower:]' '[:upper:]' | tr '-' '_')

