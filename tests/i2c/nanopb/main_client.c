#include "platform/nanopb/capi_i2c_nanopbClient_driver.h"
#include <stdio.h>
#include <stddef.h>
#include <errno.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 50051

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

struct Arguments {
    int x;
    char y;
};

void my_callback(enum capi_i2c_async_event event, void *arg, int event_extra) {
    struct Arguments *arguments = (struct Arguments *)arg;
    printf("Callback: event=%d, extra=%d, args: x=%d, y=%c\n",
           event, event_extra, arguments->x, arguments->y);
}

int main(void)
{
    int status;
    printf("=== I2C CAPI nanopb Client Example ===\n\n");

    /* Initialize nanopb client connection */
    init_nanopb_client(SERVER_HOST, SERVER_PORT);

    struct capi_i2c_config config = {
        .identifier = 0x01,
        .clk_freq_hz = 100000,
        .initiator = true,
        .address = 0x52,
        .device = NULL,
        .dma_handle = NULL,
        .extra = NULL,
        .ops = &nanopb_i2c_ops
    };

    struct capi_i2c_controller_handle *handle = NULL;

    /* Initialize I2C */
    {
        printf("Calling capi_i2c_init()...\n");
        status = capi_i2c_init(&handle, &config);
        if (print_status(status, "I2C initialization"))
            goto free_client;
    }

    /* Transfer I2C */
    {
        struct capi_i2c_device device = {
            .controller = handle,
            .address = 0x10,
            .b10addr = false,
            .speed = CAPI_I2C_SPEED_STANDARD,
            .duty_cycle = 0,
            .clk_stretch = 0,
            .extra = NULL,
        };

        // Set pins 0-3 as DAC outputs (register 0x05 = DAC pin config)
        uint8_t sub_addr = 0x05;
        uint8_t tx_data[] = {0x00, 0x0F};

        struct capi_i2c_transfer transfer = {
            .repeated_start = false,
            .sub_address = &sub_addr,
            .sub_address_len = sizeof(sub_addr),
            .buf = tx_data,
            .len = sizeof(tx_data),
            .no_stop = false,
        };

        printf("Calling capi_i2c_transmit()...\n");
        status = capi_i2c_transmit(&device, &transfer);
        if (print_status(status, "I2C transfer"))
            goto free_client;
    }

    /* Receive I2C */
    {
        uint8_t sub_addr = 0x00;
        uint8_t rx_buf[16] = {0};

        struct capi_i2c_transfer transfer2 = {
            .repeated_start = false,
            .sub_address = &sub_addr,
            .sub_address_len = sizeof(sub_addr),
            .buf = rx_buf,
            .len = 2,
            .no_stop = false,
        };

        struct capi_i2c_device device2 = {
            .controller = handle,
            .address = 0x48,
            .b10addr = false,
            .speed = CAPI_I2C_SPEED_STANDARD,
            .duty_cycle = 0,
            .clk_stretch = 0,
            .extra = NULL,
        };

        printf("Calling capi_i2c_receive()...\n");
        status = capi_i2c_receive(&device2, &transfer2);
        if (print_status(status, "I2C receive"))
            goto free_client;

        printf("Data received: ");
        for (int i = 0; i < transfer2.len; i++) {
            printf("%d ", transfer2.buf[i]);
        }
        printf("\n");
        int16_t raw = (rx_buf[0] << 8) | rx_buf[1];
        float temp_c = (raw >> 5) * 0.125f;
        printf("Temperature: %.2f °C\n", temp_c);
    }
    
    /* Register callback */
    {
        printf("Calling capi_i2c_register_callback()...\n");
        struct Arguments arguments_struct = {
            .x = 1,
            .y = 'c',
        };
        capi_i2c_callback cb_ptr = my_callback;
        status = capi_i2c_register_callback(handle, cb_ptr, (void *)&arguments_struct);
        if (print_status(status, "I2C register callback"))
            goto free_client;
    }

    /* Deinit I2C */
    {
        printf("Calling capi_i2c_deinit()...\n");
        status = capi_i2c_deinit(handle);
        if (print_status(status, "I2C deinitialization"))
            goto free_client;
    }


    status = 0;

free_client:
    deinit_nanopb_client();
    return status;

}
