#include "capi_i2c_dummy_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Forward declarations
static int dummy_i2c_init(struct capi_i2c_controller_handle **handle,
                          const struct capi_i2c_config *config);
static int dummy_i2c_deinit(struct capi_i2c_controller_handle *handle);
static int dummy_i2c_transmit(struct capi_i2c_device *device,
                               struct capi_i2c_transfer *transfer);
static int dummy_i2c_receive(struct capi_i2c_device *device,
                              struct capi_i2c_transfer *transfer);
static int dummyc_i2c_register_callback(struct capi_i2c_controller_handle *handle,
                             capi_i2c_callback const callback, void *const callback_arg);

struct capi_i2c_ops dummy_i2c_ops = {
    .init = dummy_i2c_init,
    .deinit = dummy_i2c_deinit,
    .transmit = dummy_i2c_transmit,
    .receive = dummy_i2c_receive,
    .register_callback = dummyc_i2c_register_callback,
    .transmit_async = NULL,
    .receive_async = NULL,
    .isr = NULL
};

static struct capi_i2c_dummmy_handle {
    capi_i2c_callback callback;
    void *callback_arg;
} dummy_handle = { NULL, NULL };  // Initialize to NULL

static int dummy_i2c_init(struct capi_i2c_controller_handle **handle,
                          const struct capi_i2c_config *config) {
    printf("Server: Init I2C identifier 0x%lx\n", config->identifier);

     printf("Request Init I2C address %ld", config->identifier);
    if (config->identifier == 0x53) {
        return -EINVAL;
    }
    // Allocate handle if not provided
    if (*handle == NULL) {
        *handle = (struct capi_i2c_controller_handle*)malloc(sizeof(struct capi_i2c_controller_handle));
        if (*handle == NULL) {
            return -ENOMEM;
        }
        (*handle)->init_allocated = true;
    }
    (*handle)->ops = &dummy_i2c_ops;
    (*handle)->lock = NULL;
    (*handle)->priv = NULL;

    return 0;
}

static int dummy_i2c_deinit(struct capi_i2c_controller_handle *handle) {
    printf("Server: Deinit I2C handle 0x%lx\n", (uint64_t)handle);

    if (handle == NULL) {
        return -EINVAL;
    }

    if (handle->init_allocated) {
        free(handle);
    }

    return 0;
}

static int dummy_i2c_transmit(struct capi_i2c_device *device,
                               struct capi_i2c_transfer *transfer) {
    printf("Server: Transmit %u bytes to device 0x%x\n", transfer->len, device->address);
    printf("Server: Data: ");
    for (uint32_t i = 0; i < transfer->len; i++) {
        printf("%d ", transfer->buf[i]);
    }
    printf("\n");

    return 0;
}

static int dummy_i2c_receive(struct capi_i2c_device *device,
                              struct capi_i2c_transfer *transfer) {
    printf("Server: Receive %u bytes from device 0x%x\n", transfer->len, device->address);

    // Call callback if registered
    if (dummy_handle.callback != NULL) {
        dummy_handle.callback(CAPI_I2C_XFR_DONE, dummy_handle.callback_arg, 2);
    } else {
        printf("Callback unavailable\n");
    }

    // Dummy: fill buffer with test numbers
    for (uint32_t i = 0; i < transfer->len; i++) {
        transfer->buf[i] = (uint8_t)(10 + i);
    }

    printf("Server: Data: ");
    for (uint32_t i = 0; i < transfer->len; i++) {
        printf("%d ", transfer->buf[i]);
    }
    printf("\n");

    return 0;
}

static int dummyc_i2c_register_callback(struct capi_i2c_controller_handle *handle,
                             capi_i2c_callback const callback, void *const callback_arg) {
    dummy_handle.callback = callback;
    dummy_handle.callback_arg = callback_arg;
    return 0;
}
