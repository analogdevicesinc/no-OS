/* OSAL implementation for FreeRTOS.
 * Maps rt-labs OSAL functions to FreeRTOS API calls. */

#include "osal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Mutex */

os_mutex_t * os_mutex_create (void)
{
   return (os_mutex_t *)xSemaphoreCreateMutex();
}

void os_mutex_lock (os_mutex_t * mutex)
{
   xSemaphoreTake ((SemaphoreHandle_t)mutex, portMAX_DELAY);
}

void os_mutex_unlock (os_mutex_t * mutex)
{
   xSemaphoreGive ((SemaphoreHandle_t)mutex);
}

void os_mutex_destroy (os_mutex_t * mutex)
{
   vSemaphoreDelete ((SemaphoreHandle_t)mutex);
}

/* Event flags */

os_event_t * os_event_create (void)
{
   return (os_event_t *)xEventGroupCreate();
}

bool os_event_wait (
   os_event_t * event,
   uint32_t mask,
   uint32_t * value,
   uint32_t time_ms)
{
   TickType_t ticks;
   EventBits_t bits;

   if (time_ms == OS_WAIT_FOREVER)
   {
      ticks = portMAX_DELAY;
   }
   else
   {
      ticks = pdMS_TO_TICKS (time_ms);
   }

   bits = xEventGroupWaitBits (
      (EventGroupHandle_t)event,
      (EventBits_t)mask,
      pdFALSE,
      pdFALSE,
      ticks);

   *value = (uint32_t)bits & mask;

   if ((*value & mask) == 0)
   {
      return true;
   }

   return false;
}

void os_event_set (os_event_t * event, uint32_t mask)
{
   BaseType_t higher_prio_woken = pdFALSE;

   if (xPortIsInsideInterrupt())
   {
      xEventGroupSetBitsFromISR (
         (EventGroupHandle_t)event,
         (EventBits_t)mask,
         &higher_prio_woken);
      portYIELD_FROM_ISR (higher_prio_woken);
   }
   else
   {
      xEventGroupSetBits ((EventGroupHandle_t)event, (EventBits_t)mask);
   }
}

void os_event_clr (os_event_t * event, uint32_t mask)
{
   xEventGroupClearBits ((EventGroupHandle_t)event, (EventBits_t)mask);
}

/* Thread */

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stack_size,
   void (*entry) (void * arg),
   void * arg)
{
   TaskHandle_t handle = NULL;

   /* stack_size is in bytes, xTaskCreate expects words */
   xTaskCreate (
      entry,
      name,
      stack_size / sizeof (StackType_t),
      arg,
      (UBaseType_t)priority,
      &handle);

   return (os_thread_t *)handle;
}

/* Mailbox (FreeRTOS queue of pointers) */

os_mbox_t * os_mbox_create (size_t size)
{
   return (os_mbox_t *)xQueueCreate ((UBaseType_t)size, sizeof (void *));
}

bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time_ms)
{
   TickType_t ticks;

   if (time_ms == OS_WAIT_FOREVER)
   {
      ticks = portMAX_DELAY;
   }
   else
   {
      ticks = pdMS_TO_TICKS (time_ms);
   }

   if (xQueueSend ((QueueHandle_t)mbox, &msg, ticks) != pdTRUE)
   {
      return true;
   }

   return false;
}

bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time_ms)
{
   TickType_t ticks;

   if (time_ms == OS_WAIT_FOREVER)
   {
      ticks = portMAX_DELAY;
   }
   else
   {
      ticks = pdMS_TO_TICKS (time_ms);
   }

   if (xQueueReceive ((QueueHandle_t)mbox, msg, ticks) != pdTRUE)
   {
      return true;
   }

   return false;
}

void os_mbox_destroy (os_mbox_t * mbox)
{
   vQueueDelete ((QueueHandle_t)mbox);
}

/* Timer */

/* FreeRTOS timer callbacks have a different signature than the OSAL ones.
 * We store the OSAL callback and arg in the timer ID field. */
typedef struct
{
   void (*callback) (os_timer_t * timer, void * arg);
   void * arg;
} osal_timer_ctx_t;

static void osal_timer_cb (TimerHandle_t xTimer)
{
   osal_timer_ctx_t * ctx = (osal_timer_ctx_t *)pvTimerGetTimerID (xTimer);

   ctx->callback ((os_timer_t *)xTimer, ctx->arg);
}

os_timer_t * os_timer_create (
   uint32_t period_us,
   void (*callback) (os_timer_t * timer, void * arg),
   void * arg,
   bool oneshot)
{
   osal_timer_ctx_t * ctx = pvPortMalloc (sizeof (osal_timer_ctx_t));

   if (ctx == NULL)
   {
      return NULL;
   }

   ctx->callback = callback;
   ctx->arg      = arg;

   TickType_t ticks = pdMS_TO_TICKS (period_us / 1000);

   if (ticks == 0)
   {
      ticks = 1;
   }

   TimerHandle_t handle = xTimerCreate (
      "osal",
      ticks,
      oneshot ? pdFALSE : pdTRUE,
      ctx,
      osal_timer_cb);

   if (handle == NULL)
   {
      vPortFree (ctx);
   }

   return (os_timer_t *)handle;
}

void os_timer_start (os_timer_t * timer)
{
   xTimerStart ((TimerHandle_t)timer, portMAX_DELAY);
}

void os_timer_stop (os_timer_t * timer)
{
   xTimerStop ((TimerHandle_t)timer, portMAX_DELAY);
}

void os_timer_set (os_timer_t * timer, uint32_t period_us)
{
   TickType_t ticks = pdMS_TO_TICKS (period_us / 1000);

   if (ticks == 0)
   {
      ticks = 1;
   }

   xTimerChangePeriod ((TimerHandle_t)timer, ticks, portMAX_DELAY);
}

/* Utility */

void os_usleep (uint32_t us)
{
   TickType_t ticks = pdMS_TO_TICKS (us / 1000);

   if (ticks == 0)
   {
      ticks = 1;
   }

   vTaskDelay (ticks);
}

uint32_t os_get_current_time_us (void)
{
   return (uint32_t)(xTaskGetTickCount() * (1000000 / configTICK_RATE_HZ));
}
