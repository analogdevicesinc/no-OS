#
#	Makefile to create project dependent configs files to enable VS Code IntelliSense
#
ifeq 'yes' '$(VSCODE_SUPPORT)'

# Definitions
CPP_PROP_TEMPLATE        = $(NO-OS)/tools/scripts/platform/template_c_cpp_properties.json
CPP_PROP_JSON            = $(VSCODE_CFG_DIR)/c_cpp_properties.json
COMMA                    = ,

# Remove -I and -D
FLAGS_WITHOUT_D 		= $(sort $(subst -D,,$(filter -D%, $(CFLAGS))))
INCS_WITHOUT_I 			= $(sort $(subst -I,,$(filter -I%, $(CFLAGS))))

# Add quotation marks
INCLUDEPATH_INTELLI                 := $(patsubst %,"%",$(strip $(INCS_WITHOUT_I)))
DEFINES_INTELLI                     := $(patsubst %,"%",$(strip $(FLAGS_WITHOUT_D)))
COMPILER_INTELLISENSE_PATH          := $(patsubst %,"%",$(strip $(COMPILER_INTELLISENSE_PATH)))

# Add Comma after each include/define but exclude last item
INCLUDEPATH_INTELLI		:= $(subst " ,"$(COMMA) ,$(INCLUDEPATH_INTELLI))
DEFINES_INTELLI			:= $(subst " ,"$(COMMA) ,$(DEFINES_INTELLI))

# Correct include path to point at the source not at the build directory
INCLUDE_CORE_PATTERN 	= $(PROJECT_BUILD)/$(PROJECT_NAME)
INCLUDE_CORE_CORRECTED	= $(PROJECT)
INCLUDE_NOOS_PATTERN	= $(PROJECT_BUILD)/noos
INCLUDE_NOOS_CORRECTED	= $(NO-OS)

# Apply the path corrections
INCLUDEPATH_INTELLI		:= $(subst ${INCLUDE_CORE_PATTERN},$(INCLUDE_CORE_CORRECTED),$(strip $(INCLUDEPATH_INTELLI)))
INCLUDEPATH_INTELLI		:= $(subst ${INCLUDE_NOOS_PATTERN},$(INCLUDE_NOOS_CORRECTED),$(strip $(INCLUDEPATH_INTELLI)))

ifneq "$(INCLUDE_OTHER_CORRECTED)" ""
    INCLUDEPATH_INTELLI		:= $(subst ${INCLUDE_OTHER_PATTERN},$(INCLUDE_OTHER_CORRECTED),$(strip $(INCLUDEPATH_INTELLI)))
endif

# Read content of template into variable and subst keywords
CPP_FINAL_CONTENT		:= $(file < $(CPP_PROP_TEMPLATE))
CPP_FINAL_CONTENT		:= $(subst VSCODE_INCLUDEPATH_INTELLI,$(INCLUDEPATH_INTELLI),$(CPP_FINAL_CONTENT))
CPP_FINAL_CONTENT 		:= $(subst VSCODE_DEFINES_INTELLI,$(DEFINES_INTELLI),$(CPP_FINAL_CONTENT))
CPP_FINAL_CONTENT 		:= $(subst COMPILER_INTELLISENSE_PATH,$(COMPILER_INTELLISENSE_PATH),$(CPP_FINAL_CONTENT))

endif

