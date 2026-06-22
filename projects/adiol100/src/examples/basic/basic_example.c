#include "basic_example.h"
#include <stdint.h>
#include "adiol100.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"

int basic_example_main(void)
{
    struct adiol100_dev *dev;
    int ret;

    ret = adiol100_init(&dev, &adiol100_ip);

    if (ret)
        return ret;

    pr_info("hello adiol100\r\n");

    uint8_t chip_addr   = 0x01;
    uint8_t reg         = 0x00;
    uint8_t rw          = 1;
    uint8_t cmd         = (chip_addr << 6) | (reg << 1) | rw;

    uint8_t buf[3] = { cmd, 0x00, 0x00 };

    no_os_spi_write_and_read(dev->spi_desc, buf, 3);

    pr_info("MISO: %02X %02X %02X\r\n", buf[0], buf[1], buf[2]);

    uint16_t value = (buf[1] << 8) | buf[2];

    pr_info("InterruptG = 0x%04X\r\n", value);

    chip_addr = 0x01;
    reg       = 0x02;        /* InterruptA */
    rw        = 1;
    cmd       = (chip_addr << 6) | (reg << 1) | rw;

    uint8_t bufinta[3] = { cmd, 0x00, 0x00 };
    no_os_spi_write_and_read(dev->spi_desc, bufinta, 3);

    pr_info("MISO: %02X %02X %02X\r\n", bufinta[0], bufinta[1], bufinta[2]);
    value = (bufinta[1] << 8) | bufinta[2];
    pr_info("InterruptA = 0x%04X\r\n", value);

    chip_addr = 0x01;
    reg       = 0x12;        /* InterruptB */
    rw        = 1;
    cmd       = (chip_addr << 6) | (reg << 1) | rw;

    uint8_t bufintb[3] = { cmd, 0x00, 0x00 };
    no_os_spi_write_and_read(dev->spi_desc, bufintb, 3);

    pr_info("MISO: %02X %02X %02X\r\n", bufintb[0], bufintb[1], bufintb[2]);
    value = (bufintb[1] << 8) | bufintb[2];
    pr_info("InterruptB = 0x%04X\r\n", value);

    chip_addr = 0x01;
    reg       = 0x1F;        /* DeviceID */
    rw        = 1;
    cmd       = (chip_addr << 6) | (reg << 1) | rw;

    uint8_t reg_lo = 0x03;
    uint8_t bufdevid[4] = { cmd, reg_lo, 0x00, 0x00 };
    no_os_spi_write_and_read(dev->spi_desc, bufdevid, 4);

    pr_info("MISO: %02X %02X %02X %02X\r\n", bufdevid[0], bufdevid[1], bufdevid[2], bufdevid[3]);
    value = (bufdevid[2] << 8) | bufdevid[3];
    pr_info("DeviceID = 0x%04X\r\n", value);


    adiol100_remove(dev);
    return 0;
}
