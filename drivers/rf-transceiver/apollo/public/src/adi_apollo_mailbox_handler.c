/*!
 * \brief     APIs for MAILBOX
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_MAILBOX
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_arm.h"
#include "adi_apollo_mailbox_handler.h"
#include "adi_apollo_mailbox_handler_local.h"

#define USE_PRIVATE_BF
#include "adi_apollo_bf_core.h"
#include "adi_apollo_bf_custom.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

#define ADI_ADI_APOLLO_CPU1_MAILBOX_START         0x21000000
#define ADI_ADI_APOLLO_RAM_BOOT_STEP_MAILBOX_INIT 48U
#define ADI_ADI_APOLLO_RAM_BOOT_STEP_MAILBOX_READY 52U

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_mailbox_ready_check(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t status = 0x00;
    int32_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < 50; i++)
    {
        err = adi_apollo_arm_ram_boot_error_check(device);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_arm_boot_status(device, &status);
        ADI_APOLLO_ERROR_RETURN(err);
        if(status >= ADI_ADI_APOLLO_RAM_BOOT_STEP_MAILBOX_READY)
        {    
            err = API_CMS_ERROR_OK;
            break;
        }
        
        //delay 100ms
        err = adi_apollo_hal_delay_us(device, 100000);
        ADI_APOLLO_ERROR_RETURN(err);
        
	    err = API_CMS_ERROR_ERROR;
    }

    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_mailbox_busy_wait(adi_apollo_device_t *device)
{
    uint32_t delay_us = 100000;             // 100ms polling delay
    uint32_t max_delay_us = 150*delay_us;   // 15s timeout
    return adi_apollo_hal_bf_wait_to_clear(device, BF_ARM1_SPI0_COMMAND_BUSY_INFO, max_delay_us, delay_us);
}

int32_t adi_apollo_mailbox_read(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_id_e cmd_id, uint8_t* ptr_resp, const size_t size_resp)
{

    int32_t err;
    uint32_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(ptr_resp);
    // ADI_APOLLO_LOG_FUNC();

    /* Wait for non-busy state */
    err = adi_apollo_mailbox_busy_wait(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Read buffer */
    for (i = 0; i < size_resp; i++)
    {
        err = adi_apollo_hal_reg_get(device, (ADI_ADI_APOLLO_CPU1_MAILBOX_START + i), (ptr_resp + i));
        ADI_APOLLO_ERROR_RETURN(err);
    }
    
    return API_CMS_ERROR_OK; 
}

int32_t adi_apollo_mailbox_write(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_id_e cmd_id, uint8_t* ptr_cmd, const size_t size_cmd)
{

    int32_t err;
    uint32_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    if (size_cmd != 0)
        ADI_APOLLO_NULL_POINTER_RETURN(ptr_cmd);
    // ADI_APOLLO_LOG_FUNC();

    /* Wait for non-busy state */
    err = adi_apollo_mailbox_busy_wait(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Write buffer */
    if (size_cmd != 0)
    {
        for (i = 0; i < size_cmd; i++)
        {
            err = adi_apollo_hal_reg_set(device, (ADI_ADI_APOLLO_CPU1_MAILBOX_START + i), *(ptr_cmd + i));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    /* Write OPCODE */
    err = adi_apollo_hal_bf_set(device, BF_ARM1_SPI0_COMMAND_INFO, cmd_id);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK; 

}

int32_t adi_apollo_mailbox_xfer(adi_apollo_device_t *device, adi_apollo_mailbox_cmd_id_e cmd_id, uint8_t* ptr_cmd, const size_t size_cmd, uint8_t* ptr_resp, const size_t size_resp)
{

    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    if (size_cmd != 0)
        ADI_APOLLO_NULL_POINTER_RETURN(ptr_cmd);
    ADI_APOLLO_NULL_POINTER_RETURN(ptr_resp);
    // ADI_APOLLO_LOG_FUNC();
    
    err = adi_apollo_mailbox_write(device, cmd_id, ptr_cmd, size_cmd);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_mailbox_read(device, cmd_id, ptr_resp, size_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK; 

}
/*! @} */
