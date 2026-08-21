/*!
 * \brief     APIs for BMEM
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_BMEM
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_bmem.h"
#include "adi_apollo_private_bmem.h"
#include "adi_apollo_private_blk_sel_types.h"

#include "adi_apollo_bf_rx_bmem.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== L O C A L   F U N C T I O N S ====================*/


/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_bmem_hsdin_awg_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_awg_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);


    err = adi_apollo_private_bmem_awg_config(device, ADI_APOLLO_BMEM_HSDIN, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_awg_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_awg_start(device, ADI_APOLLO_BMEM_HSDIN, bmems);
    ADI_CMS_ERROR_RETURN(err);
    
    return err;
}

int32_t adi_apollo_bmem_hsdin_awg_stop(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_awg_stop(device, ADI_APOLLO_BMEM_HSDIN, bmems);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_awg_sram_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data);
    ADI_CMS_INVALID_PARAM_CHECK(length > ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_WORDS);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_awg_sram_set(device, ADI_APOLLO_BMEM_HSDIN, bmems, data, length);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_awg_sample_write(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, int16_t data16[], uint32_t data16_len)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data16);
    ADI_CMS_INVALID_PARAM_CHECK(data16_len/2 > ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_WORDS);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_awg_sample_write(device, ADI_APOLLO_BMEM_HSDIN, bmems, data16, data16_len);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_awg_sample_shared_write(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems,
                                             int16_t data16_0[], int16_t data16_1[], uint32_t data16_len,
                                             uint32_t scratch32[], uint32_t scratch32_len)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data16_0);
    ADI_CMS_NULL_PTR_CHECK(data16_1);
    ADI_CMS_NULL_PTR_CHECK(scratch32);
    ADI_CMS_INVALID_PARAM_CHECK(data16_len > (ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_BYTES / (2 * sizeof(uint16_t))));
    ADI_CMS_INVALID_PARAM_CHECK(scratch32_len < data16_len);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_awg_sample_shared_write(device, ADI_APOLLO_BMEM_HSDIN, bmems, data16_0, data16_1, data16_len, scratch32, scratch32_len);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_capture_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data);
    ADI_CMS_SINGLE_SELECT_CHECK(bmems);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_capture_get(device, ADI_APOLLO_BMEM_HSDIN, bmems, data, length);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_capture_run(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_capture_run(device, ADI_APOLLO_BMEM_HSDIN, bmems);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_capture_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_capture_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_CMS_INVALID_PARAM_CHECK(config->end_addr_cpt - config->st_addr_cpt >= ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_WORDS);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_capture_config(device, ADI_APOLLO_BMEM_HSDIN, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_delay_sample_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_sample_set(device, ADI_APOLLO_BMEM_HSDIN, bmems, sample_delay);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_delay_hop_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(sample_delay);
    ADI_CMS_INVALID_PARAM_CHECK(sample_delay_length != ADI_APOLLO_BMEM_HOP_PROFILES);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_hop_set(device, ADI_APOLLO_BMEM_HSDIN, bmems, sample_delay, sample_delay_length);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_delay_sample_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_sample_config(device, ADI_APOLLO_BMEM_HSDIN, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_delay_hop_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_hop_config(device, ADI_APOLLO_BMEM_HSDIN, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_hsdin_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_start(device, ADI_APOLLO_BMEM_HSDIN, bmems);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_cddc_delay_sample_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_sample_set(device, ADI_APOLLO_BMEM_CDDC, bmems, sample_delay);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_cddc_delay_hop_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(sample_delay);
    ADI_CMS_INVALID_PARAM_CHECK(sample_delay_length != ADI_APOLLO_BMEM_HOP_PROFILES);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_hop_set(device, ADI_APOLLO_BMEM_CDDC, bmems, sample_delay, sample_delay_length);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_cddc_delay_sample_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_sample_config(device, ADI_APOLLO_BMEM_CDDC, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_cddc_delay_hop_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_hop_config(device, ADI_APOLLO_BMEM_CDDC, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_cddc_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_start(device, ADI_APOLLO_BMEM_CDDC, bmems);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_fddc_delay_sample_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay)
{
    int32_t err;
    
    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_sample_set(device, ADI_APOLLO_BMEM_FDDC, bmems, sample_delay);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_fddc_delay_hop_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(sample_delay);
    ADI_CMS_INVALID_PARAM_CHECK(sample_delay_length != ADI_APOLLO_BMEM_HOP_PROFILES);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_hop_set(device, ADI_APOLLO_BMEM_FDDC, bmems, sample_delay, sample_delay_length);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_fddc_delay_sample_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_sample_config(device, ADI_APOLLO_BMEM_FDDC, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_fddc_delay_hop_config(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_hop_config(device, ADI_APOLLO_BMEM_FDDC, bmems, config);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_bmem_fddc_delay_start(adi_apollo_device_t *device, adi_apollo_blk_sel_t bmems)
{
    int32_t err;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    err = adi_apollo_private_bmem_delay_start(device, ADI_APOLLO_BMEM_FDDC, bmems);
    ADI_CMS_ERROR_RETURN(err);
    
    return err;
}
/*! @} */