/***************************************************************************//**
 *   @file   adiol100.c
 *   @brief  Implementation of ADIOL100 Driver.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "adiol100.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include <errno.h>
#include <stdint.h>

/**
 * @brief Read RevisionID and DeviceID to verify the chip is present.
 * @param dev - The device structure.
 * @return 0 in case of success, -ENODEV if IDs read back as zero.
 */
int adiol100_verify_chip(struct adiol100_dev *dev)
{
    uint16_t rev_id;
    uint16_t dev_id;
    int ret;

    ret = adiol100_read(dev, ADIOL100_REG_REVISIONID, &rev_id);
    if (ret)
        return ret;

    ret = adiol100_read(dev, ADIOL100_REG_DEVICEID, &dev_id);
    if (ret)
        return ret;

    if (rev_id == 0x0000 && dev_id == 0x0000)
        return -ENODEV;

    return 0;
}

/**
 * @brief Configure the clock source, divider, and enable clock output.
 * @param dev - The device structure.
 * @param ip  - Initialization parameters containing clock_src and clk_div.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_setup_clock(struct adiol100_dev *dev,
                         struct adiol100_init_param *ip)
{
    uint16_t val;

    val = ADIOL100_CLKOEN |
          no_os_field_prep(ADIOL100_CLKDIV_MSK, ip->clk_div) |
          no_os_field_prep(ADIOL100_CLOCKCFG_MSK, ip->clock_src);

    return adiol100_write(dev, ADIOL100_REG_CLOCKCFG, val);
}

static void adiol100_irqg_isr(void *arg)
{
    struct adiol100_dev *dev = arg;

    if (dev->irq_cb_g)
        dev->irq_cb_g(dev->irq_cb_ctx_g);
}

static void adiol100_irqa_isr(void *arg)
{
    struct adiol100_dev *dev = arg;

    if (dev->irq_cb[ADIOL100_CH_A])
        dev->irq_cb[ADIOL100_CH_A](dev->irq_cb_ctx[ADIOL100_CH_A]);
}

static void adiol100_irqb_isr(void *arg)
{
    struct adiol100_dev *dev = arg;

    if (dev->irq_cb[ADIOL100_CH_B])
        dev->irq_cb[ADIOL100_CH_B](dev->irq_cb_ctx[ADIOL100_CH_B]);
}

/**
 * @brief Initialize the ADIOL100 device.
 * @param dev - Pointer to the device descriptor (allocated internally).
 * @param ip  - Initialization parameters (SPI, address, clock).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_init(struct adiol100_dev **dev, struct adiol100_init_param *ip)
{
    int ret;

    struct adiol100_dev *descriptor = no_os_calloc(1, sizeof(*descriptor));
    if(!descriptor) 
        return -ENOMEM;

    ret = no_os_spi_init(&descriptor->spi_desc, ip->spi_ip);
    if (ret)
    {
        no_os_free(descriptor);
        return ret;
    }

    descriptor->chip_addr = ip->chip_addr;

    ret = adiol100_verify_chip(descriptor);
    if (ret)
        goto err;

    ret = adiol100_setup_clock(descriptor, ip);
    if (ret)
        goto err;

    ret = adiol100_global_reset(descriptor);
    if (ret)
        goto err;

    ret = adiol100_reset_channel(descriptor, ADIOL100_CH_A);
    if (ret)
        goto err;

    ret = adiol100_reset_channel(descriptor, ADIOL100_CH_B);
    if (ret)
        goto err;

    if (ip->irq_gpio_g || ip->irq_gpio_a || ip->irq_gpio_b) {
        ret = no_os_irq_ctrl_init(&descriptor->irq_ctrl, ip->irq_ip);
        if (ret)
            goto err;
    }

    if (ip->irq_gpio_g) {
        ret = no_os_gpio_get(&descriptor->irq_gpio_g, ip->irq_gpio_g);
        if (ret)
            goto err_irq;

        ret = no_os_gpio_direction_input(descriptor->irq_gpio_g);
        if (ret)
            goto err_gpio_g;

        struct no_os_callback_desc cb_g = {
            .callback = adiol100_irqg_isr,
            .ctx = descriptor,
            .event = NO_OS_EVT_GPIO,
            .peripheral = NO_OS_GPIO_IRQ,
            .handle = descriptor->irq_gpio_g,
        };
        ret = no_os_irq_register_callback(descriptor->irq_ctrl,
                                           ip->irq_gpio_g->number, &cb_g);
        if (ret)
            goto err_gpio_g;

        ret = no_os_irq_trigger_level_set(descriptor->irq_ctrl,
                                           ip->irq_gpio_g->number,
                                           NO_OS_IRQ_EDGE_FALLING);
        if (ret)
            goto err_gpio_g;

        ret = no_os_irq_enable(descriptor->irq_ctrl,
                                ip->irq_gpio_g->number);
        if (ret)
            goto err_gpio_g;
    }

    if (ip->irq_gpio_a) {
        ret = no_os_gpio_get(&descriptor->irq_gpio_a, ip->irq_gpio_a);
        if (ret)
            goto err_gpio_g;

        ret = no_os_gpio_direction_input(descriptor->irq_gpio_a);
        if (ret)
            goto err_gpio_a;

        struct no_os_callback_desc cb_a = {
            .callback = adiol100_irqa_isr,
            .ctx = descriptor,
            .event = NO_OS_EVT_GPIO,
            .peripheral = NO_OS_GPIO_IRQ,
            .handle = descriptor->irq_gpio_a,
        };
        ret = no_os_irq_register_callback(descriptor->irq_ctrl,
                                           ip->irq_gpio_a->number, &cb_a);
        if (ret)
            goto err_gpio_a;

        ret = no_os_irq_trigger_level_set(descriptor->irq_ctrl,
                                           ip->irq_gpio_a->number,
                                           NO_OS_IRQ_EDGE_FALLING);
        if (ret)
            goto err_gpio_a;

        ret = no_os_irq_enable(descriptor->irq_ctrl,
                                ip->irq_gpio_a->number);
        if (ret)
            goto err_gpio_a;
    }

    if (ip->irq_gpio_b) {
        ret = no_os_gpio_get(&descriptor->irq_gpio_b, ip->irq_gpio_b);
        if (ret)
            goto err_gpio_a;

        ret = no_os_gpio_direction_input(descriptor->irq_gpio_b);
        if (ret)
            goto err_gpio_b;

        struct no_os_callback_desc cb_b = {
            .callback = adiol100_irqb_isr,
            .ctx = descriptor,
            .event = NO_OS_EVT_GPIO,
            .peripheral = NO_OS_GPIO_IRQ,
            .handle = descriptor->irq_gpio_b,
        };
        ret = no_os_irq_register_callback(descriptor->irq_ctrl,
                                           ip->irq_gpio_b->number, &cb_b);
        if (ret)
            goto err_gpio_b;

        ret = no_os_irq_trigger_level_set(descriptor->irq_ctrl,
                                           ip->irq_gpio_b->number,
                                           NO_OS_IRQ_EDGE_FALLING);
        if (ret)
            goto err_gpio_b;

        ret = no_os_irq_enable(descriptor->irq_ctrl,
                                ip->irq_gpio_b->number);
        if (ret)
            goto err_gpio_b;
    }

    *dev = descriptor;
    return 0;

err_gpio_b:
    no_os_gpio_remove(descriptor->irq_gpio_b);
err_gpio_a:
    if (descriptor->irq_gpio_a)
        no_os_gpio_remove(descriptor->irq_gpio_a);
err_gpio_g:
    if (descriptor->irq_gpio_g)
        no_os_gpio_remove(descriptor->irq_gpio_g);
err_irq:
    if (descriptor->irq_ctrl)
        no_os_irq_ctrl_remove(descriptor->irq_ctrl);
err:
    no_os_spi_remove(descriptor->spi_desc);
    no_os_free(descriptor);
    return ret;
}

/**
 * @brief Free resources allocated by adiol100_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_remove(struct adiol100_dev *dev)
{
    int ret;

    if (!dev)
        return -ENODEV;

    if (dev->irq_gpio_b)
        no_os_gpio_remove(dev->irq_gpio_b);

    if (dev->irq_gpio_a)
        no_os_gpio_remove(dev->irq_gpio_a);

    if (dev->irq_gpio_g)
        no_os_gpio_remove(dev->irq_gpio_g);

    if (dev->irq_ctrl)
        no_os_irq_ctrl_remove(dev->irq_ctrl);

    ret = no_os_spi_remove(dev->spi_desc);
    if (ret)
        return -EINVAL;

    no_os_free(dev);

    return 0;
}

/**
 * @brief Read a 16-bit register. Handles paged access automatically.
 * @param dev   - The device structure.
 * @param reg   - Register address (0x1Fxx for paged registers).
 * @param value - Pointer to store the read value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_read(struct adiol100_dev *dev, uint16_t reg, uint16_t *value)
{
    uint8_t buf[4];
    uint8_t len;
    uint8_t paged;
    int ret;

    paged = (reg & 0xFF00) == ADIOL100_PAGED_BASE;

    if (paged) {
        buf[0] = (dev->chip_addr << 6) | (ADIOL100_REG_EXTRAPAGE << 1) | ADIOL100_READ;
        buf[1] = reg & 0xFF;
        buf[2] = 0x00;
        buf[3] = 0x00;
        len = 4;
    } else {
        buf[0] = (dev->chip_addr << 6) | (reg << 1) | ADIOL100_READ;
        buf[1] = 0x00;
        buf[2] = 0x00;
        len = 3;
    }

    ret = no_os_spi_write_and_read(dev->spi_desc, buf, len);
    if (ret)
        return ret;

    *value = (buf[len - 2] << 8) | buf[len - 1];
    return 0;
}

/**
 * @brief Write a 16-bit register. Handles paged access automatically.
 * @param dev   - The device structure.
 * @param reg   - Register address (0x1Fxx for paged registers).
 * @param value - The value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_write(struct adiol100_dev *dev, uint16_t reg, uint16_t value)
{
    uint8_t buf[4];
    uint8_t len;
    uint8_t paged;

    paged = (reg & 0xFF00) == ADIOL100_PAGED_BASE;

    if (paged) {
        buf[0] = (dev->chip_addr << 6) | (ADIOL100_REG_EXTRAPAGE << 1) | ADIOL100_WRITE;
        buf[1] = reg & 0xFF;
        buf[2] = (value >> 8) & 0xFF;
        buf[3] = value & 0xFF;
        len = 4;
    } else {
        buf[0] = (dev->chip_addr << 6) | (reg << 1) | ADIOL100_WRITE;
        buf[1] = (value >> 8) & 0xFF;
        buf[2] = value & 0xFF;
        len = 3;
    }

    return no_os_spi_write_and_read(dev->spi_desc, buf, len);
}

/**
 * @brief Read-modify-write a register using a bit mask.
 * @param dev   - The device structure.
 * @param reg   - Register address.
 * @param mask  - Bit mask of the field to modify.
 * @param value - New value (pre-shifted to match mask position).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_update(struct adiol100_dev *dev, uint16_t reg, uint16_t mask,
                    uint16_t value)
{
    uint16_t tmp;
    int ret;

    ret = adiol100_read(dev, reg, &tmp);
    if (ret)
        return ret;

    tmp &= ~mask;
    tmp |= value & mask;

    return adiol100_write(dev, reg, tmp);
}

/**
 * @brief Load IO-Link payload into TxFIFO without triggering CQSend.
 * @param dev     - The device structure.
 * @param ch      - Channel selection (A or B).
 * @param data    - IO-Link payload bytes (MC + CKT + OD).
 * @param txbytes - Number of payload bytes to transmit.
 * @param rxbytes - Number of response bytes expected from the device.
 * @param keep    - Retain message in TxFIFO for cyclic re-sending.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_load_msg(struct adiol100_dev *dev, enum adiol100_channel ch,
                      uint8_t *data, uint8_t txbytes, uint8_t rxbytes,
                      enum adiol100_keep_msg keep)
{
    uint8_t buf[ADIOL100_FIFO_MAX_LEN + 1];
    uint8_t write_len = 3 + txbytes;
    uint8_t read_len = 2 + rxbytes;
    uint16_t data_reg;
    uint16_t fc1_reg;
    int ret;

    if (ch == ADIOL100_CH_A) {
        data_reg = ADIOL100_REG_TXRXDATA_A;
        fc1_reg = ADIOL100_REG_FRAMCTRL1_A;
    } else {
        data_reg = ADIOL100_REG_TXRXDATA_B;
        fc1_reg = ADIOL100_REG_FRAMCTRL1_B;
    }

    if (write_len > ADIOL100_FIFO_MAX_LEN)
        return -EINVAL;

    ret = adiol100_set_burst_len(dev, ch, write_len, read_len);
    if (ret)
        return ret;

    buf[0] = (dev->chip_addr << 6) | (data_reg << 1) | ADIOL100_WRITE;
    buf[1] = dev->msg_id++;
    buf[2] = rxbytes;
    buf[3] = txbytes;
    for (int i = 0; i < txbytes; i++)
        buf[4 + i] = data[i];

    ret = no_os_spi_write_and_read(dev->spi_desc, buf, write_len + 1);
    if (ret)
        return ret;

    return adiol100_update(dev, fc1_reg, ADIOL100_TXKEEPMSG,
                           no_os_field_prep(ADIOL100_TXKEEPMSG, keep));
}

/**
 * @brief Assert CQSend to trigger transmission of the message in the TxFIFO.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_send_msg(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t fc1_reg;

    if (ch == ADIOL100_CH_A)
        fc1_reg = ADIOL100_REG_FRAMCTRL1_A;
    else
        fc1_reg = ADIOL100_REG_FRAMCTRL1_B;

    return adiol100_update(dev, fc1_reg, ADIOL100_CQSEND, ADIOL100_CQSEND);
}

/**
 * @brief Load IO-Link payload into TxFIFO and trigger CQSend.
 * @param dev     - The device structure.
 * @param ch      - Channel selection (A or B).
 * @param data    - IO-Link payload bytes (MC + CKT + OD).
 * @param txbytes - Number of payload bytes to transmit.
 * @param rxbytes - Number of response bytes expected from the device.
 * @param keep    - Retain message in TxFIFO for cyclic re-sending.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_load_and_send_msg(struct adiol100_dev *dev, enum adiol100_channel ch,
                               uint8_t *data, uint8_t txbytes, uint8_t rxbytes,
                               enum adiol100_keep_msg keep)
{
    int ret;

    ret = adiol100_load_msg(dev, ch, data, txbytes, rxbytes, keep);
    if (ret)
        return ret;

    return adiol100_send_msg(dev, ch);
}

/**
 * @brief Read device response from RxFIFO, stripping the MsgID/RxBytesAct header.
 * @param dev  - The device structure.
 * @param ch   - Channel selection (A or B).
 * @param data - Buffer to store the device response bytes.
 * @param len  - Pointer to store the number of bytes read.
 * @return 0 in case of success, -EIO on FIFO mismatch.
 */
