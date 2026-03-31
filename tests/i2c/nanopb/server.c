//#include "platform/linux/capi_i2c_linux_driver.h"
#include "platform/linux/capi_i2c_linux_driver.h"
#include "capi_rpc.h"
#include "capi_dma.h"
#include "transport/transport.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


struct capi_i2c_ops *used_i2c_ops = &linux_i2c_ops;//&dummy_i2c_ops;


bool Init(const capi_I2CInitParams *request,
                 capi_I2CInitResponse *response, void *user_data)  {

    capi_I2CConfig req_config = request->config;
    struct capi_i2c_config config;

    config.identifier = req_config.identifier;
    config.clk_freq_hz = req_config.clk_freq_hz;
    config.initiator = req_config.initiator;
    config.address = (uint8_t)req_config.address;
    config.device = (void*)(req_config.device);

    config.dma_handle = (struct capi_dma_handle*)(req_config.dma_handle.pointer_address);
    config.extra = (void*)(req_config.extra);
    config.ops = used_i2c_ops;  // Use server-side platform ops


    capi_I2CHandle req_handle = request->handle;
    struct capi_i2c_controller_handle *handle = NULL;
    if (req_handle.pointer_address != 0) {
        handle = (struct capi_i2c_controller_handle*)(req_handle.pointer_address);
    }
    printf("Identifier %lu, clock %u\n", config.identifier, config.clk_freq_hz);

    // Call the platform implementation
    int ret = used_i2c_ops->init(&handle, &config);

    if (handle != NULL) {
        response->has_params = true;
        response->params.has_handle = true;
        response->params.handle.init_allocated = handle->init_allocated;
        response->params.handle.lock = (uint64_t)handle->lock;
        response->params.handle.priv = (uint64_t)handle->priv;
        response->params.handle.pointer_address = (uint64_t)handle;
    }

    // TODO
    // auto* resp_params = response->mutable_params();
    // auto* resp_config = resp_params->mutable_config();
    // auto* resp_handle = resp_params->mutable_handle();

    // // Copy config fields back
    // resp_config->set_identifier(config.identifier);
    // resp_config->set_clk_freq_hz(config.clk_freq_hz);
    // resp_config->set_initiator(config.initiator);
    // resp_config->set_address(config.address);
    // resp_config->set_device(reinterpret_cast<uint64_t>(config.device));
    // // Copy DMA handle fields back (ops excluded - local only)
    // auto* resp_dma = resp_config->mutable_dma_handle();
    // if (config.dma_handle != NULL) {
    //     resp_dma->set_owned_by_app(config.dma_handle->owned_by_app);
    //     resp_dma->set_init_allocated(config.dma_handle->init_allocated);
    //     resp_dma->set_lock(reinterpret_cast<uint64_t>(config.dma_handle->lock));
    //     resp_dma->set_user_data(reinterpret_cast<uint64_t>(config.dma_handle->user_data));
    //     resp_dma->set_extra(reinterpret_cast<uint64_t>(config.dma_handle->extra));
    //     resp_dma->set_pointer_address(reinterpret_cast<uint64_t>(config.dma_handle));
    // }
    // resp_config->set_extra(reinterpret_cast<uint64_t>(config.extra));

    // // Copy handle fields back (ops excluded - local only)
    // if (handle != NULL) {
    //     resp_handle->set_init_allocated(handle->init_allocated);
    //     resp_handle->set_lock(reinterpret_cast<uint64_t>(handle->lock));
    //     resp_handle->set_priv(reinterpret_cast<uint64_t>(handle->priv));
    //     resp_handle->set_pointer_address(reinterpret_cast<uint64_t>(handle));
    // }

    response->status = ret;

    printf("Server: Init called, pointer_address = 0x%lx, status = %d\n",
            (unsigned long)handle, ret);

    return true;
}


bool Deinit(const capi_I2CDeinitParams *request,
            capi_I2CDeinitResponse *response, void *user_data) {

    struct capi_i2c_controller_handle *handle =
        (struct capi_i2c_controller_handle*)(request->handle.pointer_address);

    printf("Server: Deinit called, pointer_address = 0x%lx\n",
           (unsigned long)handle);

    int ret = used_i2c_ops->deinit(handle);

    response->status = ret;
    return true;
}

