/***************************************************************************//**
 *   @file   no_os_mutex_stubs.c
 *   @brief  Stub mutex implementation for freestanding RISC-V firmware.
 *   @author Analog Devices Inc.
 *
 * RISC-V firmware runs in a freestanding environment with no operating system
 * and no threading. Mutex operations are no-ops since there's no concurrency.
 *
 * The I2C API layer uses mutexes for thread safety, but on bare-metal RISC-V:
 * - Only one thread of execution exists
 * - No preemption or context switching
 * - No need for synchronization primitives
 *
 * These stubs satisfy the linker and maintain API compatibility.
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Initialize a mutex (stub)
 *
 * On bare-metal RISC-V, no mutex is actually created.
 *
 * @param mutex Pointer to store the mutex pointer (set to dummy value)
 */
void no_os_mutex_init(void **mutex)
{
	if (mutex)
		*mutex = (void *)1; /* Dummy non-NULL value */
}

/**
 * @brief Lock a mutex (stub)
 *
 * On bare-metal RISC-V, no locking needed (single-threaded).
 *
 * @param mutex Mutex pointer
 */
void no_os_mutex_lock(void *mutex)
{
	/* No-op */
	(void)mutex;
}

/**
 * @brief Unlock a mutex (stub)
 *
 * On bare-metal RISC-V, no unlocking needed (single-threaded).
 *
 * @param mutex Mutex pointer
 */
void no_os_mutex_unlock(void *mutex)
{
	/* No-op */
	(void)mutex;
}

/**
 * @brief Remove a mutex (stub)
 *
 * On bare-metal RISC-V, nothing to clean up.
 *
 * @param mutex Mutex pointer to remove
 */
void no_os_mutex_remove(void *mutex)
{
	/* No-op */
	(void)mutex;
}
