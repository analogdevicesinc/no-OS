#ifndef CAPI_I2C_NANOPB_CLIENT_DRIVER_H
#define CAPI_I2C_NANOPB_CLIENT_DRIVER_H

#include <capi_i2c.h>
#include <capi_dma.h>

#include "capi_rpc.h"

/* nanopb-based I2C ops structure */
extern struct capi_i2c_ops nanopb_i2c_ops;

/* Global stub - accessible for cleanup */
extern I2CService_Stub *nanopb_stub;

/* Initialize nanopb client connection */
void init_nanopb_client(const char *server_address, int port);

/* Cleanup nanopb client connection */
void deinit_nanopb_client(void);

#endif /* CAPI_I2C_NANOPB_CLIENT_DRIVER_H */