int adiol100_read_msg(struct adiol100_dev *dev, enum adiol100_channel ch,
                       uint8_t *data, uint8_t *len)
{
    uint16_t rxstat;
    uint8_t fifo_lvl;
    uint8_t total;
    uint8_t payload;
    uint8_t buf[ADIOL100_FIFO_MAX_LEN + 1];
    uint16_t stat_reg;
    uint16_t data_reg;
    int ret;

    if (ch == ADIOL100_CH_A) {
        stat_reg = ADIOL100_REG_RXFIFOSTAT_A;
        data_reg = ADIOL100_REG_TXRXDATA_A;
    } else {
        stat_reg = ADIOL100_REG_RXFIFOSTAT_B;
        data_reg = ADIOL100_REG_TXRXDATA_B;
    }

    ret = adiol100_read(dev, stat_reg, &rxstat);
    if (ret)
        return ret;

    fifo_lvl = rxstat & ADIOL100_RXFIFOLVL_MSK;
    if (fifo_lvl == 0) {
        *len = 0;
        return 0;
    }

    total = fifo_lvl + 2;
    if (total > ADIOL100_FIFO_MAX_LEN)
        total = ADIOL100_FIFO_MAX_LEN;

    buf[0] = (dev->chip_addr << 6) | (data_reg << 1) | ADIOL100_READ;
    for (int i = 0; i < total; i++)
        buf[1 + i] = 0x00;

    ret = no_os_spi_write_and_read(dev->spi_desc, buf, total + 1);
    if (ret)
        return ret;

    if (buf[2] != fifo_lvl) {
        adiol100_reset_rx_fifo(dev, ch);
        *len = 0;
        return -EIO;
    }

    payload = fifo_lvl;
    for (int i = 0; i < payload; i++)
        data[i] = buf[3 + i];

    *len = payload;
    return 0;
}

