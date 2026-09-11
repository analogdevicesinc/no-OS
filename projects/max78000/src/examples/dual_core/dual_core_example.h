/***************************************************************************//**
 *   @file   dual_core_example.h
 *   @brief  Dual-core example header (ARM side).
 *   @author Victor Pascu (victor.pascu@analog.com)
 *******************************************************************************/

#ifndef _DUAL_CORE_EXAMPLE_H_
#define _DUAL_CORE_EXAMPLE_H_

/**
 * @brief ARM dual-core example main function.
 * Initializes coprocessor, boots RISC-V, publishes callback, and waits
 * for RISC-V to ring the doorbell periodically.
 * @return 0 on success, negative error code on failure.
 */
int dual_core_example_main(void);

#endif /* _DUAL_CORE_EXAMPLE_H_ */
