/* OSAL function declarations for the i-link IO-Link stack.
 * Implementations are in osal_freertos.c. */

#ifndef OSAL_H
#define OSAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sys/osal_sys.h"
#include "sys/osal_cc.h"

#define OS_WAIT_FOREVER 0xFFFFFFFF

os_mutex_t * os_mutex_create (void);
void os_mutex_lock (os_mutex_t * mutex);
void os_mutex_unlock (os_mutex_t * mutex);
void os_mutex_destroy (os_mutex_t * mutex);

os_event_t * os_event_create (void);
bool os_event_wait (
   os_event_t * event,
   uint32_t mask,
   uint32_t * value,
   uint32_t time_ms);
void os_event_set (os_event_t * event, uint32_t mask);
void os_event_clr (os_event_t * event, uint32_t mask);

os_thread_t * os_thread_create (
   const char * name,
   uint32_t priority,
   size_t stack_size,
   void (*entry) (void * arg),
   void * arg);

os_mbox_t * os_mbox_create (size_t size);
bool os_mbox_post (os_mbox_t * mbox, void * msg, uint32_t time_ms);
bool os_mbox_fetch (os_mbox_t * mbox, void ** msg, uint32_t time_ms);
void os_mbox_destroy (os_mbox_t * mbox);

os_timer_t * os_timer_create (
   uint32_t period_us,
   void (*callback) (os_timer_t * timer, void * arg),
   void * arg,
   bool oneshot);
void os_timer_start (os_timer_t * timer);
void os_timer_stop (os_timer_t * timer);
void os_timer_set (os_timer_t * timer, uint32_t period_us);

void os_usleep (uint32_t us);
uint32_t os_get_current_time_us (void);

#endif /* OSAL_H */
