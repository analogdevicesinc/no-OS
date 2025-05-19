ESH_ROOT := $(NO-OS)/libraries/esh/esh

EXTRA_LSCRIPT += $(NO-OS)/tools/scripts/platform/esh_sections.ld
CFLAGS += -D__PROMPT__="esh>"
CFLAGS += -D__USER_REPO_VERSION__=$(GIT_VERSION)
CFLAGS += -D__BUILD_USER__=$(USER)
CFLAGS += -D__BUILD_HOST__=$(USER)
CFLAGS += -DECHO_INIT_VALUE=1

SHELL_VERSION=$(shell cd $(ESH_ROOT)/shell && git rev-parse --short HEAD 2>/dev/null)
CFLAGS += -D__SHELL_VERSION__=$(SHELL_VERSION)

SRC_DIRS += $(ESH_ROOT)/shell/shell
SRC_DIRS += $(ESH_ROOT)/shell/lib/a2i
SRC_DIRS += $(ESH_ROOT)/shell/lib/memlog
SRC_DIRS += $(ESH_ROOT)/shell/lib/printf
SRC_DIRS += $(ESH_ROOT)/shell/lib/regs
SRC_DIRS += $(ESH_ROOT)/shell/lib/utils

SRCS += $(ESH_ROOT)/../no_os_esh.c
INCS += $(ESH_ROOT)/../no_os_esh.h

LDFLAGS += -T $(NO-OS)/libraries/esh/esh_linker.ld
