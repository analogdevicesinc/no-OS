-include $(PROJECT)/src/platform/$(PLATFORM)/*.mk
-include $(PROJECT)/src/examples/$(EXAMPLE)/*.mk

ifneq ($(strip $(EXAMPLE)),)
NO_OS_INC_DIRS += $(PROJECT)/src/examples/$(EXAMPLE) \
		  $(PROJECT)/src/platform/$(PLATFORM) \
		  $(PROJECT)/src/common

SRC_DIRS += $(PROJECT)/src/examples/$(EXAMPLE) \
	    $(PROJECT)/src/platform/$(PLATFORM) \
	    $(PROJECT)/src/common
endif