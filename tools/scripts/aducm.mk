# New line variable
define ENDL


endef

# Initialize CCES_HOME to default, if directory not found show error
#	WINDOWS
ifeq ($(OS), Windows_NT)
#cces works to but has no console output
CCES = ccesc
CCES_HOME ?= $(wildcard C:/Analog\ Devices/CrossCore\ Embedded\ Studio*)
ifeq ($(CCES_HOME),)
$(error $(ENDL)$(ENDL)CCES_HOME not found at c:/Analog Devices/[CrossCore...]\
		$(ENDL)$(ENDL)\
Please run command "set CCES_HOME=c:\Analog Devices\[CrossCore...]"$(ENDL)\
Ex: set CCES_HOME=c:\Analog Devices\[CrossCore...] Embedded Studio 2.8.0$(ENDL)$(ENDL))
endif
#	LINUX
else

CCES = cces
CCES_HOME ?= $(wildcard /opt/analog/cces/*)
ifeq ($(CCES_HOME),)
$(error $(ENDL)$(ENDL)CCES_HOME not found at /opt/analog/cces/[version_number]\
		$(ENDL)$(ENDL)\
		Please run command "export CCES_HOME=[cces_path]"$(ENDL)\
		Ex: export CCES_HOME=/opt/analog/cces/2.9.2$(ENDL)$(ENDL))
endif
endif

#Set PATH variables where used binaries are found
COMPILER_BIN = $(CCES_HOME)/ARM/gcc-arm-embedded/bin
OPENOCD_SCRIPTS = $(CCES_HOME)/ARM/openocd/share/openocd/scripts
OPENOCD_BIN = $(CCES_HOME)/ARM/openocd/bin
CCES_EXE = $(CCES_HOME)/Eclipse

export PATH := $(CCES_EXE):$(OPENOCD_SCRIPTS):$(OPENOCD_BIN):$(COMPILER_BIN):$(PATH)

#------------------------------------------------------------------------------
#                          FIX SPACES PROBLEM                              
#------------------------------------------------------------------------------

null :=
SPACE := $(null) $(null)
#This work for wildcards
escape_spaces = $(subst $(SPACE),\$(SPACE),$1)

#If dfp have spaces, copy sources from dfp in platform_tools
ADUCM_DFP = $(wildcard \
$(call escape_spaces,$(CCES_HOME))/ARM/packs/AnalogDevices/ADuCM302x_DFP/*)

CMSIS_CORE = $(wildcard \
$(call escape_spaces,$(CCES_HOME))/ARM/packs/ARM/CMSIS/*/CMSIS/Core)

ifneq ($(words $(CCES_HOME)), 1)

ifeq ($(wildcard $(PLATFORM_TOOLS)/dfp_drivers),)
#Create link to dfp to noos if it contains spaces (default on windows)
DUMMY_VAR := $(shell $(call make_dir_link,$(CCES_HOME),$(PLATFORM_TOOLS)/dfp_drivers))
endif
DFP_DRIVERS = $(PLATFORM_TOOLS)/dfp_drivers

else

DFP_DRIVERS = $(ADUCM_DFP)/Source/drivers

endif

#------------------------------------------------------------------------------
#                             DFP DEPENDENCIES                          
#------------------------------------------------------------------------------

PROJECT_BUILD = $(BUILD_DIR)/$(PROJECT_NAME)

#DFP Files
DFP_FILES = $(call rwildcard,$(DFP_DRIVERS),*.c)
#Not for aducm3029
DFP_IGNORED_FILES += $(DFP_DRIVERS)/flash/adi_flash_data.c $(DFP_DRIVERS)/rtc/adi_rtc_data.c

ADUCM_SRCS += $(filter-out $(DFP_IGNORED_FILES), $(DFP_FILES))

ADUCM_INCS += $(ADUCM_DFP)/Include
ADUCM_INCS += $(CMSIS_CORE)/Include
PIN_MUX = $(PROJECT)/pinmux_config.c
PROJECT_PIN_MUX = $(PROJECT_BUILD)/system/pinmux/GeneratedSources/pinmux_config.c

