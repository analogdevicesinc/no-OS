#include "capi_i2c_nanopbClient_driver.h"
#include "transport/transport.h"
#include "capi.pb.h"
#include <capi_dma.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

/* Global stub */
I2CService_Stub *nanopb_stub = NULL;
static transport_conn_t *conn = NULL;

/* Handle wrapper to track server-side pointer address */
struct nanopb_i2c_handle_wrapper {
    struct capi_i2c_controller_handle base;  /* Must be first member */
    uint64_t server_pointer_address;
};

/* Forward declarations */
static int nanopb_i2c_init(struct capi_i2c_controller_handle **handle,
                           const struct capi_i2c_config *config);
static int nanopb_i2c_deinit(struct capi_i2c_controller_handle *handle);
static int nanopb_i2c_transmit(struct capi_i2c_device *device,
                               struct capi_i2c_transfer *transfer);
static int nanopb_i2c_receive(struct capi_i2c_device *device,
                              struct capi_i2c_transfer *transfer);
static int nanopb_i2c_register_callback(struct capi_i2c_controller_handle *handle,
                             capi_i2c_callback const callback, void *const callback_arg);

/* nanopb ops structure */
struct capi_i2c_ops nanopb_i2c_ops = {
    .init = nanopb_i2c_init,
    .deinit = nanopb_i2c_deinit,
    .transmit = nanopb_i2c_transmit,
    .receive = nanopb_i2c_receive,
    .register_callback = nanopb_i2c_register_callback,
    .transmit_async = NULL,
    .receive_async = NULL,
    .isr = NULL
};

void init_nanopb_client(const char *server_address, int port)
{
    int ret;

    printf("Connecting to %s:%d...\n", server_address, port);
    conn = malloc(sizeof(transport_conn_t));
    if (!conn) {
        fprintf(stderr, "Failed to allocate connection\n");
        return;
    }

    ret = transport_connect(conn, server_address, port);
    if (ret != 0) {
        printf("Connection failed: %d\n", ret);
        free(conn);
        conn = NULL;
        return;
    }
    printf("Connected!\n\n");

    nanopb_stub = I2CService_NewStub(conn);
}

void deinit_nanopb_client(void)
{
    if (nanopb_stub) {
        if (nanopb_stub->channel) {
            transport_disconnect(nanopb_stub->channel);
        }
        I2CService_DeleteStub(nanopb_stub);
        nanopb_stub = NULL;
    }
    if (conn) {
        free(conn);
        conn = NULL;
    }
    printf("Disconnected.\n");
}

/* nanopb-based implementation of capi_i2c_init */
static int nanopb_i2c_init(struct capi_i2c_controller_handle **handle,
                           const struct capi_i2c_config *config)
{
    if (handle == NULL || config == NULL) {
        return -EINVAL;
    }

    if (!nanopb_stub) {
        fprintf(stderr, "Error: nanopb client not initialized.\n");
        return -EINVAL;
    }

    /* Build request */
    capi_I2CInitParams request = capi_I2CInitParams_init_zero;

    /* Set config fields from passed config */
    request.has_config = true;
    request.config.identifier = config->identifier;
    request.config.clk_freq_hz = config->clk_freq_hz;
    request.config.initiator = config->initiator;
    request.config.address = config->address;
    request.config.device = (uint64_t)config->device;
    request.config.extra = (uint64_t)config->extra;

    /* Set DMA handle if present */
    if (config->dma_handle != NULL) {
        request.config.has_dma_handle = true;
        request.config.dma_handle.owned_by_app = config->dma_handle->owned_by_app;
        request.config.dma_handle.init_allocated = config->dma_handle->init_allocated;
        request.config.dma_handle.lock = (uint64_t)config->dma_handle->lock;
        request.config.dma_handle.user_data = (uint64_t)config->dma_handle->user_data;
        request.config.dma_handle.extra = (uint64_t)config->dma_handle->extra;
        request.config.dma_handle.pointer_address = (uint64_t)config->dma_handle;
    }

    request.has_handle = true;
    /* Set handle fields */
    if (*handle != NULL) {
        /* Recover server address from wrapper */
        struct nanopb_i2c_handle_wrapper *wrapper = (struct nanopb_i2c_handle_wrapper *)(*handle);
        request.handle.init_allocated = (*handle)->init_allocated;
        request.handle.lock = (uint64_t)(*handle)->lock;
        request.handle.priv = (uint64_t)(*handle)->priv;
        request.handle.pointer_address = wrapper->server_pointer_address;
    } else {
        /* First call - pointer_address = 0, server will allocate */
        request.handle.pointer_address = 0;
    }

    /* Make RPC call */
    capi_I2CInitResponse response = capi_I2CInitResponse_init_zero;

    printf("Client: Sending I2C Init request...\n");
    bool ok = nanopb_stub->Init(nanopb_stub, &request, &response);

    if (!ok) {
        fprintf(stderr, "Client: RPC call failed\n");
        return -EIO;
    }

    if (response.status != 0) {
        return response.status;
    }

    /* Create local handle wrapper to store the remote handle reference */
    struct nanopb_i2c_handle_wrapper *wrapper;
    if (*handle == NULL) {
        wrapper = malloc(sizeof(struct nanopb_i2c_handle_wrapper));
        if (wrapper == NULL) {
            return -ENOMEM;
        }
        *handle = &wrapper->base;
    } else {
        wrapper = (struct nanopb_i2c_handle_wrapper *)(*handle);
    }

    /* Populate local handle with response data */
    (*handle)->ops = &nanopb_i2c_ops;
    (*handle)->init_allocated = response.params.handle.init_allocated;
    (*handle)->lock = (void *)response.params.handle.lock;
    (*handle)->priv = (void *)response.params.handle.priv;

    /* Store server's handle pointer_address in wrapper */
    wrapper->server_pointer_address = response.params.handle.pointer_address;

    printf("Client: Received pointer_address = 0x%lx\n",
           (unsigned long)wrapper->server_pointer_address);

    return 0;
}