/**
 * @brief Disable global interrupts, clear trigger, and read-clear all IRQ registers.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_global_reset(struct adiol100_dev *dev)
{
    uint16_t dummy;
    int ret;

    ret = adiol100_write(dev, ADIOL100_REG_INTERRUPTG_EN, 0);
    if (ret)
        return ret;

    ret = adiol100_write(dev, ADIOL100_REG_TRIGGER, 0);
    if (ret)
        return ret;

    ret = adiol100_read(dev, ADIOL100_REG_INTERRUPTG, &dummy);
    if (ret)
        return ret;

    ret = adiol100_read(dev, ADIOL100_REG_INTERRUPT_A, &dummy);
    if (ret)
        return ret;

    ret = adiol100_read(dev, ADIOL100_REG_INTERRUPT_B, &dummy);
    if (ret)
        return ret;

    ret = adiol100_read(dev, ADIOL100_REG_STATUS_A, &dummy);
    if (ret)
        return ret;

    return adiol100_read(dev, ADIOL100_REG_STATUS_B, &dummy);
}

/**
 * @brief Reset the transmit FIFO for a channel.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_reset_tx_fifo(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL2_A;
    else
        reg = ADIOL100_REG_FRAMCTRL2_B;

    return adiol100_update(dev, reg, ADIOL100_TXFIFORSTN, ADIOL100_TXFIFORSTN);
}

/**
 * @brief Reset the receive FIFO for a channel.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_reset_rx_fifo(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL2_A;
    else
        reg = ADIOL100_REG_FRAMCTRL2_B;

    return adiol100_update(dev, reg, ADIOL100_RXFIFORSTN, ADIOL100_RXFIFORSTN);
}

/**
 * @brief Assert CHANRST to reset all channel-specific registers.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_reset_channel(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t reg;
    int ret;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL2_A;
    else
        reg = ADIOL100_REG_FRAMCTRL2_B;

    ret = adiol100_write(dev, reg, ADIOL100_CHANRST);
    if (ret)
        return ret;

    no_os_mdelay(5);

    return 0;
}

/**
 * @brief Configure the CQ driver: output mode, enable, sink current, slew rate.
 * @param dev       - The device structure.
 * @param ch        - Channel selection (A or B).
 * @param mode      - NPN or push-pull output.
 * @param drv_en    - Enable or disable the CQ driver.
 * @param sink_sel  - Input sink current selection.
 * @param slew_rate - CQ output slew rate.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_config_cq(struct adiol100_dev *dev, enum adiol100_channel ch,
                       enum adiol100_cq_mode mode,
                       enum adiol100_cq_drv drv_en,
                       enum adiol100_sink_sel sink_sel,
                       enum adiol100_cq_slew_rate slew_rate)
{
    uint16_t reg;
    uint16_t mask;
    uint16_t val = 0;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_CQSET_A;
    else
        reg = ADIOL100_REG_CQSET_B;

    mask = ADIOL100_PUSHPUL | ADIOL100_DRVEN |
           ADIOL100_SINKSEL_MSK | ADIOL100_CQSLEW_MSK;

    val |= no_os_field_prep(ADIOL100_PUSHPUL, mode);
    val |= no_os_field_prep(ADIOL100_DRVEN, drv_en);
    val |= no_os_field_prep(ADIOL100_SINKSEL_MSK, sink_sel);
    val |= no_os_field_prep(ADIOL100_CQSLEW_MSK, slew_rate);

    return adiol100_update(dev, reg, mask, val);
}

/**
 * @brief Configure CQ protection: current limit, blanking, thresholds, autoretry.
 * @param dev           - The device structure.
 * @param ch            - Channel selection (A or B).
 * @param current_limit - CQ driver current limit.
 * @param blanking      - Current limit blanking time.
 * @param retry_tmo     - Autoretry timeout after current limit event.
 * @param retry_en      - Enable or disable autoretry.
 * @param tx_vthr_h     - CQ transmit high voltage threshold.
 * @param tx_vthr_l     - CQ transmit low voltage threshold.
 * @param rx_vthr_h     - CQ receive high voltage threshold.
 * @param rx_vthr_l     - CQ receive low voltage threshold.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_config_cq_protection(struct adiol100_dev *dev,
                                  enum adiol100_channel ch,
                                  enum adiol100_cq_current_limit current_limit,
                                  enum adiol100_blanking_time blanking,
                                  enum adiol100_autoretry_timeout retry_tmo,
                                  enum adiol100_autoretry retry_en,
                                  enum adiol100_voltage_threshold tx_vthr_h,
                                  enum adiol100_voltage_threshold tx_vthr_l,
                                  enum adiol100_voltage_threshold rx_vthr_h,
                                  enum adiol100_voltage_threshold rx_vthr_l)
{
    uint16_t reg;
    uint16_t val = 0;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_CQCURLIM_A;
    else
        reg = ADIOL100_REG_CQCURLIM_B;

    val |= no_os_field_prep(ADIOL100_CQVTHRH_MSK, tx_vthr_h);
    val |= no_os_field_prep(ADIOL100_CQVTHRL_MSK, tx_vthr_l);
    val |= no_os_field_prep(ADIOL100_RXVTHRH_MSK, rx_vthr_h);
    val |= no_os_field_prep(ADIOL100_RXVTHRL_MSK, rx_vthr_l);
    val |= no_os_field_prep(ADIOL100_CQCL_MSK, current_limit);
    val |= no_os_field_prep(ADIOL100_CLBL_MSK, blanking);
    val |= no_os_field_prep(ADIOL100_ARTTMO_MSK, retry_tmo);
    val |= no_os_field_prep(ADIOL100_ARTEN, retry_en);

    return adiol100_write(dev, reg, val);
}

/**
 * @brief Configure the L+ sensor supply: enable and reverse polarity protection.
 * @param dev    - The device structure.
 * @param ch     - Channel selection (A or B).
 * @param enable - Enable or disable L+ supply.
 * @param rev_en - Enable or disable reverse polarity protection.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_config_lp(struct adiol100_dev *dev, enum adiol100_channel ch,
                       enum adiol100_lp_en enable,
                       enum adiol100_lp_rev rev_en)
{
    uint16_t reg;
    uint16_t mask;
    uint16_t val = 0;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_LPCFG_A;
    else
        reg = ADIOL100_REG_LPCFG_B;

    mask = ADIOL100_LPEN | ADIOL100_LPFETREVEN;

    val |= no_os_field_prep(ADIOL100_LPEN, enable);
    val |= no_os_field_prep(ADIOL100_LPFETREVEN, rev_en);

    return adiol100_update(dev, reg, mask, val);
}

/**
 * @brief Configure L+ FET protection: current, power, timeouts, slope, retry.
 * @param dev        - The device structure.
 * @param ch         - Channel selection (A or B).
 * @param cl_nom     - Nominal current limit (LPCLNom register).
 * @param pwr_max    - Maximum power limit (LPFetPwrCfg register).
 * @param curr_max   - Maximum current limit (LPCurrentCfg register).
 * @param curr_min   - Minimum current limit (LPCurrentCfg register).
 * @param ol_timeout - Open-load timeout (LPFETProtect register).
 * @param cl_timeout - Current-limit timeout (LPFETProtect register).
 * @param slope      - Slope control (LPSlope register).
 * @param slope_bl   - Slope blanking (LPSlope register).
 * @param ar_time    - Autoretry time (LPRetry register).
 * @param ar_count   - Autoretry count (LPRetry register).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_config_lp_protection(struct adiol100_dev *dev,
                                  enum adiol100_channel ch,
                                  uint8_t cl_nom, uint8_t pwr_max,
                                  uint8_t curr_max, uint8_t curr_min,
                                  uint8_t ol_timeout, uint8_t cl_timeout,
                                  uint8_t slope, uint8_t slope_bl,
                                  uint8_t ar_time, uint8_t ar_count)
{
    uint16_t val;
    int ret;

    uint16_t lpcfg_reg, pwrcfg_reg, currcfg_reg;
    uint16_t protect_reg, slope_reg, retry_reg;

    if (ch == ADIOL100_CH_A) {
        lpcfg_reg   = ADIOL100_REG_LPCFG_A;
        pwrcfg_reg  = ADIOL100_REG_LPFETPWRCFG_A;
        currcfg_reg = ADIOL100_REG_LPCURRENTCFG_A;
        protect_reg = ADIOL100_REG_LPFETPROTECT_A;
        slope_reg   = ADIOL100_REG_LPSLOPE_A;
        retry_reg   = ADIOL100_REG_LPRETRY_A;
    } else {
        lpcfg_reg   = ADIOL100_REG_LPCFG_B;
        pwrcfg_reg  = ADIOL100_REG_LPFETPWRCFG_B;
        currcfg_reg = ADIOL100_REG_LPCURRENTCFG_B;
        protect_reg = ADIOL100_REG_LPFETPROTECT_B;
        slope_reg   = ADIOL100_REG_LPSLOPE_B;
        retry_reg   = ADIOL100_REG_LPRETRY_B;
    }

    /* LPCfg — nominal current limit */
    ret = adiol100_update(dev, lpcfg_reg, ADIOL100_LPCLNOM_MSK,
                          no_os_field_prep(ADIOL100_LPCLNOM_MSK, cl_nom));
    if (ret)
        return ret;

    /* LPFetPwrCfg */
    val = no_os_field_prep(ADIOL100_LPFETPWRMAX_MSK, pwr_max);
    ret = adiol100_write(dev, pwrcfg_reg, val);
    if (ret)
        return ret;

    /* LPCurrentCfg */
    val = no_os_field_prep(ADIOL100_LPCURRMAX_MSK, curr_max) |
          no_os_field_prep(ADIOL100_LPCURRMIN_MSK, curr_min);
    ret = adiol100_write(dev, currcfg_reg, val);
    if (ret)
        return ret;

    /* LPFETProtect */
    val = no_os_field_prep(ADIOL100_LPOLTIMOUT_MSK, ol_timeout) |
          no_os_field_prep(ADIOL100_LPCLTIMOUT_MSK, cl_timeout);
    ret = adiol100_write(dev, protect_reg, val);
    if (ret)
        return ret;

    /* LPSlope */
    val = no_os_field_prep(ADIOL100_LPSLOPE_MSK, slope) |
          no_os_field_prep(ADIOL100_LPSLOPEBL_MSK, slope_bl);
    ret = adiol100_write(dev, slope_reg, val);
    if (ret)
        return ret;

    /* LPRetry */
    val = no_os_field_prep(ADIOL100_LPARTIM_MSK, ar_time) |
          no_os_field_prep(ADIOL100_LPAR_MSK, ar_count);
    return adiol100_write(dev, retry_reg, val);
}

