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

PLATFORM_RELATIVE_PATH = $1
PLATFORM_FULL_PATH = $1

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
DUMMY_VAR := $(shell $(call make_dir_link,$(ADUCM_DFP)/Source/drivers,$(PLATFORM_TOOLS)/dfp_drivers))
endif
DFP_DRIVERS = $(PLATFORM_TOOLS)/dfp_drivers

else

DFP_DRIVERS = $(ADUCM_DFP)/Source/drivers

endif

#------------------------------------------------------------------------------
#                             DFP DEPENDENCIES                          
#------------------------------------------------------------------------------

CCES_PROJ_NAME = $(notdir $(PROJECT_BUILD))

#DFP Files
DFP_FILES = $(call rwildcard,$(DFP_DRIVERS),*.c)
#Not for aducm3029
DFP_IGNORED_FILES += $(DFP_DRIVERS)/flash/adi_flash_data.c $(DFP_DRIVERS)/rtc/adi_rtc_data.c

PLATFORM_SRCS += $(filter-out $(DFP_IGNORED_FILES), $(DFP_FILES))

PLATFORM_INCS = -I"$(ADUCM_DFP)/Include"
PLATFORM_INCS += -I"$(CMSIS_CORE)/Include"
PIN_MUX = $(PROJECT)/pinmux_config.c
PROJECT_PIN_MUX = $(PROJECT_BUILD)/system/pinmux/GeneratedSources/pinmux_config.c

PLATFORM_SRCS += $(PROJECT_PIN_MUX)
PLATFORM_SRCS += $(PROJECT_BUILD)/system/adi_initialize.c
PLATFORM_SRCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029/startup_ADuCM3029.c
PLATFORM_SRCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029/system_ADuCM3029.c
ASM_SRCS += $(PROJECT_BUILD)/RTE/Device/ADuCM3029/reset_ADuCM3029.S
PLATFORM_INCS += -I$(PROJECT_BUILD)/RTE/Device/ADuCM3029
PLATFORM_INCS += -I$(PROJECT_BUILD)/RTE
PLATFORM_INCS += -I$(PROJECT_BUILD)/system

#------------------------------------------------------------------------------
#                           COMPILING DATA                              
#------------------------------------------------------------------------------

GENERIC_FLAGS += -DCORE0 -D_RTE_ -D__ADUCM3029__ -D__SILICON_REVISION__=0xffff -mcpu=cortex-m3 -mthumb

GENERIC_DEBUG_FLAGS = -g -gdwarf-2 -D_DEBUG

GENERIC_RELEASE_FLAGS = -DNDEBUG 

CFLAGS += -DADUCM_PLATFORM

ASFLAGS	+= $(GENERIC_FLAGS) $(PLATFORM_INCS) -x assembler-with-cpp 
CFLAGS	+= $(GENERIC_FLAGS) -Wall -ffunction-sections -fdata-sections

CFLAGS	+= $(GENERIC_RELEASE_FLAGS)
ASFLAGS += $(GENERIC_RELEASE_FLAGS)

CFLAGS	+= $(GENERIC_DEBUG_FLAGS)
ASFLAGS += $(GENERIC_DEBUG_FLAGS)

LSCRIPT	= $(PROJECT_BUILD)/RTE/Device/ADuCM3029/ADuCM3029.ld

LDFLAGS	= -Wl,--gc-sections -mcpu=cortex-m3 -mthumb -lm

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
AR = arm-none-eabi-ar

HEX = $(basename $(BINARY)).hex

#------------------------------------------------------------------------------
#                                 RULES                              
#------------------------------------------------------------------------------

post_build: $(HEX)

$(HEX): $(BINARY)
	$(MUTE) $(call print,[HEX] $(notdir $@))
	$(MUTE) arm-none-eabi-objcopy -O ihex $(BINARY) $(HEX)
	$(MUTE) $(call print,$(notdir $@) is ready)

clean: clean_hex

clean_hex:
	@$(call print,[Delete] $(HEX))
	-$(MUTE) $(call remove_fun,$(HEX)) $(HIDE)

ifneq ($(wildcard $(BUILD_DIR)),)
all: $(PIN_MUX)
endif

#Used to update pinmux if updated on project
$(PIN_MUX): $(PROJECT_PIN_MUX)
	$(call print,Updating pinmux)
	$(MUTE) $(call copy_fun,$(PROJECT_PIN_MUX),$(PIN_MUX)) $(HIDE)

# Upload binary to target
PHONY += aducm3029_run
aducm3029_run: all
	$(MUTE) openocd -s "$(OPENOCD_SCRIPTS)" -f interface/cmsis-dap.cfg \
		-s "$(ADUCM_DFP)/openocd/scripts" -f target/aducm3029.cfg \
		-c "program  $(BINARY) verify reset exit" $(HIDE)

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:3333 > $(BINARY).gdb
	@echo load $(BINARY) >> $(BINARY).gdb
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo monitor reset halt >> $(BINARY).gdb
	@echo tb main >> $(BINARY).gdb
	@echo c >> $(BINARY).gdb
	@echo refresh >> $(BINARY).gdb

