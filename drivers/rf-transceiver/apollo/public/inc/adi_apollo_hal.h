/*!
 * \brief     Contains apollo API HAL functions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_HAL
 * @{
 */
#ifndef __APOLLO_HAL_H__
#define __APOLLO_HAL_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_hal_types.h"
#include "adi_apollo_common_types.h"

#ifdef __KERNEL__
#include <linux/math64.h>
#include <linux/string.h>
#endif

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Get bit field value.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  reg              register to read from
 * \param[in]  info             information about bitfield
 * \param[out] value            value of bitfield
 * \param[in]  value_size_bytes no of bytes 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_bf_get(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint8_t value[], uint8_t value_size_bytes);

/**
 * \brief  Set bit field value.
 *
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg      register to read from
 * \param[in] info     information about bitfield
 * \param[in] value    value of bitfield
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_bf_set(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint64_t value);

/**
 * \brief  Set register value based on a mask
 *
 * \param[in] device   Context variable - Pointer to the device structure
 * \param[in] reg      Register write
 * \param[in] data     Value to write to register
 * \param[in] mask     Bitfield mask. Only bits set in the mask will be written to register
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_masked_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data, uint8_t mask);

/**
 * \brief  Set paged register value based on a mask
 *
 * \param[in] device   Context variable - Pointer to the device structure
 * \param[in] reg      Register write
 * \param[in] data     Value to write to register
 * \param[in] mask     Bitfield mask. Only bits set in the mask will be written to register
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_paged_masked_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data, uint8_t mask);

/**
 * \brief  Get 8-bit register value.
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  reg      Register to read from
 * \param[out] data     Value of register (8-bit)
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_reg_get(adi_apollo_device_t *device, uint32_t reg, uint8_t *data);

/**
 * \brief  Get 32-bit register value.
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  reg      Register to read from
 * \param[out] data     Value of register (32-bit)
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_reg32_get(adi_apollo_device_t *device, uint32_t reg, uint32_t *data);

/**
 * \brief  Set the 8-bit register value.
 *
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg      Register to write to
 * \param[in] data     Value to write to register
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data);

/**
 * \brief  Set the 32-bit register value.
 *
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg      Register to write to
 * \param[in] data     Value to write to register
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_reg32_set(adi_apollo_device_t *device, uint32_t reg, uint32_t data);

/**
 * \brief Get the register value for a polling operation.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  reg          Register to read from
 * \param[out] data         Value of register (8-bits)
 * \param[in]  timeout_us   Suggested timeout value from API (usecs)
 * \param[in]  reg_expect   Typical register exp (not masked)
 * \param[in]  reg_mask     Bitfield register mask
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_reg_poll_get(adi_apollo_device_t *device, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);



/**
 * \brief Sets the page base address for subsequent paged register accesses.
 * 
 * This function sets up the base address used for paged register access.
 * 
 * The following APIs are affected:
 *      \ref adi_apollo_hal_paged_reg_set
 *      \ref adi_apollo_hal_paged_reg32_set
 *      \ref adi_apollo_hal_paged_bf_set
 * 
 * \note Paging is only valid for writes. Device doesn't support paging reads.
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  base_addr    Page base address
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_paged_base_addr_set(adi_apollo_device_t *device, uint32_t base_addr);

/**
 * \brief Set the 8-bit register value offset from the page base address.
 *
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg      Register to write. \ref adi_apollo_hal_paged_base_addr_set
 * \param[in] data     Value to write to register
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_paged_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data);


/**
 * \brief Set the 32-bit register value offset from the page base address.
 *
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg      Register to write, offset from the paged base address. \ref adi_apollo_hal_paged_base_addr_set
 * \param[in] data     Value to write to register
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_paged_reg32_set(adi_apollo_device_t *device, uint32_t reg, uint32_t data);

/**
 * \brief  8-bit register stream writes.
 *
 * \param[in] device          Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg             Start address register to write
 * \param[in] data            Data array to write to the register
 * \param[in] data_len_bytes  Total number of bytes to stream
 * \param[in] is_cont         Flag to decide if stream is continuous to previous txn 
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_stream_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data[], uint32_t data_len_bytes, uint8_t is_cont);

/**
 * \brief  32-bit register stream writes.
 *
 * \param[in] device          Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg             Start address register to write
 * \param[in] data            Data array to write to the register
 * \param[in] data_len_words  Total number of words to stream
 * \param[in] is_cont         Flag to decide if stream is continuous to previous txn 
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_stream_reg32_set(adi_apollo_device_t *device, uint32_t reg, uint32_t data[], uint32_t data_len_words, uint8_t is_cont);

/**
 * \brief  8-bit register stream read.
 *
 * \param[in] device          Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg             Start address register to read
 * \param[out] data           Data array read from start address reg
 * \param[in] data_len_bytes  Total number of bytes to stream
 * \param[in] is_cont         Flag to decide if stream is continuous to previous txn 
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_stream_reg_get(adi_apollo_device_t *device, uint32_t reg, uint8_t data[], uint32_t data_len_bytes, uint8_t is_cont);

/**
 * \brief  32-bit register stream read.
 *
 * \param[in] device          Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg             Start address register to read
 * \param[out] data           Data array read from start address reg
 * \param[in] data_len_words  Total number of bytes to stream
 * \param[in] is_cont         Flag to decide if stream is continuous to previous txn 
 *  
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_stream_reg32_get(adi_apollo_device_t *device, uint32_t reg, uint32_t data[], uint32_t data_len_words, uint8_t is_cont);

/**
 * \brief Set bit field value using paged register addressing.
 *
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg      Register to write. \ref adi_apollo_hal_paged_base_addr_set
 * \param[in] info     Information about bitfield
 * \param[in] value    Value of bitfield
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_paged_bf_set(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint64_t value);

/**
 * \brief Sets the active protocol for API
 * 
 * This function changes the active protocol used for register access. If the platform
 * supports multiple protocols, then realtime switching is possible. For example, one
 * can switch between SPI0 and HSCI by calling this function.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] protocol  Selects to active protocol for API register access. \ref adi_apollo_hal_protocol_e
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_active_protocol_set(adi_apollo_device_t *device, adi_apollo_hal_protocol_e protocol);

/**
 * \brief Gets the active protocol for API
 * 
 * This function returns the active protocol used for register access. If the platform
 * supports multiple protocols, then realtime switching is possible. For example, one
 * can switch between SPI0 and HSCI by calling the \ref adi_apollo_hal_active_protocol_set function.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[out] protocol Active protocol for API register access. \ref adi_apollo_hal_protocol_e
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_active_protocol_get(adi_apollo_device_t *device, adi_apollo_hal_protocol_e *protocol);

/**
 * \brief Enable the read-modify-write feature for bit field writes.
 * 
 * Apollo supports a hardware read-modify-write feature that eliminates
 * the need to first read a register (e.g from spi) when modifying a bitfield within a register.
 * 
 * \note It is recommended that r-m-w is enabled for optimal register transacton performance.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] protocol  Selects to active protocol for API register access. \ref adi_apollo_hal_protocol_e
 * \param[in] rmw_en    0 = disable rmw, 1 = enable rmw
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_rmw_enable_set(adi_apollo_device_t *device, adi_apollo_hal_protocol_e protocol, uint8_t rmw_en);

/**
 * \brief Returns the read-modify-write feature enable state.
 * 
 * Apollo supports a hardware read-modify-write feature that eliminates
 * the need to first read a register (e.g from spi) when modifying a bitfield within a register.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] protocol  Selects to active protocol for API register access. \ref adi_apollo_hal_protocol_e
 * \param[out] rmw_en   0 = disable rmw, 1 = enable rmw
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_rmw_enable_get(adi_apollo_device_t *device, adi_apollo_hal_protocol_e protocol, uint8_t *rmw_en);

/**
 * \brief  Configuring the manual link-up operation
 *
 * \param[in]  device                Context variable - Pointer to the APOLLO device data structure
 * \param[in]  enable                Set 1 to enable manual link-up
 * \param[in]  link_up_signal_bits   Clock signal used to achieve linkup
 *
 * \return API_CMS_ERROR_OK          API Completed Successfully
 * \return <0                        Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_manual_linkup(adi_apollo_device_t *device, uint8_t enable, uint16_t link_up_signal_bits);

/**
 * \brief  Configuring the auto link-up operation
 *
 * \param[in]  device                Context variable - Pointer to the APOLLO device data structure
 * \param[in]  enable                Set 1 to enable auto link-up
 * \param[in]  hscim_mosi_clk_inv   Set 1, inverts HSCIM's mosi_clk output clk to Apollo
 * \param[in]  hscim_miso_clk_inv   Set 1, inverts HSCIM's miso_clk input clk from Apollo
 *
 * \return API_CMS_ERROR_OK          API Completed Successfully
 * \return <0                        Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_auto_linkup(adi_apollo_device_t *device, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv);

/**
 * \brief  Get the HSCIM auto link_table
 * \note   Retrive HSCIM ALINK_TABLE after Apollo's link_active is set
 *
 * \param[in]  device                Context variable - Pointer to the APOLLO device data structure
 * \param[in]  enable                Set 1 to enable auto link-up
 * \param[out] hscim_alink_table     Pointer to a variable that stores link table value from HSCI_M
 *
 * \return API_CMS_ERROR_OK          API Completed Successfully
 * \return <0                        Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_alink_tbl_get(adi_apollo_device_t *device, uint16_t *hscim_alink_table);

#ifndef CLIENT_IGNORE

/**
 * \brief Calls the platform hw_open function
 *
 * \note hw_open is optional.
 * 
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_hw_open(adi_apollo_device_t *device);

/**
 * \brief Calls the platform hw_close function
 *
 * \note hw_open is optional.
 * 
 * \param[in] device   Context variable - Pointer to the APOLLO device data structure
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_hw_close(adi_apollo_device_t *device);

/**
 * \brief Calls the platform delay_us function
 *
 * \note delay_us is required.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] us        Delay in micro seconds
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_delay_us(adi_apollo_device_t *device, uint32_t us);


/**
 * \brief Calls the platform reset_pin_ctrl function
 *
 * \note reset_pin_ctrl is required.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] enable    0 = disable, 1 = enable
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_reset_pin_ctrl(adi_apollo_device_t *device, uint8_t enable);


/**
 * \brief Calls the platform log_write function
 *
 * \note log_write is optional.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] type      Type of log message. \ref adi_cms_log_type_e
 * \param[in] comment   Comment string
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_log_write(adi_apollo_device_t *device, adi_cms_log_type_e type, const char* comment, ...);

/**
 * \brief Wait for a bitfield to clear
 *
 * \note Supports single bit bit-field only.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg       Register to read from
 * \param[in] info      Information about bitfield
 * \param[in] timeout   Timeout in us
 * \param[in] poll_dly  Polling delay in us
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_bf_wait_to_clear(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint32_t timeout, uint32_t poll_dly);

/**
 * \brief Wait for a bitfield to set
 * 
 * \note Supports single bit bit-field only.
 *
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] reg       Register to read from
 * \param[in] info      Information about bitfield
*  \param[in] timeout   Timeout in us
 * \param[in] poll_dly  Polling delay in us
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_bf_wait_to_set(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint32_t timeout, uint32_t poll_dly);

/**
 * \brief Creates formatted log error message.
 * 
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] log_type  Log type. See \ref adi_cms_log_type_e
 * \param[in] error     Error code
 * \param[in] file_name Name for file where error occurred
 * \param[in] func_name Name of function where error occurred
 * \param[in] line_num  Line number in files where error occurred
 * \param[in] var_name  Variable name
 * \param[in] comment   Comment string
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_error_report(adi_apollo_device_t* device, adi_cms_log_type_e log_type,
    int32_t error, const char* file_name, const char* func_name, uint32_t line_num,
    const char* var_name, const char* comment);

/**
 * \brief Assigns user allocated memory for use by HAL APIs
 *
 * \note Currently only applicable for HSCI protocol
 *
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] protocol  Protocol to assign buffer to. See \ref adi_apollo_hal_protocol_e
 * \param[in] buff      Pointer to user allocated memory
 * \param[in] buff_len  Nummber of bytes in 'buff'. Max 32K, Min 128
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_buffer_set(adi_apollo_device_t* device, adi_apollo_hal_protocol_e protocol, uint8_t *buff, uint32_t buff_len);

/**
 * \brief Gets pointer to user allocated memory that is used by API for various transactions
 *
 * \param[in] device    Context variable - Pointer to the APOLLO device data structure
 * \param[in] protocol  Protocol to get buffer from. See \ref adi_apollo_hal_protocol_e
 * \param[out] buff     Pointer to user allocated memory
 * \param[out] buff_len Nummber of bytes in 'buff'.
 *  
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_hal_buffer_get(adi_apollo_device_t* device, adi_apollo_hal_protocol_e protocol, uint8_t **buff, uint32_t *buff_len);

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __APOLLO_REG_H__ */

/*! @} */
