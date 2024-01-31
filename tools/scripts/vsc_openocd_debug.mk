#
#	Makefile to create project dependent configs files for debugging with openocd
#
ifeq 'yes' '$(VSCODE_SUPPORT)'

# Definitions
SETTINGS_TEMPLATE	:= $(NO-OS)/tools/scripts/platform/template_settings.json
LAUNCH_TEMPLATE		:= $(NO-OS)/tools/scripts/platform/template_launch.json
SETTINGSJSON 		:= $(VSCODE_CFG_DIR)/settings.json
LAUNCHJSON 			:= $(VSCODE_CFG_DIR)/launch.json

# Determine arm-none-eabi-gdb path
GDB_PATH			?= $(realpath $(dir $(shell which $(GDB))))

# Read content of settings template into variable and subst keywords
VSC_SET_CONTENT		:= $(file < $(SETTINGS_TEMPLATE))
VSC_SET_CONTENT		:= $(subst VSCODE_GDB_PATH,"$(GDB_PATH)/$(GDB)",$(VSC_SET_CONTENT))
VSC_SET_CONTENT		:= $(subst VSCODE_OPENOCD_PATH,"$(OPENOCD_BIN)/openocd",$(VSC_SET_CONTENT))

# Read content of launch template into variable and subst keywords
VSC_LAUNCH_CONTENT	:= $(file < $(LAUNCH_TEMPLATE))
VSC_LAUNCH_CONTENT	:= $(subst VSCODE_ELF_FILE,"$(BINARY)",$(VSC_LAUNCH_CONTENT))
VSC_LAUNCH_CONTENT	:= $(subst VSCODE_SEARCH_DIR,"$(OPENOCD_SCRIPTS)",$(VSC_LAUNCH_CONTENT))
VSC_LAUNCH_CONTENT	:= $(subst VSCODE_CMSISCFG_FILE,$(VSCODE_CMSISCFG_FILE),$(VSC_LAUNCH_CONTENT))
VSC_LAUNCH_CONTENT	:= $(subst VSCODE_STLINKCFG_FILE,"$(BINARY).openocd",$(VSC_LAUNCH_CONTENT))
VSC_LAUNCH_CONTENT	:= $(subst VSCODE_SVD_FILE,"$(OPENOCD_SVD)/$(TARGETSVD).svd",$(VSC_LAUNCH_CONTENT))
VSC_LAUNCH_CONTENT	:= $(subst PLATFORM_VAR,"$(PLATFORM) Build",$(VSC_LAUNCH_CONTENT))

endif

