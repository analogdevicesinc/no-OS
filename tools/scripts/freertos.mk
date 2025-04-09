FREERTOS_KERNEL := $(NO-OS)/libraries/free_rtos/free_rtos

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

# Include freeRTOS platform specifics
SRCS += $(NO-OS)/drivers/platform/freeRTOS/freertos_mutex.c \
        $(NO-OS)/drivers/platform/freeRTOS/freertos_semaphore.c \
        $(NO-OS)/drivers/platform/freeRTOS/freertos_delay.c 

# Include FreeRTOS specific configurations
ifeq ($(wildcard $(FREERTOS_CONFIG_PATH)),)
$(error File FreeRTOSConfig.h does not exists)
else
$(info Found FreeRTOSConfig.h config file)
INCS += $(FREERTOS_CONFIG_PATH)
endif

