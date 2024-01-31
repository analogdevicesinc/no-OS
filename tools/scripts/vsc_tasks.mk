ifeq 'yes' '$(VSCODE_SUPPORT)'

# Definitions
TASKS_TEMPLATE	:= $(NO-OS)/tools/scripts/platform/template_tasks.json
TASKSJSON       := $(VSCODE_CFG_DIR)/tasks.json

# Read content of settings template into variable and subst keywords
VSC_TASKS_CONTENT		:= $(file < $(TASKS_TEMPLATE))
VSC_TASKS_CONTENT		:= $(subst PLATFORM_VAR,$(PLATFORM),$(VSC_TASKS_CONTENT))
VSC_TASKS_CONTENT		:= $(subst TARGET_HARDWARE,$(TARGET_HARDWARE),$(VSC_TASKS_CONTENT))

endif
