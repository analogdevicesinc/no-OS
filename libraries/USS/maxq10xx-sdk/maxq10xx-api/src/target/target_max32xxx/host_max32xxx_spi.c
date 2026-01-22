/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/


#include <MXQ_API.h>

#if defined(USE_PROTOCOL_SPI)

#include "mxc_errors.h"
#include "gpio.h"
#include "spi.h"
#include "mxc_delay.h"

#define MAX32650 1
#if defined(MAX32690)
#define SET_VDDIOH
#define SPI_SPEED               7500000
#if defined(APARD)
#define SPI                     (MXC_SPI2)
#define SPI_SS                  (0)
#define SPI_GPIO_PORT           (MXC_GPIO2)
#define SPI_GPIO_PINS           (MXC_GPIO_PIN_2 | MXC_GPIO_PIN_3 | MXC_GPIO_PIN_4)
#define SPI_SS_GPIO_PORT        (MXC_GPIO2)
#define SPI_SS_GPIO_PINS        (MXC_GPIO_PIN_5)
#else
#define SPI                     (MXC_SPI0)
#define SPI_SS                  (1)
#define SPI_GPIO_PORT           (MXC_GPIO2)
#define SPI_GPIO_PINS           (MXC_GPIO_PIN_27 | MXC_GPIO_PIN_28 | MXC_GPIO_PIN_29)
#define SPI_SS_GPIO_PORT        (MXC_GPIO2)
#define SPI_SS_GPIO_PINS        (MXC_GPIO_PIN_26)
#endif //APARD
#elif defined (MAX32650)
#define SET_VDDIOH
#define SPI                     (MXC_SPI0)
#define SPI_SS                  (0)
#define SPI_GPIO_PORT           (MXC_GPIO3)
#define SPI_GPIO_PINS           (MXC_GPIO_PIN_1 | MXC_GPIO_PIN_2 | MXC_GPIO_PIN_3)
#define SPI_SS_GPIO_PORT        (MXC_GPIO0)
#define SPI_SS_GPIO_PINS        (MXC_GPIO_PIN_22)
#define SPI_SPEED               7500000
#elif defined (MAX32670)
#define SPI                     (MXC_SPI0)
#define SPI_SS                  (0)
#define SPI_GPIO_PORT           (MXC_GPIO0)
#define SPI_GPIO_PINS           (MXC_GPIO_PIN_2 | MXC_GPIO_PIN_3 | MXC_GPIO_PIN_4)
#define SPI_SS_GPIO_PORT        (MXC_GPIO0)
#define SPI_SS_GPIO_PINS        (MXC_GPIO_PIN_5)
#define SPI_SPEED               7500000
#endif //if defined(MAX32690)

#define RDY_WAIT_TIMEOUT   (10000000U)


#define SPI_POLLING_VALUE  (0xCC)
#define SPI_RESPONSE_START (0x55)
#define RDY_DELAY           (10000)
#define RSP_DELAY           (10000)

#define SPI_DATA_SIZE (8)

#define SPI_MASTER_MODE 1


volatile int SPI_FLAG;


//#if defined(MAX32690)

extern mxc_gpio_cfg_t maxq_pwdn;
//#endif

#ifndef READY_NOT_CONNECTED
extern mxc_gpio_cfg_t maxq_rdy;
extern uint32_t HOST_block_until_pin(mxc_gpio_regs_t *port, uint32_t mask, uint32_t blocking_value, uint32_t waittime);
#endif
extern void HOST_maxq10xx_reset(void);


void SPI_Callback(mxc_spi_req_t *req, int error)
{
	(void) req;
	(void) error;
    SPI_FLAG = error;
}
mxq_err_t HOST_init_spi(void)
{
    mxq_err_t retVal;
#ifndef READY_NOT_CONNECTED
    uint32_t flags;
#endif

#if defined(MAX32690)

    mxc_spi_pins_t spi_pins;

#if defined(APARD)
    spi_pins.ss0 = TRUE;
	spi_pins.ss1 = FALSE;
	spi_pins.ss2 = FALSE;
#else
    spi_pins.ss0 = FALSE;
    spi_pins.ss1 = TRUE;
    spi_pins.ss2 = FALSE;  
#endif //#if defined(APARD)
    retVal = MXC_SPI_Init(SPI, SPI_MASTER_MODE, 0, 1, 0, SPI_SPEED, spi_pins);
#else
    retVal = MXC_SPI_Init(SPI, SPI_MASTER_MODE, 0, 1, 0, SPI_SPEED);
#endif

    if (retVal != E_NO_ERROR) {
        return MXQ_ERR_TRANS_ERROR;
    }

    retVal = MXC_SPI_SetDataSize(SPI, SPI_DATA_SIZE);
    if (retVal != E_NO_ERROR) {
        return MXQ_ERR_TRANS_ERROR;
    }

    retVal = MXC_SPI_SetWidth(SPI, SPI_WIDTH_STANDARD);
    if (retVal != E_NO_ERROR) {
        return MXQ_ERR_TRANS_ERROR;
    }

#if defined(SET_VDDIOH)
    MXC_GPIO_SetVSSEL(SPI_GPIO_PORT, MXC_GPIO_VSSEL_VDDIOH, SPI_GPIO_PINS);
    MXC_GPIO_SetVSSEL(SPI_SS_GPIO_PORT, MXC_GPIO_VSSEL_VDDIOH, SPI_SS_GPIO_PINS);
#endif

    HOST_maxq10xx_reset();

#ifndef READY_NOT_CONNECTED
    flags = MXC_GPIO_GetFlags(maxq_rdy.port);
    MXC_GPIO_ClearFlags(maxq_rdy.port, flags);
    MXC_GPIO_Config(&maxq_rdy);
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        retVal = MXQ_ERR_TRANS_ERROR;
    }
