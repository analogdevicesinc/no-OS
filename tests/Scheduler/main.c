#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "Queue.h"
#include "/home/ruben/ADI/core-sw-common-hal/api/capi_i2c.h"
#include "capi.pb.h"
#include "encoders.h"

#define INIT_I2C 0
#define DEINIT_I2C 1
#define RECEIVE_I2C 2
#define TRANSMIT_I2C 3

struct pair_int_double {
    void* first;
    int second;
};

static const struct pair_int_double func_map[] = {
    { (void*)capi_i2c_init, INIT_I2C },
    { (void*)capi_i2c_deinit, DEINIT_I2C },
    { (void*)capi_i2c_transmit, TRANSMIT_I2C },
    { (void*)capi_i2c_receive, RECEIVE_I2C },
};

int get_id_from_address(void *func) {
    for (int i = 0; i < 4; i++) {
        if (func_map[i].first == func)
            return func_map[i].second;
    }
    return -1;
}


int print_status(int status, const char *text) {
    if (status == 0) {
        printf("%s successful!\n", text);
        return 0;
    } else if (status == -EINVAL) {
        printf("%s failed, invalid address\n", text);
        return 1;
    } else {
        printf("%s failed with status: %d\n", text, status);
        return 1;
    }
}


int RPC_encode(uint8_t CMD_FUNC, uint8_t *buffer, size_t len, va_list args) {
    capi_RPC rpc = capi_RPC_init_zero;
    rpc.FUNCTION_CMD = CMD_FUNC;

    switch (CMD_FUNC) {
        case INIT_I2C: {
            rpc.which_functionParam = capi_RPC_init_param_tag;
            struct capi_i2c_controller_handle **handle = va_arg(args, struct capi_i2c_controller_handle **);
            const struct capi_i2c_config *config = va_arg(args, const struct capi_i2c_config *);
            if (handle && *handle) {
                rpc.functionParam.init_param.has_handle = true;
                rpc.functionParam.init_param.handle = encode_controller_handle(*handle);
            }
            (void)config; /* TODO: encode config when needed */
            break;
        }
        case DEINIT_I2C: {
            rpc.which_functionParam = capi_RPC_deinit_param_tag;
            struct capi_i2c_controller_handle *handle = va_arg(args, struct capi_i2c_controller_handle *);
            if (handle) {
                rpc.functionParam.deinit_param.has_handle = true;
                rpc.functionParam.deinit_param.handle = encode_controller_handle(handle);
            }
            break;
        }
        case RECEIVE_I2C:
        case TRANSMIT_I2C: {
            rpc.which_functionParam = capi_RPC_transfer_param_tag;
            struct capi_i2c_device *device = va_arg(args, struct capi_i2c_device *);
            struct capi_i2c_transfer *transfer = va_arg(args, struct capi_i2c_transfer *);
            rpc.functionParam.transfer_param = encode_transfer_params(device, transfer);
            break;
        }
        default:
            return 0;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, len);
    if (!pb_encode_delimited(&stream, capi_RPC_fields, &rpc)) {
        return -1;
    }
    return (int)stream.bytes_written;
}

void Capi_enqueue_function(SchedulerQueue *scheduler, int trigger_time, void (*func)(void), ...) {
    int CMD_FUNC = get_id_from_address(func);
    uint8_t rpc_tmp_buffer[128];
    capi_ItemSchedule item = { .t = trigger_time };

    va_list args;
    va_start(args, func);
    int rpc_len = RPC_encode(CMD_FUNC, rpc_tmp_buffer, sizeof(rpc_tmp_buffer), args);
    va_end(args);

    if (rpc_len < 0) {
        return;
    }

    item.cmd.size = (pb_size_t)rpc_len;
    memcpy(item.cmd.bytes, rpc_tmp_buffer, rpc_len);

    uint8_t final_output[128];
    pb_ostream_t stream = pb_ostream_from_buffer(final_output, sizeof(final_output));

    if (pb_encode_delimited(&stream, capi_ItemSchedule_fields, &item)) {
        scheduler->enqueue(scheduler, final_output);
    }
}

