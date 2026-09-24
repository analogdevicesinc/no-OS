/*!
 * \brief     Device configuration header
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CONFIG
 * @{
 */
#ifndef __ADI_APOLLO_CONFIG_H__
#define __ADI_APOLLO_CONFIG_H__

/*============= D E F I N E S ==============*/

/*============= I N C L U D E S ============*/
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ == 199901L)
    #define __FUNCTION_NAME__  __func__
#else
    #define __FUNCTION_NAME__  __FUNCTION__
#endif

#define ADI_APOLLO_API_REV 0x00000001

/* var error report */
#define ADI_APOLLO_MSG_REPORT(var, comment) \
    adi_apollo_hal_error_report(device, ADI_CMS_LOG_MSG,  API_CMS_ERROR_OK, __FILE__, __FUNCTION_NAME__, __LINE__, #var, comment)
#define ADI_APOLLO_WARN_REPORT(var, comment) \
    adi_apollo_hal_error_report(device, ADI_CMS_LOG_WARN, API_CMS_ERROR_OK, __FILE__, __FUNCTION_NAME__, __LINE__, #var, comment)
#define ADI_APOLLO_ERROR_REPORT(error, var, comment) \
    adi_apollo_hal_error_report(device, ADI_CMS_LOG_ERR,  error, __FILE__, __FUNCTION_NAME__, __LINE__, #var, comment)

/* log report */
#define ADI_APOLLO_LOG_FUNC() \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_API, "%s(...)", __FUNCTION_NAME__)
#define ADI_APOLLO_LOG_SPIR(addr, data) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_SPI, "apollo: r@%.4x = %.2x", addr, data)
#define ADI_APOLLO_LOG_SPIW(addr, data) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_SPI, "apollo: w@%.4x = %.2x", addr, data)
#define ADI_APOLLO_LOG_SPIR32(addr, data) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_SPI, "apollo: r32@%.4x = %.8x", addr, data)
#define ADI_APOLLO_LOG_SPIW32(addr, data) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_SPI, "apollo: w32@%.4x = %.8x", addr, data)

#define ADI_APOLLO_LOG_VAR(type, msg, ...) \
    adi_apollo_hal_log_write(device, type, msg, ##__VA_ARGS__)

#define ADI_APOLLO_LOG_MSG(msg) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_MSG, msg)
#define ADI_APOLLO_LOG_MSG_VAR(msg, ...) \
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_MSG, msg, ##__VA_ARGS__)

#define ADI_APOLLO_LOG_WARN(msg) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_WARN, msg)
#define ADI_APOLLO_LOG_WARN_VAR(msg, ...) \
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_WARN, msg, ##__VA_ARGS__)

#define ADI_APOLLO_LOG_ERR(msg) \
    adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, msg)
#define ADI_APOLLO_LOG_ERR_VAR(msg, ...) \
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_ERR, msg, ##__VA_ARGS__)

/* var error check */
#define ADI_APOLLO_LOG_ERROR_RETURN_CODE(err, msg, code) \
{ \
    if ((err) != API_CMS_ERROR_OK) { \
        ADI_APOLLO_LOG_ERR(msg); \
        return (code); \
    } \
}

#define ADI_APOLLO_LOG_ERROR_RETURN(err, msg) \
    ADI_APOLLO_LOG_ERROR_RETURN_CODE(err, msg, err);

#define ADI_APOLLO_ERROR_RETURN(r) \
{ \
    if (r != API_CMS_ERROR_OK) { \
        return r; \
    } \
}

#define ADI_APOLLO_LOG_ERROR_RETURN_VAR(r, msg, ...) \
{ \
    if (r != API_CMS_ERROR_OK) { \
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, msg, ##__VA_ARGS__); \
        return r; \
    } \
}

#define ADI_APOLLO_ERROR_GOTO(r, label) \
{ \
    if (r != API_CMS_ERROR_OK) { \
        goto label; \
    } \
}

#define ADI_APOLLO_LOG_ERROR_GOTO_VAR(r, label, msg, ...) \
{ \
    if (r != API_CMS_ERROR_OK) { \
        adi_apollo_hal_log_write(device, ADI_CMS_LOG_ERR, msg, ##__VA_ARGS__); \
        goto label; \
    } \
}

#define ADI_APOLLO_NULL_POINTER_RETURN(p) \
{ \
    if(p == NULL) { \
        ADI_APOLLO_ERROR_REPORT(API_CMS_ERROR_NULL_PARAM, p, "Null pointer passed."); \
        return API_CMS_ERROR_NULL_PARAM; \
    } \
}

#define ADI_APOLLO_INVALID_PARAM_RETURN(r) \
{ \
    if(r) { \
        ADI_APOLLO_ERROR_REPORT(API_CMS_ERROR_INVALID_PARAM, r, "Invalid param passed."); \
        return API_CMS_ERROR_INVALID_PARAM; \
    } \
}

#define ADI_APOLLO_NOT_IMPLEMENTED_RETURN(r) \
{ \
    if(r) { \
        ADI_APOLLO_ERROR_REPORT(API_CMS_ERROR_NOT_IMPLEMENTED, r, "Not implemented feature or param."); \
        return API_CMS_ERROR_NOT_IMPLEMENTED; \
    } \
}

#define ADI_APOLLO_INVALID_PARAM_WARN(r) \
{ \
    if(r) { \
        ADI_APOLLO_WARN_REPORT(r, "Invalid param passed."); \
    } \
}

#define ADI_APOLLO_DEV_FEAT_LOCKOUT_RETURN(term, feat) \
{ \
    if (adi_apollo_private_device_lockout_get(device, term, feat)) { \
        ADI_APOLLO_ERROR_REPORT(API_CMS_ERROR_FEAT_LOCKOUT, feat, "Device feature is locked out."); \
        return API_CMS_ERROR_FEAT_LOCKOUT; \
    } \
}

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_CONFIG_H__ */

/*! @} */