/**
 * @brief Configure the IO-Link framer: checksum insertion and framer enable.
 * @param dev       - The device structure.
 * @param ch        - Channel selection (A or B).
 * @param ins_chks  - Enable or disable checksum insertion.
 * @param framer_en - Enable or disable the framer.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_config_framer(struct adiol100_dev *dev, enum adiol100_channel ch,
                           enum adiol100_ins_chks ins_chks,
                           enum adiol100_framer framer_en)
{
    uint16_t reg;
    uint16_t mask;
    uint16_t val = 0;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL1_A;
    else
        reg = ADIOL100_REG_FRAMCTRL1_B;

    mask = ADIOL100_INSCHKS | ADIOL100_FRAMEREN;

    val |= no_os_field_prep(ADIOL100_INSCHKS, ins_chks);
    val |= no_os_field_prep(ADIOL100_FRAMEREN, framer_en);

    return adiol100_update(dev, reg, mask, val);
}

/**
 * @brief Set the SPI burst length for FIFO access.
 * @param dev       - The device structure.
 * @param ch        - Channel selection (A or B).
 * @param write_len - Number of bytes in the write burst.
 * @param read_len  - Number of bytes in the read burst.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_set_burst_len(struct adiol100_dev *dev, enum adiol100_channel ch,
                           int write_len, int read_len)
{
    uint16_t reg;
    uint16_t val;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_SPIFIFO_A_BURST;
    else
        reg = ADIOL100_REG_SPIFIFO_B_BURST;

    val = (read_len << 8) | write_len;

    return adiol100_write(dev, reg, val);
}

/**
 * @brief Get the communication rate determined by EstCom (COM1/2/3).
 * @param dev   - The device structure.
 * @param ch    - Channel selection (A or B).
 * @param comrt - Pointer to store the COM rate value (1, 2, or 3).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_get_comrt(struct adiol100_dev *dev, enum adiol100_channel ch,
                       uint8_t *comrt)
{
    uint16_t reg;
    uint16_t val;
    int ret;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL1_A;
    else
        reg = ADIOL100_REG_FRAMCTRL1_B;

    ret = adiol100_read(dev, reg, &val);
    if (ret)
        return ret;

    *comrt = no_os_field_get(ADIOL100_COMRT_MSK, val);
    return 0;
}

/**
 * @brief Set the IO-Link cycle timer value.
 * @param dev   - The device structure.
 * @param ch    - Channel selection (A or B).
 * @param value - IO-Link encoded cycle time byte.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_set_cycle_tmr(struct adiol100_dev *dev, enum adiol100_channel ch,
                           uint8_t value)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_CYCLTMR_A;
    else
        reg = ADIOL100_REG_CYCLTMR_B;

    return adiol100_update(dev, reg, ADIOL100_CYCLTMR_MSK, (uint16_t)value);
}

/**
 * @brief Get the current cycle timer register value.
 * @param dev   - The device structure.
 * @param ch    - Channel selection (A or B).
 * @param value - Pointer to store the cycle timer value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_get_cycle_tmr(struct adiol100_dev *dev, enum adiol100_channel ch,
                           uint16_t *value)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_CYCLTMR_A;
    else
        reg = ADIOL100_REG_CYCLTMR_B;

    return adiol100_read(dev, reg, value);
}

/**
 * @brief Enable the cycle timer for autonomous cyclic operation.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_enable_cycle_timer(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL1_A;
    else
        reg = ADIOL100_REG_FRAMCTRL1_B;

    return adiol100_update(dev, reg, ADIOL100_CYCLETMREN, ADIOL100_CYCLETMREN);
}

/**
 * @brief Disable the cycle timer.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_disable_cycle_timer(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL1_A;
    else
        reg = ADIOL100_REG_FRAMCTRL1_B;

    return adiol100_update(dev, reg, ADIOL100_CYCLETMREN, 0);
}

/**
 * @brief Read and clear the channel interrupt register.
 * @param dev   - The device structure.
 * @param ch    - Channel selection (A or B).
 * @param flags - Pointer to store the interrupt flags.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_get_channel_irq(struct adiol100_dev *dev, enum adiol100_channel ch,
                             uint16_t *flags)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_INTERRUPT_A;
    else
        reg = ADIOL100_REG_INTERRUPT_B;

    return adiol100_read(dev, reg, flags);
}

/**
 * @brief Read and clear the global interrupt register.
 * @param dev   - The device structure.
 * @param flags - Pointer to store the interrupt flags.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_get_global_irq(struct adiol100_dev *dev, uint16_t *flags)
{
    return adiol100_read(dev, ADIOL100_REG_INTERRUPTG, flags);
}

/**
 * @brief Set the global interrupt enable mask.
 * @param dev  - The device structure.
 * @param mask - Bitmask of interrupts to enable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_enable_global_irq(struct adiol100_dev *dev, uint16_t mask)
{
    return adiol100_write(dev, ADIOL100_REG_INTERRUPTG_EN, mask);
}

/**
 * @brief Set the channel interrupt enable mask.
 * @param dev  - The device structure.
 * @param ch   - Channel selection (A or B).
 * @param mask - Bitmask of interrupts to enable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_enable_channel_irq(struct adiol100_dev *dev,
                                enum adiol100_channel ch, uint16_t mask)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_INTEN_A;
    else
        reg = ADIOL100_REG_INTEN_B;

    return adiol100_write(dev, reg, mask);
}

/**
 * @brief Read the RxFIFO status register (level + error flags).
 * @param dev   - The device structure.
 * @param ch    - Channel selection (A or B).
 * @param flags - Pointer to store the status flags.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_get_fifo_status(struct adiol100_dev *dev, enum adiol100_channel ch,
                             uint16_t *flags)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_RXFIFOSTAT_A;
    else
        reg = ADIOL100_REG_RXFIFOSTAT_B;

    return adiol100_read(dev, reg, flags);
}

/**
 * @brief Read the channel status register.
 * @param dev   - The device structure.
 * @param ch    - Channel selection (A or B).
 * @param flags - Pointer to store the status flags.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_get_status(struct adiol100_dev *dev, enum adiol100_channel ch,
                        uint16_t *flags)
{
    uint16_t reg;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_STATUS_A;
    else
        reg = ADIOL100_REG_STATUS_B;

    return adiol100_read(dev, reg, flags);
}

/**
 * @brief Configure the watchdog timer.
 * @param dev      - The device structure.
 * @param timebase - Timer tick period (100us, 500us, or 2ms).
 * @param time     - Timeout multiplier (6-bit, 0–63).
 * @param mode     - SPI activity reset or explicit clear.
 * @param enable   - Enable or disable the watchdog.
 * @param lock     - Lock the watchdog configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_config_watchdog(struct adiol100_dev *dev,
                             enum adiol100_wdg_timebase timebase,
                             uint8_t time,
                             enum adiol100_wdg_mode mode,
                             enum adiol100_wdg_en enable,
                             enum adiol100_wdg_lock lock)
{
    uint16_t val = 0;

    val |= no_os_field_prep(ADIOL100_WDGTIMEBASE_MSK, timebase);
    val |= no_os_field_prep(ADIOL100_WDGTIME_MSK, time);
    val |= no_os_field_prep(ADIOL100_WDGMODE, mode);
    val |= no_os_field_prep(ADIOL100_WDGENABLE, enable);
    val |= no_os_field_prep(ADIOL100_WDGLOCK, lock);

    return adiol100_write(dev, ADIOL100_REG_WATCHDOG, val);
}

/**
 * @brief Clear the watchdog timer (write-1-to-clear).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adiol100_clear_watchdog(struct adiol100_dev *dev)
{
    return adiol100_update(dev, ADIOL100_REG_WATCHDOG,
                           ADIOL100_WDGCLEAR, ADIOL100_WDGCLEAR);
}

/**
 * @brief Register an IRQ callback for a channel.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @param cb  - Callback function (called from ISR context).
 * @param ctx - User context passed to the callback.
 * @return 0 in case of success, -ENOTSUP if IRQ is not configured.
 */
