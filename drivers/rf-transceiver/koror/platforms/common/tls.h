#ifndef TLS_H
#define TLS_H

#include <adi_platform.h>

/**
 * Not part of the HAL interface. Must be called before any of the HAL TLS functions are called.
 * This function depends on HAL functionality so the HAL function pointers must be assigned
 * before it is called.
 */
ADI_API adi_hal_Err_e common_TlsInit(void);

/**
 * Provides an implementation for the corresponding HAL interface function. See the HAL interface
 * definition for details.
 */
ADI_API adi_hal_Err_e common_TlsSet(const adi_hal_TlsType_e tlsType, void* const value);

/**
 * Provides an implementation for the corresponding HAL interface function. See the HAL interface
 * definition for details.
 */
ADI_API void* common_TlsGet(const adi_hal_TlsType_e tlsType);

#endif