.PHONY: debug
debug: all $(BINARY).gdb
	(openocd -s "$(OPENOCD_SCRIPTS)" -f interface/cmsis-dap.cfg \
		-s "$(ADUCM_DFP)/openocd/scripts" -f target/aducm3029.cfg \
		-c "init" &);
	/usr/bin/arm-none-eabi-gdb -tui --command=$(BINARY).gdb || \
		arm-none-eabi-gdb --command=$(BINARY).gdb

#------------------------------------------------------------------------------
#                             PROJECT RULES                              
#------------------------------------------------------------------------------

PROJECT_BINARY = $(PROJECT_BUILD)/Release/$(PROJECT_NAME)

develop:
	cces -data $(WORKSPACE)

#This way will not work if the rest button is press or if a printf is executed
#because there is a bug in crossCore that doesn't enable to remove semihosting
#from command line. If semihosting is removed from the IDE this will work to
#CCESS bug: https://labrea.ad.analog.com/browse/CCES-22274
PHONY += project_run
project_run: build_project
	$(MUTE) openocd -s "$(OPENOCD_SCRIPTS)" -f interface/cmsis-dap.cfg \
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
	-c exit $(HIDE)

PHONY += aducm3029_project_build
aducm3029_project_build: aducm3029_project $(LIB_TARGETS)
	$(MUTE) $(CCES) -nosplash -application com.analog.crosscore.headlesstools \
		-data $(WORKSPACE) \
		-project $(CCES_PROJ_NAME) \
		-build Release $(HIDE)


EXTRA_FLAGS = $(sort $(subst -D,,$(filter -D%, $(filter-out $(GENERIC_FLAGS),$(CFLAGS)))))
ADD_COMPILER_DEFINES = $(foreach flag, $(EXTRA_FLAGS), \
			-append-switch compiler -D=$(flag))

RELATIVE_INCS=$(patsubst $(PROJECT_BUILD)/%,%,$(EXTRA_INC_PATHS))
#Flags for each include directory using as reference the project directory
ifeq ($(OS), Windows_NT)
INCLUDE_FLAGS = $(addprefix -append-switch compiler -I=$${ProjDirPath}/,$(RELATIVE_INCS))
else
INCLUDE_FLAGS = $(addprefix -append-switch compiler -I='$${ProjDirPath}/,$(addsuffix ',$(RELATIVE_INCS)))
endif

#Create new project with platform driver and utils source folders linked
$(PROJECT_TARGET):
	$(call print,Creating IDE project)
	$(MUTE) $(CCES) -nosplash -application com.analog.crosscore.headlesstools \
		-command projectcreate \
		-data $(WORKSPACE) \
		-project $(PROJECT_BUILD) \
		-project-name $(CCES_PROJ_NAME) \
		-processor ADuCM3029 \
		-type Executable \
		-revision any \
		-language C \
		-config Release \
		-remove-switch linker -specs=rdimon.specs $(HIDE)
	$(call print,Configuring project)
#Overwrite system.rteconfig file with one that enables all DFP feautres neede by noos
	$(MUTE) $(call copy_fun,$(PLATFORM_TOOLS)/system.rteconfig,$(PROJECT_BUILD)/system.rteconfig) $(HIDE)
#Adding pinmux plugin (Did not work to add it in the first command) and update project
	$(MUTE) $(CCES) -nosplash -application com.analog.crosscore.headlesstools \
 		-command addaddin \
 		-data $(WORKSPACE) \
 		-project $(CCES_PROJ_NAME) \
 		-id com.analog.crosscore.ssldd.pinmux.component \
		-version latest \
		-regensrc \
		$(INCLUDE_FLAGS) \
		$(ADD_COMPILER_DEFINES) \
		-append-switch linker additionaloption="$(LIB_PATHS) $(LIB_FLAGS)" \
		$(HIDE)
#The default startup_ADuCM3029.c has compiling errors
#TODO Replace with patch if team think is a better aproch to install a windows
#program for patching	
	$(MUTE) $(call copy_fun\
	,$(PLATFORM_TOOLS)/startup_ADuCM3029_patch.c,$(PROJECT_BUILD)/RTE/Device/ADuCM3029/startup_ADuCM3029.c) $(HIDE)
#Remove default files from projectsrc
	$(MUTE) $(call remove_dir,$(PROJECT_BUILD)/src) $(HIDE)
	$(MUTE) $(call set_one_time_rule,$@) $(HIDE)

copy_pinmux:
	$(MUTE) $(call copy_fun,$(PIN_MUX),$(PROJECT_PIN_MUX)) $(HIDE)

update_srcs: copy_pinmux

PHONY += clean_project
clean_project:
	-$(MUTE) $(call remove_dir,$(PROJECT_BUILD)/Release) $(HIDE)
#OR	
#	$(CCES) -nosplash -application com.analog.crosscore.headlesstools \
 		-data $(WORKSPACE) \
 		-project $(PROJECT_NAME) \
 		-cleanOnly all

.PHONY: $(PHONY)
