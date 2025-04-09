ifeq ($(FREERTOS_KERNEL),)
$(error $(ENDL)$(ENDL)FREERTOS_KERNEL not defined\
		$(ENDL)$(ENDL)\
		Please run command "export FREERTOS_KERNEL=/path/to/FreeRTOS-Kernel"$(ENDL))
endif # FREERTOS_KERNEL check

# link freeRTOS kernel files
RTOS_INCS = $(FREERTOS_KERNEL)/include/projdefs.h \
        $(FREERTOS_KERNEL)/include/FreeRTOS.h \
        $(FREERTOS_KERNEL)/include/list.h \
        $(FREERTOS_KERNEL)/include/portable.h \
        $(FREERTOS_KERNEL)/include/deprecated_definitions.h \
        $(FREERTOS_KERNEL)/include/mpu_wrappers.h \
        $(FREERTOS_KERNEL)/include/newlib-freertos.h \
        $(FREERTOS_KERNEL)/include/queue.h \
        $(FREERTOS_KERNEL)/include/stack_macros.h \
        $(FREERTOS_KERNEL)/include/semphr.h \
        $(FREERTOS_KERNEL)/include/task.h \
        $(FREERTOS_KERNEL)/include/timers.h \
        $(FREERTOS_KERNEL)/portable/GCC/ARM_CM4F/portmacro.h # TO DO: make port finder generic, based on platform


#Link heap implementation
RTOS_SRCS = $(FREERTOS_KERNEL)/portable/MemMang/heap_2.c

RTOS_SRCS += $(FREERTOS_KERNEL)/tasks.c \
        $(FREERTOS_KERNEL)/list.c \
        $(FREERTOS_KERNEL)/queue.c \
        $(FREERTOS_KERNEL)/timers.c 

RTOS_SRCS += $(FREERTOS_KERNEL)/portable/GCC/ARM_CM4F/port.c

SRCS += $(RTOS_SRCS)
INCS += $(RTOS_INCS)
