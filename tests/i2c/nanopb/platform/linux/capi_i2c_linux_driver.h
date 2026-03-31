#ifndef CAPI_I2C_LINUX_DRIVER_H
#define CAPI_I2C_LINUX_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "capi_i2c.h"

// Dummy/simple I2C platform ops (for demonstration)
extern struct capi_i2c_ops linux_i2c_ops;

/**
 * @struct linux_i2c_desc
 * @brief Linux platform specific I2C descriptor
 */
struct linux_i2c_desc {
	/** /dev/i2c-"device_id" file descriptor */
	int fd;
	/** struct for write - repeated start - read operations */
	struct i2c_msg *messages;
	/** count of i2c messages in array */
	int len_messages;
};

#ifdef __cplusplus
}
#endif

#endif // CAPI_I2C_DUMMY_DRIVER_H