bool Transmit(const capi_I2CTransferParams *request,
                 capi_I2CTransferResponse *response, void *user_data) {
    
    capi_I2CDevice req_device = request->device;
    struct capi_i2c_device device;
    device.controller = (struct capi_i2c_controller_handle*)(req_device.controller.pointer_address);
    device.address = (uint16_t)req_device.address;
    device.b10addr = req_device.b10addr;
    device.speed = (req_device.speed);
    device.duty_cycle = (uint8_t)req_device.duty_cycle;
    device.clk_stretch = req_device.clk_stretch;
    device.extra = (void*)(req_device.extra);

    // Convert proto transfer to CAPI transfer
    capi_I2CTransfer req_transfer = request->transfer;
    struct capi_i2c_transfer transfer;
    transfer.repeated_start = req_transfer.repeated_start;
    transfer.no_stop = req_transfer.no_stop;

    // Copy sub_address bytes
    transfer.sub_address_len = req_transfer.sub_address.size;
    if (transfer.sub_address_len > 0) {
        transfer.sub_address = malloc(transfer.sub_address_len);
        memcpy(transfer.sub_address, req_transfer.sub_address.bytes,
               transfer.sub_address_len);
    } else {
        transfer.sub_address = NULL;
    }

    // Copy buf bytes
    transfer.len = req_transfer.buf.size;
    if (transfer.len > 0) {
        transfer.buf = malloc(transfer.len);
        memcpy(transfer.buf, req_transfer.buf.bytes, transfer.len);
    } else {
        transfer.buf = NULL;
    }
    

    // Call platform implementation
    int ret = used_i2c_ops->transmit(&device, &transfer);

    // Build response
    response->has_transfer = true;
    response->transfer.repeated_start = transfer.repeated_start;
    response->transfer.no_stop = transfer.no_stop;

    // Copy response data
    response->transfer.sub_address.size = transfer.sub_address_len;
    if (transfer.sub_address_len > 0) {
        memcpy(response->transfer.sub_address.bytes, transfer.sub_address,
               transfer.sub_address_len);
    }

    response->transfer.buf.size = transfer.len;
    if (transfer.len > 0) {
        memcpy(response->transfer.buf.bytes, transfer.buf, transfer.len);
    }

    response->status = ret;

    // Cleanup
    if (transfer.sub_address) free(transfer.sub_address);
    if (transfer.buf) free(transfer.buf);

    return true;
}

bool Receive(const capi_I2CTransferParams *request,
                 capi_I2CTransferResponse *response, void *user_data) {
    
    capi_I2CDevice req_device = request->device;
    struct capi_i2c_device device;
    device.controller = (struct capi_i2c_controller_handle*)(req_device.controller.pointer_address);
    device.address = (uint16_t)req_device.address;
    device.b10addr = req_device.b10addr;
    device.speed = (req_device.speed);
    device.duty_cycle = (uint8_t)req_device.duty_cycle;
    device.clk_stretch = req_device.clk_stretch;
    device.extra = (void*)(req_device.extra);

    // Convert proto transfer to CAPI transfer
    capi_I2CTransfer req_transfer = request->transfer;
    struct capi_i2c_transfer transfer;
    transfer.repeated_start = req_transfer.repeated_start;
    transfer.no_stop = req_transfer.no_stop;

    // Copy sub_address bytes
    transfer.sub_address_len = req_transfer.sub_address.size;
    if (transfer.sub_address_len > 0) {
        transfer.sub_address = malloc(transfer.sub_address_len);
        memcpy(transfer.sub_address, req_transfer.sub_address.bytes,
               transfer.sub_address_len);
    } else {
        transfer.sub_address = NULL;
    }

    // Copy buf bytes
    transfer.len = req_transfer.buf.size;
    if (transfer.len > 0) {
        transfer.buf = malloc(transfer.len);
        memcpy(transfer.buf, req_transfer.buf.bytes, transfer.len);
    } else {
        transfer.buf = NULL;
    }
    

    // Call platform implementation
    int ret = used_i2c_ops->receive(&device, &transfer);

    // Build response
    response->has_transfer = true;
    response->transfer.repeated_start = transfer.repeated_start;
    response->transfer.no_stop = transfer.no_stop;

    // Copy response data
    response->transfer.sub_address.size = transfer.sub_address_len;
    if (transfer.sub_address_len > 0) {
        memcpy(response->transfer.sub_address.bytes, transfer.sub_address,
               transfer.sub_address_len);
    }

    response->transfer.buf.size = transfer.len;
    if (transfer.len > 0) {
        memcpy(response->transfer.buf.bytes, transfer.buf, transfer.len);
    }

    response->status = ret;

    // Cleanup
    if (transfer.sub_address) free(transfer.sub_address);
    if (transfer.buf) free(transfer.buf);

    return true;
}