#endif
    return retVal;
}

mxq_err_t HOST_exchange_bytes_spi(const mxq_u1 *src, mxq_length len, mxq_u1 *dest)
{
    mxq_err_t retVal;
    mxc_spi_req_t req;

    req.spi = SPI;
    req.txData = (uint8_t *)src;
    req.rxData = (uint8_t *)dest;
    req.txLen = len;
    req.rxLen = len;
    req.ssIdx = SPI_SS;
    req.ssDeassert = 1;
    req.txCnt = 0;
    req.rxCnt = 0;
    req.completeCB = (spi_complete_cb_t)SPI_Callback;
    SPI_FLAG = 1;

    retVal = MXC_SPI_MasterTransaction(&req);
    if (retVal != E_NO_ERROR) {
        retVal = MXQ_ERR_TRANS_ERROR;
    }
    else {
        retVal = MXQ_OK;
    }

    return retVal;
}

mxq_err_t HOST_send_bytes_spi(const mxq_u1 *src, mxq_length len)
{
    mxq_err_t retVal;
#ifndef READY_NOT_CONNECTED
    /* Block until RDY is high */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif

    retVal = HOST_exchange_bytes_spi(src, len, (mxq_u1 *)src);
#ifndef READY_NOT_CONNECTED
    /* Block until RDY is low */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 1, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif
    return retVal;
}

mxq_err_t HOST_receive_bytes_spi(mxq_u1 *dest, mxq_length len)
{
    mxq_err_t retVal;
#ifndef READY_NOT_CONNECTED
    /* Block until RDY is high */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif

    retVal = HOST_exchange_bytes_spi(NULL, len, dest);

    if(retVal != MXQ_OK) {
        retVal = MXQ_ERR_TRANS_ERROR;
    }
    else {
        retVal = len;
    }
    return retVal;
}

mxq_err_t HOST_receive_bytes_spi_first(mxq_u1 *dest, mxq_length len)
{
    mxq_err_t retVal;

    mxq_u1 dummy = SPI_POLLING_VALUE;
    dest[0] = SPI_POLLING_VALUE;
#ifndef READY_NOT_CONNECTED
    /* Block until RDY is high */
    if(HOST_block_until_pin(maxq_rdy.port, maxq_rdy.mask, 0, RDY_WAIT_TIMEOUT) != MXQ_OK) {
        return MXQ_ERR_TRANS_TIMEOUT;
    }
#else
    MXC_Delay(RDY_DELAY);
#endif

    while(dest[0] != SPI_RESPONSE_START)
    {
        MXC_Delay(RSP_DELAY);
        retVal = HOST_exchange_bytes_spi(&dummy, 1, dest);
        if (retVal != E_NO_ERROR){
            return MXQ_ERR_TRANS_ERROR;
        }
    }

    retVal = 1 + HOST_receive_bytes_spi(&dest[1], len - 1);

    return retVal;
}

#else // defined(USE_PROTOCOL_SPI) 

mxq_err_t HOST_init_spi(void)
{
    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_exchange_bytes_spi(const mxq_u1 *src, mxq_length len, mxq_u1 *dest)
{
    (void) src;
    (void) dest;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_send_bytes_spi(const mxq_u1 *src, mxq_length len)
{
    (void) src;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_receive_bytes_spi_first(mxq_u1 *dest, mxq_length len)
{
    (void) dest;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

mxq_err_t HOST_receive_bytes_spi(mxq_u1 *dest, mxq_length len)
{
    (void) dest;
    (void) len;

    return MXQ_RSP_ERR_NOT_IMPLEMENTED;
}

#endif // defined(USE_PROTOCOL_SPI)