int adiol100_register_irq_callback(struct adiol100_dev *dev,
                                   enum adiol100_channel ch,
                                   adiol100_irq_cb_t cb,
                                   void *ctx)
{
    if (!dev->irq_ctrl)
        return -ENOTSUP;

    dev->irq_cb[ch] = cb;
    dev->irq_cb_ctx[ch] = ctx;

    return 0;
}

/**
 * @brief Register an IRQ callback for global interrupts.
 * @param dev - The device structure.
 * @param cb  - Callback function (called from ISR context).
 * @param ctx - User context passed to the callback.
 * @return 0 in case of success, -ENOTSUP if IRQ is not configured.
 */
int adiol100_register_global_irq_callback(struct adiol100_dev *dev,
                                          adiol100_irq_cb_t cb,
                                          void *ctx)
{
    if (!dev->irq_ctrl)
        return -ENOTSUP;

    dev->irq_cb_g = cb;
    dev->irq_cb_ctx_g = ctx;

    return 0;
}

/**
 * @brief Run EstablishCommunication: reset FIFOs, set EstCom, poll for result.
 * @param dev - The device structure.
 * @param ch  - Channel selection (A or B).
 * @return 0 on success, -ENODEV if no device responded, -ETIMEDOUT on timeout.
 */
int adiol100_estcom(struct adiol100_dev *dev, enum adiol100_channel ch)
{
    uint16_t reg;
    int ret;

    if (ch == ADIOL100_CH_A)
        reg = ADIOL100_REG_FRAMCTRL1_A;
    else
        reg = ADIOL100_REG_FRAMCTRL1_B;

    ret = adiol100_update(dev, reg, ADIOL100_CQSEND, 0);
    if (ret)
        return ret;

    ret = adiol100_disable_cycle_timer(dev, ch);
    if (ret)
        return ret;

    ret = adiol100_reset_tx_fifo(dev, ch);
    if (ret)
        return ret;

    ret = adiol100_reset_rx_fifo(dev, ch);
    if (ret)
        return ret;

    ret = adiol100_update(dev, reg, ADIOL100_ESTCOM, ADIOL100_ESTCOM);
    if (ret)
        return ret;

    uint16_t irq;
    for (int i = 0; i < 100; i++) {
        no_os_mdelay(1);
        ret = adiol100_get_channel_irq(dev, ch, &irq);
        if (ret)
            return ret;
        if (irq & (ADIOL100_ESTCOMSUCINT | ADIOL100_ESTCOMFAILINT))
            break;
    }

    if (irq & ADIOL100_ESTCOMFAILINT)
        return -ENODEV;

    if (!(irq & ADIOL100_ESTCOMSUCINT))
        return -ETIMEDOUT;

    return 0;
}
