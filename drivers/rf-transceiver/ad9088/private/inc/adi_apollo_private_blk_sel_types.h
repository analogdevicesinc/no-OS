/*!
 * \brief     API private block select types header file
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PRIVATE_DEVICE
 * @{
 */
#ifndef __ADI_APOLLO_PRIVATE_BLK_SEL_TYPES_H__
#define __ADI_APOLLO_PRIVATE_BLK_SEL_TYPES_H__

/*============= I N C L U D E S ============*/

#define ADI_APOLLO_ADC_BLK_SEL_MASK(sel)        (sel) = (device->dev_info.blk_sel_mask.adc & (sel))
#define ADI_APOLLO_BMEM_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.bmem & (sel))
#define ADI_APOLLO_DAC_BLK_SEL_MASK(sel)        (sel) = (device->dev_info.blk_sel_mask.dac & (sel))
#define ADI_APOLLO_CDDC_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.cddc & (sel))
#define ADI_APOLLO_CDUC_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.cduc & (sel))
#define ADI_APOLLO_CFIR_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.cfir & (sel))
#define ADI_APOLLO_CNCO_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.cnco & (sel))
#define ADI_APOLLO_FDDC_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.fddc & (sel))
#define ADI_APOLLO_FDUC_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.fduc & (sel))
#define ADI_APOLLO_FSRC_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.fsrc & (sel))
#define ADI_APOLLO_FNCO_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.fnco & (sel))
#define ADI_APOLLO_INVSINC_BLK_SEL_MASK(sel)    (sel) = (device->dev_info.blk_sel_mask.invsinc & (sel))
#define ADI_APOLLO_PFILT_BLK_SEL_MASK(sel)      (sel) = (device->dev_info.blk_sel_mask.pfilt & (sel))
#define ADI_APOLLO_RXEN_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.rxen & (sel))
#define ADI_APOLLO_TXEN_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.txen & (sel))
#define ADI_APOLLO_SMON_BLK_SEL_MASK(sel)       (sel) = (device->dev_info.blk_sel_mask.smon & (sel))
#define ADI_APOLLO_SNIFFER_BLK_SEL_MASK(sel)    (sel) = (device->dev_info.blk_sel_mask.sniffer & (sel))

#endif /* __ADI_APOLLO_PRIVATE_BLK_SEL_TYPES_H__ */
/*! @} */
