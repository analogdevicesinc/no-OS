#include "encoders.h"
#include <string.h>

/* ============================================================================
 * Controller Handle
 * ============================================================================ */

capi_I2CHandle encode_controller_handle(const struct capi_i2c_controller_handle *handle)
{
    capi_I2CHandle pb_handle = capi_I2CHandle_init_zero;

    if (handle == NULL) {
        return pb_handle;
    }

    pb_handle.init_allocated = handle->init_allocated;
    pb_handle.lock = (uint64_t)(uintptr_t)handle->lock;
    pb_handle.priv = (uint64_t)(uintptr_t)handle->priv;
    pb_handle.pointer_address = (uint64_t)(uintptr_t)handle;

    return pb_handle;
}

void decode_controller_handle(const capi_I2CHandle *pb_handle, struct capi_i2c_controller_handle *handle)
{
    if (pb_handle == NULL || handle == NULL) {
        return;
    }

    handle->init_allocated = pb_handle->init_allocated;
    handle->lock = (void *)(uintptr_t)pb_handle->lock;
    handle->priv = (void *)(uintptr_t)pb_handle->priv;
    /* Note: ops field must be set locally, not transmitted */
}

/* ============================================================================
 * Device
 * ============================================================================ */

capi_I2CDevice encode_device(const struct capi_i2c_device *device)
{
    capi_I2CDevice pb_device = capi_I2CDevice_init_zero;

    if (device == NULL) {
        return pb_device;
    }

    if (device->controller != NULL) {
        pb_device.has_controller = true;
        pb_device.controller = encode_controller_handle(device->controller);
    }

    pb_device.address = device->address;
    pb_device.b10addr = device->b10addr;
    pb_device.speed = (int32_t)device->speed;
    pb_device.duty_cycle = device->duty_cycle;
    pb_device.clk_stretch = device->clk_stretch;
    pb_device.extra = (uint64_t)(uintptr_t)device->extra;
    pb_device.pointer_address = (uint64_t)(uintptr_t)device;

    return pb_device;
}

void decode_device(const capi_I2CDevice *pb_device, struct capi_i2c_device *device)
{
    if (pb_device == NULL || device == NULL) {
        return;
    }

    /* Controller pointer must be resolved separately using pointer_address */
    if (pb_device->has_controller) {
        device->controller = (struct capi_i2c_controller_handle *)(uintptr_t)pb_device->controller.pointer_address;
    } else {
        device->controller = NULL;
    }

    device->address = (uint16_t)pb_device->address;
    device->b10addr = pb_device->b10addr;
    device->speed = (enum capi_i2c_speed)pb_device->speed;
    device->duty_cycle = (uint8_t)pb_device->duty_cycle;
    device->clk_stretch = pb_device->clk_stretch;
    device->extra = (void *)(uintptr_t)pb_device->extra;
}

/* ============================================================================
 * Transfer
 * ============================================================================ */

capi_I2CTransfer encode_transfer(const struct capi_i2c_transfer *transfer)
{
    capi_I2CTransfer pb_transfer = capi_I2CTransfer_init_zero;

    if (transfer == NULL) {
        return pb_transfer;
    }

    pb_transfer.repeated_start = transfer->repeated_start;
    pb_transfer.no_stop = transfer->no_stop;

    /* Copy sub_address if present */
    if (transfer->sub_address != NULL && transfer->sub_address_len > 0) {
        size_t len = transfer->sub_address_len;
        if (len > sizeof(pb_transfer.sub_address.bytes)) {
            len = sizeof(pb_transfer.sub_address.bytes);
        }
        memcpy(pb_transfer.sub_address.bytes, transfer->sub_address, len);
        pb_transfer.sub_address.size = (pb_size_t)len;
    }

    /* Copy buf if present */
    if (transfer->buf != NULL && transfer->len > 0) {
        size_t len = transfer->len;
        if (len > sizeof(pb_transfer.buf.bytes)) {
            len = sizeof(pb_transfer.buf.bytes);
        }
        memcpy(pb_transfer.buf.bytes, transfer->buf, len);
        pb_transfer.buf.size = (pb_size_t)len;
    }

    return pb_transfer;
}

void decode_transfer(const capi_I2CTransfer *pb_transfer, struct capi_i2c_transfer *transfer)
{
    if (pb_transfer == NULL || transfer == NULL) {
        return;
    }

    transfer->repeated_start = pb_transfer->repeated_start;
    transfer->no_stop = pb_transfer->no_stop;

    /* Note: sub_address and buf pointers must point to valid memory allocated by caller */
    if (transfer->sub_address != NULL && pb_transfer->sub_address.size > 0) {
        memcpy(transfer->sub_address, pb_transfer->sub_address.bytes, pb_transfer->sub_address.size);
        transfer->sub_address_len = (uint8_t)pb_transfer->sub_address.size;
    }

    if (transfer->buf != NULL && pb_transfer->buf.size > 0) {
        memcpy(transfer->buf, pb_transfer->buf.bytes, pb_transfer->buf.size);
        transfer->len = pb_transfer->buf.size;
    }
}

/* ============================================================================
 * Combined TransferParams (device + transfer)
 * ============================================================================ */

capi_I2CTransferParams encode_transfer_params(const struct capi_i2c_device *device,
                                               const struct capi_i2c_transfer *transfer)
{
    capi_I2CTransferParams params = capi_I2CTransferParams_init_zero;

    if (device != NULL) {
        params.has_device = true;
        params.device = encode_device(device);
    }

    if (transfer != NULL) {
        params.has_transfer = true;
        params.transfer = encode_transfer(transfer);
    }

    return params;
}

void decode_transfer_params(const capi_I2CTransferParams *pb_params,
                            struct capi_i2c_device *device,
                            struct capi_i2c_transfer *transfer)
{
    if (pb_params == NULL) {
        return;
    }

    if (device != NULL && pb_params->has_device) {
        decode_device(&pb_params->device, device);
    }

    if (transfer != NULL && pb_params->has_transfer) {
        decode_transfer(&pb_params->transfer, transfer);
    }
}
