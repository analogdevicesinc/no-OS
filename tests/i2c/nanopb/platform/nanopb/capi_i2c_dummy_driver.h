#ifndef CAPI_I2C_DUMMY_DRIVER_H
#define CAPI_I2C_DUMMY_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "capi_i2c.h"

// Dummy/simple I2C platform ops (for demonstration)
extern struct capi_i2c_ops dummy_i2c_ops;

#ifdef __cplusplus
}
#endif

#endif // CAPI_I2C_DUMMY_DRIVER_H