ADUCM_SRCS += $(PROJECT_PIN_MUX)
ADUCM_SRCS += $(PROJECT_BUILD)/system/adi_initialize.c
ADUCM_SRCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029/startup_ADuCM3029.c
ADUCM_SRCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029/system_ADuCM3029.c
ASM_SRCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029/reset_ADuCM3029.S
ADUCM_INCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029
ADUCM_INCS += $(PROJECT_BUILD)/RTE
ADUCM_INCS += $(PROJECT_BUILD)/system

SRCS += $(ADUCM_SRCS)

#------------------------------------------------------------------------------
#                           COMPILING DATA                              
#------------------------------------------------------------------------------

GENERIC_FLAGS = -DCORE0 -D_RTE_ -D__ADUCM3029__ -D__SILICON_REVISION__=0xffff -mcpu=cortex-m3 -mthumb

GENERIC_FLAGS += $(addprefix -I,$(ADUCM_INCS))

GENERIC_DEBUG_FLAGS = -g -gdwarf-2 -D_DEBUG

GENERIC_RELEASE_FLAGS = -DNDEBUG 
C_RELEASE_FLAGS = -O2

CFLAGS += -DADUCM_PLATFORM

ASFLAGS	+= $(GENERIC_FLAGS) -x assembler-with-cpp 
CFLAGS	+= $(GENERIC_FLAGS) -Wall -ffunction-sections -fdata-sections

CFLAGS	+= $(GENERIC_RELEASE_FLAGS) $(C_RELEASE_FLAGS)
ASFLAGS += $(GENERIC_RELEASE_FLAGS)

#CFLAGS	+= $(GENERIC_DEBUG_FLAGS)
#ASFLAGS += $(GENERIC_DEBUG_FLAGS)

LSCRIPT	= $(PROJECT_BUILD)/RTE/Device/ADuCM3029/ADuCM3029.ld

LDFLAGS	= -Wl,--gc-sections -mcpu=cortex-m3 -mthumb -lm

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
AR = arm-none-eabi-ar

HEX = $(BINARY).hex

#------------------------------------------------------------------------------
#                                 RULES                              
#------------------------------------------------------------------------------

$(HEX): $(BINARY)
	arm-none-eabi-objcopy -O ihex $(BINARY) $(HEX)

clean: clean_hex

clean_hex:
	-$(call remove_fun,$(HEX))

ifneq ($(wildcard $(BUILD_DIR)),)
all: $(HEX) $(PIN_MUX)
endif

#Used to update pinmux if updated on project
$(PIN_MUX): $(PROJECT_PIN_MUX)
	@echo UPDATEING PINMUX
	$(call copy_fun,$(PROJECT_PIN_MUX),$(PIN_MUX))

