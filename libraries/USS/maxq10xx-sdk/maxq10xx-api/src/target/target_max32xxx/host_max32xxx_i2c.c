/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/


#include <MXQ_API.h>
#include "mxc_delay.h"
#ifdef MAX32690
#include "mxc_device.h"

#if defined(USE_PROTOCOL_I2C)
static const mxc_gpio_cfg_t bypass_smngr_cfg_i2c0 = {MXC_GPIO2, (MXC_GPIO_PIN_7 | MXC_GPIO_PIN_8), MXC_GPIO_FUNC_ALT1,
										MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_0};
#endif
#endif
#if defined(USE_PROTOCOL_I2C)

#include "i2c.h"
#include <i2c_regs.h>
#include "tmr.h"

#define I2C_RESPONDER_ADDR (0x60)
#define RDY_WAIT_TIMEOUT   (200000000U)
#define RDY_DELAY           (10000)
#ifndef READY_NOT_CONNECTED
extern mxc_gpio_cfg_t maxq_rdy;
#endif
extern uint32_t HOST_block_until_pin(mxc_gpio_regs_t *port, uint32_t mask, uint32_t blocking_value, uint32_t waittime);
extern void HOST_maxq10xx_reset(void);
mxq_err_t HOST_init_i2c(void)
{
    mxq_err_t retVal;

    retVal = MXC_I2C_Init(MXC_I2C0, 1, 0);
#if defined (MAX32650)
    MXC_I2C0->ctrl0 |= MXC_S_I2C_CTRL0_SCL_STRD_DIS;
#endif
#if defined (MAX32690)
    MXC_I2C0->ctrl |= MXC_F_I2C_CTRL_CLKSTR_DIS;
    MXC_GPIO_Config(&bypass_smngr_cfg_i2c0);
#endif
    if (retVal != E_NO_ERROR) {
        return MXQ_ERR_TRANS_ERROR;
    }

    MXC_I2C_SetFrequency(MXC_I2C0, MXC_I2C_FAST_SPEED);
#ifndef READY_NOT_CONNECTED
    uint32_t flags = MXC_GPIO_GetFlags(maxq_rdy.port);
    MXC_GPIO_ClearFlags(maxq_rdy.port, flags);
#endif
    /*
     *   Set up RDY pin.
     */
    HOST_maxq10xx_reset();
#ifndef READY_NOT_CONNECTED
    MXC_GPIO_Config(&maxq_rdy);
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        retVal = MXQ_ERR_TRANS_ERROR;
    }
#endif
    return retVal;
}

mxq_err_t HOST_send_bytes_i2c(const mxq_u1 *src, mxq_u2 len)
{
    mxc_i2c_req_t i2c_transaction;

    i2c_transaction.i2c = MXC_I2C0;
    i2c_transaction.addr = I2C_RESPONDER_ADDR;
    i2c_transaction.tx_buf = (unsigned char *)src;
    i2c_transaction.tx_len = len;
    i2c_transaction.rx_buf = NULL;
    i2c_transaction.rx_len = 0;
    i2c_transaction.restart = 0;
    i2c_transaction.callback = NULL;

#ifndef READY_NOT_CONNECTED
    /* Block until RDY is high */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif

    MXC_I2C_Start(i2c_transaction.i2c);
    MXC_I2C_MasterTransaction(&i2c_transaction);

    if (i2c_transaction.tx_len != len) {
        return MXQ_ERR_TRANS_ERROR;      
    }

#ifndef READY_NOT_CONNECTED
    /* Block until RDY is low */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 1, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#endif

    return MXQ_OK;
}

mxq_err_t HOST_receive_bytes_i2c(mxq_u1 *dest, mxq_u2 len)
{
    mxq_err_t retVal = MXQ_OK;
    mxc_i2c_req_t i2c_transaction;
    
    i2c_transaction.i2c = MXC_I2C0;
    i2c_transaction.addr = I2C_RESPONDER_ADDR;
    i2c_transaction.tx_buf = NULL;
    i2c_transaction.tx_len = 0;
    i2c_transaction.rx_buf = (unsigned char *)dest;
    i2c_transaction.rx_len = len;
    i2c_transaction.restart = 0;
    i2c_transaction.callback = NULL;

#ifndef READY_NOT_CONNECTED
    /* Block until RDY is high */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif

    MXC_I2C_MasterTransaction(&i2c_transaction);

    if (i2c_transaction.rx_len != len) {
        return MXQ_ERR_TRANS_ERROR;
        
    }
    retVal = i2c_transaction.rx_len;

    return(retVal);
}

mxq_err_t HOST_receive_bytes_i2c_first(mxq_u1 *dest, mxq_u2 len)
{
    mxq_err_t retVal = MXQ_OK;
    mxc_i2c_req_t i2c_transaction;

    i2c_transaction.i2c = MXC_I2C0;
    i2c_transaction.addr = I2C_RESPONDER_ADDR;
    i2c_transaction.tx_buf = NULL;
    i2c_transaction.tx_len = 0;
    i2c_transaction.rx_buf = (unsigned char *)dest;
    i2c_transaction.rx_len = len;
    i2c_transaction.restart = 0;
    i2c_transaction.callback = NULL;

#ifndef READY_NOT_CONNECTED
    /* Block until RDY is high */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif

    MXC_I2C_MasterTransaction(&i2c_transaction);

    if (i2c_transaction.rx_len != len) {
        return MXQ_ERR_TRANS_ERROR;
    }

    retVal = i2c_transaction.rx_len;
    return(retVal);
}

#else // defined(USE_PROTOCOL_I2C) 

mxq_err_t HOST_init_i2c(void)
{
    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_send_bytes_i2c(const mxq_u1 *src, mxq_length len)
{
    (void) src;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_receive_bytes_i2c_first(mxq_u1 *dest, mxq_length len)
{
    (void) dest;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_receive_bytes_i2c(mxq_u1 *dest, mxq_length len)
{
    (void) dest;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_exchange_bytes_i2c(const mxq_u1 *src, mxq_length len, mxq_u1 *dest)
{
    (void) src;
    (void) dest;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

#endif // defined(USE_PROTOCOL_I2C) 