/* nanopb-based implementation of capi_i2c_deinit */
static int nanopb_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
    printf("Client: nanopb_i2c_deinit called, handle=%p\n", (void*)handle);

    if (handle == NULL) {
        fprintf(stderr, "Client: handle is NULL\n");
        return -EINVAL;
    }

    if (!nanopb_stub) {
        fprintf(stderr, "Error: nanopb client not initialized.\n");
        return -EINVAL;
    }

    printf("Client: nanopb_stub is valid\n");

    /* Get server pointer from wrapper */
    struct nanopb_i2c_handle_wrapper *wrapper = (struct nanopb_i2c_handle_wrapper *)handle;

    /* Build request */
    capi_I2CDeinitParams request = capi_I2CDeinitParams_init_zero;
    request.has_handle = true;
    request.handle.init_allocated = handle->init_allocated;
    request.handle.lock = (uint64_t)handle->lock;
    request.handle.priv = (uint64_t)handle->priv;
    request.handle.pointer_address = wrapper->server_pointer_address;

    /* Make RPC call */
    capi_I2CDeinitResponse response = capi_I2CDeinitResponse_init_zero;

    printf("Client: Sending I2C Deinit request...\n");
    bool ok = nanopb_stub->Deinit(nanopb_stub, &request, &response);

    if (!ok) {
        fprintf(stderr, "Client: RPC call failed\n");
        return -EIO;
    }

    /* Free local wrapper */
    free(wrapper);

    return response.status;
}

/* nanopb-based implementation of capi_i2c_transmit */
static int nanopb_i2c_transmit(struct capi_i2c_device *device,
                               struct capi_i2c_transfer *transfer)
{
    if (device == NULL || transfer == NULL) {
        return -EINVAL;
    }

    if (!nanopb_stub) {
        fprintf(stderr, "Error: nanopb client not initialized.\n");
        return -EINVAL;
    }

    /* Get server pointer from controller handle wrapper */
    struct nanopb_i2c_handle_wrapper *wrapper =
        (struct nanopb_i2c_handle_wrapper *)device->controller;

    /* Build request */
    capi_I2CTransferParams request = capi_I2CTransferParams_init_zero;

    /* Set device fields */
    request.has_device = true;
    request.device.has_controller = true;
    request.device.controller.pointer_address = wrapper->server_pointer_address;
    request.device.address = device->address;
    request.device.b10addr = device->b10addr;
    request.device.speed = (int32_t)device->speed;
    request.device.duty_cycle = device->duty_cycle;
    request.device.clk_stretch = device->clk_stretch;
    request.device.extra = (uint64_t)device->extra;

    /* Set transfer fields */
    request.has_transfer = true;

    request.transfer.repeated_start = transfer->repeated_start;
    request.transfer.no_stop = transfer->no_stop;

    /* Copy sub_address */
    if (transfer->sub_address_len > 0 && transfer->sub_address_len <= sizeof(request.transfer.sub_address.bytes)) {
        memcpy(request.transfer.sub_address.bytes, transfer->sub_address, transfer->sub_address_len);
        request.transfer.sub_address.size = transfer->sub_address_len;
    }

    /* Copy buf */
    if (transfer->len > 0 && transfer->len <= sizeof(request.transfer.buf.bytes)) {
        memcpy(request.transfer.buf.bytes, transfer->buf, transfer->len);
        request.transfer.buf.size = transfer->len;
    }

    /* Make RPC call */
    capi_I2CTransferResponse response = capi_I2CTransferResponse_init_zero;

    printf("Client: Sending I2C Transmit request...\n");
    bool ok = nanopb_stub->Transmit(nanopb_stub, &request, &response);

    if (!ok) {
        fprintf(stderr, "Client: RPC call failed\n");
        return -EIO;
    }

    return response.status;
}