PHONY += install_dfp
install_dfp:
	$(call copy_folder,$(ADUCM_DFP)/Source/drivers/*,$(PLATFORM_TOOLS)/dfp_drivers/)

# Upload binary to target
PHONY += aducm3029_run
aducm3029_run: all
	openocd -s "$(OPENOCD_SCRIPTS)" -f interface/cmsis-dap.cfg \
		-s "$(ADUCM_DFP)/openocd/scripts" -f target/aducm3029.cfg \
		-c "program  $(BINARY) verify reset exit"

#------------------------------------------------------------------------------
#                             PROJECT RULES                              
#------------------------------------------------------------------------------

PROJECT_BINARY = $(PROJECT_BUILD)/Release/$(PROJECT_NAME)

#This way will not work if the rest button is press or if a printf is executed
#because there is a bug in crossCore that doesn't enable to remove semihosting
#from command line. If semihosting is removed from the IDE this will work to
#CCESS bug: https://labrea.ad.analog.com/browse/CCES-22274
PHONY += project_run
project_run: build_project
	openocd -s "$(OPENOCD_SCRIPTS)" -f interface/cmsis-dap.cfg \
		-s "$(ADUCM_DFP)/openocd/scripts" -f target/aducm3029.cfg \
	-c init \
	-c "program  $(PROJECT_BINARY) verify" \
	-c "arm semihosting enable" \
	-c "reset run" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c "resume" \
	-c exit

PHONY += aducm3029_project_build
aducm3029_project_build: aducm3029_project $(LIB_TARGETS)
	$(CCES) -nosplash -application com.analog.crosscore.headlesstools \
		-data $(WORKSPACE) \
		-project $(PROJECT_NAME) \
		-build Release

ADD_COMPILER_DEFINES = $(foreach flag, $(FLAGS_WITHOUT_D), \
			-append-switch compiler -D=$(flag))

escape_project_name = $(subst $(PROJECT_NAME),_$(PROJECT_NAME), $1)
#Flags for each include directory
INCLUDE_FLAGS = $(foreach dir, $(EXTRA_INC_PATHS),\
		-append-switch compiler -I=$(dir))
FILES_TO_COPY = $(call rwildcard, $(SRC_DIRS),*) $(FILES_TO_LINK) 
#Flags for each linked resource
SRC_FLAGS = $(foreach dir,$(SRC_DIRS), -link $(dir)\
			$(call escape_project_name, $(call get_relative_path,$(dir))))

SRC_FLAGS += $(foreach file,$(FILES_TO_LINK), -link $(file)\
			 $(call escape_project_name, $(call get_relative_path,$(file))))

PHONY += aducm3029_update_srcs
aducm3029_update_srcs:
	$(CCES) -nosplash -application com.analog.crosscore.headlesstools \
		-data $(WORKSPACE) \
		-project $(PROJECT_NAME) \
		$(SRC_FLAGS)

aducm3029_project: $(PROJECT_BUILD)/.project.target

#Create new project with platform driver and utils source folders linked
$(PROJECT_BUILD)/.project.target: $(LIB_TARGETS)
	$(CCES) -nosplash -application com.analog.crosscore.headlesstools \
		-command projectcreate \
		-data $(WORKSPACE) \
		-project $(PROJECT_BUILD) \
		-project-name $(PROJECT_NAME) \
		-processor ADuCM3029 \
		-type Executable \
		-revision any \
		-language C \
		-config Release \
		-remove-switch linker -specs=rdimon.specs
#Overwrite system.rteconfig file with one that enables all DFP feautres neede by noos
	$(call copy_fun,$(PLATFORM_TOOLS)/system.rteconfig,$(PROJECT_BUILD))
#Adding pinmux plugin (Did not work to add it in the first command) and update project
	$(CCES) -nosplash -application com.analog.crosscore.headlesstools \
 		-command addaddin \
 		-data $(WORKSPACE) \
 		-project $(PROJECT_NAME) \
 		-id com.analog.crosscore.ssldd.pinmux.component \
		-version latest \
		-regensrc \
		$(INCLUDE_FLAGS) \
		$(ADD_COMPILER_DEFINES) \
		-append-switch linker additionaloption="$(LIB_PATHS) $(LIB_FLAGS)"

#The default startup_ADuCM3029.c has compiling errors
#TODO Replace with patch if team think is a better aproch to install a windows
#program for patching	
	$(call copy_fun\
	,$(PLATFORM_TOOLS)/startup_ADuCM3029_patch.c,$(PROJECT_BUILD)/RTE/Device/ADuCM3029/startup_ADuCM3029.c)
#Remove default files from projectsrc
	$(call remove_dir,$(PROJECT_BUILD)/src)
	$(call copy_fun,$(PIN_MUX),$(PROJECT_PIN_MUX))
	$(MAKE) aducm3029_update_srcs
	echo project created at $(PROJECT_BUILD)
	$(call set_one_time_rule,$@)

PHONY += clean_project
clean_project:
	-$(call remove_dir,$(PROJECT_BUILD)/Release)
#OR	
#	$(CCES) -nosplash -application com.analog.crosscore.headlesstools \
 		-data $(WORKSPACE) \
 		-project $(PROJECT_NAME) \
 		-cleanOnly all

.PHONY: $(PHONY)
