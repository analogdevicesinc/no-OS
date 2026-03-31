#ifndef ENCODERS_H
#define ENCODERS_H

#include <stdbool.h>
#include <stdint.h>
#include "capi.pb.h"
#include "/home/ruben/ADI/core-sw-common-hal/api/capi_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Controller Handle encoders/decoders */
capi_I2CHandle encode_controller_handle(const struct capi_i2c_controller_handle *handle);
void decode_controller_handle(const capi_I2CHandle *pb_handle, struct capi_i2c_controller_handle *handle);

/* Device encoders/decoders */
capi_I2CDevice encode_device(const struct capi_i2c_device *device);
void decode_device(const capi_I2CDevice *pb_device, struct capi_i2c_device *device);

/* Transfer encoders/decoders */
capi_I2CTransfer encode_transfer(const struct capi_i2c_transfer *transfer);
void decode_transfer(const capi_I2CTransfer *pb_transfer, struct capi_i2c_transfer *transfer);

/* Combined TransferParams encoder (device + transfer) */
capi_I2CTransferParams encode_transfer_params(const struct capi_i2c_device *device,
                                               const struct capi_i2c_transfer *transfer);
void decode_transfer_params(const capi_I2CTransferParams *pb_params,
                            struct capi_i2c_device *device,
                            struct capi_i2c_transfer *transfer);

#ifdef __cplusplus
}
#endif

#endif /* ENCODERS_H */
