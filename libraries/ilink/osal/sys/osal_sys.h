/* OSAL type definitions for FreeRTOS.
 * Maps rt-labs OSAL types to FreeRTOS handles. The i-link stack only
 * uses pointers to these types, so we typedef to the underlying
 * FreeRTOS structs that each handle points to. */

#ifndef OSAL_SYS_H
#define OSAL_SYS_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

typedef struct QueueDefinition   os_mutex_t;
typedef struct EventGroupDef_t  os_event_t;
typedef struct tskTaskControlBlock os_thread_t;
typedef struct QueueDefinition   os_mbox_t;
typedef struct tmrTimerControl   os_timer_t;

#endif /* OSAL_SYS_H */