/* nanopb-based implementation of capi_i2c_receive */
static int nanopb_i2c_receive(struct capi_i2c_device *device,
                              struct capi_i2c_transfer *transfer)
{
    if (device == NULL || transfer == NULL) {
        return -EINVAL;
    }

    if (!nanopb_stub) {
        fprintf(stderr, "Error: nanopb client not initialized.\n");
        return -EINVAL;
    }

    /* Get server pointer from controller handle wrapper */
    struct nanopb_i2c_handle_wrapper *wrapper =
        (struct nanopb_i2c_handle_wrapper *)device->controller;

    /* Build request */
    capi_I2CTransferParams request = capi_I2CTransferParams_init_zero;

    /* Set device fields */
    request.has_device = true;
    request.device.has_controller = true;
    request.device.controller.pointer_address = wrapper->server_pointer_address;
    request.device.address = device->address;
    request.device.b10addr = device->b10addr;
    request.device.speed = (int32_t)device->speed;
    request.device.duty_cycle = device->duty_cycle;
    request.device.clk_stretch = device->clk_stretch;
    request.device.extra = (uint64_t)device->extra;

    /* Set transfer fields */
    request.has_transfer = true;
    request.transfer.repeated_start = transfer->repeated_start;
    request.transfer.no_stop = transfer->no_stop;

    /* Copy sub_address */
    if (transfer->sub_address_len > 0 && transfer->sub_address_len <= sizeof(request.transfer.sub_address.bytes)) {
        memcpy(request.transfer.sub_address.bytes, transfer->sub_address, transfer->sub_address_len);
        request.transfer.sub_address.size = transfer->sub_address_len;
    }

    /* Set buffer size for receive */
    request.transfer.buf.size = transfer->len;

    /* Make RPC call */
    capi_I2CTransferResponse response = capi_I2CTransferResponse_init_zero;

    printf("Client: Sending I2C Receive request...\n");
    bool ok = nanopb_stub->Receive(nanopb_stub, &request, &response);

    if (!ok) {
        fprintf(stderr, "Client: RPC call failed\n");
        return -EIO;
    }

    /* Copy received data to caller's buffer */
    size_t copy_len = response.transfer.buf.size;
    if (copy_len > transfer->len) {
        copy_len = transfer->len;
    }
    memcpy(transfer->buf, response.transfer.buf.bytes, copy_len);

    return response.status;
}

typedef struct {
    uint64_t server_ptr;
    capi_i2c_callback cb;
    void *cb_arg;
    volatile int ack_status;
    volatile bool ack_received;
} subscribe_ctx_t;

void* i2c_listener_thread(void* arg) {
    subscribe_ctx_t *ctx = (subscribe_ctx_t*)arg;

    capi_I2CDeinitParams request = capi_I2CDeinitParams_init_zero;
    request.has_handle = true;
    request.handle.pointer_address = ctx->server_ptr;

    capi_I2CEvent event;
    I2CService_SubscribeEvents_Reader *reader = nanopb_stub->SubscribeEvents(nanopb_stub, &request);

    if (!reader) {
        ctx->ack_status = -1;
        ctx->ack_received = true;
        return NULL;
    }

    // --- YOUR EXACT WHILE LOOP START ---
    while (reader->read(reader, &event)) {
        if (event.is_ack) {
            printf("Client: Received ack, status=%d\n", event.status);
            ctx->ack_status = event.status;
            ctx->ack_received = true; // Signal main thread to continue
        } else {
            ctx->cb((enum capi_i2c_async_event)(event.event),
                    ctx->cb_arg,
                    event.event_extra);
        }
    }
    // --- YOUR EXACT WHILE LOOP END ---

    reader->destroy(reader);
    free(ctx); 
    return NULL;
}

static int nanopb_i2c_register_callback(struct capi_i2c_controller_handle *handle,
                                       capi_i2c_callback const callback, void *const callback_arg) {
    if (handle == NULL || callback == NULL) {
        return -EINVAL;
    }

    struct nanopb_i2c_handle_wrapper* wrapper = (struct nanopb_i2c_handle_wrapper*)(handle);
    
    // Allocate context on heap so it survives after this function returns
    subscribe_ctx_t *ctx = malloc(sizeof(subscribe_ctx_t));
    ctx->server_ptr = wrapper->server_pointer_address;
    ctx->cb = callback;
    ctx->cb_arg = callback_arg;
    ctx->ack_status = 0;
    ctx->ack_received = false;

    pthread_t tid;
    if (pthread_create(&tid, NULL, i2c_listener_thread, ctx) != 0) {
        free(ctx);
        return -1;
    }
    pthread_detach(tid); // Thread lives on in the background

    // Wait here ONLY until the first ACK arrives
    printf("Client: Waiting for server ack...\n");
    while (!ctx->ack_received) {
        usleep(1000); // 1ms poll
    }

    int final_ack = ctx->ack_status;
    printf("Client: Callback registered, status=%d\n", final_ack);

    return final_ack; 
}
