// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Description:       AXI AXI_ADRV9102_GPIO Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_adrv9102_gpio_regs.h"
#include "axi_adrv9102_gpio.h"


/*  register field read, axi_adrv9102_gpio_timer */

int32_t axi_adrv9102_gpio_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_timer */

int32_t axi_adrv9102_gpio_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_adrv9102_gpio_timer */

int32_t axi_adrv9102_gpio_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_data */

int32_t axi_adrv9102_gpio_dgpio_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_data_out */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_data_out */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_data_out_enb */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_data_out_enb */

int32_t axi_adrv9102_gpio_dgpio_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_data_mode */

int32_t axi_adrv9102_gpio_dgpio_pio_data_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_data_mode */

int32_t axi_adrv9102_gpio_dgpio_pio_data_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_DATA_MODE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_sel */

int32_t axi_adrv9102_gpio_dgpio_pio_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_GPIO_DGPIO_PIO_SEL_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_SEL_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_sel */

int32_t axi_adrv9102_gpio_dgpio_pio_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_GPIO_DGPIO_PIO_SEL_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_SEL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_buf_enable */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_ENABLE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_buf_enable */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_buf_data */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_DATA_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_buf_data */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_buf_mode */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_MODE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_buf_mode */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_MODE_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_gpio_reg_dgpio_pio_buf_status */

int32_t axi_adrv9102_gpio_reg_dgpio_pio_buf_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_REG_DGPIO_PIO_BUF_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_buf_full */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_full_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_FULL_ADDR + addr_offset), AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_FULL_POS, AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_FULL_MASK, data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_buf_empty */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_empty_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_EMPTY_ADDR + addr_offset), AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_EMPTY_POS, AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_EMPTY_MASK, data));
}


/*  register read, axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers */

int32_t axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_REG_DGPIO_PIO_BUF_TRIGGERS_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers */

int32_t axi_adrv9102_gpio_reg_dgpio_pio_buf_triggers_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_REG_DGPIO_PIO_BUF_TRIGGERS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    pos_offset = (index_id * AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_ADDR + addr_offset), (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_POS + pos_offset), AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_MASK, data));
}


/*  register field write, axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode */

int32_t axi_adrv9102_gpio_dgpio_pio_buf_trigger_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;
    uint32_t pos_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    pos_offset = (index_id * AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_POS_OFFSET);

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_ADDR + addr_offset), (AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_POS + pos_offset), AXI_ADRV9102_GPIO_DGPIO_PIO_BUF_TRIGGER_MODE_MASK, data));
}


/*  register field read, axi_adrv9102_gpio_rx0_pio_data */

int32_t axi_adrv9102_gpio_rx0_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX0_PIO_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_rx0_pio_data_out */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX0_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_rx0_pio_data_out */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX0_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_rx0_pio_data_out_enb */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX0_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_rx0_pio_data_out_enb */

int32_t axi_adrv9102_gpio_rx0_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX0_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_rx1_pio_data */

int32_t axi_adrv9102_gpio_rx1_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX1_PIO_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_rx1_pio_data_out */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX1_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_rx1_pio_data_out */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX1_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_rx1_pio_data_out_enb */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX1_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_rx1_pio_data_out_enb */

int32_t axi_adrv9102_gpio_rx1_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_RX1_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_tx0_pio_data */

int32_t axi_adrv9102_gpio_tx0_pio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TX0_PIO_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_tx0_pio_data_out */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TX0_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_tx0_pio_data_out */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TX0_PIO_DATA_OUT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_tx0_pio_data_out_enb */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TX0_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_tx0_pio_data_out_enb */

int32_t axi_adrv9102_gpio_tx0_pio_data_out_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TX0_PIO_DATA_OUT_ENB_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_trig_src_group */

int32_t axi_adrv9102_gpio_trig_src_group_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TRIG_SRC_GROUP_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_trig_src_group */

int32_t axi_adrv9102_gpio_trig_src_group_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TRIG_SRC_GROUP_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_trig_src_pin */

int32_t axi_adrv9102_gpio_trig_src_pin_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TRIG_SRC_PIN_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_trig_src_pin */

int32_t axi_adrv9102_gpio_trig_src_pin_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_TRIG_SRC_PIN_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_gainindex_src_group */

int32_t axi_adrv9102_gpio_gainindex_src_group_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_GAININDEX_SRC_GROUP_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_gainindex_src_group */

int32_t axi_adrv9102_gpio_gainindex_src_group_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_GAININDEX_SRC_GROUP_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_gainindex_src_pin */

int32_t axi_adrv9102_gpio_gainindex_src_pin_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_GPIO_GAININDEX_SRC_PIN_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_GAININDEX_SRC_PIN_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_gpio_gainindex_src_pin */

int32_t axi_adrv9102_gpio_gainindex_src_pin_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_GPIO_GAININDEX_SRC_PIN_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_GPIO_GAININDEX_SRC_PIN_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_gpio_clk_mon_count */

int32_t axi_adrv9102_gpio_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_GPIO_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_adrv9102_gpio_clk_mon_count */

int32_t axi_adrv9102_gpio_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_ADRV9102_GPIO_CLK_MON_COUNT_ADDR + addr_offset), data));
}

// **********************************************************************************
// **********************************************************************************