typedef struct {
    int trigger_time;
    int func_id;
    union {
        capi_I2CInitParams init;
        capi_I2CDeinitParams deinit;
        capi_I2CTransferParams transfer;
    } params;
} DecodedRPC;

DecodedRPC Capi_dequeue_function(SchedulerQueue *scheduler) {
    DecodedRPC result = {0};
    capi_ItemSchedule item = capi_ItemSchedule_init_zero;

    uint8_t *dequeued = scheduler->dequeue(scheduler);
    if (dequeued == NULL) {
        return result;
    }

    uint8_t buffer[128];
    memcpy(buffer, dequeued, 128);

    pb_istream_t stream = pb_istream_from_buffer(buffer, sizeof(buffer));

    capi_RPC rpc = capi_RPC_init_zero;
    if (pb_decode_delimited(&stream, capi_ItemSchedule_fields, &item)) {
        pb_istream_t stream2 = pb_istream_from_buffer(item.cmd.bytes, item.cmd.size);
        result.trigger_time = item.t;
        if (pb_decode_delimited(&stream2, capi_RPC_fields, &rpc)) {
            result.func_id = rpc.FUNCTION_CMD;
            switch(rpc.FUNCTION_CMD) {
                case INIT_I2C:
                    result.params.init = rpc.functionParam.init_param;
                    break;
                case DEINIT_I2C:
                    result.params.deinit = rpc.functionParam.deinit_param;
                    break;
                case RECEIVE_I2C:
                case TRANSMIT_I2C:
                    result.params.transfer = rpc.functionParam.transfer_param;
                    break;
                default:
                    break;
            }
        }
    }
    return result;
}

void run_task(DecodedRPC rpc) {
    struct capi_i2c_device device = {0};
    struct capi_i2c_transfer transfer = {0};
    uint8_t sub_addr_buf[16] = {0};
    uint8_t data_buf[256] = {0};

    /* Pre-allocate buffers for transfer decode */
    transfer.sub_address = sub_addr_buf;
    transfer.buf = data_buf;

    switch(rpc.func_id) {
        case INIT_I2C:
            // TODO
            break;
        case DEINIT_I2C:
            // TODO
            break;
        case RECEIVE_I2C:
            decode_transfer_params(&rpc.params.transfer, &device, &transfer);
            capi_i2c_receive(&device, &transfer);
            break;
        case TRANSMIT_I2C:
            decode_transfer_params(&rpc.params.transfer, &device, &transfer);
            capi_i2c_transmit(&device, &transfer);
            break;
        default:
            break;
    }
}

int main() {
    SchedulerQueue *scheduler = &SchedulerQueue_default;

    scheduler->init(scheduler);

    // 1. Client create params from function
    uint8_t sub_addr = 0x00;
    uint8_t rx_buf[16] = {0};

    struct capi_i2c_transfer transfer = {
        .repeated_start = false,
        .sub_address = &sub_addr,
        .sub_address_len = sizeof(sub_addr),
        .buf = rx_buf,
        .len = 2,
        .no_stop = false,
    };

    struct capi_i2c_device device = {
        .controller = NULL,
        .address = 0x48,
        .b10addr = false,
        .speed = CAPI_I2C_SPEED_STANDARD,
        .duty_cycle = 0,
        .clk_stretch = 0,
        .extra = NULL,
    };

    // 2. Capi enqueue a function and its parameters
    Capi_enqueue_function(scheduler, 500, (void (*)(void))capi_i2c_receive, &device, &transfer);

    // 3.Data transfer
    // TODO

    // 4. Received by server
    DecodedRPC decoded = Capi_dequeue_function(scheduler);

    // 4.5 Add decoder to schedule
    // capi_add_to_schedule(decoded); // TODO

    // 5. Schedule logic
    // run_schedule(); // TODO

    // 6. Schedule triggers task
    run_task(decoded);

    printf("Trigger time: %d, Func ID: %d\n", decoded.trigger_time, decoded.func_id);
}