static void my_callback(enum capi_i2c_async_event event, void *arg, int event_extra) {
    printf("Server: my_callback triggered, event=%d, extra=%d\n", event, event_extra);
    capi_I2CEvent nanopb_event;
    nanopb_event.event = event;
    nanopb_event.event_extra = event_extra;
    I2CService_SubscribeEvents_Writer *writer = (I2CService_SubscribeEvents_Writer *)arg;
    writer->write(writer, &nanopb_event);
    printf("Server: Event written to stream\n");
}

/*
Once a new callback is registered, it must run continuously since it executes the actual callback logic.
We should be careful: adding too many callbacks means running multiple SubscribeEvents
in different threads on the server, which could be problematic if the server is a small microcontroller.
*/

#define MAX_CLIENTS 20
#define MAX_WRITERS_PER_CLIENT 5

struct WritesrsForClient {
    transport_conn_t *conn;
    I2CService_SubscribeEvents_Writer *writers[MAX_WRITERS_PER_CLIENT];
    int num_writers;
};

static struct WritesrsForClient clients[MAX_CLIENTS];

void addClientWriter(I2CService_SubscribeEvents_Writer *writer) {
    int first_empty_index = -1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].conn == writer->conn) {
            if (clients[i].num_writers < MAX_WRITERS_PER_CLIENT) {
                clients[i].writers[clients[i].num_writers++] = writer;
            } else {
                free(writer);
            }
            return;
        }

        if (clients[i].conn == NULL && first_empty_index < 0) {
            first_empty_index = i;
        }
    }

    if (first_empty_index != -1) {
        clients[first_empty_index].conn = writer->conn;
        clients[first_empty_index].writers[0] = writer;
        clients[first_empty_index].num_writers = 1;
    } else {
        free(writer);
    }
}

void removeClient(transport_conn_t *conn) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].conn == conn) {
            for (int j = 0; j < clients[i].num_writers; j++) {
                clients[i].writers[j]->Destroy(clients[i].writers[j]);
            }
            memset(&clients[i], 0, sizeof(struct WritesrsForClient));
            return;
        }
    }
}

bool SubscribeEvents(const capi_I2CDeinitParams* req, I2CService_SubscribeEvents_Writer* writer, void *user_data) {


    addClientWriter(writer);

    struct capi_i2c_controller_handle *handle =
        (struct capi_i2c_controller_handle*)(req->handle.pointer_address);

    // capi_i2c_callback* expects pointer to function pointer
    capi_i2c_callback cb = my_callback;
    int ret = used_i2c_ops->register_callback(handle, cb, (void*)writer);

    // Send acknowledgment event with status
    capi_I2CEvent ack_event = capi_I2CEvent_init_zero;
    ack_event.handle_pointer_address = req->handle.pointer_address;
    ack_event.is_ack = true;
    ack_event.status = ret;
    writer->write(writer, &ack_event);
    

    printf("Server: Callback registered, ack sent (status=%d)\n", ret);

    return true;
}

I2CService_Server handlers = {
    .Init = Init,
    .Transmit = Transmit,
    .Receive = Receive,
    .Deinit = Deinit,
    .SubscribeEvents = SubscribeEvents,
    .user_data = &used_i2c_ops
};

void RunServer() {
    transport_conn_t server;
    transport_conn_t client;

    printf("Server starting on port 50051...\n");
    transport_listen(&server, 50051);

    while (1) {
        printf("Waiting for client...\n");
        transport_accept(&server, &client);
        printf("Client connected!\n");

        /* Handle multiple requests until client disconnects */
        while (I2CService_Server_HandleRequest(&client, &handlers)) {
            printf("Request handled.\n");
        }

        transport_disconnect(&client);

        removeClient(&client);
        printf("Client disconnected.\n");
    }
}

int main() {
    RunServer();
    return 0;
}