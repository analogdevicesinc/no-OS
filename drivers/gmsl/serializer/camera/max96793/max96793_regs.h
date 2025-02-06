/*******************************************************************************
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensors.
******************************************************************************
 * \file: max96793_regs.h
 * \brief: Header file of register fields for GMSL device MAX96793
 * Developed by: Automotive Software and Systems team
*****************************************************************************/
#ifndef MAX96793_REGS_H
#define MAX96793_REGS_H

/*================================= INCLUDES ================================*/
/*================================= DEFINES =================================*/

#define CFGL_SPI_ARQ2_MASK                                      (0xFFU)
#define CFGL_GPIO_ARQ2_MASK                                     (0xFFU)
#define CFGL_IIC_X_ARQ2_MASK                                    (0xFFU)
#define CFGL_IIC_Y_ARQ2_MASK                                    (0xFFU)

#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_1B_ERR_MASK         (0x01U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_1B_ERR_POS          (0U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_1B_ERR_MASK         (0x02U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_1B_ERR_POS          (1U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_2B_ERR_MASK         (0x04U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE0_2B_ERR_POS          (2U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_2B_ERR_MASK         (0x08U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_LANE1_2B_ERR_POS          (3U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE1_ERR_MASK (0x10U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE1_ERR_POS  (4U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE0_ERR_MASK (0x20U)
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_SKEW_CALIB_LANE0_ERR_POS  (5U)

#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_2B_ERR_MASK         (0x01U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_2B_ERR_POS          (0U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_2B_ERR_MASK         (0x02U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_2B_ERR_POS          (1U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_1B_ERR_MASK         (0x04U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE0_1B_ERR_POS          (2U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_1B_ERR_MASK         (0x08U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_LANE1_1B_ERR_POS          (3U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE1_ERR_MASK (0x10U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE1_ERR_POS  (4U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE0_ERR_MASK (0x20U)
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_SKEW_CALIB_LANE0_ERR_POS  (5U)

#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_1B_ECC_ERR_MASK       (0x01U)
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_1B_ECC_ERR_POS        (0U)
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_2B_ECC_ERR_MASK       (0x02U)
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_2B_ECC_ERR_POS        (1U)
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_CRC_ERR_MASK          (0x80U)
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_CRC_ERR_POS           (7U)

#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_ALL_MASK              (0xFFU)

#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_PKT_TERM_EARLY_ERR_MASK (0x01U)
#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_PKT_TERM_EARLY_ERR_POS  (0U)
#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_FRAME_CNT_ERR_MASK      (0x02U)
#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_FRAME_CNT_ERR_POS       (1U)

#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_MASK  (0x01U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_POS   (0U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_MASK (0x02U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_POS  (1U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D0_MASK           (0x04U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D0_POS            (2U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D1_MASK           (0x08U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_D1_POS            (3U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_CLK_MASK          (0x10U)
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_INVALID_LINE_SEQ_ON_CLK_POS           (4U)

#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_MASK  (0x01U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_D0_POS   (0U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_MASK (0x02U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_UNRECOGNIZED_ESC_CMD_RCVD_ON_CLK_POS  (1U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D0_MASK           (0x04U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D0_POS            (2U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D1_MASK           (0x08U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_D1_POS            (3U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_CLK_MASK          (0x10U)
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_INVALID_LINE_SEQ_ON_CLK_POS           (4U)

#define RX_FEC_EN_GMSL_TX0_ADDR                               (0x28U)
#define RX_FEC_EN_GMSL_TX0_MASK                               (0x01U)
#define RX_FEC_EN_GMSL_TX0_POS                                (0U)

#define PHY_CONFIG_MIPI_RX_MIPI_RX0_ADDR                      (0x330U)
#define PHY_CONFIG_MIPI_RX_MIPI_RX0_MASK                      (0x07U)
#define PHY_CONFIG_MIPI_RX_MIPI_RX0_POS                       (0U)

#define PHY2_LANE_MAP_SER_LANE_0_MIPI_RX_MIPI_RX3_MASK        (0x03U)
#define PHY2_LANE_MAP_SER_LANE_0_MIPI_RX_MIPI_RX3_POS         (0U)

#define PHY2_LANE_MAP_SER_LANE_1_MIPI_RX_MIPI_RX3_MASK        (0x0CU)
#define PHY2_LANE_MAP_SER_LANE_1_MIPI_RX_MIPI_RX3_POS         (2U)

#define PHY1_LANE_MAP_SER_LANE_2_MIPI_RX_MIPI_RX2_MASK        (0x30U)
#define PHY1_LANE_MAP_SER_LANE_2_MIPI_RX_MIPI_RX2_POS         (4U)

#define PHY1_LANE_MAP_SER_LANE_3_MIPI_RX_MIPI_RX2_MASK        (0xC0U)
#define PHY1_LANE_MAP_SER_LANE_3_MIPI_RX_MIPI_RX2_POS         (6U)

#define PHY2_POL_MAP_CLK_LANE_MIPI_RX_MIPI_RX5_MASK           (0x04U)
#define PHY2_POL_MAP_CLK_LANE_MIPI_RX_MIPI_RX5_POS            (2U)
#define PHY2_POL_MAP_DATA_LANE_0_MIPI_RX_MIPI_RX5_MASK        (0x01U)
#define PHY2_POL_MAP_DATA_LANE_0_MIPI_RX_MIPI_RX5_POS         (0U)
#define PHY2_POL_MAP_DATA_LANE_1_MIPI_RX_MIPI_RX5_MASK        (0x02U)
#define PHY2_POL_MAP_DATA_LANE_1_MIPI_RX_MIPI_RX5_POS         (1U)
#define PHY1_POL_MAP_DATA_LANE_2_MIPI_RX_MIPI_RX5_MASK        (0x01U)
#define PHY1_POL_MAP_DATA_LANE_2_MIPI_RX_MIPI_RX5_POS         (0U)
#define PHY1_POL_MAP_DATA_LANE_3_MIPI_RX_MIPI_RX5_MASK        (0x02U)
#define PHY1_POL_MAP_DATA_LANE_3_MIPI_RX_MIPI_RX5_POS         (1U)

#define PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_ALL_MASK               (0xFFU)
#define PHY1_POL_MAP_MIPI_RX_MIPI_RX4_ALL_MASK                (0xFFU)

#define FRONTTOP_EXT12_ADDR                                   (0x3CAU)
#define FRONTTOP_EXT12_MASK                                   (0xFFU)

#define FRONTTOP_EXT13_ADDR                                   (0x3CBU)
#define FRONTTOP_EXT13_MASK                                   (0xFFU)

#define DATA_TYPE_MASK                                        (0x3FU)
#define DATA_TYPE_ENABLE_MASK                                 (0x40U)
#define DATA_TYPE_ENABLE_POS                                  (6U)

#define INDEPENDENT_VS_FRONTTOP_13_ADDR                       (0x315U)
#define INDEPENDENT_VS_FRONTTOP_13_MASK                       (0x80U)

#define CLK_SELZ_FRONTTOP_FRONTTOP_0_MASK                     (0x04U)
#define CLK_SELZ_FRONTTOP_FRONTTOP_0_POS                      (2U)

#define FRONTTOP_FRONTTOP_0_ALL_MASK                          (0xFFU)

#define DEV_REG0_ADDR    	 0x00U
#define DEV_REG0_DEFAULT 	 0x80U

#define CFG_BLOCK_DEV_REG0_ADDR 	 0x00U // Configuration Block  
#define CFG_BLOCK_DEV_REG0_MASK 	 0x01U
#define CFG_BLOCK_DEV_REG0_POS  	 0U

#define DEV_ADDR_DEV_REG0_ADDR 	 0x00U // Device Address  
#define DEV_ADDR_DEV_REG0_MASK 	 0xFEU
#define DEV_ADDR_DEV_REG0_POS  	 1U

#define DEV_REG1_ADDR    	 0x01U
#define DEV_REG1_DEFAULT 	 0x08U

#define RX_RATE_DEV_REG1_ADDR 	 0x01U // Receiver (reverse channel) bit rate (whe... 
#define RX_RATE_DEV_REG1_MASK 	 0x03U
#define RX_RATE_DEV_REG1_POS  	 0U

#define TX_RATE_DEV_REG1_ADDR 	 0x01U // Transmitter (forward channel) bit rate (... 
#define TX_RATE_DEV_REG1_MASK 	 0x0CU
#define TX_RATE_DEV_REG1_POS  	 2U

#define DIS_REM_CC_DEV_REG1_ADDR 	 0x01U // Disable access to remote device control-... 
#define DIS_REM_CC_DEV_REG1_MASK 	 0x10U
#define DIS_REM_CC_DEV_REG1_POS  	 4U

#define DIS_LOCAL_CC_DEV_REG1_ADDR 	 0x01U // Disable control-channel connection to RX... 
#define DIS_LOCAL_CC_DEV_REG1_MASK 	 0x20U
#define DIS_LOCAL_CC_DEV_REG1_POS  	 5U

#define IIC_1_EN_DEV_REG1_ADDR 	 0x01U // Enable pass-through I2C Channel 1 (SDA1/... 
#define IIC_1_EN_DEV_REG1_MASK 	 0x40U
#define IIC_1_EN_DEV_REG1_POS  	 6U

#define IIC_2_EN_DEV_REG1_ADDR 	 0x01U // Enable pass-through I2C Channel 2 (SDA2/... 
#define IIC_2_EN_DEV_REG1_MASK 	 0x80U
#define IIC_2_EN_DEV_REG1_POS  	 7U

#define DEV_REG2_ADDR    	 0x02U
#define DEV_REG2_DEFAULT 	 0x43U

#define VID_TX_EN_Z_DEV_REG2_ADDR 	 0x02U // Video Transmit Enable for Video Pipe Z  
#define VID_TX_EN_Z_DEV_REG2_MASK 	 0x40U
#define VID_TX_EN_Z_DEV_REG2_POS  	 6U

#define DEV_REG3_ADDR    	 0x03U
#define DEV_REG3_DEFAULT 	 0x00U

#define RCLKSEL_DEV_REG3_ADDR 	 0x03U // RCLKOUT clock selection  
#define RCLKSEL_DEV_REG3_MASK 	 0x03U
#define RCLKSEL_DEV_REG3_POS  	 0U

#define RCLK_ALT_DEV_REG3_ADDR 	 0x03U // Selects MFP pin to route RCLK to.  
#define RCLK_ALT_DEV_REG3_MASK 	 0x04U
#define RCLK_ALT_DEV_REG3_POS  	 2U

#define UART_1_EN_DEV_REG3_ADDR 	 0x03U // Enable pass-through UART Channel 1 (SDA1... 
#define UART_1_EN_DEV_REG3_MASK 	 0x10U
#define UART_1_EN_DEV_REG3_POS  	 4U

#define UART_2_EN_DEV_REG3_ADDR 	 0x03U // Enable pass-through UART Channel 2 (SDA2... 
#define UART_2_EN_DEV_REG3_MASK 	 0x20U
#define UART_2_EN_DEV_REG3_POS  	 5U

#define DEV_REG4_ADDR    	 0x04U
#define DEV_REG4_DEFAULT 	 0x18U

#define XTAL_PU_DEV_REG4_ADDR 	 0x04U // Enable XTAL as reference clock.  
#define XTAL_PU_DEV_REG4_MASK 	 0x01U
#define XTAL_PU_DEV_REG4_POS  	 0U

#define CC_CRC_MSGCNTR_OVR_DEV_REG4_ADDR 	 0x04U // Enable manual override of I2C/UART CRC o... 
#define CC_CRC_MSGCNTR_OVR_DEV_REG4_MASK 	 0x04U
#define CC_CRC_MSGCNTR_OVR_DEV_REG4_POS  	 2U

#define CC_CRC_EN_DEV_REG4_ADDR 	 0x04U // Enable I2C/UART CRC override when set to... 
#define CC_CRC_EN_DEV_REG4_MASK 	 0x08U
#define CC_CRC_EN_DEV_REG4_POS  	 3U

#define CC_MSGCNTR_EN_DEV_REG4_ADDR 	 0x04U // Enable I2C/UART message counter override... 
#define CC_MSGCNTR_EN_DEV_REG4_MASK 	 0x10U
#define CC_MSGCNTR_EN_DEV_REG4_POS  	 4U

#define DEV_REG5_ADDR    	 0x05U
#define DEV_REG5_DEFAULT 	 0x00U

#define PU_LF0_DEV_REG5_ADDR 	 0x05U // Power Up Line-Fault Monitor 0  
#define PU_LF0_DEV_REG5_MASK 	 0x01U
#define PU_LF0_DEV_REG5_POS  	 0U

#define PU_LF1_DEV_REG5_ADDR 	 0x05U // Power Up Line-Fault Monitor 1  
#define PU_LF1_DEV_REG5_MASK 	 0x02U
#define PU_LF1_DEV_REG5_POS  	 1U

#define ALT_ERRB_EN_DEV_REG5_ADDR 	 0x05U // Enable ERRB output on alternate output  
#define ALT_ERRB_EN_DEV_REG5_MASK 	 0x10U
#define ALT_ERRB_EN_DEV_REG5_POS  	 4U

#define ALT_LOCK_EN_DEV_REG5_ADDR 	 0x05U // Enable LOCK output on alternate output  
#define ALT_LOCK_EN_DEV_REG5_MASK 	 0x20U
#define ALT_LOCK_EN_DEV_REG5_POS  	 5U

#define ERRB_EN_DEV_REG5_ADDR 	 0x05U // Enable ERRB Output  
#define ERRB_EN_DEV_REG5_MASK 	 0x40U
#define ERRB_EN_DEV_REG5_POS  	 6U

#define LOCK_EN_DEV_REG5_ADDR 	 0x05U // Enable LOCK Output  
#define LOCK_EN_DEV_REG5_MASK 	 0x80U
#define LOCK_EN_DEV_REG5_POS  	 7U

#define DEV_REG6_ADDR    	 0x06U
#define DEV_REG6_DEFAULT 	 0x80U

#define I2CSEL_DEV_REG6_ADDR 	 0x06U // I2C/UART selection  
#define I2CSEL_DEV_REG6_MASK 	 0x10U
#define I2CSEL_DEV_REG6_POS  	 4U

#define RCLKEN_DEV_REG6_ADDR 	 0x06U // Enable/disable RCLK Output.  
#define RCLKEN_DEV_REG6_MASK 	 0x20U
#define RCLKEN_DEV_REG6_POS  	 5U

#define DEV_REG13_ADDR    	 0x0DU
#define DEV_REG13_DEFAULT 	 0xB7U

#define DEV_ID_DEV_REG13_ADDR 	 0x0DU // Device Identifier  
#define DEV_ID_DEV_REG13_MASK 	 0xFFU
#define DEV_ID_DEV_REG13_POS  	 0U

#define DEV_REG14_ADDR    	 0x0EU
#define DEV_REG14_DEFAULT 	 0x06U

#define DEV_REV_DEV_REG14_ADDR 	 0x0EU // Device Revision  
#define DEV_REV_DEV_REG14_MASK 	 0x0FU
#define DEV_REV_DEV_REG14_POS  	 0U

#define DEV_REG26_ADDR    	 0x26U
#define DEV_REG26_DEFAULT 	 0x22U

#define LF_0_DEV_REG26_ADDR 	 0x26U // Line-fault status of wire connected to L... 
#define LF_0_DEV_REG26_MASK 	 0x07U
#define LF_0_DEV_REG26_POS  	 0U

#define LF_1_DEV_REG26_ADDR 	 0x26U // Line-fault status of wire connected to L... 
#define LF_1_DEV_REG26_MASK 	 0x70U
#define LF_1_DEV_REG26_POS  	 4U

#define TCTRL_PWR0_ADDR    	 0x08U
#define TCTRL_PWR0_DEFAULT 	 0x00U

#define CMP_STATUS_TCTRL_PWR0_ADDR 	 0x08U // VDD18, VDDIO, and CAP_VDD supply voltage... 
#define CMP_STATUS_TCTRL_PWR0_MASK 	 0x1FU
#define CMP_STATUS_TCTRL_PWR0_POS  	 0U

#define VDDBAD_STATUS_TCTRL_PWR0_ADDR 	 0x08U // Switched 1V supply comparator status bit... 
#define VDDBAD_STATUS_TCTRL_PWR0_MASK 	 0xE0U
#define VDDBAD_STATUS_TCTRL_PWR0_POS  	 5U

#define TCTRL_PWR4_ADDR    	 0x0CU
#define TCTRL_PWR4_DEFAULT 	 0x15U

#define WAKE_EN_A_TCTRL_PWR4_ADDR 	 0x0CU // Enable wake-up by remote chip connected ... 
#define WAKE_EN_A_TCTRL_PWR4_MASK 	 0x10U
#define WAKE_EN_A_TCTRL_PWR4_POS  	 4U

#define DIS_LOCAL_WAKE_TCTRL_PWR4_ADDR 	 0x0CU // Disable wake-up by local μC from SDA_RX ... 
#define DIS_LOCAL_WAKE_TCTRL_PWR4_MASK 	 0x40U
#define DIS_LOCAL_WAKE_TCTRL_PWR4_POS  	 6U

#define TCTRL_CTRL0_ADDR    	 0x10U
#define TCTRL_CTRL0_DEFAULT 	 0x01U

#define SLEEP_TCTRL_CTRL0_ADDR 	 0x10U // Activate Sleep Mode  
#define SLEEP_TCTRL_CTRL0_MASK 	 0x08U
#define SLEEP_TCTRL_CTRL0_POS  	 3U

#define RESET_ONESHOT_TCTRL_CTRL0_ADDR 	 0x10U // Reset data path (keep register settings)... 
#define RESET_ONESHOT_TCTRL_CTRL0_MASK 	 0x20U
#define RESET_ONESHOT_TCTRL_CTRL0_POS  	 5U

#define RESET_LINK_TCTRL_CTRL0_ADDR 	 0x10U // Reset data path (keep register settings)... 
#define RESET_LINK_TCTRL_CTRL0_MASK 	 0x40U
#define RESET_LINK_TCTRL_CTRL0_POS  	 6U

#define RESET_ALL_TCTRL_CTRL0_ADDR 	 0x10U // Writing 1 to this bit resets the device,... 
#define RESET_ALL_TCTRL_CTRL0_MASK 	 0x80U
#define RESET_ALL_TCTRL_CTRL0_POS  	 7U

#define TCTRL_CTRL1_ADDR    	 0x11U
#define TCTRL_CTRL1_DEFAULT 	 0x02U

#define CXTP_A_TCTRL_CTRL1_ADDR 	 0x11U // Coax/Twisted-pair cable select for GMSL ... 
#define CXTP_A_TCTRL_CTRL1_MASK 	 0x01U
#define CXTP_A_TCTRL_CTRL1_POS  	 0U

#define VREF_CAP_EN_TCTRL_CTRL1_ADDR 	 0x11U // Controls RSVD pin connections  
#define VREF_CAP_EN_TCTRL_CTRL1_MASK 	 0x40U
#define VREF_CAP_EN_TCTRL_CTRL1_POS  	 6U

#define TCTRL_CTRL2_ADDR    	 0x12U
#define TCTRL_CTRL2_DEFAULT 	 0x04U

#define LDO_BYPASS_TCTRL_CTRL2_ADDR 	 0x12U // Enable LDO bypass  
#define LDO_BYPASS_TCTRL_CTRL2_MASK 	 0x10U
#define LDO_BYPASS_TCTRL_CTRL2_POS  	 4U

#define TCTRL_CTRL3_ADDR    	 0x13U
#define TCTRL_CTRL3_DEFAULT 	 0x10U

#define CMU_LOCKED_TCTRL_CTRL3_ADDR 	 0x13U // Clock Multiplier Unit (CMU) Locked  
#define CMU_LOCKED_TCTRL_CTRL3_MASK 	 0x02U
#define CMU_LOCKED_TCTRL_CTRL3_POS  	 1U

#define ERROR_TCTRL_CTRL3_ADDR 	 0x13U // Reflects global error status  
#define ERROR_TCTRL_CTRL3_MASK 	 0x04U
#define ERROR_TCTRL_CTRL3_POS  	 2U

#define LOCKED_TCTRL_CTRL3_ADDR 	 0x13U // GMSL Link Locked (bidirectional)  
#define LOCKED_TCTRL_CTRL3_MASK 	 0x08U
#define LOCKED_TCTRL_CTRL3_POS  	 3U

#define TCTRL_INTR0_ADDR    	 0x18U
#define TCTRL_INTR0_DEFAULT 	 0xA0U

#define DEC_ERR_THR_TCTRL_INTR0_ADDR 	 0x18U // Decoding and idle-error reporting thresh... 
#define DEC_ERR_THR_TCTRL_INTR0_MASK 	 0x07U
#define DEC_ERR_THR_TCTRL_INTR0_POS  	 0U

#define AUTO_ERR_RST_EN_TCTRL_INTR0_ADDR 	 0x18U // Automatically resets DEC_ERR_A (0x22) an... 
#define AUTO_ERR_RST_EN_TCTRL_INTR0_MASK 	 0x08U
#define AUTO_ERR_RST_EN_TCTRL_INTR0_POS  	 3U

#define TCTRL_INTR1_ADDR    	 0x19U
#define TCTRL_INTR1_DEFAULT 	 0x00U

#define AUTO_CNT_RST_EN_TCTRL_INTR1_ADDR 	 0x19U // Automatically reset PKT_CNT (0x25) bitfi... 
#define AUTO_CNT_RST_EN_TCTRL_INTR1_MASK 	 0x08U
#define AUTO_CNT_RST_EN_TCTRL_INTR1_POS  	 3U

#define PKT_CNT_EXP_TCTRL_INTR1_ADDR 	 0x19U // Packet Count Multiplier Exponent  
#define PKT_CNT_EXP_TCTRL_INTR1_MASK 	 0xF0U
#define PKT_CNT_EXP_TCTRL_INTR1_POS  	 4U

#define TCTRL_INTR2_ADDR    	 0x1AU
#define TCTRL_INTR2_DEFAULT 	 0x09U

#define DEC_ERR_OEN_A_TCTRL_INTR2_ADDR 	 0x1AU // Enable reporting of decoding errors (DEC... 
#define DEC_ERR_OEN_A_TCTRL_INTR2_MASK 	 0x01U
#define DEC_ERR_OEN_A_TCTRL_INTR2_POS  	 0U

#define IDLE_ERR_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enable reporting of idle-word errors (ID... 
#define IDLE_ERR_OEN_TCTRL_INTR2_MASK 	 0x04U
#define IDLE_ERR_OEN_TCTRL_INTR2_POS  	 2U

#define LFLT_INT_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enable reporting of line-fault interrupt... 
#define LFLT_INT_OEN_TCTRL_INTR2_MASK 	 0x08U
#define LFLT_INT_OEN_TCTRL_INTR2_POS  	 3U

#define REM_ERR_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enable reporting of remote error status ... 
#define REM_ERR_OEN_TCTRL_INTR2_MASK 	 0x20U
#define REM_ERR_OEN_TCTRL_INTR2_POS  	 5U

#define REFGEN_UNLOCKED_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enable reporting of reference clock DPLL... 
#define REFGEN_UNLOCKED_OEN_TCTRL_INTR2_MASK 	 0x80U
#define REFGEN_UNLOCKED_OEN_TCTRL_INTR2_POS  	 7U

#define TCTRL_INTR3_ADDR    	 0x1BU
#define TCTRL_INTR3_DEFAULT 	 0x00U

#define DEC_ERR_FLAG_A_TCTRL_INTR3_ADDR 	 0x1BU // GMSL Packet Decoding Error Flag  
#define DEC_ERR_FLAG_A_TCTRL_INTR3_MASK 	 0x01U
#define DEC_ERR_FLAG_A_TCTRL_INTR3_POS  	 0U

#define IDLE_ERR_FLAG_TCTRL_INTR3_ADDR 	 0x1BU // Idle-Word Error Flag  
#define IDLE_ERR_FLAG_TCTRL_INTR3_MASK 	 0x04U
#define IDLE_ERR_FLAG_TCTRL_INTR3_POS  	 2U

#define LFLT_INT_TCTRL_INTR3_ADDR 	 0x1BU // Line-Fault Interrupt  
#define LFLT_INT_TCTRL_INTR3_MASK 	 0x08U
#define LFLT_INT_TCTRL_INTR3_POS  	 3U

#define REM_ERR_FLAG_TCTRL_INTR3_ADDR 	 0x1BU // Received remote side error status (inver... 
#define REM_ERR_FLAG_TCTRL_INTR3_MASK 	 0x20U
#define REM_ERR_FLAG_TCTRL_INTR3_POS  	 5U

#define REFGEN_UNLOCKED_TCTRL_INTR3_ADDR 	 0x1BU // Reference DPLL generating RCLKOUT is not... 
#define REFGEN_UNLOCKED_TCTRL_INTR3_MASK 	 0x80U
#define REFGEN_UNLOCKED_TCTRL_INTR3_POS  	 7U

#define TCTRL_INTR4_ADDR    	 0x1CU
#define TCTRL_INTR4_DEFAULT 	 0x08U

#define PKT_CNT_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of packet count flag (P... 
#define PKT_CNT_OEN_TCTRL_INTR4_MASK 	 0x02U
#define PKT_CNT_OEN_TCTRL_INTR4_POS  	 1U

#define RT_CNT_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of combined ARQ retrans... 
#define RT_CNT_OEN_TCTRL_INTR4_MASK 	 0x04U
#define RT_CNT_OEN_TCTRL_INTR4_POS  	 2U

#define MAX_RT_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of combined ARQ maximum... 
#define MAX_RT_OEN_TCTRL_INTR4_MASK 	 0x08U
#define MAX_RT_OEN_TCTRL_INTR4_POS  	 3U

#define VDD18_OV_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable VDD18 overvoltage status on ERRB  
#define VDD18_OV_OEN_TCTRL_INTR4_MASK 	 0x10U
#define VDD18_OV_OEN_TCTRL_INTR4_POS  	 4U

#define VDD_OV_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable VDD overvoltage status on ERRB  
#define VDD_OV_OEN_TCTRL_INTR4_MASK 	 0x20U
#define VDD_OV_OEN_TCTRL_INTR4_POS  	 5U

#define EOM_ERR_OEN_A_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of eye-opening monitor ... 
#define EOM_ERR_OEN_A_TCTRL_INTR4_MASK 	 0x40U
#define EOM_ERR_OEN_A_TCTRL_INTR4_POS  	 6U

#define VREG_OV_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable CAP_VDD overvoltage status on ERR... 
#define VREG_OV_OEN_TCTRL_INTR4_MASK 	 0x80U
#define VREG_OV_OEN_TCTRL_INTR4_POS  	 7U

#define TCTRL_INTR5_ADDR    	 0x1DU
#define TCTRL_INTR5_DEFAULT 	 0x00U

#define PKT_CNT_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Packet Count Flag  
#define PKT_CNT_FLAG_TCTRL_INTR5_MASK 	 0x02U
#define PKT_CNT_FLAG_TCTRL_INTR5_POS  	 1U

#define RT_CNT_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Combined ARQ retransmission event flag  
#define RT_CNT_FLAG_TCTRL_INTR5_MASK 	 0x04U
#define RT_CNT_FLAG_TCTRL_INTR5_POS  	 2U

#define MAX_RT_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Combined ARQ maximum retransmission limi... 
#define MAX_RT_FLAG_TCTRL_INTR5_MASK 	 0x08U
#define MAX_RT_FLAG_TCTRL_INTR5_POS  	 3U

#define VDD18_OV_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // VDD18 Overvoltage Flag  
#define VDD18_OV_FLAG_TCTRL_INTR5_MASK 	 0x10U
#define VDD18_OV_FLAG_TCTRL_INTR5_POS  	 4U

#define VDD_OV_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // VDD Overvoltage Indication  
#define VDD_OV_FLAG_TCTRL_INTR5_MASK 	 0x20U
#define VDD_OV_FLAG_TCTRL_INTR5_POS  	 5U

#define EOM_ERR_FLAG_A_TCTRL_INTR5_ADDR 	 0x1DU // Eye-opening is below the configured thre... 
#define EOM_ERR_FLAG_A_TCTRL_INTR5_MASK 	 0x40U
#define EOM_ERR_FLAG_A_TCTRL_INTR5_POS  	 6U

#define VREG_OV_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // CAP_VDD Overvoltage Indication  
#define VREG_OV_FLAG_TCTRL_INTR5_MASK 	 0x80U
#define VREG_OV_FLAG_TCTRL_INTR5_POS  	 7U

#define TCTRL_INTR6_ADDR    	 0x1EU
#define TCTRL_INTR6_DEFAULT 	 0xFBU

#define MIPI_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of MIPI RX errors (MIPI... 
#define MIPI_ERR_OEN_TCTRL_INTR6_MASK 	 0x01U
#define MIPI_ERR_OEN_TCTRL_INTR6_POS  	 0U

#define ADC_INT_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of ADC interrupts (ADC_... 
#define ADC_INT_OEN_TCTRL_INTR6_MASK 	 0x04U
#define ADC_INT_OEN_TCTRL_INTR6_POS  	 2U

#define RTTN_CRC_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of CRC errors (RTTN_CRC... 
#define RTTN_CRC_ERR_OEN_TCTRL_INTR6_MASK 	 0x08U
#define RTTN_CRC_ERR_OEN_TCTRL_INTR6_POS  	 3U

#define EFUSE_CRC_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting efuse CRC at ERRB pin  
#define EFUSE_CRC_ERR_OEN_TCTRL_INTR6_MASK 	 0x10U
#define EFUSE_CRC_ERR_OEN_TCTRL_INTR6_POS  	 4U

#define VDDBAD_INT_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of VDDBAD interrupt (VD... 
#define VDDBAD_INT_OEN_TCTRL_INTR6_MASK 	 0x20U
#define VDDBAD_INT_OEN_TCTRL_INTR6_POS  	 5U

#define PORZ_INT_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of PORZ interrupt (PORZ... 
#define PORZ_INT_OEN_TCTRL_INTR6_MASK 	 0x40U
#define PORZ_INT_OEN_TCTRL_INTR6_POS  	 6U

#define VDDCMP_INT_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of combined undervoltag... 
#define VDDCMP_INT_OEN_TCTRL_INTR6_MASK 	 0x80U
#define VDDCMP_INT_OEN_TCTRL_INTR6_POS  	 7U

#define TCTRL_INTR7_ADDR    	 0x1FU
#define TCTRL_INTR7_DEFAULT 	 0x00U

#define MIPI_ERR_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // MIPI RX error flag, asserted when any of... 
#define MIPI_ERR_FLAG_TCTRL_INTR7_MASK 	 0x01U
#define MIPI_ERR_FLAG_TCTRL_INTR7_POS  	 0U

#define ADC_INT_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // ADC Interrupt. Individual interrupts als... 
#define ADC_INT_FLAG_TCTRL_INTR7_MASK 	 0x04U
#define ADC_INT_FLAG_TCTRL_INTR7_POS  	 2U

#define RTTN_CRC_INT_TCTRL_INTR7_ADDR 	 0x1FU // Retention memory restore CRC error inter... 
#define RTTN_CRC_INT_TCTRL_INTR7_MASK 	 0x08U
#define RTTN_CRC_INT_TCTRL_INTR7_POS  	 3U

#define EFUSE_CRC_ERR_TCTRL_INTR7_ADDR 	 0x1FU // efuse CRC error indicator  
#define EFUSE_CRC_ERR_TCTRL_INTR7_MASK 	 0x10U
#define EFUSE_CRC_ERR_TCTRL_INTR7_POS  	 4U

#define VDDBAD_INT_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // Combined VDD bad indicator. Asserts when... 
#define VDDBAD_INT_FLAG_TCTRL_INTR7_MASK 	 0x20U
#define VDDBAD_INT_FLAG_TCTRL_INTR7_POS  	 5U

#define PORZ_INT_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // PORZ interrupt flag. PORZ is monitoring ... 
#define PORZ_INT_FLAG_TCTRL_INTR7_MASK 	 0x40U
#define PORZ_INT_FLAG_TCTRL_INTR7_POS  	 6U

#define VDDCMP_INT_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // Combined undervoltage comparator output  
#define VDDCMP_INT_FLAG_TCTRL_INTR7_MASK 	 0x80U
#define VDDCMP_INT_FLAG_TCTRL_INTR7_POS  	 7U

#define TCTRL_INTR8_ADDR    	 0x20U
#define TCTRL_INTR8_DEFAULT 	 0x9FU

#define ERR_TX_ID_TCTRL_INTR8_ADDR 	 0x20U // GPIO ID used for transmitting ERR_TX. Er... 
#define ERR_TX_ID_TCTRL_INTR8_MASK 	 0x1FU
#define ERR_TX_ID_TCTRL_INTR8_POS  	 0U

#define ERR_TX_EN_TCTRL_INTR8_ADDR 	 0x20U // Transmit local error status (inverse of ... 
#define ERR_TX_EN_TCTRL_INTR8_MASK 	 0x80U
#define ERR_TX_EN_TCTRL_INTR8_POS  	 7U

#define TCTRL_INTR9_ADDR    	 0x21U
#define TCTRL_INTR9_DEFAULT 	 0xDFU

#define ERR_RX_ID_TCTRL_INTR9_ADDR 	 0x21U // GPIO ID used for receiving ERR_RX. Error... 
#define ERR_RX_ID_TCTRL_INTR9_MASK 	 0x1FU
#define ERR_RX_ID_TCTRL_INTR9_POS  	 0U

#define ERR_RX_EN_TCTRL_INTR9_ADDR 	 0x21U // Enable reception of remote error status ... 
#define ERR_RX_EN_TCTRL_INTR9_MASK 	 0x80U
#define ERR_RX_EN_TCTRL_INTR9_POS  	 7U

#define TCTRL_CNT0_ADDR    	 0x22U
#define TCTRL_CNT0_DEFAULT 	 0x00U

#define DEC_ERR_A_TCTRL_CNT0_ADDR 	 0x22U // Number of decoding (disparity) errors de... 
#define DEC_ERR_A_TCTRL_CNT0_MASK 	 0xFFU
#define DEC_ERR_A_TCTRL_CNT0_POS  	 0U

#define TCTRL_CNT2_ADDR    	 0x24U
#define TCTRL_CNT2_DEFAULT 	 0x00U

#define IDLE_ERR_TCTRL_CNT2_ADDR 	 0x24U // Number of idle-word errors detected.  
#define IDLE_ERR_TCTRL_CNT2_MASK 	 0xFFU
#define IDLE_ERR_TCTRL_CNT2_POS  	 0U

#define TCTRL_CNT3_ADDR    	 0x25U
#define TCTRL_CNT3_DEFAULT 	 0x00U

#define PKT_CNT_TCTRL_CNT3_ADDR 	 0x25U // Number of received packets of a selected... 
#define PKT_CNT_TCTRL_CNT3_MASK 	 0xFFU
#define PKT_CNT_TCTRL_CNT3_POS  	 0U

#define GMSL_TX0_ADDR    	 0x28U
#define GMSL_TX0_DEFAULT 	 0x60U

#define TX_FEC_EN_GMSL_TX0_ADDR 	 0x28U // Enable Forward-Error Correction (FEC) in... 
#define TX_FEC_EN_GMSL_TX0_MASK 	 0x02U
#define TX_FEC_EN_GMSL_TX0_POS  	 1U

#define GMSL_TX1_ADDR    	 0x29U
#define GMSL_TX1_DEFAULT 	 0x08U

#define DIS_ENC_GMSL_TX1_ADDR 	 0x29U // Disable 9b10b encoding  
#define DIS_ENC_GMSL_TX1_MASK 	 0x01U
#define DIS_ENC_GMSL_TX1_POS  	 0U

#define DIS_SCR_GMSL_TX1_ADDR 	 0x29U // Disable scrambler  
#define DIS_SCR_GMSL_TX1_MASK 	 0x02U
#define DIS_SCR_GMSL_TX1_POS  	 1U

#define TX_FEC_CRC_EN_GMSL_TX1_ADDR 	 0x29U // Enable CRC at each FEC block  
#define TX_FEC_CRC_EN_GMSL_TX1_MASK 	 0x08U
#define TX_FEC_CRC_EN_GMSL_TX1_POS  	 3U

#define ERRG_EN_A_GMSL_TX1_ADDR 	 0x29U // Error Generator Enable for GMSL Link. Er... 
#define ERRG_EN_A_GMSL_TX1_MASK 	 0x10U
#define ERRG_EN_A_GMSL_TX1_POS  	 4U

#define LINK_PRBS_GEN_GMSL_TX1_ADDR 	 0x29U // Enable link PRBS-7 generator  
#define LINK_PRBS_GEN_GMSL_TX1_MASK 	 0x80U
#define LINK_PRBS_GEN_GMSL_TX1_POS  	 7U

#define GMSL_TX2_ADDR    	 0x2AU
#define GMSL_TX2_DEFAULT 	 0x20U

#define ERRG_PER_GMSL_TX2_ADDR 	 0x2AU // Error generator error distribution selec... 
#define ERRG_PER_GMSL_TX2_MASK 	 0x01U
#define ERRG_PER_GMSL_TX2_POS  	 0U

#define ERRG_BURST_GMSL_TX2_ADDR 	 0x2AU // Error generator burst error length in bi... 
#define ERRG_BURST_GMSL_TX2_MASK 	 0x0EU
#define ERRG_BURST_GMSL_TX2_POS  	 1U

#define ERRG_RATE_GMSL_TX2_ADDR 	 0x2AU // Error generator average bit error rate  
#define ERRG_RATE_GMSL_TX2_MASK 	 0x30U
#define ERRG_RATE_GMSL_TX2_POS  	 4U

#define ERRG_CNT_GMSL_TX2_ADDR 	 0x2AU // Number of errors to be generated  
#define ERRG_CNT_GMSL_TX2_MASK 	 0xC0U
#define ERRG_CNT_GMSL_TX2_POS  	 6U

#define GMSL_TX3_ADDR    	 0x2BU
#define GMSL_TX3_DEFAULT 	 0x44U

#define TX_FEC_ACTIVE_GMSL_TX3_ADDR 	 0x2BU // Indicates FEC is active  
#define TX_FEC_ACTIVE_GMSL_TX3_MASK 	 0x20U
#define TX_FEC_ACTIVE_GMSL_TX3_POS  	 5U

#define GMSL_RX0_ADDR    	 0x2CU
#define GMSL_RX0_DEFAULT 	 0x00U

#define PKT_CNT_SEL_GMSL_RX0_ADDR 	 0x2CU // Select the type of received packets to c... 
#define PKT_CNT_SEL_GMSL_RX0_MASK 	 0x0FU
#define PKT_CNT_SEL_GMSL_RX0_POS  	 0U

#define PKT_CNT_LBW_GMSL_RX0_ADDR 	 0x2CU // Select the subtype of low-bandwidth pack... 
#define PKT_CNT_LBW_GMSL_RX0_MASK 	 0xC0U
#define PKT_CNT_LBW_GMSL_RX0_POS  	 6U

#define GMSL_RX1_ADDR    	 0x2DU
#define GMSL_RX1_DEFAULT 	 0x28U

#define LINK_PRBS_CHK_GMSL_RX1_ADDR 	 0x2DU // Enable link PRBS-7 checker  
#define LINK_PRBS_CHK_GMSL_RX1_MASK 	 0x80U
#define LINK_PRBS_CHK_GMSL_RX1_POS  	 7U

#define GMSL_GPIOA_ADDR    	 0x30U
#define GMSL_GPIOA_DEFAULT 	 0x41U

#define GPIO_FWD_CDLY_GMSL_GPIOA_ADDR 	 0x30U // Compensation delay multiplier for the fo... 
#define GPIO_FWD_CDLY_GMSL_GPIOA_MASK 	 0x3FU
#define GPIO_FWD_CDLY_GMSL_GPIOA_POS  	 0U

#define GMSL_GPIOB_ADDR    	 0x31U
#define GMSL_GPIOB_DEFAULT 	 0x88U

#define GPIO_REV_CDLY_GMSL_GPIOB_ADDR 	 0x31U // Compensation delay multiplier for the re... 
#define GPIO_REV_CDLY_GMSL_GPIOB_MASK 	 0x3FU
#define GPIO_REV_CDLY_GMSL_GPIOB_POS  	 0U

#define CC_I2C_0_ADDR    	 0x40U
#define CC_I2C_0_DEFAULT 	 0x26U

#define SLV_TO_CC_I2C_0_ADDR 	 0x40U // I2C-to-I2C slave timeout setting.  
#define SLV_TO_CC_I2C_0_MASK 	 0x07U
#define SLV_TO_CC_I2C_0_POS  	 0U

#define SLV_SH_CC_I2C_0_ADDR 	 0x40U // I2C-to-I2C slave-setup and hold-time set... 
#define SLV_SH_CC_I2C_0_MASK 	 0x30U
#define SLV_SH_CC_I2C_0_POS  	 4U

#define CC_I2C_1_ADDR    	 0x41U
#define CC_I2C_1_DEFAULT 	 0x56U

#define MST_TO_CC_I2C_1_ADDR 	 0x41U // I2C-to-I2C master timeout setting  
#define MST_TO_CC_I2C_1_MASK 	 0x07U
#define MST_TO_CC_I2C_1_POS  	 0U

#define MST_BT_CC_I2C_1_ADDR 	 0x41U // I2C-to-I2C master bit rate setting  
#define MST_BT_CC_I2C_1_MASK 	 0x70U
#define MST_BT_CC_I2C_1_POS  	 4U

#define CC_I2C_2_ADDR    	 0x42U
#define CC_I2C_2_DEFAULT 	 0x00U

#define SRC_A_CC_I2C_2_ADDR 	 0x42U // I2C address translator source A for main... 
#define SRC_A_CC_I2C_2_MASK 	 0xFEU
#define SRC_A_CC_I2C_2_POS  	 1U

#define CC_I2C_3_ADDR    	 0x43U
#define CC_I2C_3_DEFAULT 	 0x00U

#define DST_A_CC_I2C_3_ADDR 	 0x43U // I2C address translator destination A for... 
#define DST_A_CC_I2C_3_MASK 	 0xFEU
#define DST_A_CC_I2C_3_POS  	 1U

#define CC_I2C_4_ADDR    	 0x44U
#define CC_I2C_4_DEFAULT 	 0x00U

#define SRC_B_CC_I2C_4_ADDR 	 0x44U // I2C address translator source B for main... 
#define SRC_B_CC_I2C_4_MASK 	 0xFEU
#define SRC_B_CC_I2C_4_POS  	 1U

#define CC_I2C_5_ADDR    	 0x45U
#define CC_I2C_5_DEFAULT 	 0x00U

#define DST_B_CC_I2C_5_ADDR 	 0x45U // I2C address translator destination B for... 
#define DST_B_CC_I2C_5_MASK 	 0xFEU
#define DST_B_CC_I2C_5_POS  	 1U

#define CC_UART_0_ADDR    	 0x48U
#define CC_UART_0_DEFAULT 	 0x42U

#define BYPASS_EN_CC_UART_0_ADDR 	 0x48U // Enable UART soft-bypass mode.  
#define BYPASS_EN_CC_UART_0_MASK 	 0x01U
#define BYPASS_EN_CC_UART_0_POS  	 0U

#define BYPASS_TO_CC_UART_0_ADDR 	 0x48U // UART soft-bypass timeout duration  
#define BYPASS_TO_CC_UART_0_MASK 	 0x06U
#define BYPASS_TO_CC_UART_0_POS  	 1U

#define BYPASS_DIS_PAR_CC_UART_0_ADDR 	 0x48U // Selects whether or not to receive and se... 
#define BYPASS_DIS_PAR_CC_UART_0_MASK 	 0x08U
#define BYPASS_DIS_PAR_CC_UART_0_POS  	 3U

#define LOC_MS_EN_CC_UART_0_ADDR 	 0x48U // Enables UART bypass mode control by loca... 
#define LOC_MS_EN_CC_UART_0_MASK 	 0x10U
#define LOC_MS_EN_CC_UART_0_POS  	 4U

#define REM_MS_EN_CC_UART_0_ADDR 	 0x48U // Enables UART Bypass Mode Control by Remo... 
#define REM_MS_EN_CC_UART_0_MASK 	 0x20U
#define REM_MS_EN_CC_UART_0_POS  	 5U

#define CC_I2C_PT_0_ADDR    	 0x4CU
#define CC_I2C_PT_0_DEFAULT 	 0x26U

#define SLV_TO_PT_CC_I2C_PT_0_ADDR 	 0x4CU // Pass-through 1 and 2 I2C-to-I2C slave ti... 
#define SLV_TO_PT_CC_I2C_PT_0_MASK 	 0x07U
#define SLV_TO_PT_CC_I2C_PT_0_POS  	 0U

#define SLV_SH_PT_CC_I2C_PT_0_ADDR 	 0x4CU // Pass-through 1 and 2 I2C-to-I2C slave se... 
#define SLV_SH_PT_CC_I2C_PT_0_MASK 	 0x30U
#define SLV_SH_PT_CC_I2C_PT_0_POS  	 4U

#define CC_I2C_PT_1_ADDR    	 0x4DU
#define CC_I2C_PT_1_DEFAULT 	 0x56U

#define MST_TO_PT_CC_I2C_PT_1_ADDR 	 0x4DU // Pass-through 1 and 2 I2C-to-I2C master t... 
#define MST_TO_PT_CC_I2C_PT_1_MASK 	 0x07U
#define MST_TO_PT_CC_I2C_PT_1_POS  	 0U

#define MST_BT_PT_CC_I2C_PT_1_ADDR 	 0x4DU // Pass-through 1 and 2 I2C-to-I2C master b... 
#define MST_BT_PT_CC_I2C_PT_1_MASK 	 0x70U
#define MST_BT_PT_CC_I2C_PT_1_POS  	 4U

#define CC_UART_PT_0_ADDR    	 0x4FU
#define CC_UART_PT_0_DEFAULT 	 0x00U

#define DIS_PAR_1_CC_UART_PT_0_ADDR 	 0x4FU // Disable parity bit in pass-through UART ... 
#define DIS_PAR_1_CC_UART_PT_0_MASK 	 0x04U
#define DIS_PAR_1_CC_UART_PT_0_POS  	 2U

#define BITLEN_MAN_CFG_1_CC_UART_PT_0_ADDR 	 0x4FU // Use the custom UART bit rate (selected b... 
#define BITLEN_MAN_CFG_1_CC_UART_PT_0_MASK 	 0x08U
#define BITLEN_MAN_CFG_1_CC_UART_PT_0_POS  	 3U

#define DIS_PAR_2_CC_UART_PT_0_ADDR 	 0x4FU // Disable parity bit in pass-through UART ... 
#define DIS_PAR_2_CC_UART_PT_0_MASK 	 0x40U
#define DIS_PAR_2_CC_UART_PT_0_POS  	 6U

#define BITLEN_MAN_CFG_2_CC_UART_PT_0_ADDR 	 0x4FU // Use the custom UART bit rate (selected b... 
#define BITLEN_MAN_CFG_2_CC_UART_PT_0_MASK 	 0x80U
#define BITLEN_MAN_CFG_2_CC_UART_PT_0_POS  	 7U

#define CFGV_VIDEO_Z_TX0_ADDR    	 0x58U
#define CFGV_VIDEO_Z_TX0_DEFAULT 	 0x30U

#define TX_CRC_EN_CFGV_VIDEO_Z_TX0_ADDR 	 0x58U // Transmit CRC Enable  
#define TX_CRC_EN_CFGV_VIDEO_Z_TX0_MASK 	 0x80U
#define TX_CRC_EN_CFGV_VIDEO_Z_TX0_POS  	 7U

#define CFGV_VIDEO_Z_TX3_ADDR    	 0x5BU
#define CFGV_VIDEO_Z_TX3_DEFAULT 	 0x02U

#define TX_STR_SEL_CFGV_VIDEO_Z_TX3_ADDR 	 0x5BU // Stream ID used in packets transmitted fr... 
#define TX_STR_SEL_CFGV_VIDEO_Z_TX3_MASK 	 0x03U
#define TX_STR_SEL_CFGV_VIDEO_Z_TX3_POS  	 0U

#define CFGI_INFOFR_TR0_ADDR    	 0x78U
#define CFGI_INFOFR_TR0_DEFAULT 	 0xF0U

#define RX_CRC_EN_CFGI_INFOFR_TR0_ADDR 	 0x78U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGI_INFOFR_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGI_INFOFR_TR0_POS  	 6U

#define TX_CRC_EN_CFGI_INFOFR_TR0_ADDR 	 0x78U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGI_INFOFR_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGI_INFOFR_TR0_POS  	 7U

#define CFGI_INFOFR_TR3_ADDR    	 0x7BU
#define CFGI_INFOFR_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGI_INFOFR_TR3_ADDR 	 0x7BU // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGI_INFOFR_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGI_INFOFR_TR3_POS  	 0U

#define CFGI_INFOFR_TR4_ADDR    	 0x7CU
#define CFGI_INFOFR_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGI_INFOFR_TR4_ADDR 	 0x7CU // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGI_INFOFR_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGI_INFOFR_TR4_POS  	 0U

#define CFGL_SPI_TR0_ADDR    	 0x80U
#define CFGL_SPI_TR0_DEFAULT 	 0xF0U

#define RX_CRC_EN_CFGL_SPI_TR0_ADDR 	 0x80U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGL_SPI_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGL_SPI_TR0_POS  	 6U

#define TX_CRC_EN_CFGL_SPI_TR0_ADDR 	 0x80U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGL_SPI_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGL_SPI_TR0_POS  	 7U

#define CFGL_SPI_TR3_ADDR    	 0x83U
#define CFGL_SPI_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGL_SPI_TR3_ADDR 	 0x83U // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGL_SPI_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGL_SPI_TR3_POS  	 0U

#define CFGL_SPI_TR4_ADDR    	 0x84U
#define CFGL_SPI_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGL_SPI_TR4_ADDR 	 0x84U // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGL_SPI_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGL_SPI_TR4_POS  	 0U

#define CFGL_SPI_ARQ0_ADDR    	 0x85U
#define CFGL_SPI_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGL_SPI_ARQ0_ADDR 	 0x85U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGL_SPI_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGL_SPI_ARQ0_POS  	 2U

#define ARQ0_EN_CFGL_SPI_ARQ0_ADDR 	 0x85U // Enable ARQ. It is not recommended to cha... 
#define ARQ0_EN_CFGL_SPI_ARQ0_MASK 	 0x08U
#define ARQ0_EN_CFGL_SPI_ARQ0_POS  	 3U

#define CFGL_SPI_ARQ1_ADDR    	 0x86U
#define CFGL_SPI_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGL_SPI_ARQ1_ADDR 	 0x86U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGL_SPI_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGL_SPI_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGL_SPI_ARQ1_ADDR 	 0x86U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGL_SPI_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGL_SPI_ARQ1_POS  	 1U

#define CFGL_SPI_ARQ2_ADDR    	 0x87U
#define CFGL_SPI_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGL_SPI_ARQ2_ADDR 	 0x87U // Total retransmission count in this chann... 
#define RT_CNT_CFGL_SPI_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGL_SPI_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGL_SPI_ARQ2_ADDR 	 0x87U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGL_SPI_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGL_SPI_ARQ2_POS  	 7U

#define CFGL_GPIO_TR0_ADDR    	 0x90U
#define CFGL_GPIO_TR0_DEFAULT 	 0xF0U

#define RX_CRC_EN_CFGL_GPIO_TR0_ADDR 	 0x90U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGL_GPIO_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGL_GPIO_TR0_POS  	 6U

#define TX_CRC_EN_CFGL_GPIO_TR0_ADDR 	 0x90U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGL_GPIO_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGL_GPIO_TR0_POS  	 7U

#define CFGL_GPIO_TR3_ADDR    	 0x93U
#define CFGL_GPIO_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGL_GPIO_TR3_ADDR 	 0x93U // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGL_GPIO_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGL_GPIO_TR3_POS  	 0U

#define CFGL_GPIO_TR4_ADDR    	 0x94U
#define CFGL_GPIO_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGL_GPIO_TR4_ADDR 	 0x94U // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGL_GPIO_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGL_GPIO_TR4_POS  	 0U

#define CFGL_GPIO_ARQ0_ADDR    	 0x95U
#define CFGL_GPIO_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGL_GPIO_ARQ0_ADDR 	 0x95U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGL_GPIO_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGL_GPIO_ARQ0_POS  	 2U

#define ARQ0_EN_CFGL_GPIO_ARQ0_ADDR 	 0x95U // Enable ARQ. It is not recommended to cha... 
#define ARQ0_EN_CFGL_GPIO_ARQ0_MASK 	 0x08U
#define ARQ0_EN_CFGL_GPIO_ARQ0_POS  	 3U

#define CFGL_GPIO_ARQ1_ADDR    	 0x96U
#define CFGL_GPIO_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGL_GPIO_ARQ1_ADDR 	 0x96U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGL_GPIO_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGL_GPIO_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGL_GPIO_ARQ1_ADDR 	 0x96U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGL_GPIO_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGL_GPIO_ARQ1_POS  	 1U

#define CFGL_GPIO_ARQ2_ADDR    	 0x97U
#define CFGL_GPIO_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGL_GPIO_ARQ2_ADDR 	 0x97U // Total retransmission count in this chann... 
#define RT_CNT_CFGL_GPIO_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGL_GPIO_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGL_GPIO_ARQ2_ADDR 	 0x97U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGL_GPIO_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGL_GPIO_ARQ2_POS  	 7U

#define CFGL_IIC_X_TR0_ADDR    	 0xA0U
#define CFGL_IIC_X_TR0_DEFAULT 	 0xF0U

#define RX_CRC_EN_CFGL_IIC_X_TR0_ADDR 	 0xA0U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGL_IIC_X_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGL_IIC_X_TR0_POS  	 6U

#define TX_CRC_EN_CFGL_IIC_X_TR0_ADDR 	 0xA0U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGL_IIC_X_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGL_IIC_X_TR0_POS  	 7U

#define CFGL_IIC_X_TR3_ADDR    	 0xA3U
#define CFGL_IIC_X_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGL_IIC_X_TR3_ADDR 	 0xA3U // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGL_IIC_X_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGL_IIC_X_TR3_POS  	 0U

#define CFGL_IIC_X_TR4_ADDR    	 0xA4U
#define CFGL_IIC_X_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGL_IIC_X_TR4_ADDR 	 0xA4U // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGL_IIC_X_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGL_IIC_X_TR4_POS  	 0U

#define CFGL_IIC_X_ARQ0_ADDR    	 0xA5U
#define CFGL_IIC_X_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGL_IIC_X_ARQ0_ADDR 	 0xA5U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGL_IIC_X_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGL_IIC_X_ARQ0_POS  	 2U

#define ARQ0_EN_CFGL_IIC_X_ARQ0_ADDR 	 0xA5U // Enable ARQ. It is not recommended to cha... 
#define ARQ0_EN_CFGL_IIC_X_ARQ0_MASK 	 0x08U
#define ARQ0_EN_CFGL_IIC_X_ARQ0_POS  	 3U

#define CFGL_IIC_X_ARQ1_ADDR    	 0xA6U
#define CFGL_IIC_X_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGL_IIC_X_ARQ1_ADDR 	 0xA6U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGL_IIC_X_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGL_IIC_X_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGL_IIC_X_ARQ1_ADDR 	 0xA6U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGL_IIC_X_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGL_IIC_X_ARQ1_POS  	 1U

#define CFGL_IIC_X_ARQ2_ADDR    	 0xA7U
#define CFGL_IIC_X_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGL_IIC_X_ARQ2_ADDR 	 0xA7U // Total retransmission count in this chann... 
#define RT_CNT_CFGL_IIC_X_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGL_IIC_X_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGL_IIC_X_ARQ2_ADDR 	 0xA7U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGL_IIC_X_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGL_IIC_X_ARQ2_POS  	 7U

#define CFGL_IIC_Y_TR0_ADDR    	 0xA8U
#define CFGL_IIC_Y_TR0_DEFAULT 	 0xF0U

#define RX_CRC_EN_CFGL_IIC_Y_TR0_ADDR 	 0xA8U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGL_IIC_Y_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGL_IIC_Y_TR0_POS  	 6U

#define TX_CRC_EN_CFGL_IIC_Y_TR0_ADDR 	 0xA8U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGL_IIC_Y_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGL_IIC_Y_TR0_POS  	 7U

#define CFGL_IIC_Y_TR3_ADDR    	 0xABU
#define CFGL_IIC_Y_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGL_IIC_Y_TR3_ADDR 	 0xABU // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGL_IIC_Y_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGL_IIC_Y_TR3_POS  	 0U

#define CFGL_IIC_Y_TR4_ADDR    	 0xACU
#define CFGL_IIC_Y_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGL_IIC_Y_TR4_ADDR 	 0xACU // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGL_IIC_Y_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGL_IIC_Y_TR4_POS  	 0U

#define CFGL_IIC_Y_ARQ0_ADDR    	 0xADU
#define CFGL_IIC_Y_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGL_IIC_Y_ARQ0_ADDR 	 0xADU // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGL_IIC_Y_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGL_IIC_Y_ARQ0_POS  	 2U

#define ARQ0_EN_CFGL_IIC_Y_ARQ0_ADDR 	 0xADU // Enable ARQ. It is not recommended to cha... 
#define ARQ0_EN_CFGL_IIC_Y_ARQ0_MASK 	 0x08U
#define ARQ0_EN_CFGL_IIC_Y_ARQ0_POS  	 3U

#define CFGL_IIC_Y_ARQ1_ADDR    	 0xAEU
#define CFGL_IIC_Y_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGL_IIC_Y_ARQ1_ADDR 	 0xAEU // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGL_IIC_Y_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGL_IIC_Y_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGL_IIC_Y_ARQ1_ADDR 	 0xAEU // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGL_IIC_Y_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGL_IIC_Y_ARQ1_POS  	 1U

#define CFGL_IIC_Y_ARQ2_ADDR    	 0xAFU
#define CFGL_IIC_Y_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGL_IIC_Y_ARQ2_ADDR 	 0xAFU // Total retransmission count in this chann... 
#define RT_CNT_CFGL_IIC_Y_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGL_IIC_Y_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGL_IIC_Y_ARQ2_ADDR 	 0xAFU // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGL_IIC_Y_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGL_IIC_Y_ARQ2_POS  	 7U

#define VID_TX_Z_VIDEO_TX0_ADDR    	 0x110U
#define VID_TX_Z_VIDEO_TX0_DEFAULT 	 0x68U

#define CLKDET_BYP_VID_TX_Z_VIDEO_TX0_ADDR 	 0x110U // Bypass PCLK detector  
#define CLKDET_BYP_VID_TX_Z_VIDEO_TX0_MASK 	 0x04U
#define CLKDET_BYP_VID_TX_Z_VIDEO_TX0_POS  	 2U

#define AUTO_BPP_VID_TX_Z_VIDEO_TX0_ADDR 	 0x110U // Select bits per pixel (BPP) source. Set ... 
#define AUTO_BPP_VID_TX_Z_VIDEO_TX0_MASK 	 0x08U
#define AUTO_BPP_VID_TX_Z_VIDEO_TX0_POS  	 3U

#define ENC_MODE_VID_TX_Z_VIDEO_TX0_ADDR 	 0x110U // HS, VS, and DE Encoding mode. This Encod... 
#define ENC_MODE_VID_TX_Z_VIDEO_TX0_MASK 	 0x30U
#define ENC_MODE_VID_TX_Z_VIDEO_TX0_POS  	 4U

#define LINE_CRC_EN_VID_TX_Z_VIDEO_TX0_ADDR 	 0x110U // Line CRC Enable  
#define LINE_CRC_EN_VID_TX_Z_VIDEO_TX0_MASK 	 0x40U
#define LINE_CRC_EN_VID_TX_Z_VIDEO_TX0_POS  	 6U

#define LINE_CRC_SEL_VID_TX_Z_VIDEO_TX0_ADDR 	 0x110U // Line CRC checksum generation with DE or ... 
#define LINE_CRC_SEL_VID_TX_Z_VIDEO_TX0_MASK 	 0x80U
#define LINE_CRC_SEL_VID_TX_Z_VIDEO_TX0_POS  	 7U

#define VID_TX_Z_VIDEO_TX1_ADDR    	 0x111U
#define VID_TX_Z_VIDEO_TX1_DEFAULT 	 0x58U

#define BPP_VID_TX_Z_VIDEO_TX1_ADDR 	 0x111U // Color bits per pixel (RGB888 = 24)  
#define BPP_VID_TX_Z_VIDEO_TX1_MASK 	 0x3FU
#define BPP_VID_TX_Z_VIDEO_TX1_POS  	 0U

#define VID_TX_Z_VIDEO_TX2_ADDR    	 0x112U
#define VID_TX_Z_VIDEO_TX2_DEFAULT 	 0x0AU

#define LIM_HEART_VID_TX_Z_VIDEO_TX2_ADDR 	 0x112U // Disable heartbeat during blanking  
#define LIM_HEART_VID_TX_Z_VIDEO_TX2_MASK 	 0x04U
#define LIM_HEART_VID_TX_Z_VIDEO_TX2_POS  	 2U

#define FIFO_WARN_VID_TX_Z_VIDEO_TX2_ADDR 	 0x112U // VID_TX FIFO is more than half full, vide... 
#define FIFO_WARN_VID_TX_Z_VIDEO_TX2_MASK 	 0x10U
#define FIFO_WARN_VID_TX_Z_VIDEO_TX2_POS  	 4U

#define OVERFLOW_VID_TX_Z_VIDEO_TX2_ADDR 	 0x112U // VID_TX FIFO has overflowed, video input ... 
#define OVERFLOW_VID_TX_Z_VIDEO_TX2_MASK 	 0x20U
#define OVERFLOW_VID_TX_Z_VIDEO_TX2_POS  	 5U

#define DRIFT_ERR_VID_TX_Z_VIDEO_TX2_ADDR 	 0x112U // VID_TX PCLK drift error detected.  
#define DRIFT_ERR_VID_TX_Z_VIDEO_TX2_MASK 	 0x40U
#define DRIFT_ERR_VID_TX_Z_VIDEO_TX2_POS  	 6U

#define PCLKDET_VID_TX_Z_VIDEO_TX2_ADDR 	 0x112U // PCLK detected. This bit is asserted when... 
#define PCLKDET_VID_TX_Z_VIDEO_TX2_MASK 	 0x80U
#define PCLKDET_VID_TX_Z_VIDEO_TX2_POS  	 7U

#define SPI_SPI_0_ADDR    	 0x170U
#define SPI_SPI_0_DEFAULT 	 0x08U

#define SPI_EN_SPI_SPI_0_ADDR 	 0x170U // Enable SPI channel  
#define SPI_EN_SPI_SPI_0_MASK 	 0x01U
#define SPI_EN_SPI_SPI_0_POS  	 0U

#define MST_SLVN_SPI_SPI_0_ADDR 	 0x170U // Selects if SPI is master or slave  
#define MST_SLVN_SPI_SPI_0_MASK 	 0x02U
#define MST_SLVN_SPI_SPI_0_POS  	 1U

#define SPI_CC_EN_SPI_SPI_0_ADDR 	 0x170U // Enable control channel SPI bridge functi... 
#define SPI_CC_EN_SPI_SPI_0_MASK 	 0x04U
#define SPI_CC_EN_SPI_SPI_0_POS  	 2U

#define SPI_IGNR_ID_SPI_SPI_0_ADDR 	 0x170U // Selects if SPI should use or ignore head... 
#define SPI_IGNR_ID_SPI_SPI_0_MASK 	 0x08U
#define SPI_IGNR_ID_SPI_SPI_0_POS  	 3U

#define SPI_CC_TRG_ID_SPI_SPI_0_ADDR 	 0x170U // ID for GMSL header in SPI control-channe... 
#define SPI_CC_TRG_ID_SPI_SPI_0_MASK 	 0x30U
#define SPI_CC_TRG_ID_SPI_SPI_0_POS  	 4U

#define SPI_LOC_ID_SPI_SPI_0_ADDR 	 0x170U // Program to local ID if filtering packets... 
#define SPI_LOC_ID_SPI_SPI_0_MASK 	 0xC0U
#define SPI_LOC_ID_SPI_SPI_0_POS  	 6U

#define SPI_SPI_1_ADDR    	 0x171U
#define SPI_SPI_1_DEFAULT 	 0x1DU

#define SPI_BASE_PRIO_SPI_SPI_1_ADDR 	 0x171U // Starting GMSL Request Priority, advances... 
#define SPI_BASE_PRIO_SPI_SPI_1_MASK 	 0x03U
#define SPI_BASE_PRIO_SPI_SPI_1_POS  	 0U

#define SPI_LOC_N_SPI_SPI_1_ADDR 	 0x171U // Sets the packet size ((2N + 1) bytes) fo... 
#define SPI_LOC_N_SPI_SPI_1_MASK 	 0xFCU
#define SPI_LOC_N_SPI_SPI_1_POS  	 2U

#define SPI_SPI_2_ADDR    	 0x172U
#define SPI_SPI_2_DEFAULT 	 0x03U

#define SPIM_SS1_ACT_H_SPI_SPI_2_ADDR 	 0x172U // Sets the polarity for SS1 when the SPI i... 
#define SPIM_SS1_ACT_H_SPI_SPI_2_MASK 	 0x01U
#define SPIM_SS1_ACT_H_SPI_SPI_2_POS  	 0U

#define SPIM_SS2_ACT_H_SPI_SPI_2_ADDR 	 0x172U // Sets the polarity for SS2 when the SPI i... 
#define SPIM_SS2_ACT_H_SPI_SPI_2_MASK 	 0x02U
#define SPIM_SS2_ACT_H_SPI_SPI_2_POS  	 1U

#define SPI_MOD3_SPI_SPI_2_ADDR 	 0x172U // Selects SPI mode 0 or 3  
#define SPI_MOD3_SPI_SPI_2_MASK 	 0x04U
#define SPI_MOD3_SPI_SPI_2_POS  	 2U

#define SPI_MOD3_F_SPI_SPI_2_ADDR 	 0x172U // Allows the suppression of an extra SCLK ... 
#define SPI_MOD3_F_SPI_SPI_2_MASK 	 0x08U
#define SPI_MOD3_F_SPI_SPI_2_POS  	 3U

#define FULL_SCK_SETUP_SPI_SPI_2_ADDR 	 0x172U // Sample MISO after half or full SCLK peri... 
#define FULL_SCK_SETUP_SPI_SPI_2_MASK 	 0x10U
#define FULL_SCK_SETUP_SPI_SPI_2_POS  	 4U

#define REQ_HOLD_OFF_SPI_SPI_2_ADDR 	 0x172U // The SPI port transmits data across the G... 
#define REQ_HOLD_OFF_SPI_SPI_2_MASK 	 0xE0U
#define REQ_HOLD_OFF_SPI_SPI_2_POS  	 5U

#define SPI_SPI_3_ADDR    	 0x173U
#define SPI_SPI_3_DEFAULT 	 0x00U

#define SPIM_SS_DLY_CLKS_SPI_SPI_3_ADDR 	 0x173U // Number of 300MHz clock cycles to delay b... 
#define SPIM_SS_DLY_CLKS_SPI_SPI_3_MASK 	 0xFFU
#define SPIM_SS_DLY_CLKS_SPI_SPI_3_POS  	 0U

#define SPI_SPI_4_ADDR    	 0x174U
#define SPI_SPI_4_DEFAULT 	 0x00U

#define SPIM_SCK_LO_CLKS_SPI_SPI_4_ADDR 	 0x174U // Number of 300MHz clock cycles for SCK lo... 
#define SPIM_SCK_LO_CLKS_SPI_SPI_4_MASK 	 0xFFU
#define SPIM_SCK_LO_CLKS_SPI_SPI_4_POS  	 0U

#define SPI_SPI_5_ADDR    	 0x175U
#define SPI_SPI_5_DEFAULT 	 0x00U

#define SPIM_SCK_HI_CLKS_SPI_SPI_5_ADDR 	 0x175U // Number of 300MHz clock cycles for SCLK h... 
#define SPIM_SCK_HI_CLKS_SPI_SPI_5_MASK 	 0xFFU
#define SPIM_SCK_HI_CLKS_SPI_SPI_5_POS  	 0U

#define SPI_SPI_6_ADDR    	 0x176U
#define SPI_SPI_6_DEFAULT 	 0x00U

#define RWN_IO_EN_SPI_SPI_6_ADDR 	 0x176U // Enable MFP for use as RO input for contr... 
#define RWN_IO_EN_SPI_SPI_6_MASK 	 0x01U
#define RWN_IO_EN_SPI_SPI_6_POS  	 0U

#define BNE_IO_EN_SPI_SPI_6_ADDR 	 0x176U // Enable MFP for use as BNE output for SPI... 
#define BNE_IO_EN_SPI_SPI_6_MASK 	 0x02U
#define BNE_IO_EN_SPI_SPI_6_POS  	 1U

#define SS_IO_EN_1_SPI_SPI_6_ADDR 	 0x176U // Enable MFP for use as Slave Select 1 out... 
#define SS_IO_EN_1_SPI_SPI_6_MASK 	 0x04U
#define SS_IO_EN_1_SPI_SPI_6_POS  	 2U

#define SS_IO_EN_2_SPI_SPI_6_ADDR 	 0x176U // Enable MFP for use as Slave Select 2 out... 
#define SS_IO_EN_2_SPI_SPI_6_MASK 	 0x08U
#define SS_IO_EN_2_SPI_SPI_6_POS  	 3U

#define SPIS_RWN_SPI_SPI_6_ADDR 	 0x176U // Alternate GPU control register to use fo... 
#define SPIS_RWN_SPI_SPI_6_MASK 	 0x10U
#define SPIS_RWN_SPI_SPI_6_POS  	 4U

#define BNE_SPI_SPI_6_ADDR 	 0x176U // Alternate status register to use for BNE... 
#define BNE_SPI_SPI_6_MASK 	 0x20U
#define BNE_SPI_SPI_6_POS  	 5U

#define SPI_SPI_7_ADDR    	 0x177U
#define SPI_SPI_7_DEFAULT 	 0x00U

#define SPIS_BYTE_CNT_SPI_SPI_7_ADDR 	 0x177U // Number of SPI data bytes available for r... 
#define SPIS_BYTE_CNT_SPI_SPI_7_MASK 	 0x1FU
#define SPIS_BYTE_CNT_SPI_SPI_7_POS  	 0U

#define SPI_TX_OVRFLW_SPI_SPI_7_ADDR 	 0x177U // SPI Tx Buffer Overflow flag  
#define SPI_TX_OVRFLW_SPI_SPI_7_MASK 	 0x40U
#define SPI_TX_OVRFLW_SPI_SPI_7_POS  	 6U

#define SPI_RX_OVRFLW_SPI_SPI_7_ADDR 	 0x177U // SPI Rx Buffer Overflow Flag  
#define SPI_RX_OVRFLW_SPI_SPI_7_MASK 	 0x80U
#define SPI_RX_OVRFLW_SPI_SPI_7_POS  	 7U

#define SPI_SPI_8_ADDR    	 0x178U
#define SPI_SPI_8_DEFAULT 	 0x00U

#define REQ_HOLD_OFF_TO_SPI_SPI_8_ADDR 	 0x178U // Timeout delay (in 100nS increments) for ... 
#define REQ_HOLD_OFF_TO_SPI_SPI_8_MASK 	 0xFFU
#define REQ_HOLD_OFF_TO_SPI_SPI_8_POS  	 0U

#define VTX_Z_CROSS_0_ADDR    	 0x236U
#define VTX_Z_CROSS_0_DEFAULT 	 0x00U

#define CROSS0_VTX_Z_CROSS_0_ADDR 	 0x236U // Maps incoming bit position set by this f... 
#define CROSS0_VTX_Z_CROSS_0_MASK 	 0x1FU
#define CROSS0_VTX_Z_CROSS_0_POS  	 0U

#define CROSS0_F_VTX_Z_CROSS_0_ADDR 	 0x236U // Force outgoing bit 0 to 0. Applied befor... 
#define CROSS0_F_VTX_Z_CROSS_0_MASK 	 0x20U
#define CROSS0_F_VTX_Z_CROSS_0_POS  	 5U

#define CROSS0_I_VTX_Z_CROSS_0_ADDR 	 0x236U // Invert outgoing bit 0  
#define CROSS0_I_VTX_Z_CROSS_0_MASK 	 0x40U
#define CROSS0_I_VTX_Z_CROSS_0_POS  	 6U

#define VTX_Z_CROSS_1_ADDR    	 0x237U
#define VTX_Z_CROSS_1_DEFAULT 	 0x01U

#define CROSS1_VTX_Z_CROSS_1_ADDR 	 0x237U // Maps incoming bit position set by this f... 
#define CROSS1_VTX_Z_CROSS_1_MASK 	 0x1FU
#define CROSS1_VTX_Z_CROSS_1_POS  	 0U

#define CROSS1_F_VTX_Z_CROSS_1_ADDR 	 0x237U // Force outgoing bit 1 to 0. Applied befor... 
#define CROSS1_F_VTX_Z_CROSS_1_MASK 	 0x20U
#define CROSS1_F_VTX_Z_CROSS_1_POS  	 5U

#define CROSS1_I_VTX_Z_CROSS_1_ADDR 	 0x237U // Invert outgoing bit 1  
#define CROSS1_I_VTX_Z_CROSS_1_MASK 	 0x40U
#define CROSS1_I_VTX_Z_CROSS_1_POS  	 6U

#define VTX_Z_CROSS_2_ADDR    	 0x238U
#define VTX_Z_CROSS_2_DEFAULT 	 0x02U

#define CROSS2_VTX_Z_CROSS_2_ADDR 	 0x238U // Maps incoming bit position set by this f... 
#define CROSS2_VTX_Z_CROSS_2_MASK 	 0x1FU
#define CROSS2_VTX_Z_CROSS_2_POS  	 0U

#define CROSS2_F_VTX_Z_CROSS_2_ADDR 	 0x238U // Force outgoing bit 2 to 0. Applied befor... 
#define CROSS2_F_VTX_Z_CROSS_2_MASK 	 0x20U
#define CROSS2_F_VTX_Z_CROSS_2_POS  	 5U

#define CROSS2_I_VTX_Z_CROSS_2_ADDR 	 0x238U // Invert outgoing bit 2  
#define CROSS2_I_VTX_Z_CROSS_2_MASK 	 0x40U
#define CROSS2_I_VTX_Z_CROSS_2_POS  	 6U

#define VTX_Z_CROSS_3_ADDR    	 0x239U
#define VTX_Z_CROSS_3_DEFAULT 	 0x03U

#define CROSS3_VTX_Z_CROSS_3_ADDR 	 0x239U // Maps incoming bit position set by this f... 
#define CROSS3_VTX_Z_CROSS_3_MASK 	 0x1FU
#define CROSS3_VTX_Z_CROSS_3_POS  	 0U

#define CROSS3_F_VTX_Z_CROSS_3_ADDR 	 0x239U // Force outgoing bit 3 to 0. Applied befor... 
#define CROSS3_F_VTX_Z_CROSS_3_MASK 	 0x20U
#define CROSS3_F_VTX_Z_CROSS_3_POS  	 5U

#define CROSS3_I_VTX_Z_CROSS_3_ADDR 	 0x239U // Invert outgoing bit 3  
#define CROSS3_I_VTX_Z_CROSS_3_MASK 	 0x40U
#define CROSS3_I_VTX_Z_CROSS_3_POS  	 6U

#define VTX_Z_CROSS_4_ADDR    	 0x23AU
#define VTX_Z_CROSS_4_DEFAULT 	 0x04U

#define CROSS4_VTX_Z_CROSS_4_ADDR 	 0x23AU // Maps incoming bit position set by this f... 
#define CROSS4_VTX_Z_CROSS_4_MASK 	 0x1FU
#define CROSS4_VTX_Z_CROSS_4_POS  	 0U

#define CROSS4_F_VTX_Z_CROSS_4_ADDR 	 0x23AU // Force outgoing bit 4 to 0. Applied befor... 
#define CROSS4_F_VTX_Z_CROSS_4_MASK 	 0x20U
#define CROSS4_F_VTX_Z_CROSS_4_POS  	 5U

#define CROSS4_I_VTX_Z_CROSS_4_ADDR 	 0x23AU // Invert outgoing bit 4  
#define CROSS4_I_VTX_Z_CROSS_4_MASK 	 0x40U
#define CROSS4_I_VTX_Z_CROSS_4_POS  	 6U

#define VTX_Z_CROSS_5_ADDR    	 0x23BU
#define VTX_Z_CROSS_5_DEFAULT 	 0x05U

#define CROSS5_VTX_Z_CROSS_5_ADDR 	 0x23BU // Maps incoming bit position set by this f... 
#define CROSS5_VTX_Z_CROSS_5_MASK 	 0x1FU
#define CROSS5_VTX_Z_CROSS_5_POS  	 0U

#define CROSS5_F_VTX_Z_CROSS_5_ADDR 	 0x23BU // Force outgoing bit 5 to 0. Applied befor... 
#define CROSS5_F_VTX_Z_CROSS_5_MASK 	 0x20U
#define CROSS5_F_VTX_Z_CROSS_5_POS  	 5U

#define CROSS5_I_VTX_Z_CROSS_5_ADDR 	 0x23BU // Invert outgoing bit 5  
#define CROSS5_I_VTX_Z_CROSS_5_MASK 	 0x40U
#define CROSS5_I_VTX_Z_CROSS_5_POS  	 6U

#define VTX_Z_CROSS_6_ADDR    	 0x23CU
#define VTX_Z_CROSS_6_DEFAULT 	 0x06U

#define CROSS6_VTX_Z_CROSS_6_ADDR 	 0x23CU // Maps incoming bit position set by this f... 
#define CROSS6_VTX_Z_CROSS_6_MASK 	 0x1FU
#define CROSS6_VTX_Z_CROSS_6_POS  	 0U

#define CROSS6_F_VTX_Z_CROSS_6_ADDR 	 0x23CU // Force outgoing bit 6 to 0. Applied befor... 
#define CROSS6_F_VTX_Z_CROSS_6_MASK 	 0x20U
#define CROSS6_F_VTX_Z_CROSS_6_POS  	 5U

#define CROSS6_I_VTX_Z_CROSS_6_ADDR 	 0x23CU // Invert outgoing bit 6  
#define CROSS6_I_VTX_Z_CROSS_6_MASK 	 0x40U
#define CROSS6_I_VTX_Z_CROSS_6_POS  	 6U

#define VTX_Z_CROSS_7_ADDR    	 0x23DU
#define VTX_Z_CROSS_7_DEFAULT 	 0x07U

#define CROSS7_VTX_Z_CROSS_7_ADDR 	 0x23DU // Maps incoming bit position set by this f... 
#define CROSS7_VTX_Z_CROSS_7_MASK 	 0x1FU
#define CROSS7_VTX_Z_CROSS_7_POS  	 0U

#define CROSS7_F_VTX_Z_CROSS_7_ADDR 	 0x23DU // Force outgoing bit 7 to 0. Applied befor... 
#define CROSS7_F_VTX_Z_CROSS_7_MASK 	 0x20U
#define CROSS7_F_VTX_Z_CROSS_7_POS  	 5U

#define CROSS7_I_VTX_Z_CROSS_7_ADDR 	 0x23DU // Invert outgoing bit 7  
#define CROSS7_I_VTX_Z_CROSS_7_MASK 	 0x40U
#define CROSS7_I_VTX_Z_CROSS_7_POS  	 6U

#define VTX_Z_CROSS_8_ADDR    	 0x23EU
#define VTX_Z_CROSS_8_DEFAULT 	 0x08U

#define CROSS8_VTX_Z_CROSS_8_ADDR 	 0x23EU // Maps incoming bit position set by this f... 
#define CROSS8_VTX_Z_CROSS_8_MASK 	 0x1FU
#define CROSS8_VTX_Z_CROSS_8_POS  	 0U

#define CROSS8_F_VTX_Z_CROSS_8_ADDR 	 0x23EU // Force outgoing bit 8 to 0. Applied befor... 
#define CROSS8_F_VTX_Z_CROSS_8_MASK 	 0x20U
#define CROSS8_F_VTX_Z_CROSS_8_POS  	 5U

#define CROSS8_I_VTX_Z_CROSS_8_ADDR 	 0x23EU // Invert outgoing bit 8  
#define CROSS8_I_VTX_Z_CROSS_8_MASK 	 0x40U
#define CROSS8_I_VTX_Z_CROSS_8_POS  	 6U

#define VTX_Z_CROSS_9_ADDR    	 0x23FU
#define VTX_Z_CROSS_9_DEFAULT 	 0x09U

#define CROSS9_VTX_Z_CROSS_9_ADDR 	 0x23FU // Maps incoming bit position set by this f... 
#define CROSS9_VTX_Z_CROSS_9_MASK 	 0x1FU
#define CROSS9_VTX_Z_CROSS_9_POS  	 0U

#define CROSS9_F_VTX_Z_CROSS_9_ADDR 	 0x23FU // Force outgoing bit 9 to 0. Applied befor... 
#define CROSS9_F_VTX_Z_CROSS_9_MASK 	 0x20U
#define CROSS9_F_VTX_Z_CROSS_9_POS  	 5U

#define CROSS9_I_VTX_Z_CROSS_9_ADDR 	 0x23FU // Invert outgoing bit 9  
#define CROSS9_I_VTX_Z_CROSS_9_MASK 	 0x40U
#define CROSS9_I_VTX_Z_CROSS_9_POS  	 6U

#define VTX_Z_CROSS_10_ADDR    	 0x240U
#define VTX_Z_CROSS_10_DEFAULT 	 0x0AU

#define CROSS10_VTX_Z_CROSS_10_ADDR 	 0x240U // Maps incoming bit position set by this f... 
#define CROSS10_VTX_Z_CROSS_10_MASK 	 0x1FU
#define CROSS10_VTX_Z_CROSS_10_POS  	 0U

#define CROSS10_F_VTX_Z_CROSS_10_ADDR 	 0x240U // Force outgoing bit 10 to 0. Applied befo... 
#define CROSS10_F_VTX_Z_CROSS_10_MASK 	 0x20U
#define CROSS10_F_VTX_Z_CROSS_10_POS  	 5U

#define CROSS10_I_VTX_Z_CROSS_10_ADDR 	 0x240U // Invert outgoing bit 10  
#define CROSS10_I_VTX_Z_CROSS_10_MASK 	 0x40U
#define CROSS10_I_VTX_Z_CROSS_10_POS  	 6U

#define VTX_Z_CROSS_11_ADDR    	 0x241U
#define VTX_Z_CROSS_11_DEFAULT 	 0x0BU

#define CROSS11_VTX_Z_CROSS_11_ADDR 	 0x241U // Maps incoming bit position set by this f... 
#define CROSS11_VTX_Z_CROSS_11_MASK 	 0x1FU
#define CROSS11_VTX_Z_CROSS_11_POS  	 0U

#define CROSS11_F_VTX_Z_CROSS_11_ADDR 	 0x241U // Force outgoing bit 11 to 0. Applied befo... 
#define CROSS11_F_VTX_Z_CROSS_11_MASK 	 0x20U
#define CROSS11_F_VTX_Z_CROSS_11_POS  	 5U

#define CROSS11_I_VTX_Z_CROSS_11_ADDR 	 0x241U // Invert outgoing bit 11  
#define CROSS11_I_VTX_Z_CROSS_11_MASK 	 0x40U
#define CROSS11_I_VTX_Z_CROSS_11_POS  	 6U

#define VTX_Z_CROSS_12_ADDR    	 0x242U
#define VTX_Z_CROSS_12_DEFAULT 	 0x0CU

#define CROSS12_VTX_Z_CROSS_12_ADDR 	 0x242U // Maps incoming bit position set by this f... 
#define CROSS12_VTX_Z_CROSS_12_MASK 	 0x1FU
#define CROSS12_VTX_Z_CROSS_12_POS  	 0U

#define CROSS12_F_VTX_Z_CROSS_12_ADDR 	 0x242U // Force outgoing bit 12 to 0. Applied befo... 
#define CROSS12_F_VTX_Z_CROSS_12_MASK 	 0x20U
#define CROSS12_F_VTX_Z_CROSS_12_POS  	 5U

#define CROSS12_I_VTX_Z_CROSS_12_ADDR 	 0x242U // Invert outgoing bit 12  
#define CROSS12_I_VTX_Z_CROSS_12_MASK 	 0x40U
#define CROSS12_I_VTX_Z_CROSS_12_POS  	 6U

#define VTX_Z_CROSS_13_ADDR    	 0x243U
#define VTX_Z_CROSS_13_DEFAULT 	 0x0DU

#define CROSS13_VTX_Z_CROSS_13_ADDR 	 0x243U // Maps incoming bit position set by this f... 
#define CROSS13_VTX_Z_CROSS_13_MASK 	 0x1FU
#define CROSS13_VTX_Z_CROSS_13_POS  	 0U

#define CROSS13_F_VTX_Z_CROSS_13_ADDR 	 0x243U // Force outgoing bit 13 to 0. Applied befo... 
#define CROSS13_F_VTX_Z_CROSS_13_MASK 	 0x20U
#define CROSS13_F_VTX_Z_CROSS_13_POS  	 5U

#define CROSS13_I_VTX_Z_CROSS_13_ADDR 	 0x243U // Invert outgoing bit 13  
#define CROSS13_I_VTX_Z_CROSS_13_MASK 	 0x40U
#define CROSS13_I_VTX_Z_CROSS_13_POS  	 6U

#define VTX_Z_CROSS_14_ADDR    	 0x244U
#define VTX_Z_CROSS_14_DEFAULT 	 0x0EU

#define CROSS14_VTX_Z_CROSS_14_ADDR 	 0x244U // Maps incoming bit position set by this f... 
#define CROSS14_VTX_Z_CROSS_14_MASK 	 0x1FU
#define CROSS14_VTX_Z_CROSS_14_POS  	 0U

#define CROSS14_F_VTX_Z_CROSS_14_ADDR 	 0x244U // Force outgoing bit 14 to 0. Applied befo... 
#define CROSS14_F_VTX_Z_CROSS_14_MASK 	 0x20U
#define CROSS14_F_VTX_Z_CROSS_14_POS  	 5U

#define CROSS14_I_VTX_Z_CROSS_14_ADDR 	 0x244U // Invert outgoing bit 14  
#define CROSS14_I_VTX_Z_CROSS_14_MASK 	 0x40U
#define CROSS14_I_VTX_Z_CROSS_14_POS  	 6U

#define VTX_Z_CROSS_15_ADDR    	 0x245U
#define VTX_Z_CROSS_15_DEFAULT 	 0x0FU

#define CROSS15_VTX_Z_CROSS_15_ADDR 	 0x245U // Maps incoming bit position set by this f... 
#define CROSS15_VTX_Z_CROSS_15_MASK 	 0x1FU
#define CROSS15_VTX_Z_CROSS_15_POS  	 0U

#define CROSS15_F_VTX_Z_CROSS_15_ADDR 	 0x245U // Force outgoing bit 15 to 0. Applied befo... 
#define CROSS15_F_VTX_Z_CROSS_15_MASK 	 0x20U
#define CROSS15_F_VTX_Z_CROSS_15_POS  	 5U

#define CROSS15_I_VTX_Z_CROSS_15_ADDR 	 0x245U // Invert outgoing bit 15  
#define CROSS15_I_VTX_Z_CROSS_15_MASK 	 0x40U
#define CROSS15_I_VTX_Z_CROSS_15_POS  	 6U

#define VTX_Z_CROSS_16_ADDR    	 0x246U
#define VTX_Z_CROSS_16_DEFAULT 	 0x10U

#define CROSS16_VTX_Z_CROSS_16_ADDR 	 0x246U // Maps incoming bit position set by this f... 
#define CROSS16_VTX_Z_CROSS_16_MASK 	 0x1FU
#define CROSS16_VTX_Z_CROSS_16_POS  	 0U

#define CROSS16_F_VTX_Z_CROSS_16_ADDR 	 0x246U // Force outgoing bit 16 to 0. Applied befo... 
#define CROSS16_F_VTX_Z_CROSS_16_MASK 	 0x20U
#define CROSS16_F_VTX_Z_CROSS_16_POS  	 5U

#define CROSS16_I_VTX_Z_CROSS_16_ADDR 	 0x246U // Invert outgoing bit 16  
#define CROSS16_I_VTX_Z_CROSS_16_MASK 	 0x40U
#define CROSS16_I_VTX_Z_CROSS_16_POS  	 6U

#define VTX_Z_CROSS_17_ADDR    	 0x247U
#define VTX_Z_CROSS_17_DEFAULT 	 0x11U

#define CROSS17_VTX_Z_CROSS_17_ADDR 	 0x247U // Maps incoming bit position set by this f... 
#define CROSS17_VTX_Z_CROSS_17_MASK 	 0x1FU
#define CROSS17_VTX_Z_CROSS_17_POS  	 0U

#define CROSS17_F_VTX_Z_CROSS_17_ADDR 	 0x247U // Force outgoing bit 17 to 0. Applied befo... 
#define CROSS17_F_VTX_Z_CROSS_17_MASK 	 0x20U
#define CROSS17_F_VTX_Z_CROSS_17_POS  	 5U

#define CROSS17_I_VTX_Z_CROSS_17_ADDR 	 0x247U // Invert outgoing bit 17  
#define CROSS17_I_VTX_Z_CROSS_17_MASK 	 0x40U
#define CROSS17_I_VTX_Z_CROSS_17_POS  	 6U

#define VTX_Z_CROSS_18_ADDR    	 0x248U
#define VTX_Z_CROSS_18_DEFAULT 	 0x12U

#define CROSS18_VTX_Z_CROSS_18_ADDR 	 0x248U // Maps incoming bit position set by this f... 
#define CROSS18_VTX_Z_CROSS_18_MASK 	 0x1FU
#define CROSS18_VTX_Z_CROSS_18_POS  	 0U

#define CROSS18_F_VTX_Z_CROSS_18_ADDR 	 0x248U // Force outgoing bit 18 to 0. Applied befo... 
#define CROSS18_F_VTX_Z_CROSS_18_MASK 	 0x20U
#define CROSS18_F_VTX_Z_CROSS_18_POS  	 5U

#define CROSS18_I_VTX_Z_CROSS_18_ADDR 	 0x248U // Invert outgoing bit 18  
#define CROSS18_I_VTX_Z_CROSS_18_MASK 	 0x40U
#define CROSS18_I_VTX_Z_CROSS_18_POS  	 6U

#define VTX_Z_CROSS_19_ADDR    	 0x249U
#define VTX_Z_CROSS_19_DEFAULT 	 0x13U

#define CROSS19_VTX_Z_CROSS_19_ADDR 	 0x249U // Maps incoming bit position set by this f... 
#define CROSS19_VTX_Z_CROSS_19_MASK 	 0x1FU
#define CROSS19_VTX_Z_CROSS_19_POS  	 0U

#define CROSS19_F_VTX_Z_CROSS_19_ADDR 	 0x249U // Force outgoing bit 19 to 0. Applied befo... 
#define CROSS19_F_VTX_Z_CROSS_19_MASK 	 0x20U
#define CROSS19_F_VTX_Z_CROSS_19_POS  	 5U

#define CROSS19_I_VTX_Z_CROSS_19_ADDR 	 0x249U // Invert outgoing bit 19  
#define CROSS19_I_VTX_Z_CROSS_19_MASK 	 0x40U
#define CROSS19_I_VTX_Z_CROSS_19_POS  	 6U

#define VTX_Z_CROSS_20_ADDR    	 0x24AU
#define VTX_Z_CROSS_20_DEFAULT 	 0x14U

#define CROSS20_VTX_Z_CROSS_20_ADDR 	 0x24AU // Maps incoming bit position set by this f... 
#define CROSS20_VTX_Z_CROSS_20_MASK 	 0x1FU
#define CROSS20_VTX_Z_CROSS_20_POS  	 0U

#define CROSS20_F_VTX_Z_CROSS_20_ADDR 	 0x24AU // Force outgoing bit 20 to 0. Applied befo... 
#define CROSS20_F_VTX_Z_CROSS_20_MASK 	 0x20U
#define CROSS20_F_VTX_Z_CROSS_20_POS  	 5U

#define CROSS20_I_VTX_Z_CROSS_20_ADDR 	 0x24AU // Invert outgoing bit 20  
#define CROSS20_I_VTX_Z_CROSS_20_MASK 	 0x40U
#define CROSS20_I_VTX_Z_CROSS_20_POS  	 6U

#define VTX_Z_CROSS_21_ADDR    	 0x24BU
#define VTX_Z_CROSS_21_DEFAULT 	 0x15U

#define CROSS21_VTX_Z_CROSS_21_ADDR 	 0x24BU // Maps incoming bit position set by this f... 
#define CROSS21_VTX_Z_CROSS_21_MASK 	 0x1FU
#define CROSS21_VTX_Z_CROSS_21_POS  	 0U

#define CROSS21_F_VTX_Z_CROSS_21_ADDR 	 0x24BU // Force outgoing bit 21 to 0. Applied befo... 
#define CROSS21_F_VTX_Z_CROSS_21_MASK 	 0x20U
#define CROSS21_F_VTX_Z_CROSS_21_POS  	 5U

#define CROSS21_I_VTX_Z_CROSS_21_ADDR 	 0x24BU // Invert outgoing bit 21  
#define CROSS21_I_VTX_Z_CROSS_21_MASK 	 0x40U
#define CROSS21_I_VTX_Z_CROSS_21_POS  	 6U

#define VTX_Z_CROSS_22_ADDR    	 0x24CU
#define VTX_Z_CROSS_22_DEFAULT 	 0x16U

#define CROSS22_VTX_Z_CROSS_22_ADDR 	 0x24CU // Maps incoming bit position set by this f... 
#define CROSS22_VTX_Z_CROSS_22_MASK 	 0x1FU
#define CROSS22_VTX_Z_CROSS_22_POS  	 0U

#define CROSS22_F_VTX_Z_CROSS_22_ADDR 	 0x24CU // Force outgoing bit 22 to 0. Applied befo... 
#define CROSS22_F_VTX_Z_CROSS_22_MASK 	 0x20U
#define CROSS22_F_VTX_Z_CROSS_22_POS  	 5U

#define CROSS22_I_VTX_Z_CROSS_22_ADDR 	 0x24CU // Invert outgoing bit 22  
#define CROSS22_I_VTX_Z_CROSS_22_MASK 	 0x40U
#define CROSS22_I_VTX_Z_CROSS_22_POS  	 6U

#define VTX_Z_CROSS_23_ADDR    	 0x24DU
#define VTX_Z_CROSS_23_DEFAULT 	 0x17U

#define CROSS23_VTX_Z_CROSS_23_ADDR 	 0x24DU // Maps incoming bit position set by this f... 
#define CROSS23_VTX_Z_CROSS_23_MASK 	 0x1FU
#define CROSS23_VTX_Z_CROSS_23_POS  	 0U

#define CROSS23_F_VTX_Z_CROSS_23_ADDR 	 0x24DU // Force outgoing bit 23 to 0. Applied befo... 
#define CROSS23_F_VTX_Z_CROSS_23_MASK 	 0x20U
#define CROSS23_F_VTX_Z_CROSS_23_POS  	 5U

#define CROSS23_I_VTX_Z_CROSS_23_ADDR 	 0x24DU // Invert outgoing bit 23  
#define CROSS23_I_VTX_Z_CROSS_23_MASK 	 0x40U
#define CROSS23_I_VTX_Z_CROSS_23_POS  	 6U

#define VTX_Z_VTX0_ADDR    	 0x24EU
#define VTX_Z_VTX0_DEFAULT 	 0x03U

#define VTG_MODE_VTX_Z_VTX0_ADDR 	 0x24EU // Video interface timing-generation mode. ... 
#define VTG_MODE_VTX_Z_VTX0_MASK 	 0x03U
#define VTG_MODE_VTX_Z_VTX0_POS  	 0U

#define DE_INV_VTX_Z_VTX0_ADDR 	 0x24EU // Invert DE output of video-timing generat... 
#define DE_INV_VTX_Z_VTX0_MASK 	 0x04U
#define DE_INV_VTX_Z_VTX0_POS  	 2U

#define HS_INV_VTX_Z_VTX0_ADDR 	 0x24EU // Invert HSYNC output of video-timing gene... 
#define HS_INV_VTX_Z_VTX0_MASK 	 0x08U
#define HS_INV_VTX_Z_VTX0_POS  	 3U

#define VS_INV_VTX_Z_VTX0_ADDR 	 0x24EU // Invert VSYNC output of video-timing gene... 
#define VS_INV_VTX_Z_VTX0_MASK 	 0x10U
#define VS_INV_VTX_Z_VTX0_POS  	 4U

#define GEN_DE_VTX_Z_VTX0_ADDR 	 0x24EU // Enable to generate DE output according t... 
#define GEN_DE_VTX_Z_VTX0_MASK 	 0x20U
#define GEN_DE_VTX_Z_VTX0_POS  	 5U

#define GEN_HS_VTX_Z_VTX0_ADDR 	 0x24EU // Enable to generate HS output according t... 
#define GEN_HS_VTX_Z_VTX0_MASK 	 0x40U
#define GEN_HS_VTX_Z_VTX0_POS  	 6U

#define GEN_VS_VTX_Z_VTX0_ADDR 	 0x24EU // Enable to generate VS output according t... 
#define GEN_VS_VTX_Z_VTX0_MASK 	 0x80U
#define GEN_VS_VTX_Z_VTX0_POS  	 7U

#define VTX_Z_VTX1_ADDR    	 0x24FU
#define VTX_Z_VTX1_DEFAULT 	 0x01U

#define VS_TRIG_VTX_Z_VTX1_ADDR 	 0x24FU // Select VS trigger edge (positive vs. neg... 
#define VS_TRIG_VTX_Z_VTX1_MASK 	 0x01U
#define VS_TRIG_VTX_Z_VTX1_POS  	 0U

#define PATGEN_CLK_SRC_VTX_Z_VTX1_ADDR 	 0x24FU // Pattern generator clock source for video... 
#define PATGEN_CLK_SRC_VTX_Z_VTX1_MASK 	 0x0EU
#define PATGEN_CLK_SRC_VTX_Z_VTX1_POS  	 1U

#define PCLKDET_VTX_VTX_Z_VTX1_ADDR 	 0x24FU // PCLK detected. This bit is asserted when... 
#define PCLKDET_VTX_VTX_Z_VTX1_MASK 	 0x20U
#define PCLKDET_VTX_VTX_Z_VTX1_POS  	 5U

#define VTX_Z_VTX2_ADDR    	 0x250U
#define VTX_Z_VTX2_DEFAULT 	 0x00U

#define VS_DLY_2_VTX_Z_VTX2_ADDR 	 0x250U // VS delay in terms of PCLK cycles  
#define VS_DLY_2_VTX_Z_VTX2_MASK 	 0xFFU
#define VS_DLY_2_VTX_Z_VTX2_POS  	 0U

#define VTX_Z_VTX3_ADDR    	 0x251U
#define VTX_Z_VTX3_DEFAULT 	 0x00U

#define VS_DLY_1_VTX_Z_VTX3_ADDR 	 0x251U // VS delay in terms of PCLK cycles  
#define VS_DLY_1_VTX_Z_VTX3_MASK 	 0xFFU
#define VS_DLY_1_VTX_Z_VTX3_POS  	 0U

#define VTX_Z_VTX4_ADDR    	 0x252U
#define VTX_Z_VTX4_DEFAULT 	 0x00U

#define VS_DLY_0_VTX_Z_VTX4_ADDR 	 0x252U // VS delay in terms of PCLK cycles  
#define VS_DLY_0_VTX_Z_VTX4_MASK 	 0xFFU
#define VS_DLY_0_VTX_Z_VTX4_POS  	 0U

#define VTX_Z_VTX5_ADDR    	 0x253U
#define VTX_Z_VTX5_DEFAULT 	 0x00U

#define VS_HIGH_2_VTX_Z_VTX5_ADDR 	 0x253U // VS high period in terms of PCLK cycles (... 
#define VS_HIGH_2_VTX_Z_VTX5_MASK 	 0xFFU
#define VS_HIGH_2_VTX_Z_VTX5_POS  	 0U

#define VTX_Z_VTX6_ADDR    	 0x254U
#define VTX_Z_VTX6_DEFAULT 	 0x00U

#define VS_HIGH_1_VTX_Z_VTX6_ADDR 	 0x254U // VS high period in terms of PCLK cycles (... 
#define VS_HIGH_1_VTX_Z_VTX6_MASK 	 0xFFU
#define VS_HIGH_1_VTX_Z_VTX6_POS  	 0U

#define VTX_Z_VTX7_ADDR    	 0x255U
#define VTX_Z_VTX7_DEFAULT 	 0x00U

#define VS_HIGH_0_VTX_Z_VTX7_ADDR 	 0x255U // VS high period in terms of PCLK cycles (... 
#define VS_HIGH_0_VTX_Z_VTX7_MASK 	 0xFFU
#define VS_HIGH_0_VTX_Z_VTX7_POS  	 0U

#define VTX_Z_VTX8_ADDR    	 0x256U
#define VTX_Z_VTX8_DEFAULT 	 0x00U

#define VS_LOW_2_VTX_Z_VTX8_ADDR 	 0x256U // VS low period in terms of PCLK cycles (b... 
#define VS_LOW_2_VTX_Z_VTX8_MASK 	 0xFFU
#define VS_LOW_2_VTX_Z_VTX8_POS  	 0U

#define VTX_Z_VTX9_ADDR    	 0x257U
#define VTX_Z_VTX9_DEFAULT 	 0x00U

#define VS_LOW_1_VTX_Z_VTX9_ADDR 	 0x257U // VS low period in terms of PCLK cycles (b... 
#define VS_LOW_1_VTX_Z_VTX9_MASK 	 0xFFU
#define VS_LOW_1_VTX_Z_VTX9_POS  	 0U

#define VTX_Z_VTX10_ADDR    	 0x258U
#define VTX_Z_VTX10_DEFAULT 	 0x00U

#define VS_LOW_0_VTX_Z_VTX10_ADDR 	 0x258U // VS low period in terms of PCLK cycles (b... 
#define VS_LOW_0_VTX_Z_VTX10_MASK 	 0xFFU
#define VS_LOW_0_VTX_Z_VTX10_POS  	 0U

#define VTX_Z_VTX11_ADDR    	 0x259U
#define VTX_Z_VTX11_DEFAULT 	 0x00U

#define V2H_2_VTX_Z_VTX11_ADDR 	 0x259U // VS edge to the rising edge of the first ... 
#define V2H_2_VTX_Z_VTX11_MASK 	 0xFFU
#define V2H_2_VTX_Z_VTX11_POS  	 0U

#define VTX_Z_VTX12_ADDR    	 0x25AU
#define VTX_Z_VTX12_DEFAULT 	 0x00U

#define V2H_1_VTX_Z_VTX12_ADDR 	 0x25AU // VS edge to the rising edge of the first ... 
#define V2H_1_VTX_Z_VTX12_MASK 	 0xFFU
#define V2H_1_VTX_Z_VTX12_POS  	 0U

#define VTX_Z_VTX13_ADDR    	 0x25BU
#define VTX_Z_VTX13_DEFAULT 	 0x00U

#define V2H_0_VTX_Z_VTX13_ADDR 	 0x25BU // VS edge to the rising edge of the first ... 
#define V2H_0_VTX_Z_VTX13_MASK 	 0xFFU
#define V2H_0_VTX_Z_VTX13_POS  	 0U

#define VTX_Z_VTX14_ADDR    	 0x25CU
#define VTX_Z_VTX14_DEFAULT 	 0x00U

#define HS_HIGH_1_VTX_Z_VTX14_ADDR 	 0x25CU // HS high period in terms of PCLK cycles (... 
#define HS_HIGH_1_VTX_Z_VTX14_MASK 	 0xFFU
#define HS_HIGH_1_VTX_Z_VTX14_POS  	 0U

#define VTX_Z_VTX15_ADDR    	 0x25DU
#define VTX_Z_VTX15_DEFAULT 	 0x00U

#define HS_HIGH_0_VTX_Z_VTX15_ADDR 	 0x25DU // HS high period in terms of PCLK cycles (... 
#define HS_HIGH_0_VTX_Z_VTX15_MASK 	 0xFFU
#define HS_HIGH_0_VTX_Z_VTX15_POS  	 0U

#define VTX_Z_VTX16_ADDR    	 0x25EU
#define VTX_Z_VTX16_DEFAULT 	 0x00U

#define HS_LOW_1_VTX_Z_VTX16_ADDR 	 0x25EU // HS low period in terms of PCLK cycles (b... 
#define HS_LOW_1_VTX_Z_VTX16_MASK 	 0xFFU
#define HS_LOW_1_VTX_Z_VTX16_POS  	 0U

#define VTX_Z_VTX17_ADDR    	 0x25FU
#define VTX_Z_VTX17_DEFAULT 	 0x00U

#define HS_LOW_0_VTX_Z_VTX17_ADDR 	 0x25FU // HS low period in terms of PCLK cycles (b... 
#define HS_LOW_0_VTX_Z_VTX17_MASK 	 0xFFU
#define HS_LOW_0_VTX_Z_VTX17_POS  	 0U

#define VTX_Z_VTX18_ADDR    	 0x260U
#define VTX_Z_VTX18_DEFAULT 	 0x00U

#define HS_CNT_1_VTX_Z_VTX18_ADDR 	 0x260U // HS pulses per frame (bits [15:8])  
#define HS_CNT_1_VTX_Z_VTX18_MASK 	 0xFFU
#define HS_CNT_1_VTX_Z_VTX18_POS  	 0U

#define VTX_Z_VTX19_ADDR    	 0x261U
#define VTX_Z_VTX19_DEFAULT 	 0x00U

#define HS_CNT_0_VTX_Z_VTX19_ADDR 	 0x261U // HS pulses per frame (bits [7:0])  
#define HS_CNT_0_VTX_Z_VTX19_MASK 	 0xFFU
#define HS_CNT_0_VTX_Z_VTX19_POS  	 0U

#define VTX_Z_VTX20_ADDR    	 0x262U
#define VTX_Z_VTX20_DEFAULT 	 0x00U

#define V2D_2_VTX_Z_VTX20_ADDR 	 0x262U // VS edge to the rising edge of the first ... 
#define V2D_2_VTX_Z_VTX20_MASK 	 0xFFU
#define V2D_2_VTX_Z_VTX20_POS  	 0U

#define VTX_Z_VTX21_ADDR    	 0x263U
#define VTX_Z_VTX21_DEFAULT 	 0x00U

#define V2D_1_VTX_Z_VTX21_ADDR 	 0x263U // VS edge to the rising edge of the first ... 
#define V2D_1_VTX_Z_VTX21_MASK 	 0xFFU
#define V2D_1_VTX_Z_VTX21_POS  	 0U

#define VTX_Z_VTX22_ADDR    	 0x264U
#define VTX_Z_VTX22_DEFAULT 	 0x00U

#define V2D_0_VTX_Z_VTX22_ADDR 	 0x264U // VS edge to the rising edge of the first ... 
#define V2D_0_VTX_Z_VTX22_MASK 	 0xFFU
#define V2D_0_VTX_Z_VTX22_POS  	 0U

#define VTX_Z_VTX23_ADDR    	 0x265U
#define VTX_Z_VTX23_DEFAULT 	 0x00U

#define DE_HIGH_1_VTX_Z_VTX23_ADDR 	 0x265U // DE high period in terms of PCLK cycles (... 
#define DE_HIGH_1_VTX_Z_VTX23_MASK 	 0xFFU
#define DE_HIGH_1_VTX_Z_VTX23_POS  	 0U

#define VTX_Z_VTX24_ADDR    	 0x266U
#define VTX_Z_VTX24_DEFAULT 	 0x00U

#define DE_HIGH_0_VTX_Z_VTX24_ADDR 	 0x266U // DE high period in terms of PCLK cycles (... 
#define DE_HIGH_0_VTX_Z_VTX24_MASK 	 0xFFU
#define DE_HIGH_0_VTX_Z_VTX24_POS  	 0U

#define VTX_Z_VTX25_ADDR    	 0x267U
#define VTX_Z_VTX25_DEFAULT 	 0x00U

#define DE_LOW_1_VTX_Z_VTX25_ADDR 	 0x267U // DE low period in terms of PCLK cycles (b... 
#define DE_LOW_1_VTX_Z_VTX25_MASK 	 0xFFU
#define DE_LOW_1_VTX_Z_VTX25_POS  	 0U

#define VTX_Z_VTX26_ADDR    	 0x268U
#define VTX_Z_VTX26_DEFAULT 	 0x00U

#define DE_LOW_0_VTX_Z_VTX26_ADDR 	 0x268U // DE low period in terms of PCLK cycles (b... 
#define DE_LOW_0_VTX_Z_VTX26_MASK 	 0xFFU
#define DE_LOW_0_VTX_Z_VTX26_POS  	 0U

#define VTX_Z_VTX27_ADDR    	 0x269U
#define VTX_Z_VTX27_DEFAULT 	 0x00U

#define DE_CNT_1_VTX_Z_VTX27_ADDR 	 0x269U // Active lines per frame (DE pulses) (bits... 
#define DE_CNT_1_VTX_Z_VTX27_MASK 	 0xFFU
#define DE_CNT_1_VTX_Z_VTX27_POS  	 0U

#define VTX_Z_VTX28_ADDR    	 0x26AU
#define VTX_Z_VTX28_DEFAULT 	 0x00U

#define DE_CNT_0_VTX_Z_VTX28_ADDR 	 0x26AU // Active lines per frame (DE pulses) (bits... 
#define DE_CNT_0_VTX_Z_VTX28_MASK 	 0xFFU
#define DE_CNT_0_VTX_Z_VTX28_POS  	 0U

#define VTX_Z_VTX29_ADDR    	 0x26BU
#define VTX_Z_VTX29_DEFAULT 	 0x00U

#define PATGEN_MODE_VTX_Z_VTX29_ADDR 	 0x26BU // Pattern-generator mode  
#define PATGEN_MODE_VTX_Z_VTX29_MASK 	 0x03U
#define PATGEN_MODE_VTX_Z_VTX29_POS  	 0U

#define GRAD_MODE_VTX_Z_VTX29_ADDR 	 0x26BU // Gradient pattern-generator mode  
#define GRAD_MODE_VTX_Z_VTX29_MASK 	 0x04U
#define GRAD_MODE_VTX_Z_VTX29_POS  	 2U

#define VPRBS_FAIL_VTX_Z_VTX29_ADDR 	 0x26BU // Video PRBS check pass/fail  
#define VPRBS_FAIL_VTX_Z_VTX29_MASK 	 0x20U
#define VPRBS_FAIL_VTX_Z_VTX29_POS  	 5U

#define VID_PRBS_EN_VTX_Z_VTX29_ADDR 	 0x26BU // Enable video PRBS generator  
#define VID_PRBS_EN_VTX_Z_VTX29_MASK 	 0x80U
#define VID_PRBS_EN_VTX_Z_VTX29_POS  	 7U

#define VTX_Z_VTX30_ADDR    	 0x26CU
#define VTX_Z_VTX30_DEFAULT 	 0x04U

#define GRAD_INC_VTX_Z_VTX30_ADDR 	 0x26CU // Gradient mode increment amount (incremen... 
#define GRAD_INC_VTX_Z_VTX30_MASK 	 0xFFU
#define GRAD_INC_VTX_Z_VTX30_POS  	 0U

#define VTX_Z_VTX31_ADDR    	 0x26DU
#define VTX_Z_VTX31_DEFAULT 	 0x00U

#define CHKR_A_L_VTX_Z_VTX31_ADDR 	 0x26DU // Checkerboard Mode Color A Low Byte  
#define CHKR_A_L_VTX_Z_VTX31_MASK 	 0xFFU
#define CHKR_A_L_VTX_Z_VTX31_POS  	 0U

#define VTX_Z_VTX32_ADDR    	 0x26EU
#define VTX_Z_VTX32_DEFAULT 	 0x00U

#define CHKR_A_M_VTX_Z_VTX32_ADDR 	 0x26EU // Checkerboard Mode Color A Middle Byte  
#define CHKR_A_M_VTX_Z_VTX32_MASK 	 0xFFU
#define CHKR_A_M_VTX_Z_VTX32_POS  	 0U

#define VTX_Z_VTX33_ADDR    	 0x26FU
#define VTX_Z_VTX33_DEFAULT 	 0x00U

#define CHKR_A_H_VTX_Z_VTX33_ADDR 	 0x26FU // Checkerboard Mode Color A High Byte  
#define CHKR_A_H_VTX_Z_VTX33_MASK 	 0xFFU
#define CHKR_A_H_VTX_Z_VTX33_POS  	 0U

#define VTX_Z_VTX34_ADDR    	 0x270U
#define VTX_Z_VTX34_DEFAULT 	 0x00U

#define CHKR_B_L_VTX_Z_VTX34_ADDR 	 0x270U // Checkerboard Mode Color B Low Byte  
#define CHKR_B_L_VTX_Z_VTX34_MASK 	 0xFFU
#define CHKR_B_L_VTX_Z_VTX34_POS  	 0U

#define VTX_Z_VTX35_ADDR    	 0x271U
#define VTX_Z_VTX35_DEFAULT 	 0x00U

#define CHKR_B_M_VTX_Z_VTX35_ADDR 	 0x271U // Checkerboard Mode Color B Middle Byte  
#define CHKR_B_M_VTX_Z_VTX35_MASK 	 0xFFU
#define CHKR_B_M_VTX_Z_VTX35_POS  	 0U

#define VTX_Z_VTX36_ADDR    	 0x272U
#define VTX_Z_VTX36_DEFAULT 	 0x00U

#define CHKR_B_H_VTX_Z_VTX36_ADDR 	 0x272U // Checkerboard Mode Color B High Byte  
#define CHKR_B_H_VTX_Z_VTX36_MASK 	 0xFFU
#define CHKR_B_H_VTX_Z_VTX36_POS  	 0U

#define VTX_Z_VTX37_ADDR    	 0x273U
#define VTX_Z_VTX37_DEFAULT 	 0x00U

#define CHKR_RPT_A_VTX_Z_VTX37_ADDR 	 0x273U // Checkerboard Mode Color A: Dimension of ... 
#define CHKR_RPT_A_VTX_Z_VTX37_MASK 	 0xFFU
#define CHKR_RPT_A_VTX_Z_VTX37_POS  	 0U

#define VTX_Z_VTX38_ADDR    	 0x274U
#define VTX_Z_VTX38_DEFAULT 	 0x00U

#define CHKR_RPT_B_VTX_Z_VTX38_ADDR 	 0x274U // Checkerboard Mode Color B: Dimension of ... 
#define CHKR_RPT_B_VTX_Z_VTX38_MASK 	 0xFFU
#define CHKR_RPT_B_VTX_Z_VTX38_POS  	 0U

#define VTX_Z_VTX39_ADDR    	 0x275U
#define VTX_Z_VTX39_DEFAULT 	 0x00U

#define CHKR_ALT_VTX_Z_VTX39_ADDR 	 0x275U // Checkerboard Mode Alternate Line Count: ... 
#define CHKR_ALT_VTX_Z_VTX39_MASK 	 0xFFU
#define CHKR_ALT_VTX_Z_VTX39_POS  	 0U

#define VTX_Z_VTX40_ADDR    	 0x276U
#define VTX_Z_VTX40_DEFAULT 	 0x18U

#define CROSSHS_VTX_Z_VTX40_ADDR 	 0x276U // Map selected internal signal to HS  
#define CROSSHS_VTX_Z_VTX40_MASK 	 0x1FU
#define CROSSHS_VTX_Z_VTX40_POS  	 0U

#define CROSSHS_F_VTX_Z_VTX40_ADDR 	 0x276U // Force HS to 0. Applied before inversion ... 
#define CROSSHS_F_VTX_Z_VTX40_MASK 	 0x20U
#define CROSSHS_F_VTX_Z_VTX40_POS  	 5U

#define CROSSHS_I_VTX_Z_VTX40_ADDR 	 0x276U // Invert outgoing HS  
#define CROSSHS_I_VTX_Z_VTX40_MASK 	 0x40U
#define CROSSHS_I_VTX_Z_VTX40_POS  	 6U

#define VTX_Z_VTX41_ADDR    	 0x277U
#define VTX_Z_VTX41_DEFAULT 	 0x19U

#define CROSSVS_VTX_Z_VTX41_ADDR 	 0x277U // Map selected internal signal to VS  
#define CROSSVS_VTX_Z_VTX41_MASK 	 0x1FU
#define CROSSVS_VTX_Z_VTX41_POS  	 0U

#define CROSSVS_F_VTX_Z_VTX41_ADDR 	 0x277U // Force VS to 0. Applied before inversion ... 
#define CROSSVS_F_VTX_Z_VTX41_MASK 	 0x20U
#define CROSSVS_F_VTX_Z_VTX41_POS  	 5U

#define CROSSVS_I_VTX_Z_VTX41_ADDR 	 0x277U // Invert outgoing VS  
#define CROSSVS_I_VTX_Z_VTX41_MASK 	 0x40U
#define CROSSVS_I_VTX_Z_VTX41_POS  	 6U

#define VTX_Z_VTX42_ADDR    	 0x278U
#define VTX_Z_VTX42_DEFAULT 	 0x1AU

#define CROSSDE_VTX_Z_VTX42_ADDR 	 0x278U // Map selected internal signal to DE  
#define CROSSDE_VTX_Z_VTX42_MASK 	 0x1FU
#define CROSSDE_VTX_Z_VTX42_POS  	 0U

#define CROSSDE_F_VTX_Z_VTX42_ADDR 	 0x278U // Force DE to 0. Applied before inversion ... 
#define CROSSDE_F_VTX_Z_VTX42_MASK 	 0x20U
#define CROSSDE_F_VTX_Z_VTX42_POS  	 5U

#define CROSSDE_I_VTX_Z_VTX42_ADDR 	 0x278U // Invert outgoing DE  
#define CROSSDE_I_VTX_Z_VTX42_MASK 	 0x40U
#define CROSSDE_I_VTX_Z_VTX42_POS  	 6U

#define GPIO0_0_GPIO_A_ADDR    	 0x2BEU
#define GPIO0_0_GPIO_A_DEFAULT 	 0x99U

#define GPIO_OUT_DIS_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO0_0_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO0_0_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // GPIO Tx source control  
#define GPIO_TX_EN_GPIO0_0_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO0_0_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // GPIO out source control.  
#define GPIO_RX_EN_GPIO0_0_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO0_0_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO0_0_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO0_0_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO0_0_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO0_0_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO0_0_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO0_0_GPIO_A_POS  	 5U

#define RES_CFG_GPIO0_0_GPIO_A_ADDR 	 0x2BEU // Resistor pullup/pulldown strength  
#define RES_CFG_GPIO0_0_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO0_0_GPIO_A_POS  	 7U

#define GPIO0_0_GPIO_B_ADDR    	 0x2BFU
#define GPIO0_0_GPIO_B_DEFAULT 	 0xA0U

#define GPIO_TX_ID_GPIO0_0_GPIO_B_ADDR 	 0x2BFU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO0_0_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO0_0_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO0_0_GPIO_B_ADDR 	 0x2BFU // Driver type selection  
#define OUT_TYPE_GPIO0_0_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO0_0_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO0_0_GPIO_B_ADDR 	 0x2BFU // Buffer pullup/pulldown configuration  
#define PULL_UPDN_SEL_GPIO0_0_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO0_0_GPIO_B_POS  	 6U

#define GPIO0_0_GPIO_C_ADDR    	 0x2C0U
#define GPIO0_0_GPIO_C_DEFAULT 	 0x40U

#define GPIO_RX_ID_GPIO0_0_GPIO_C_ADDR 	 0x2C0U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO0_0_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO0_0_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO0_0_GPIO_C_ADDR 	 0x2C0U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO0_0_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO0_0_GPIO_C_POS  	 7U

#define GPIO1_1_GPIO_A_ADDR    	 0x2C1U
#define GPIO1_1_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO1_1_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO1_1_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // GPIO Tx source control  
#define GPIO_TX_EN_GPIO1_1_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO1_1_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // GPIO out source control.  
#define GPIO_RX_EN_GPIO1_1_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO1_1_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO1_1_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO1_1_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO1_1_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO1_1_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO1_1_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO1_1_GPIO_A_POS  	 5U

#define RES_CFG_GPIO1_1_GPIO_A_ADDR 	 0x2C1U // Resistor Pullup/Pulldown Strength  
#define RES_CFG_GPIO1_1_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO1_1_GPIO_A_POS  	 7U

#define GPIO1_1_GPIO_B_ADDR    	 0x2C2U
#define GPIO1_1_GPIO_B_DEFAULT 	 0x21U

#define GPIO_TX_ID_GPIO1_1_GPIO_B_ADDR 	 0x2C2U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO1_1_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO1_1_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO1_1_GPIO_B_ADDR 	 0x2C2U // Driver type selection  
#define OUT_TYPE_GPIO1_1_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO1_1_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO1_1_GPIO_B_ADDR 	 0x2C2U // Buffer pullup/pulldown configuration  
#define PULL_UPDN_SEL_GPIO1_1_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO1_1_GPIO_B_POS  	 6U

#define GPIO1_1_GPIO_C_ADDR    	 0x2C3U
#define GPIO1_1_GPIO_C_DEFAULT 	 0x41U

#define GPIO_RX_ID_GPIO1_1_GPIO_C_ADDR 	 0x2C3U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO1_1_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO1_1_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO1_1_GPIO_C_ADDR 	 0x2C3U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO1_1_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO1_1_GPIO_C_POS  	 7U

#define GPIO2_2_GPIO_A_ADDR    	 0x2C4U
#define GPIO2_2_GPIO_A_DEFAULT 	 0x99U

#define GPIO_OUT_DIS_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO2_2_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO2_2_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // GPIO Tx source control  
#define GPIO_TX_EN_GPIO2_2_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO2_2_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // GPIO out source control.  
#define GPIO_RX_EN_GPIO2_2_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO2_2_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO2_2_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO2_2_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO2_2_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO2_2_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO2_2_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO2_2_GPIO_A_POS  	 5U

#define RES_CFG_GPIO2_2_GPIO_A_ADDR 	 0x2C4U // Resistor pullup/pulldown strength  
#define RES_CFG_GPIO2_2_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO2_2_GPIO_A_POS  	 7U

#define GPIO2_2_GPIO_B_ADDR    	 0x2C5U
#define GPIO2_2_GPIO_B_DEFAULT 	 0x22U

#define GPIO_TX_ID_GPIO2_2_GPIO_B_ADDR 	 0x2C5U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO2_2_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO2_2_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO2_2_GPIO_B_ADDR 	 0x2C5U // Driver type selection  
#define OUT_TYPE_GPIO2_2_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO2_2_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO2_2_GPIO_B_ADDR 	 0x2C5U // Buffer pullup/pulldown configuration  
#define PULL_UPDN_SEL_GPIO2_2_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO2_2_GPIO_B_POS  	 6U

#define GPIO2_2_GPIO_C_ADDR    	 0x2C6U
#define GPIO2_2_GPIO_C_DEFAULT 	 0x42U

#define GPIO_RX_ID_GPIO2_2_GPIO_C_ADDR 	 0x2C6U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO2_2_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO2_2_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO2_2_GPIO_C_ADDR 	 0x2C6U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO2_2_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO2_2_GPIO_C_POS  	 7U

#define GPIO3_3_GPIO_A_ADDR    	 0x2C7U
#define GPIO3_3_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO3_3_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO3_3_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // GPIO Tx source control  
#define GPIO_TX_EN_GPIO3_3_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO3_3_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // GPIO out source control.  
#define GPIO_RX_EN_GPIO3_3_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO3_3_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO3_3_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO3_3_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO3_3_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO3_3_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO3_3_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO3_3_GPIO_A_POS  	 5U

#define RES_CFG_GPIO3_3_GPIO_A_ADDR 	 0x2C7U // Resistor pullup/pulldown strength  
#define RES_CFG_GPIO3_3_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO3_3_GPIO_A_POS  	 7U

#define GPIO3_3_GPIO_B_ADDR    	 0x2C8U
#define GPIO3_3_GPIO_B_DEFAULT 	 0xA3U

#define GPIO_TX_ID_GPIO3_3_GPIO_B_ADDR 	 0x2C8U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO3_3_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO3_3_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO3_3_GPIO_B_ADDR 	 0x2C8U // Driver type selection  
#define OUT_TYPE_GPIO3_3_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO3_3_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO3_3_GPIO_B_ADDR 	 0x2C8U // Buffer pullup/pulldown configuration  
#define PULL_UPDN_SEL_GPIO3_3_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO3_3_GPIO_B_POS  	 6U

#define GPIO3_3_GPIO_C_ADDR    	 0x2C9U
#define GPIO3_3_GPIO_C_DEFAULT 	 0x43U

#define GPIO_RX_ID_GPIO3_3_GPIO_C_ADDR 	 0x2C9U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO3_3_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO3_3_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO3_3_GPIO_C_ADDR 	 0x2C9U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO3_3_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO3_3_GPIO_C_POS  	 7U

#define GPIO4_4_GPIO_A_ADDR    	 0x2CAU
#define GPIO4_4_GPIO_A_DEFAULT 	 0x99U

#define GPIO_OUT_DIS_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO4_4_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO4_4_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // GPIO Tx source control  
#define GPIO_TX_EN_GPIO4_4_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO4_4_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // GPIO out source control.  
#define GPIO_RX_EN_GPIO4_4_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO4_4_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO4_4_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO4_4_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO4_4_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO4_4_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO4_4_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO4_4_GPIO_A_POS  	 5U

#define RES_CFG_GPIO4_4_GPIO_A_ADDR 	 0x2CAU // Resistor pullup/pulldown strength  
#define RES_CFG_GPIO4_4_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO4_4_GPIO_A_POS  	 7U

#define GPIO4_4_GPIO_B_ADDR    	 0x2CBU
#define GPIO4_4_GPIO_B_DEFAULT 	 0xA4U

#define GPIO_TX_ID_GPIO4_4_GPIO_B_ADDR 	 0x2CBU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO4_4_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO4_4_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO4_4_GPIO_B_ADDR 	 0x2CBU // Driver type selection  
#define OUT_TYPE_GPIO4_4_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO4_4_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO4_4_GPIO_B_ADDR 	 0x2CBU // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO4_4_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO4_4_GPIO_B_POS  	 6U

#define GPIO4_4_GPIO_C_ADDR    	 0x2CCU
#define GPIO4_4_GPIO_C_DEFAULT 	 0x44U

#define GPIO_RX_ID_GPIO4_4_GPIO_C_ADDR 	 0x2CCU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO4_4_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO4_4_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO4_4_GPIO_C_ADDR 	 0x2CCU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO4_4_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO4_4_GPIO_C_POS  	 7U

#define GPIO5_5_GPIO_A_ADDR    	 0x2CDU
#define GPIO5_5_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO5_5_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO5_5_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // GPIO TX source control  
#define GPIO_TX_EN_GPIO5_5_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO5_5_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // GPIO out source control.  
#define GPIO_RX_EN_GPIO5_5_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO5_5_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO5_5_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO5_5_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO5_5_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO5_5_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO5_5_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO5_5_GPIO_A_POS  	 5U

#define RES_CFG_GPIO5_5_GPIO_A_ADDR 	 0x2CDU // Resistor pull-up/pull-down strength  
#define RES_CFG_GPIO5_5_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO5_5_GPIO_A_POS  	 7U

#define GPIO5_5_GPIO_B_ADDR    	 0x2CEU
#define GPIO5_5_GPIO_B_DEFAULT 	 0xA5U

#define GPIO_TX_ID_GPIO5_5_GPIO_B_ADDR 	 0x2CEU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO5_5_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO5_5_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO5_5_GPIO_B_ADDR 	 0x2CEU // Driver type selection  
#define OUT_TYPE_GPIO5_5_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO5_5_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO5_5_GPIO_B_ADDR 	 0x2CEU // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO5_5_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO5_5_GPIO_B_POS  	 6U

#define GPIO5_5_GPIO_C_ADDR    	 0x2CFU
#define GPIO5_5_GPIO_C_DEFAULT 	 0x45U

#define GPIO_RX_ID_GPIO5_5_GPIO_C_ADDR 	 0x2CFU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO5_5_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO5_5_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO5_5_GPIO_C_ADDR 	 0x2CFU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO5_5_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO5_5_GPIO_C_POS  	 7U

#define GPIO6_6_GPIO_A_ADDR    	 0x2D0U
#define GPIO6_6_GPIO_A_DEFAULT 	 0x99U

#define GPIO_OUT_DIS_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO6_6_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO6_6_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // GPIO TX source control  
#define GPIO_TX_EN_GPIO6_6_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO6_6_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // GPIO out source control.  
#define GPIO_RX_EN_GPIO6_6_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO6_6_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO6_6_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO6_6_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO6_6_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO6_6_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO6_6_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO6_6_GPIO_A_POS  	 5U

#define RES_CFG_GPIO6_6_GPIO_A_ADDR 	 0x2D0U // Resistor pull-up/pull-down strength  
#define RES_CFG_GPIO6_6_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO6_6_GPIO_A_POS  	 7U

#define GPIO6_6_GPIO_B_ADDR    	 0x2D1U
#define GPIO6_6_GPIO_B_DEFAULT 	 0xA6U

#define GPIO_TX_ID_GPIO6_6_GPIO_B_ADDR 	 0x2D1U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO6_6_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO6_6_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO6_6_GPIO_B_ADDR 	 0x2D1U // Driver type selection  
#define OUT_TYPE_GPIO6_6_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO6_6_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO6_6_GPIO_B_ADDR 	 0x2D1U // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO6_6_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO6_6_GPIO_B_POS  	 6U

#define GPIO6_6_GPIO_C_ADDR    	 0x2D2U
#define GPIO6_6_GPIO_C_DEFAULT 	 0x46U

#define GPIO_RX_ID_GPIO6_6_GPIO_C_ADDR 	 0x2D2U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO6_6_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO6_6_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO6_6_GPIO_C_ADDR 	 0x2D2U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO6_6_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO6_6_GPIO_C_POS  	 7U

#define GPIO7_7_GPIO_A_ADDR    	 0x2D3U
#define GPIO7_7_GPIO_A_DEFAULT 	 0x83U

#define GPIO_OUT_DIS_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO7_7_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO7_7_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // GPIO TX source control  
#define GPIO_TX_EN_GPIO7_7_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO7_7_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // GPIO out source control.  
#define GPIO_RX_EN_GPIO7_7_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO7_7_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO7_7_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO7_7_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO7_7_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO7_7_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO7_7_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO7_7_GPIO_A_POS  	 5U

#define RES_CFG_GPIO7_7_GPIO_A_ADDR 	 0x2D3U // Resistor pull-up/pull-down strength  
#define RES_CFG_GPIO7_7_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO7_7_GPIO_A_POS  	 7U

#define GPIO7_7_GPIO_B_ADDR    	 0x2D4U
#define GPIO7_7_GPIO_B_DEFAULT 	 0xA7U

#define GPIO_TX_ID_GPIO7_7_GPIO_B_ADDR 	 0x2D4U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO7_7_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO7_7_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO7_7_GPIO_B_ADDR 	 0x2D4U // Driver type selection  
#define OUT_TYPE_GPIO7_7_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO7_7_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO7_7_GPIO_B_ADDR 	 0x2D4U // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO7_7_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO7_7_GPIO_B_POS  	 6U

#define GPIO7_7_GPIO_C_ADDR    	 0x2D5U
#define GPIO7_7_GPIO_C_DEFAULT 	 0x47U

#define GPIO_RX_ID_GPIO7_7_GPIO_C_ADDR 	 0x2D5U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO7_7_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO7_7_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO7_7_GPIO_C_ADDR 	 0x2D5U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO7_7_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO7_7_GPIO_C_POS  	 7U

#define GPIO8_8_GPIO_A_ADDR    	 0x2D6U
#define GPIO8_8_GPIO_A_DEFAULT 	 0x9CU

#define GPIO_OUT_DIS_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO8_8_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO8_8_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // GPIO TX source control  
#define GPIO_TX_EN_GPIO8_8_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO8_8_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // GPIO out source control.  
#define GPIO_RX_EN_GPIO8_8_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO8_8_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO8_8_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO8_8_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO8_8_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO8_8_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO8_8_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO8_8_GPIO_A_POS  	 5U

#define RES_CFG_GPIO8_8_GPIO_A_ADDR 	 0x2D6U // Resistor pull-up/pull-down strength  
#define RES_CFG_GPIO8_8_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO8_8_GPIO_A_POS  	 7U

#define GPIO8_8_GPIO_B_ADDR    	 0x2D7U
#define GPIO8_8_GPIO_B_DEFAULT 	 0x28U

#define GPIO_TX_ID_GPIO8_8_GPIO_B_ADDR 	 0x2D7U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO8_8_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO8_8_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO8_8_GPIO_B_ADDR 	 0x2D7U // Driver type selection  
#define OUT_TYPE_GPIO8_8_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO8_8_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO8_8_GPIO_B_ADDR 	 0x2D7U // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO8_8_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO8_8_GPIO_B_POS  	 6U

#define GPIO8_8_GPIO_C_ADDR    	 0x2D8U
#define GPIO8_8_GPIO_C_DEFAULT 	 0x48U

#define GPIO_RX_ID_GPIO8_8_GPIO_C_ADDR 	 0x2D8U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO8_8_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO8_8_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO8_8_GPIO_C_ADDR 	 0x2D8U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO8_8_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO8_8_GPIO_C_POS  	 7U

#define GPIO9_9_GPIO_A_ADDR    	 0x2D9U
#define GPIO9_9_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO9_9_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO9_9_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // GPIO TX source control  
#define GPIO_TX_EN_GPIO9_9_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO9_9_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // GPIO out source control.  
#define GPIO_RX_EN_GPIO9_9_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO9_9_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO9_9_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO9_9_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO9_9_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO9_9_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO9_9_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO9_9_GPIO_A_POS  	 5U

#define RES_CFG_GPIO9_9_GPIO_A_ADDR 	 0x2D9U // Resistor pull-up/pull-down strength  
#define RES_CFG_GPIO9_9_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO9_9_GPIO_A_POS  	 7U

#define GPIO9_9_GPIO_B_ADDR    	 0x2DAU
#define GPIO9_9_GPIO_B_DEFAULT 	 0xA9U

#define GPIO_TX_ID_GPIO9_9_GPIO_B_ADDR 	 0x2DAU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO9_9_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO9_9_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO9_9_GPIO_B_ADDR 	 0x2DAU // Driver type selection  
#define OUT_TYPE_GPIO9_9_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO9_9_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO9_9_GPIO_B_ADDR 	 0x2DAU // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO9_9_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO9_9_GPIO_B_POS  	 6U

#define GPIO9_9_GPIO_C_ADDR    	 0x2DBU
#define GPIO9_9_GPIO_C_DEFAULT 	 0x49U

#define GPIO_RX_ID_GPIO9_9_GPIO_C_ADDR 	 0x2DBU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO9_9_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO9_9_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO9_9_GPIO_C_ADDR 	 0x2DBU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO9_9_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO9_9_GPIO_C_POS  	 7U

#define GPIO10_10_GPIO_A_ADDR    	 0x2DCU
#define GPIO10_10_GPIO_A_DEFAULT 	 0x99U

#define GPIO_OUT_DIS_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // Disable GPIO output driver  
#define GPIO_OUT_DIS_GPIO10_10_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO10_10_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // GPIO TX source control  
#define GPIO_TX_EN_GPIO10_10_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO10_10_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // GPIO out source control.  
#define GPIO_RX_EN_GPIO10_10_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO10_10_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO10_10_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO10_10_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO10_10_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO10_10_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // Jitter minimization compensation enable  
#define TX_COMP_EN_GPIO10_10_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO10_10_GPIO_A_POS  	 5U

#define RES_CFG_GPIO10_10_GPIO_A_ADDR 	 0x2DCU // Resistor pull-up/pull-down strength  
#define RES_CFG_GPIO10_10_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO10_10_GPIO_A_POS  	 7U

#define GPIO10_10_GPIO_B_ADDR    	 0x2DDU
#define GPIO10_10_GPIO_B_DEFAULT 	 0x2AU

#define GPIO_TX_ID_GPIO10_10_GPIO_B_ADDR 	 0x2DDU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO10_10_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO10_10_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO10_10_GPIO_B_ADDR 	 0x2DDU // Driver type selection  
#define OUT_TYPE_GPIO10_10_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO10_10_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO10_10_GPIO_B_ADDR 	 0x2DDU // Buffer pull up/down configuration  
#define PULL_UPDN_SEL_GPIO10_10_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO10_10_GPIO_B_POS  	 6U

#define GPIO10_10_GPIO_C_ADDR    	 0x2DEU
#define GPIO10_10_GPIO_C_DEFAULT 	 0x4AU

#define GPIO_RX_ID_GPIO10_10_GPIO_C_ADDR 	 0x2DEU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO10_10_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO10_10_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO10_10_GPIO_C_ADDR 	 0x2DEU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO10_10_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO10_10_GPIO_C_POS  	 7U

#define CMU_CMU2_ADDR    	 0x302U
#define CMU_CMU2_DEFAULT 	 0x00U

#define PFDDIV_RSHORT_CMU_CMU2_ADDR 	 0x302U // PFDDIV regulator voltage control.  
#define PFDDIV_RSHORT_CMU_CMU2_MASK 	 0x70U
#define PFDDIV_RSHORT_CMU_CMU2_POS  	 4U

#define FRONTTOP_FRONTTOP_0_ADDR    	 0x308U
#define FRONTTOP_FRONTTOP_0_DEFAULT 	 0x64U

#define START_PORTB_FRONTTOP_FRONTTOP_0_ADDR 	 0x308U // Enable CSI Port  
#define START_PORTB_FRONTTOP_FRONTTOP_0_MASK 	 0x20U
#define START_PORTB_FRONTTOP_FRONTTOP_0_POS  	 5U

#define ENABLE_LINE_INFO_FRONTTOP_FRONTTOP_0_ADDR 	 0x308U // Enable sending line start info-frames  
#define ENABLE_LINE_INFO_FRONTTOP_FRONTTOP_0_MASK 	 0x40U
#define ENABLE_LINE_INFO_FRONTTOP_FRONTTOP_0_POS  	 6U

#define FRONTTOP_FRONTTOP_5_ADDR    	 0x30DU
#define FRONTTOP_FRONTTOP_5_DEFAULT 	 0xFFU

#define VC_SELZ_L_FRONTTOP_FRONTTOP_5_ADDR 	 0x30DU // Virtual channel filter bits [7:0]. Each ... 
#define VC_SELZ_L_FRONTTOP_FRONTTOP_5_MASK 	 0xFFU
#define VC_SELZ_L_FRONTTOP_FRONTTOP_5_POS  	 0U

#define FRONTTOP_FRONTTOP_6_ADDR    	 0x30EU
#define FRONTTOP_FRONTTOP_6_DEFAULT 	 0xFFU

#define VC_SELZ_H_FRONTTOP_FRONTTOP_6_ADDR 	 0x30EU // Virtual channel filter bits [15:8]. Each... 
#define VC_SELZ_H_FRONTTOP_FRONTTOP_6_MASK 	 0xFFU
#define VC_SELZ_H_FRONTTOP_FRONTTOP_6_POS  	 0U

#define FRONTTOP_FRONTTOP_9_ADDR    	 0x311U
#define FRONTTOP_FRONTTOP_9_DEFAULT 	 0x40U

#define START_PORTBZ_FRONTTOP_FRONTTOP_9_ADDR 	 0x311U // Start video pipe Z from CSI port  
#define START_PORTBZ_FRONTTOP_FRONTTOP_9_MASK 	 0x40U
#define START_PORTBZ_FRONTTOP_FRONTTOP_9_POS  	 6U

#define FRONTTOP_FRONTTOP_10_ADDR    	 0x312U
#define FRONTTOP_FRONTTOP_10_DEFAULT 	 0x00U

#define BPP8DBLZ_FRONTTOP_FRONTTOP_10_ADDR 	 0x312U // Send 8-bit pixels as 16-bit on video pip... 
#define BPP8DBLZ_FRONTTOP_FRONTTOP_10_MASK 	 0x04U
#define BPP8DBLZ_FRONTTOP_FRONTTOP_10_POS  	 2U

#define FRONTTOP_FRONTTOP_11_ADDR    	 0x313U
#define FRONTTOP_FRONTTOP_11_DEFAULT 	 0x00U

#define BPP10DBLZ_FRONTTOP_FRONTTOP_11_ADDR 	 0x313U // Send 10-bit pixels as 20-bit on video pi... 
#define BPP10DBLZ_FRONTTOP_FRONTTOP_11_MASK 	 0x04U
#define BPP10DBLZ_FRONTTOP_FRONTTOP_11_POS  	 2U

#define BPP12DBLZ_FRONTTOP_FRONTTOP_11_ADDR 	 0x313U // Send 12-bit pixels as 24-bit on video pi... 
#define BPP12DBLZ_FRONTTOP_FRONTTOP_11_MASK 	 0x40U
#define BPP12DBLZ_FRONTTOP_FRONTTOP_11_POS  	 6U

#define FRONTTOP_FRONTTOP_16_ADDR    	 0x318U
#define FRONTTOP_FRONTTOP_16_DEFAULT 	 0x00U

#define MEM_DT1_SELZ_FRONTTOP_FRONTTOP_16_ADDR 	 0x318U // Select designated datatype to route to v... 
#define MEM_DT1_SELZ_FRONTTOP_FRONTTOP_16_MASK 	 0x7FU
#define MEM_DT1_SELZ_FRONTTOP_FRONTTOP_16_POS  	 0U

#define FRONTTOP_FRONTTOP_17_ADDR    	 0x319U
#define FRONTTOP_FRONTTOP_17_DEFAULT 	 0x00U

#define MEM_DT2_SELZ_FRONTTOP_FRONTTOP_17_ADDR 	 0x319U // Select designated datatype to route to v... 
#define MEM_DT2_SELZ_FRONTTOP_FRONTTOP_17_MASK 	 0x7FU
#define MEM_DT2_SELZ_FRONTTOP_FRONTTOP_17_POS  	 0U

#define FRONTTOP_FRONTTOP_22_ADDR    	 0x31EU
#define FRONTTOP_FRONTTOP_22_DEFAULT 	 0x18U

#define SOFT_BPPZ_FRONTTOP_FRONTTOP_22_ADDR 	 0x31EU // Software override of BPP on video pipeli... 
#define SOFT_BPPZ_FRONTTOP_FRONTTOP_22_MASK 	 0x1FU
#define SOFT_BPPZ_FRONTTOP_FRONTTOP_22_POS  	 0U

#define SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_ADDR 	 0x31EU // BPP software override enable for video p... 
#define SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_MASK 	 0x20U
#define SOFT_BPPZ_EN_FRONTTOP_FRONTTOP_22_POS  	 5U

#define SOFT_VCZ_EN_FRONTTOP_FRONTTOP_22_ADDR 	 0x31EU // Virtual channel software override enable... 
#define SOFT_VCZ_EN_FRONTTOP_FRONTTOP_22_MASK 	 0x40U
#define SOFT_VCZ_EN_FRONTTOP_FRONTTOP_22_POS  	 6U

#define SOFT_DTZ_EN_FRONTTOP_FRONTTOP_22_ADDR 	 0x31EU // Datatype software override enable for vi... 
#define SOFT_DTZ_EN_FRONTTOP_FRONTTOP_22_MASK 	 0x80U
#define SOFT_DTZ_EN_FRONTTOP_FRONTTOP_22_POS  	 7U

#define FRONTTOP_FRONTTOP_24_ADDR    	 0x320U
#define FRONTTOP_FRONTTOP_24_DEFAULT 	 0x00U

#define SOFT_VCZ_FRONTTOP_FRONTTOP_24_ADDR 	 0x320U // Virtual channel software override for vi... 
#define SOFT_VCZ_FRONTTOP_FRONTTOP_24_MASK 	 0x30U
#define SOFT_VCZ_FRONTTOP_FRONTTOP_24_POS  	 4U

#define FRONTTOP_FRONTTOP_27_ADDR    	 0x323U
#define FRONTTOP_FRONTTOP_27_DEFAULT 	 0x30U

#define SOFT_DTZ_FRONTTOP_FRONTTOP_27_ADDR 	 0x323U // Datatype software override for video cha... 
#define SOFT_DTZ_FRONTTOP_FRONTTOP_27_MASK 	 0x3FU
#define SOFT_DTZ_FRONTTOP_FRONTTOP_27_POS  	 0U

#define FRONTTOP_FRONTTOP_29_ADDR    	 0x325U
#define FRONTTOP_FRONTTOP_29_DEFAULT 	 0x00U

#define FORCE_START_MIPI_FRONTTOP_FRONTTOP_FRONTTOP_29_ADDR 	 0x325U // Force the MIPI receiver start without wa... 
#define FORCE_START_MIPI_FRONTTOP_FRONTTOP_FRONTTOP_29_MASK 	 0x80U
#define FORCE_START_MIPI_FRONTTOP_FRONTTOP_FRONTTOP_29_POS  	 7U

#define MIPI_RX_MIPI_RX0_ADDR    	 0x330U
#define MIPI_RX_MIPI_RX0_DEFAULT 	 0x00U

#define MIPI_RX_RESET_MIPI_RX_MIPI_RX0_ADDR 	 0x330U // Reset MIPI RX receiver (MIPI PHY). This ... 
#define MIPI_RX_RESET_MIPI_RX_MIPI_RX0_MASK 	 0x08U
#define MIPI_RX_RESET_MIPI_RX_MIPI_RX0_POS  	 3U

#define CTRL1_VC_MAP_EN_MIPI_RX_MIPI_RX0_ADDR 	 0x330U // Virtual channel mapping enable. When ena... 
#define CTRL1_VC_MAP_EN_MIPI_RX_MIPI_RX0_MASK 	 0x20U
#define CTRL1_VC_MAP_EN_MIPI_RX_MIPI_RX0_POS  	 5U

#define MIPI_NONCONTCLK_EN_MIPI_RX_MIPI_RX0_ADDR 	 0x330U // MIPI non-continuous clock enable  
#define MIPI_NONCONTCLK_EN_MIPI_RX_MIPI_RX0_MASK 	 0x40U
#define MIPI_NONCONTCLK_EN_MIPI_RX_MIPI_RX0_POS  	 6U

#define MIPI_RX_MIPI_RX1_ADDR    	 0x331U
#define MIPI_RX_MIPI_RX1_DEFAULT 	 0x30U

#define CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_ADDR 	 0x331U // Select number of data lanes  
#define CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_MASK 	 0x30U
#define CTRL1_NUM_LANES_MIPI_RX_MIPI_RX1_POS  	 4U

#define CTRL1_DESKEWEN_MIPI_RX_MIPI_RX1_ADDR 	 0x331U // Enable the deskew calibration for 1.5Gbp... 
#define CTRL1_DESKEWEN_MIPI_RX_MIPI_RX1_MASK 	 0x40U
#define CTRL1_DESKEWEN_MIPI_RX_MIPI_RX1_POS  	 6U

#define CTRL1_VCX_EN_MIPI_RX_MIPI_RX1_ADDR 	 0x331U // Enable the extended Virtual Channels fea... 
#define CTRL1_VCX_EN_MIPI_RX_MIPI_RX1_MASK 	 0x80U
#define CTRL1_VCX_EN_MIPI_RX_MIPI_RX1_POS  	 7U

#define MIPI_RX_MIPI_RX2_ADDR    	 0x332U
#define MIPI_RX_MIPI_RX2_DEFAULT 	 0xE0U

#define PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_ADDR 	 0x332U // Serializer lane mapping for MIPI data la... 
#define PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_MASK 	 0xF0U
#define PHY1_LANE_MAP_MIPI_RX_MIPI_RX2_POS  	 4U

#define MIPI_RX_MIPI_RX3_ADDR    	 0x333U
#define MIPI_RX_MIPI_RX3_DEFAULT 	 0x04U

#define PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_ADDR 	 0x333U // Serializer lane mapping for MIPI data la... 
#define PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_MASK 	 0x0FU
#define PHY2_LANE_MAP_MIPI_RX_MIPI_RX3_POS  	 0U

#define MIPI_RX_MIPI_RX4_ADDR    	 0x334U
#define MIPI_RX_MIPI_RX4_DEFAULT 	 0x00U

#define PHY1_POL_MAP_MIPI_RX_MIPI_RX4_ADDR 	 0x334U // Serializer lane polarity setting for MIP... 
#define PHY1_POL_MAP_MIPI_RX_MIPI_RX4_MASK 	 0x70U
#define PHY1_POL_MAP_MIPI_RX_MIPI_RX4_POS  	 4U

#define MIPI_RX_MIPI_RX5_ADDR    	 0x335U
#define MIPI_RX_MIPI_RX5_DEFAULT 	 0x00U

#define PHY2_POL_MAP_MIPI_RX_MIPI_RX5_ADDR 	 0x335U // Serializer lane polarity setting for MIP... 
#define PHY2_POL_MAP_MIPI_RX_MIPI_RX5_MASK 	 0x07U
#define PHY2_POL_MAP_MIPI_RX_MIPI_RX5_POS  	 0U

#define MIPI_RX_MIPI_RX7_ADDR    	 0x337U
#define MIPI_RX_MIPI_RX7_DEFAULT 	 0x00U

#define MIPI_RX_MIPI_RX8_ADDR    	 0x338U
#define MIPI_RX_MIPI_RX8_DEFAULT 	 0x55U

#define T_CLK_SETTLE_MIPI_RX_MIPI_RX8_ADDR 	 0x338U // Set typical DPHY Tclk_settle timing in n... 
#define T_CLK_SETTLE_MIPI_RX_MIPI_RX8_MASK 	 0x03U
#define T_CLK_SETTLE_MIPI_RX_MIPI_RX8_POS  	 0U

#define T_HS_SETTLE_MIPI_RX_MIPI_RX8_ADDR 	 0x338U // {{t_hs_settle_description}}  
#define T_HS_SETTLE_MIPI_RX_MIPI_RX8_MASK 	 0x30U
#define T_HS_SETTLE_MIPI_RX_MIPI_RX8_POS  	 4U

#define T_HS_DEC_EN_MIPI_RX_MIPI_RX8_ADDR 	 0x338U // Set CPHY delay between CDR enable to sym... 
#define T_HS_DEC_EN_MIPI_RX_MIPI_RX8_MASK 	 0xC0U
#define T_HS_DEC_EN_MIPI_RX_MIPI_RX8_POS  	 6U

#define MIPI_RX_MIPI_RX11_ADDR    	 0x33BU
#define MIPI_RX_MIPI_RX11_DEFAULT 	 0x00U

#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_ADDR 	 0x33BU // Phy1 LP status (DPHY only)  
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_MASK 	 0x1FU
#define PHY1_LP_ERR_MIPI_RX_MIPI_RX11_POS  	 0U

#define MIPI_RX_MIPI_RX12_ADDR    	 0x33CU
#define MIPI_RX_MIPI_RX12_DEFAULT 	 0x00U

#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_ADDR 	 0x33CU // PHY1 high-speed status (DPHY only)  
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_MASK 	 0xFFU
#define PHY1_HS_ERR_MIPI_RX_MIPI_RX12_POS  	 0U

#define MIPI_RX_MIPI_RX13_ADDR    	 0x33DU
#define MIPI_RX_MIPI_RX13_DEFAULT 	 0x00U

#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_ADDR 	 0x33DU // Phy2 LP status (DPHY only)  
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_MASK 	 0x1FU
#define PHY2_LP_ERR_MIPI_RX_MIPI_RX13_POS  	 0U

#define MIPI_RX_MIPI_RX14_ADDR    	 0x33EU
#define MIPI_RX_MIPI_RX14_DEFAULT 	 0x00U

#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_ADDR 	 0x33EU // PHY2 high-speed status (DPHY only)  
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_MASK 	 0xFFU
#define PHY2_HS_ERR_MIPI_RX_MIPI_RX14_POS  	 0U

#define MIPI_RX_MIPI_RX19_ADDR    	 0x343U
#define MIPI_RX_MIPI_RX19_DEFAULT 	 0x00U

#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_ADDR 	 0x343U // CSI-2 Controller Status, low byte  
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_MASK 	 0xFFU
#define CTRL1_CSI_ERR_L_MIPI_RX_MIPI_RX19_POS  	 0U

#define MIPI_RX_MIPI_RX20_ADDR    	 0x344U
#define MIPI_RX_MIPI_RX20_DEFAULT 	 0x00U

#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_ADDR 	 0x344U // CSI-2 Controller Status, high bits  
#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_MASK 	 0x07U
#define CTRL1_CSI_ERR_H_MIPI_RX_MIPI_RX20_POS  	 0U

#define MIPI_RX_MIPI_RX21_ADDR    	 0x345U
#define MIPI_RX_MIPI_RX21_DEFAULT 	 0x00U

#define CTRL1_VC_MAP0_MIPI_RX_MIPI_RX21_ADDR 	 0x345U // New virtual channel for VC=0. If ctrl_vc... 
#define CTRL1_VC_MAP0_MIPI_RX_MIPI_RX21_MASK 	 0xF0U
#define CTRL1_VC_MAP0_MIPI_RX_MIPI_RX21_POS  	 4U

#define MIPI_RX_MIPI_RX22_ADDR    	 0x346U
#define MIPI_RX_MIPI_RX22_DEFAULT 	 0x00U

#define CTRL1_VC_MAP1_MIPI_RX_MIPI_RX22_ADDR 	 0x346U // New virtual channel for VC=1. If ctrl_vc... 
#define CTRL1_VC_MAP1_MIPI_RX_MIPI_RX22_MASK 	 0xF0U
#define CTRL1_VC_MAP1_MIPI_RX_MIPI_RX22_POS  	 4U

#define MIPI_RX_MIPI_RX23_ADDR    	 0x347U
#define MIPI_RX_MIPI_RX23_DEFAULT 	 0x00U

#define CTRL1_VC_MAP2_MIPI_RX_MIPI_RX23_ADDR 	 0x347U // New virtual channel for VC=2. If ctrl_vc... 
#define CTRL1_VC_MAP2_MIPI_RX_MIPI_RX23_MASK 	 0xF0U
#define CTRL1_VC_MAP2_MIPI_RX_MIPI_RX23_POS  	 4U

#define MIPI_RX_MIPI_RX60_ADDR    	 0x36CU
#define MIPI_RX_MIPI_RX60_DEFAULT 	 0x00U

#define CTRL1_VC_MAP3_MIPI_RX_MIPI_RX60_ADDR 	 0x36CU // New virtual channel for VC=3. If ctrl_vc... 
#define CTRL1_VC_MAP3_MIPI_RX_MIPI_RX60_MASK 	 0xF0U
#define CTRL1_VC_MAP3_MIPI_RX_MIPI_RX60_POS  	 4U

#define MIPI_RX_MIPI_RX61_ADDR    	 0x36DU
#define MIPI_RX_MIPI_RX61_DEFAULT 	 0x00U

#define CTRL1_VC_MAP4_MIPI_RX_MIPI_RX61_ADDR 	 0x36DU // New virtual channel for VC=4. If ctrl_vc... 
#define CTRL1_VC_MAP4_MIPI_RX_MIPI_RX61_MASK 	 0xF0U
#define CTRL1_VC_MAP4_MIPI_RX_MIPI_RX61_POS  	 4U

#define MIPI_RX_MIPI_RX62_ADDR    	 0x36EU
#define MIPI_RX_MIPI_RX62_DEFAULT 	 0x00U

#define CTRL1_VC_MAP5_MIPI_RX_MIPI_RX62_ADDR 	 0x36EU // New virtual channel for VC=5. If ctrl_vc... 
#define CTRL1_VC_MAP5_MIPI_RX_MIPI_RX62_MASK 	 0xF0U
#define CTRL1_VC_MAP5_MIPI_RX_MIPI_RX62_POS  	 4U

#define MIPI_RX_MIPI_RX63_ADDR    	 0x36FU
#define MIPI_RX_MIPI_RX63_DEFAULT 	 0x00U

#define CTRL1_VC_MAP6_MIPI_RX_MIPI_RX63_ADDR 	 0x36FU // New virtual channel for VC=6. If ctrl_vc... 
#define CTRL1_VC_MAP6_MIPI_RX_MIPI_RX63_MASK 	 0xF0U
#define CTRL1_VC_MAP6_MIPI_RX_MIPI_RX63_POS  	 4U

#define MIPI_RX_EXT_EXT00_ADDR    	 0x377U
#define MIPI_RX_EXT_EXT00_DEFAULT 	 0x00U

#define CTRL1_VC_MAP7_MIPI_RX_EXT_EXT00_ADDR 	 0x377U // New virtual channel for VC=7. If ctrl_vc... 
#define CTRL1_VC_MAP7_MIPI_RX_EXT_EXT00_MASK 	 0xF0U
#define CTRL1_VC_MAP7_MIPI_RX_EXT_EXT00_POS  	 4U

#define MIPI_RX_EXT_EXT0_ADDR    	 0x378U
#define MIPI_RX_EXT_EXT0_DEFAULT 	 0x00U

#define CTRL1_VC_MAP8_MIPI_RX_EXT_EXT0_ADDR 	 0x378U // New virtual channel for VC=8. If ctrl_vc... 
#define CTRL1_VC_MAP8_MIPI_RX_EXT_EXT0_MASK 	 0xF0U
#define CTRL1_VC_MAP8_MIPI_RX_EXT_EXT0_POS  	 4U

#define MIPI_RX_EXT_EXT1_ADDR    	 0x379U
#define MIPI_RX_EXT_EXT1_DEFAULT 	 0x00U

#define CTRL1_VC_MAP9_MIPI_RX_EXT_EXT1_ADDR 	 0x379U // New virtual channel for VC=9. If ctrl_vc... 
#define CTRL1_VC_MAP9_MIPI_RX_EXT_EXT1_MASK 	 0xF0U
#define CTRL1_VC_MAP9_MIPI_RX_EXT_EXT1_POS  	 4U

#define MIPI_RX_EXT_EXT2_ADDR    	 0x37AU
#define MIPI_RX_EXT_EXT2_DEFAULT 	 0x00U

#define CTRL1_VC_MAP10_MIPI_RX_EXT_EXT2_ADDR 	 0x37AU // New virtual channel for VC=10. If ctrl_v... 
#define CTRL1_VC_MAP10_MIPI_RX_EXT_EXT2_MASK 	 0xF0U
#define CTRL1_VC_MAP10_MIPI_RX_EXT_EXT2_POS  	 4U

#define MIPI_RX_EXT_EXT3_ADDR    	 0x37BU
#define MIPI_RX_EXT_EXT3_DEFAULT 	 0x00U

#define CTRL1_VC_MAP11_MIPI_RX_EXT_EXT3_ADDR 	 0x37BU // New virtual channel for VC=11. If ctrl_v... 
#define CTRL1_VC_MAP11_MIPI_RX_EXT_EXT3_MASK 	 0xF0U
#define CTRL1_VC_MAP11_MIPI_RX_EXT_EXT3_POS  	 4U

#define MIPI_RX_EXT_EXT4_ADDR    	 0x37CU
#define MIPI_RX_EXT_EXT4_DEFAULT 	 0x00U

#define CTRL1_VC_MAP12_MIPI_RX_EXT_EXT4_ADDR 	 0x37CU // New virtual channel for VC=12. If ctrl_v... 
#define CTRL1_VC_MAP12_MIPI_RX_EXT_EXT4_MASK 	 0xF0U
#define CTRL1_VC_MAP12_MIPI_RX_EXT_EXT4_POS  	 4U

#define MIPI_RX_EXT_EXT5_ADDR    	 0x37DU
#define MIPI_RX_EXT_EXT5_DEFAULT 	 0x00U

#define CTRL1_VC_MAP13_MIPI_RX_EXT_EXT5_ADDR 	 0x37DU // New virtual channel for VC=13. If ctrl_v... 
#define CTRL1_VC_MAP13_MIPI_RX_EXT_EXT5_MASK 	 0xF0U
#define CTRL1_VC_MAP13_MIPI_RX_EXT_EXT5_POS  	 4U

#define MIPI_RX_EXT_EXT6_ADDR    	 0x37EU
#define MIPI_RX_EXT_EXT6_DEFAULT 	 0x00U

#define CTRL1_VC_MAP14_MIPI_RX_EXT_EXT6_ADDR 	 0x37EU // New virtual channel for VC=14. If ctrl_v... 
#define CTRL1_VC_MAP14_MIPI_RX_EXT_EXT6_MASK 	 0xF0U
#define CTRL1_VC_MAP14_MIPI_RX_EXT_EXT6_POS  	 4U

#define MIPI_RX_EXT_EXT7_ADDR    	 0x37FU
#define MIPI_RX_EXT_EXT7_DEFAULT 	 0x00U

#define CTRL1_VC_MAP15_MIPI_RX_EXT_EXT7_ADDR 	 0x37FU // New virtual channel for VC=15. If ctrl_v... 
#define CTRL1_VC_MAP15_MIPI_RX_EXT_EXT7_MASK 	 0xF0U
#define CTRL1_VC_MAP15_MIPI_RX_EXT_EXT7_POS  	 4U

#define MIPI_RX_EXT_EXT8_ADDR    	 0x380U
#define MIPI_RX_EXT_EXT8_DEFAULT 	 0x00U

#define TUN_FIFO_OVERFLOW_MIPI_RX_EXT_EXT8_ADDR 	 0x380U // Tunnel FIFO overflow  
#define TUN_FIFO_OVERFLOW_MIPI_RX_EXT_EXT8_MASK 	 0x01U
#define TUN_FIFO_OVERFLOW_MIPI_RX_EXT_EXT8_POS  	 0U

#define INVCODE_LN0_MIPI_RX_EXT_EXT8_ADDR 	 0x380U // Invalid Code error for CPHY lane 0. CPHY... 
#define INVCODE_LN0_MIPI_RX_EXT_EXT8_MASK 	 0x02U
#define INVCODE_LN0_MIPI_RX_EXT_EXT8_POS  	 1U

#define INVCODE_LN1_MIPI_RX_EXT_EXT8_ADDR 	 0x380U // Invalid Code error for CPHY lane 1. CPHY... 
#define INVCODE_LN1_MIPI_RX_EXT_EXT8_MASK 	 0x04U
#define INVCODE_LN1_MIPI_RX_EXT_EXT8_POS  	 2U

#define CPHY_HDR2_ERR_MIPI_RX_EXT_EXT8_ADDR 	 0x380U // CPHY header2 error indicator  
#define CPHY_HDR2_ERR_MIPI_RX_EXT_EXT8_MASK 	 0x18U
#define CPHY_HDR2_ERR_MIPI_RX_EXT_EXT8_POS  	 3U

#define CPHY_HDR1_ERR_MIPI_RX_EXT_EXT8_ADDR 	 0x380U // CPHY header1 error indicator  
#define CPHY_HDR1_ERR_MIPI_RX_EXT_EXT8_MASK 	 0x60U
#define CPHY_HDR1_ERR_MIPI_RX_EXT_EXT8_POS  	 5U

#define CPHY_HDR_ERR_MIPI_RX_EXT_EXT8_ADDR 	 0x380U // CPHY header error indicator  
#define CPHY_HDR_ERR_MIPI_RX_EXT_EXT8_MASK 	 0x80U
#define CPHY_HDR_ERR_MIPI_RX_EXT_EXT8_POS  	 7U

#define MIPI_RX_EXT_EXT9_ADDR    	 0x381U
#define MIPI_RX_EXT_EXT9_DEFAULT 	 0x00U

#define MIPI_RX_EXT_EXT11_ADDR    	 0x383U
#define MIPI_RX_EXT_EXT11_DEFAULT 	 0x80U

#define PHY1_CPHYCDRMASK_MIPI_RX_EXT_EXT11_ADDR 	 0x383U // Set width of the blanking (UI=400ps).  
#define PHY1_CPHYCDRMASK_MIPI_RX_EXT_EXT11_MASK 	 0x03U
#define PHY1_CPHYCDRMASK_MIPI_RX_EXT_EXT11_POS  	 0U

#define CPHY_MODE_MIPI_RX_EXT_EXT11_ADDR 	 0x383U // Select MIPI CPHY Receiver  
#define CPHY_MODE_MIPI_RX_EXT_EXT11_MASK 	 0x40U
#define CPHY_MODE_MIPI_RX_EXT_EXT11_POS  	 6U

#define TUN_MODE_MIPI_RX_EXT_EXT11_ADDR 	 0x383U // Select Tunnel mode  
#define TUN_MODE_MIPI_RX_EXT_EXT11_MASK 	 0x80U
#define TUN_MODE_MIPI_RX_EXT_EXT11_POS  	 7U

#define MIPI_RX_EXT_EXT21_ADDR    	 0x38DU
#define MIPI_RX_EXT_EXT21_DEFAULT 	 0x00U

#define PHY1_PKT_CNT_MIPI_RX_EXT_EXT21_ADDR 	 0x38DU // MIPI PHY1 Packets Received  
#define PHY1_PKT_CNT_MIPI_RX_EXT_EXT21_MASK 	 0xFFU
#define PHY1_PKT_CNT_MIPI_RX_EXT_EXT21_POS  	 0U

#define MIPI_RX_EXT_EXT22_ADDR    	 0x38EU
#define MIPI_RX_EXT_EXT22_DEFAULT 	 0x00U

#define CSI1_PKT_CNT_MIPI_RX_EXT_EXT22_ADDR 	 0x38EU // MIPI Controller 1 Packets Processed  
#define CSI1_PKT_CNT_MIPI_RX_EXT_EXT22_MASK 	 0xFFU
#define CSI1_PKT_CNT_MIPI_RX_EXT_EXT22_POS  	 0U

#define MIPI_RX_EXT_EXT23_ADDR    	 0x38FU
#define MIPI_RX_EXT_EXT23_DEFAULT 	 0x00U

#define TUN_PKT_CNT_MIPI_RX_EXT_EXT23_ADDR 	 0x38FU // MIPI Tunnel Packets Processed  
#define TUN_PKT_CNT_MIPI_RX_EXT_EXT23_MASK 	 0xFFU
#define TUN_PKT_CNT_MIPI_RX_EXT_EXT23_POS  	 0U

#define MIPI_RX_EXT_EXT24_ADDR    	 0x390U
#define MIPI_RX_EXT_EXT24_DEFAULT 	 0x00U

#define PHY_CLK_CNT_MIPI_RX_EXT_EXT24_ADDR 	 0x390U // MIPI RX Clock Received. The changing val... 
#define PHY_CLK_CNT_MIPI_RX_EXT_EXT24_MASK 	 0xFFU
#define PHY_CLK_CNT_MIPI_RX_EXT_EXT24_POS  	 0U

#define FRONTTOP_EXT_FRONTTOP_EXT8_ADDR    	 0x3C8U
#define FRONTTOP_EXT_FRONTTOP_EXT8_DEFAULT 	 0x00U

#define MEM_DT3_SELZ_FRONTTOP_EXT_FRONTTOP_EXT8_ADDR 	 0x3C8U // Select a designated datatype to route to... 
#define MEM_DT3_SELZ_FRONTTOP_EXT_FRONTTOP_EXT8_MASK 	 0xFFU
#define MEM_DT3_SELZ_FRONTTOP_EXT_FRONTTOP_EXT8_POS  	 0U

#define FRONTTOP_EXT_FRONTTOP_EXT9_ADDR    	 0x3C9U
#define FRONTTOP_EXT_FRONTTOP_EXT9_DEFAULT 	 0x00U

#define MEM_DT4_SELZ_FRONTTOP_EXT_FRONTTOP_EXT9_ADDR 	 0x3C9U // Select a designated datatype to route to... 
#define MEM_DT4_SELZ_FRONTTOP_EXT_FRONTTOP_EXT9_MASK 	 0xFFU
#define MEM_DT4_SELZ_FRONTTOP_EXT_FRONTTOP_EXT9_POS  	 0U

#define FRONTTOP_EXT_FRONTTOP_EXT10_ADDR    	 0x3CAU
#define FRONTTOP_EXT_FRONTTOP_EXT10_DEFAULT 	 0x00U

#define MEM_DT5_SELZ_FRONTTOP_EXT_FRONTTOP_EXT10_ADDR 	 0x3CAU // Select a designated datatype to route to... 
#define MEM_DT5_SELZ_FRONTTOP_EXT_FRONTTOP_EXT10_MASK 	 0xFFU
#define MEM_DT5_SELZ_FRONTTOP_EXT_FRONTTOP_EXT10_POS  	 0U

#define FRONTTOP_EXT_FRONTTOP_EXT11_ADDR    	 0x3CBU
#define FRONTTOP_EXT_FRONTTOP_EXT11_DEFAULT 	 0x00U

#define MEM_DT6_SELZ_FRONTTOP_EXT_FRONTTOP_EXT11_ADDR 	 0x3CBU // Select a designated datatype to route to... 
#define MEM_DT6_SELZ_FRONTTOP_EXT_FRONTTOP_EXT11_MASK 	 0xFFU
#define MEM_DT6_SELZ_FRONTTOP_EXT_FRONTTOP_EXT11_POS  	 0U

#define FRONTTOP_EXT_FRONTTOP_EXT17_ADDR    	 0x3D1U
#define FRONTTOP_EXT_FRONTTOP_EXT17_DEFAULT 	 0x00U

#define MEM_DT3_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_ADDR 	 0x3D1U // Enable datatype designated in mem_dt3_se... 
#define MEM_DT3_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_MASK 	 0x01U
#define MEM_DT3_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_POS  	 0U

#define MEM_DT4_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_ADDR 	 0x3D1U // Enable datatype designated in mem_dt4_se... 
#define MEM_DT4_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_MASK 	 0x02U
#define MEM_DT4_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_POS  	 1U

#define MEM_DT5_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_ADDR 	 0x3D1U // Enable datatype designated in mem_dt5_se... 
#define MEM_DT5_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_MASK 	 0x04U
#define MEM_DT5_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_POS  	 2U

#define MEM_DT6_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_ADDR 	 0x3D1U // Enable datatype designated in mem_dt6_se... 
#define MEM_DT6_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_MASK 	 0x08U
#define MEM_DT6_SELZ_EN_FRONTTOP_EXT_FRONTTOP_EXT17_POS  	 3U

#define MIPI_RX_EXT2_EXTA_ADDR    	 0x3DCU
#define MIPI_RX_EXT2_EXTA_DEFAULT 	 0x00U

#define MEM_DT7_SELZ_MIPI_RX_EXT2_EXTA_ADDR 	 0x3DCU // Select designated datatype to route to v... 
#define MEM_DT7_SELZ_MIPI_RX_EXT2_EXTA_MASK 	 0x7FU
#define MEM_DT7_SELZ_MIPI_RX_EXT2_EXTA_POS  	 0U

#define MIPI_RX_EXT2_EXTB_ADDR    	 0x3DDU
#define MIPI_RX_EXT2_EXTB_DEFAULT 	 0x00U

#define MEM_DT8_SELZ_MIPI_RX_EXT2_EXTB_ADDR 	 0x3DDU // Select designated datatype to route to v... 
#define MEM_DT8_SELZ_MIPI_RX_EXT2_EXTB_MASK 	 0x7FU
#define MEM_DT8_SELZ_MIPI_RX_EXT2_EXTB_POS  	 0U

#define REF_VTG_VTX0_ADDR    	 0x3E0U
#define REF_VTG_VTX0_DEFAULT 	 0x70U

#define GEN_VS_REF_VTG_VTX0_ADDR 	 0x3E0U // Enable generation of VS output  
#define GEN_VS_REF_VTG_VTX0_MASK 	 0x01U
#define GEN_VS_REF_VTG_VTX0_POS  	 0U

#define VS_INV_REF_VTG_VTX0_ADDR 	 0x3E0U // Invert VS output of video timing generat... 
#define VS_INV_REF_VTG_VTX0_MASK 	 0x02U
#define VS_INV_REF_VTG_VTX0_POS  	 1U

#define GEN_HS_REF_VTG_VTX0_ADDR 	 0x3E0U // Enable generation of HS output  
#define GEN_HS_REF_VTG_VTX0_MASK 	 0x04U
#define GEN_HS_REF_VTG_VTX0_POS  	 2U

#define HS_INV_REF_VTG_VTX0_ADDR 	 0x3E0U // Invert HS output of video timing generat... 
#define HS_INV_REF_VTG_VTX0_MASK 	 0x08U
#define HS_INV_REF_VTG_VTX0_POS  	 3U

#define REF_VTG_MODE_REF_VTG_VTX0_ADDR 	 0x3E0U // Selects one of the following modes for v... 
#define REF_VTG_MODE_REF_VTG_VTX0_MASK 	 0x30U
#define REF_VTG_MODE_REF_VTG_VTX0_POS  	 4U

#define VS_TRIG_REF_VTG_VTX0_ADDR 	 0x3E0U // Select VS trigger edge (positive vs. neg... 
#define VS_TRIG_REF_VTG_VTX0_MASK 	 0x40U
#define VS_TRIG_REF_VTG_VTX0_POS  	 6U

#define REF_VTG_VTX5_ADDR    	 0x3E1U
#define REF_VTG_VTX5_DEFAULT 	 0x00U

#define VS_HIGH_2_REF_VTG_VTX5_ADDR 	 0x3E1U // VS High Period in terms of PCLK cycles (... 
#define VS_HIGH_2_REF_VTG_VTX5_MASK 	 0xFFU
#define VS_HIGH_2_REF_VTG_VTX5_POS  	 0U

#define REF_VTG_VTX6_ADDR    	 0x3E2U
#define REF_VTG_VTX6_DEFAULT 	 0x00U

#define VS_HIGH_1_REF_VTG_VTX6_ADDR 	 0x3E2U // VS High Period in terms of PCLK cycles (... 
#define VS_HIGH_1_REF_VTG_VTX6_MASK 	 0xFFU
#define VS_HIGH_1_REF_VTG_VTX6_POS  	 0U

#define REF_VTG_VTX7_ADDR    	 0x3E3U
#define REF_VTG_VTX7_DEFAULT 	 0x00U

#define VS_HIGH_0_REF_VTG_VTX7_ADDR 	 0x3E3U // VS High Period in terms of PCLK cycles (... 
#define VS_HIGH_0_REF_VTG_VTX7_MASK 	 0xFFU
#define VS_HIGH_0_REF_VTG_VTX7_POS  	 0U

#define REF_VTG_VTX8_ADDR    	 0x3E4U
#define REF_VTG_VTX8_DEFAULT 	 0x00U

#define VS_LOW_2_REF_VTG_VTX8_ADDR 	 0x3E4U // VS Low Period in terms of PCLK cycles (B... 
#define VS_LOW_2_REF_VTG_VTX8_MASK 	 0xFFU
#define VS_LOW_2_REF_VTG_VTX8_POS  	 0U

#define REF_VTG_VTX9_ADDR    	 0x3E5U
#define REF_VTG_VTX9_DEFAULT 	 0x00U

#define VS_LOW_1_REF_VTG_VTX9_ADDR 	 0x3E5U // VS Low Period in terms of PCLK cycles (B... 
#define VS_LOW_1_REF_VTG_VTX9_MASK 	 0xFFU
#define VS_LOW_1_REF_VTG_VTX9_POS  	 0U

#define REF_VTG_VTX10_ADDR    	 0x3E6U
#define REF_VTG_VTX10_DEFAULT 	 0x00U

#define VS_LOW_0_REF_VTG_VTX10_ADDR 	 0x3E6U // VS Low Period in terms of PCLK cycles (B... 
#define VS_LOW_0_REF_VTG_VTX10_MASK 	 0xFFU
#define VS_LOW_0_REF_VTG_VTX10_POS  	 0U

#define REF_VTG_VTX11_ADDR    	 0x3E7U
#define REF_VTG_VTX11_DEFAULT 	 0x00U

#define V2H_2_REF_VTG_VTX11_ADDR 	 0x3E7U // Horizontal sync delay.  VS edge to the r... 
#define V2H_2_REF_VTG_VTX11_MASK 	 0xFFU
#define V2H_2_REF_VTG_VTX11_POS  	 0U

#define REF_VTG_VTX12_ADDR    	 0x3E8U
#define REF_VTG_VTX12_DEFAULT 	 0x00U

#define V2H_1_REF_VTG_VTX12_ADDR 	 0x3E8U // Horizontal sync delay.  VS edge to the r... 
#define V2H_1_REF_VTG_VTX12_MASK 	 0xFFU
#define V2H_1_REF_VTG_VTX12_POS  	 0U

#define REF_VTG_VTX13_ADDR    	 0x3E9U
#define REF_VTG_VTX13_DEFAULT 	 0x00U

#define V2H_0_REF_VTG_VTX13_ADDR 	 0x3E9U // Horizontal sync delay.  VS edge to the r... 
#define V2H_0_REF_VTG_VTX13_MASK 	 0xFFU
#define V2H_0_REF_VTG_VTX13_POS  	 0U

#define REF_VTG_VTX14_ADDR    	 0x3EAU
#define REF_VTG_VTX14_DEFAULT 	 0x00U

#define HS_HIGH_1_REF_VTG_VTX14_ADDR 	 0x3EAU // HS High Period in terms of PCLK cycles (... 
#define HS_HIGH_1_REF_VTG_VTX14_MASK 	 0xFFU
#define HS_HIGH_1_REF_VTG_VTX14_POS  	 0U

#define REF_VTG_VTX15_ADDR    	 0x3EBU
#define REF_VTG_VTX15_DEFAULT 	 0x00U

#define HS_HIGH_0_REF_VTG_VTX15_ADDR 	 0x3EBU // HS High Period in terms of PCLK cycles (... 
#define HS_HIGH_0_REF_VTG_VTX15_MASK 	 0xFFU
#define HS_HIGH_0_REF_VTG_VTX15_POS  	 0U

#define REF_VTG_VTX16_ADDR    	 0x3ECU
#define REF_VTG_VTX16_DEFAULT 	 0x00U

#define HS_LOW_1_REF_VTG_VTX16_ADDR 	 0x3ECU // HS Low Period in terms of PCLK cycles (B... 
#define HS_LOW_1_REF_VTG_VTX16_MASK 	 0xFFU
#define HS_LOW_1_REF_VTG_VTX16_POS  	 0U

#define REF_VTG_VTX17_ADDR    	 0x3EDU
#define REF_VTG_VTX17_DEFAULT 	 0x00U

#define HS_LOW_0_REF_VTG_VTX17_ADDR 	 0x3EDU // HS Low Period in terms of PCLK cycles (B... 
#define HS_LOW_0_REF_VTG_VTX17_MASK 	 0xFFU
#define HS_LOW_0_REF_VTG_VTX17_POS  	 0U

#define REF_VTG_VTX18_ADDR    	 0x3EEU
#define REF_VTG_VTX18_DEFAULT 	 0x00U

#define HS_CNT_1_REF_VTG_VTX18_ADDR 	 0x3EEU // Number of HS pulses per frame (Bits [15:... 
#define HS_CNT_1_REF_VTG_VTX18_MASK 	 0xFFU
#define HS_CNT_1_REF_VTG_VTX18_POS  	 0U

#define REF_VTG_VTX19_ADDR    	 0x3EFU
#define REF_VTG_VTX19_DEFAULT 	 0x00U

#define HS_CNT_0_REF_VTG_VTX19_ADDR 	 0x3EFU // Number of HS pulses per frame (Bits [7:0... 
#define HS_CNT_0_REF_VTG_VTX19_MASK 	 0xFFU
#define HS_CNT_0_REF_VTG_VTX19_POS  	 0U

#define REF_VTG_REF_VTG0_ADDR    	 0x3F0U
#define REF_VTG_REF_VTG0_DEFAULT 	 0x50U

#define REFGEN_EN_REF_VTG_REF_VTG0_ADDR 	 0x3F0U // Enable reference generation PLL  
#define REFGEN_EN_REF_VTG_REF_VTG0_MASK 	 0x01U
#define REFGEN_EN_REF_VTG_REF_VTG0_POS  	 0U

#define REFGEN_RST_REF_VTG_REF_VTG0_ADDR 	 0x3F0U // Reset reference generation PLL  
#define REFGEN_RST_REF_VTG_REF_VTG0_MASK 	 0x02U
#define REFGEN_RST_REF_VTG_REF_VTG0_POS  	 1U

#define REFGEN_PREDEF_FREQ_ALT_REF_VTG_REF_VTG0_ADDR 	 0x3F0U // Enable alternative predefined reference ... 
#define REFGEN_PREDEF_FREQ_ALT_REF_VTG_REF_VTG0_MASK 	 0x08U
#define REFGEN_PREDEF_FREQ_ALT_REF_VTG_REF_VTG0_POS  	 3U

#define REFGEN_PREDEF_FREQ_REF_VTG_REF_VTG0_ADDR 	 0x3F0U // Predefined reference generation PLL freq... 
#define REFGEN_PREDEF_FREQ_REF_VTG_REF_VTG0_MASK 	 0x30U
#define REFGEN_PREDEF_FREQ_REF_VTG_REF_VTG0_POS  	 4U

#define REFGEN_PREDEF_EN_REF_VTG_REF_VTG0_ADDR 	 0x3F0U // Enable predefined clock settings for ref... 
#define REFGEN_PREDEF_EN_REF_VTG_REF_VTG0_MASK 	 0x40U
#define REFGEN_PREDEF_EN_REF_VTG_REF_VTG0_POS  	 6U

#define REFGEN_LOCKED_REF_VTG_REF_VTG0_ADDR 	 0x3F0U // Reference generation PLL is locked (for ... 
#define REFGEN_LOCKED_REF_VTG_REF_VTG0_MASK 	 0x80U
#define REFGEN_LOCKED_REF_VTG_REF_VTG0_POS  	 7U

#define REF_VTG_REF_VTG1_ADDR    	 0x3F1U
#define REF_VTG_REF_VTG1_DEFAULT 	 0x00U

#define PCLKEN_REF_VTG_REF_VTG1_ADDR 	 0x3F1U // Enable output of PCLK on local MFP selec... 
#define PCLKEN_REF_VTG_REF_VTG1_MASK 	 0x01U
#define PCLKEN_REF_VTG_REF_VTG1_POS  	 0U

#define PCLK_GPIO_REF_VTG_REF_VTG1_ADDR 	 0x3F1U // Select which local MFP PCLK is outputted... 
#define PCLK_GPIO_REF_VTG_REF_VTG1_MASK 	 0x3EU
#define PCLK_GPIO_REF_VTG_REF_VTG1_POS  	 1U

#define RCLKEN_Y_REF_VTG_REF_VTG1_ADDR 	 0x3F1U // Select between REFGEN_PLL output and RCL... 
#define RCLKEN_Y_REF_VTG_REF_VTG1_MASK 	 0x80U
#define RCLKEN_Y_REF_VTG_REF_VTG1_POS  	 7U

#define REF_VTG_REF_VTG2_ADDR    	 0x3F2U
#define REF_VTG_REF_VTG2_DEFAULT 	 0x00U

#define HSEN_REF_VTG_REF_VTG2_ADDR 	 0x3F2U // Enable output of HS on local MFP selecte... 
#define HSEN_REF_VTG_REF_VTG2_MASK 	 0x01U
#define HSEN_REF_VTG_REF_VTG2_POS  	 0U

#define HS_GPIO_REF_VTG_REF_VTG2_ADDR 	 0x3F2U // Select which local MFP HS is outputted o... 
#define HS_GPIO_REF_VTG_REF_VTG2_MASK 	 0x3EU
#define HS_GPIO_REF_VTG_REF_VTG2_POS  	 1U

#define REF_VTG_REF_VTG3_ADDR    	 0x3F3U
#define REF_VTG_REF_VTG3_DEFAULT 	 0x00U

#define VSEN_REF_VTG_REF_VTG3_ADDR 	 0x3F3U // Enable output of VS on local MFP selecte... 
#define VSEN_REF_VTG_REF_VTG3_MASK 	 0x01U
#define VSEN_REF_VTG_REF_VTG3_POS  	 0U

#define VS_GPIO_REF_VTG_REF_VTG3_ADDR 	 0x3F3U // Select which local MFP VS is outputted o... 
#define VS_GPIO_REF_VTG_REF_VTG3_MASK 	 0x3EU
#define VS_GPIO_REF_VTG_REF_VTG3_POS  	 1U

#define REF_VTG_REF_VTG4_ADDR    	 0x3F4U
#define REF_VTG_REF_VTG4_DEFAULT 	 0x00U

#define REFGEN_FB_FRACT_L_REF_VTG_REF_VTG4_ADDR 	 0x3F4U // Reference generator PLL feedback divider... 
#define REFGEN_FB_FRACT_L_REF_VTG_REF_VTG4_MASK 	 0xFFU
#define REFGEN_FB_FRACT_L_REF_VTG_REF_VTG4_POS  	 0U

#define REF_VTG_REF_VTG5_ADDR    	 0x3F5U
#define REF_VTG_REF_VTG5_DEFAULT 	 0x00U

#define REFGEN_FB_FRACT_H_REF_VTG_REF_VTG5_ADDR 	 0x3F5U // Reference generator PLL feedback divider... 
#define REFGEN_FB_FRACT_H_REF_VTG_REF_VTG5_MASK 	 0x0FU
#define REFGEN_FB_FRACT_H_REF_VTG_REF_VTG5_POS  	 0U

#define REF_VTG_REF_VTG6_ADDR    	 0x3F6U
#define REF_VTG_REF_VTG6_DEFAULT 	 0x00U

#define VS_DLY_2_REF_VTG_REF_VTG6_ADDR 	 0x3F6U // VS Delay in terms of pixel clock cycles.... 
#define VS_DLY_2_REF_VTG_REF_VTG6_MASK 	 0xFFU
#define VS_DLY_2_REF_VTG_REF_VTG6_POS  	 0U

#define REF_VTG_REF_VTG7_ADDR    	 0x3F7U
#define REF_VTG_REF_VTG7_DEFAULT 	 0x00U

#define VS_DLY_1_REF_VTG_REF_VTG7_ADDR 	 0x3F7U // VS Delay in terms of pixel clock cycles.... 
#define VS_DLY_1_REF_VTG_REF_VTG7_MASK 	 0xFFU
#define VS_DLY_1_REF_VTG_REF_VTG7_POS  	 0U

#define REF_VTG_REF_VTG8_ADDR    	 0x3F8U
#define REF_VTG_REF_VTG8_DEFAULT 	 0x00U

#define VS_DLY_0_REF_VTG_REF_VTG8_ADDR 	 0x3F8U // VS Delay in terms of pixel clock cycles.... 
#define VS_DLY_0_REF_VTG_REF_VTG8_MASK 	 0xFFU
#define VS_DLY_0_REF_VTG_REF_VTG8_POS  	 0U

#define REF_VTG_REF_VTG9_ADDR    	 0x3F9U
#define REF_VTG_REF_VTG9_DEFAULT 	 0x1EU

#define REF_VTG_TRIG_ID_REF_VTG_REF_VTG9_ADDR 	 0x3F9U // GPIO ID used for receiving REF_VTG_TRIG  
#define REF_VTG_TRIG_ID_REF_VTG_REF_VTG9_MASK 	 0x1FU
#define REF_VTG_TRIG_ID_REF_VTG_REF_VTG9_POS  	 0U

#define REF_VTG_TRIG_EN_REF_VTG_REF_VTG9_ADDR 	 0x3F9U // Enable receiving REF VTG trigger signal  
#define REF_VTG_TRIG_EN_REF_VTG_REF_VTG9_MASK 	 0x80U
#define REF_VTG_TRIG_EN_REF_VTG_REF_VTG9_POS  	 7U

#define AFE_ADC_CTRL_0_ADDR    	 0x500U
#define AFE_ADC_CTRL_0_DEFAULT 	 0x00U

#define CPU_ADC_START_AFE_ADC_CTRL_0_ADDR 	 0x500U // Start ADC conversion. Bit is automatical... 
#define CPU_ADC_START_AFE_ADC_CTRL_0_MASK 	 0x01U
#define CPU_ADC_START_AFE_ADC_CTRL_0_POS  	 0U

#define ADC_PU_AFE_ADC_CTRL_0_ADDR 	 0x500U // ADC power up  
#define ADC_PU_AFE_ADC_CTRL_0_MASK 	 0x02U
#define ADC_PU_AFE_ADC_CTRL_0_POS  	 1U

#define BUF_PU_AFE_ADC_CTRL_0_ADDR 	 0x500U // ADC input buffer power up  
#define BUF_PU_AFE_ADC_CTRL_0_MASK 	 0x04U
#define BUF_PU_AFE_ADC_CTRL_0_POS  	 2U

#define ADC_REFBUF_PU_AFE_ADC_CTRL_0_ADDR 	 0x500U // ADC reference buffer power up  
#define ADC_REFBUF_PU_AFE_ADC_CTRL_0_MASK 	 0x08U
#define ADC_REFBUF_PU_AFE_ADC_CTRL_0_POS  	 3U

#define ADC_CHGPUMP_PU_AFE_ADC_CTRL_0_ADDR 	 0x500U // ADC charge pump power up  
#define ADC_CHGPUMP_PU_AFE_ADC_CTRL_0_MASK 	 0x10U
#define ADC_CHGPUMP_PU_AFE_ADC_CTRL_0_POS  	 4U

#define BUF_BYPASS_AFE_ADC_CTRL_0_ADDR 	 0x500U // Bypass input buffer  
#define BUF_BYPASS_AFE_ADC_CTRL_0_MASK 	 0x80U
#define BUF_BYPASS_AFE_ADC_CTRL_0_POS  	 7U

#define AFE_ADC_CTRL_1_ADDR    	 0x501U
#define AFE_ADC_CTRL_1_DEFAULT 	 0x00U

#define ADC_SCALE_AFE_ADC_CTRL_1_ADDR 	 0x501U // ADC scale  
#define ADC_SCALE_AFE_ADC_CTRL_1_MASK 	 0x02U
#define ADC_SCALE_AFE_ADC_CTRL_1_POS  	 1U

#define ADC_REFSEL_AFE_ADC_CTRL_1_ADDR 	 0x501U // ADC reference voltage select  
#define ADC_REFSEL_AFE_ADC_CTRL_1_MASK 	 0x04U
#define ADC_REFSEL_AFE_ADC_CTRL_1_POS  	 2U

#define ADC_CLK_EN_AFE_ADC_CTRL_1_ADDR 	 0x501U // ADC clock enable. Must be enabled to act... 
#define ADC_CLK_EN_AFE_ADC_CTRL_1_MASK 	 0x08U
#define ADC_CLK_EN_AFE_ADC_CTRL_1_POS  	 3U

#define ADC_CHSEL_AFE_ADC_CTRL_1_ADDR 	 0x501U // ADC channel select. Selects ADC input to... 
#define ADC_CHSEL_AFE_ADC_CTRL_1_MASK 	 0xF0U
#define ADC_CHSEL_AFE_ADC_CTRL_1_POS  	 4U

#define AFE_ADC_CTRL_2_ADDR    	 0x502U
#define AFE_ADC_CTRL_2_DEFAULT 	 0x00U

#define INMUX_EN_AFE_ADC_CTRL_2_ADDR 	 0x502U // Enable the input mux to the ADC to allow... 
#define INMUX_EN_AFE_ADC_CTRL_2_MASK 	 0x01U
#define INMUX_EN_AFE_ADC_CTRL_2_POS  	 0U

#define ADC_XREF_AFE_ADC_CTRL_2_ADDR 	 0x502U // Enable use of ADC external reference  
#define ADC_XREF_AFE_ADC_CTRL_2_MASK 	 0x02U
#define ADC_XREF_AFE_ADC_CTRL_2_POS  	 1U

#define ADC_DIV_AFE_ADC_CTRL_2_ADDR 	 0x502U // ADC[2:0] internal divider setting  
#define ADC_DIV_AFE_ADC_CTRL_2_MASK 	 0x0CU
#define ADC_DIV_AFE_ADC_CTRL_2_POS  	 2U

#define AFE_ADC_DATA0_ADDR    	 0x508U
#define AFE_ADC_DATA0_DEFAULT 	 0x00U

#define ADC_DATA_L_AFE_ADC_DATA0_ADDR 	 0x508U // Lower byte of 10-bit ADC converted sampl... 
#define ADC_DATA_L_AFE_ADC_DATA0_MASK 	 0xFFU
#define ADC_DATA_L_AFE_ADC_DATA0_POS  	 0U

#define AFE_ADC_DATA1_ADDR    	 0x509U
#define AFE_ADC_DATA1_DEFAULT 	 0x00U

#define ADC_DATA_H_AFE_ADC_DATA1_ADDR 	 0x509U // Upper 2-bits of 10-bit ADC converted sam... 
#define ADC_DATA_H_AFE_ADC_DATA1_MASK 	 0x03U
#define ADC_DATA_H_AFE_ADC_DATA1_POS  	 0U

#define AFE_ADC_INTRIE0_ADDR    	 0x50CU
#define AFE_ADC_INTRIE0_DEFAULT 	 0x00U

#define ADC_DONE_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // Enable ADC Conversion Done Interrupt. Ne... 
#define ADC_DONE_IE_AFE_ADC_INTRIE0_MASK 	 0x01U
#define ADC_DONE_IE_AFE_ADC_INTRIE0_POS  	 0U

#define ADC_REF_READY_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // Enable ADC ready interrupt. Need to also... 
#define ADC_REF_READY_IE_AFE_ADC_INTRIE0_MASK 	 0x02U
#define ADC_REF_READY_IE_AFE_ADC_INTRIE0_POS  	 1U

#define ADC_HI_LIMIT_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // Enable ADC high limit monitor interrupt.... 
#define ADC_HI_LIMIT_IE_AFE_ADC_INTRIE0_MASK 	 0x04U
#define ADC_HI_LIMIT_IE_AFE_ADC_INTRIE0_POS  	 2U

#define ADC_LO_LIMIT_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // Enable ADC low limit monitor interrupt. ... 
#define ADC_LO_LIMIT_IE_AFE_ADC_INTRIE0_MASK 	 0x08U
#define ADC_LO_LIMIT_IE_AFE_ADC_INTRIE0_POS  	 3U

#define ADC_TMON_CAL_OOD_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // Enable temperature sensor out-of-date in... 
#define ADC_TMON_CAL_OOD_IE_AFE_ADC_INTRIE0_MASK 	 0x20U
#define ADC_TMON_CAL_OOD_IE_AFE_ADC_INTRIE0_POS  	 5U

#define ADC_OVERRANGE_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // ADC Digital Correction Overrange enabled... 
#define ADC_OVERRANGE_IE_AFE_ADC_INTRIE0_MASK 	 0x40U
#define ADC_OVERRANGE_IE_AFE_ADC_INTRIE0_POS  	 6U

#define ADC_CALDONE_IE_AFE_ADC_INTRIE0_ADDR 	 0x50CU // Signal that ADC accuracy/temperature sen... 
#define ADC_CALDONE_IE_AFE_ADC_INTRIE0_MASK 	 0x80U
#define ADC_CALDONE_IE_AFE_ADC_INTRIE0_POS  	 7U

#define AFE_ADC_INTRIE1_ADDR    	 0x50DU
#define AFE_ADC_INTRIE1_DEFAULT 	 0x00U

#define CH0_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 0 high limit monitor inte... 
#define CH0_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x01U
#define CH0_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 0U

#define CH1_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 1 high limit monitor inte... 
#define CH1_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x02U
#define CH1_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 1U

#define CH2_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 2 high limit monitor inte... 
#define CH2_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x04U
#define CH2_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 2U

#define CH3_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 3 high limit monitor inte... 
#define CH3_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x08U
#define CH3_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 3U

#define CH4_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 4 high limit monitor inte... 
#define CH4_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x10U
#define CH4_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 4U

#define CH5_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 5 high limit monitor inte... 
#define CH5_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x20U
#define CH5_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 5U

#define CH6_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 6 high limit monitor inte... 
#define CH6_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x40U
#define CH6_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 6U

#define CH7_HI_LIMIT_IE_AFE_ADC_INTRIE1_ADDR 	 0x50DU // Enable Channel 7 high limit monitor inte... 
#define CH7_HI_LIMIT_IE_AFE_ADC_INTRIE1_MASK 	 0x80U
#define CH7_HI_LIMIT_IE_AFE_ADC_INTRIE1_POS  	 7U

#define AFE_ADC_INTRIE2_ADDR    	 0x50EU
#define AFE_ADC_INTRIE2_DEFAULT 	 0x00U

#define CH0_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 0 low limit monitor inter... 
#define CH0_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x01U
#define CH0_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 0U

#define CH1_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 1 low limit monitor inter... 
#define CH1_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x02U
#define CH1_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 1U

#define CH2_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 2 low limit monitor inter... 
#define CH2_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x04U
#define CH2_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 2U

#define CH3_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 3 low limit monitor inter... 
#define CH3_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x08U
#define CH3_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 3U

#define CH4_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 4 low limit monitor inter... 
#define CH4_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x10U
#define CH4_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 4U

#define CH5_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 5 low limit monitor inter... 
#define CH5_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x20U
#define CH5_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 5U

#define CH6_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 6 low limit monitor inter... 
#define CH6_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x40U
#define CH6_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 6U

#define CH7_LO_LIMIT_IE_AFE_ADC_INTRIE2_ADDR 	 0x50EU // Enable Channel 7 low limit monitor inter... 
#define CH7_LO_LIMIT_IE_AFE_ADC_INTRIE2_MASK 	 0x80U
#define CH7_LO_LIMIT_IE_AFE_ADC_INTRIE2_POS  	 7U

#define AFE_ADC_INTRIE3_ADDR    	 0x50FU
#define AFE_ADC_INTRIE3_DEFAULT 	 0x00U

#define TMON_ERR_IE_AFE_ADC_INTRIE3_ADDR 	 0x50FU // Enable the temperature sensor error inte... 
#define TMON_ERR_IE_AFE_ADC_INTRIE3_MASK 	 0x02U
#define TMON_ERR_IE_AFE_ADC_INTRIE3_POS  	 1U

#define REFLIMSCL3_IE_AFE_ADC_INTRIE3_ADDR 	 0x50FU // Enable the REFLIMSCL3 interrupt (for ADC... 
#define REFLIMSCL3_IE_AFE_ADC_INTRIE3_MASK 	 0x08U
#define REFLIMSCL3_IE_AFE_ADC_INTRIE3_POS  	 3U

#define REFLIMSCL2_IE_AFE_ADC_INTRIE3_ADDR 	 0x50FU // Enable the REFLIMSCL2 interrupt (for ADC... 
#define REFLIMSCL2_IE_AFE_ADC_INTRIE3_MASK 	 0x10U
#define REFLIMSCL2_IE_AFE_ADC_INTRIE3_POS  	 4U

#define REFLIMSCL1_IE_AFE_ADC_INTRIE3_ADDR 	 0x50FU // Enable the REFLIMSCL1 interrupt (for ADC... 
#define REFLIMSCL1_IE_AFE_ADC_INTRIE3_MASK 	 0x20U
#define REFLIMSCL1_IE_AFE_ADC_INTRIE3_POS  	 5U

#define REFLIM_IE_AFE_ADC_INTRIE3_ADDR 	 0x50FU // Enable the REFLIM interrupt (for ADC BIS... 
#define REFLIM_IE_AFE_ADC_INTRIE3_MASK 	 0x40U
#define REFLIM_IE_AFE_ADC_INTRIE3_POS  	 6U

#define AFE_ADC_INTR0_ADDR    	 0x510U
#define AFE_ADC_INTR0_DEFAULT 	 0x00U

#define ADC_DONE_IF_AFE_ADC_INTR0_ADDR 	 0x510U // ADC conversion done interrupt flag. Clea... 
#define ADC_DONE_IF_AFE_ADC_INTR0_MASK 	 0x01U
#define ADC_DONE_IF_AFE_ADC_INTR0_POS  	 0U

#define ADC_REF_READY_IF_AFE_ADC_INTR0_ADDR 	 0x510U // After powerup the ADC is ready to be use... 
#define ADC_REF_READY_IF_AFE_ADC_INTR0_MASK 	 0x02U
#define ADC_REF_READY_IF_AFE_ADC_INTR0_POS  	 1U

#define ADC_HI_LIMIT_IF_AFE_ADC_INTR0_ADDR 	 0x510U // ADC high limit monitor interrupt flag.  ... 
#define ADC_HI_LIMIT_IF_AFE_ADC_INTR0_MASK 	 0x04U
#define ADC_HI_LIMIT_IF_AFE_ADC_INTR0_POS  	 2U

#define ADC_LO_LIMIT_IF_AFE_ADC_INTR0_ADDR 	 0x510U // ADC low limit monitor interrupt flag.  C... 
#define ADC_LO_LIMIT_IF_AFE_ADC_INTR0_MASK 	 0x08U
#define ADC_LO_LIMIT_IF_AFE_ADC_INTR0_POS  	 3U

#define ADC_TMON_CAL_OOD_IF_AFE_ADC_INTR0_ADDR 	 0x510U // Temperature sensor calibration has expir... 
#define ADC_TMON_CAL_OOD_IF_AFE_ADC_INTR0_MASK 	 0x20U
#define ADC_TMON_CAL_OOD_IF_AFE_ADC_INTR0_POS  	 5U

#define ADC_OVERRANGE_IF_AFE_ADC_INTR0_ADDR 	 0x510U // Detected that ADC input voltage exceeds ... 
#define ADC_OVERRANGE_IF_AFE_ADC_INTR0_MASK 	 0x40U
#define ADC_OVERRANGE_IF_AFE_ADC_INTR0_POS  	 6U

#define ADC_CALDONE_IF_AFE_ADC_INTR0_ADDR 	 0x510U // ADC accuracy/temperature sensor done fla... 
#define ADC_CALDONE_IF_AFE_ADC_INTR0_MASK 	 0x80U
#define ADC_CALDONE_IF_AFE_ADC_INTR0_POS  	 7U

#define AFE_ADC_INTR1_ADDR    	 0x511U
#define AFE_ADC_INTR1_DEFAULT 	 0x00U

#define CH0_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 0 high limit monitor interrupt f... 
#define CH0_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x01U
#define CH0_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 0U

#define CH1_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 1 high limit monitor interrupt f... 
#define CH1_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x02U
#define CH1_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 1U

#define CH2_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 2 high limit monitor interrupt f... 
#define CH2_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x04U
#define CH2_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 2U

#define CH3_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 3 high limit monitor interrupt f... 
#define CH3_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x08U
#define CH3_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 3U

#define CH4_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 4 high limit monitor interrupt f... 
#define CH4_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x10U
#define CH4_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 4U

#define CH5_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 5 high limit monitor interrupt f... 
#define CH5_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x20U
#define CH5_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 5U

#define CH6_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 6 high limit monitor interrupt f... 
#define CH6_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x40U
#define CH6_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 6U

#define CH7_HI_LIMIT_IF_AFE_ADC_INTR1_ADDR 	 0x511U // Channel 7 high limit monitor interrupt f... 
#define CH7_HI_LIMIT_IF_AFE_ADC_INTR1_MASK 	 0x80U
#define CH7_HI_LIMIT_IF_AFE_ADC_INTR1_POS  	 7U

#define AFE_ADC_INTR2_ADDR    	 0x512U
#define AFE_ADC_INTR2_DEFAULT 	 0x00U

#define CH0_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 0 low limit monitor interrupt fl... 
#define CH0_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x01U
#define CH0_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 0U

#define CH1_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 1 low limit monitor interrupt fl... 
#define CH1_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x02U
#define CH1_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 1U

#define CH2_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 2 low limit monitor interrupt fl... 
#define CH2_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x04U
#define CH2_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 2U

#define CH3_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 3 low limit monitor interrupt fl... 
#define CH3_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x08U
#define CH3_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 3U

#define CH4_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 4 low limit monitor interrupt fl... 
#define CH4_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x10U
#define CH4_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 4U

#define CH5_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 5 low limit monitor interrupt fl... 
#define CH5_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x20U
#define CH5_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 5U

#define CH6_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 6 low limit monitor interrupt fl... 
#define CH6_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x40U
#define CH6_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 6U

#define CH7_LO_LIMIT_IF_AFE_ADC_INTR2_ADDR 	 0x512U // Channel 7 low limit monitor interrupt fl... 
#define CH7_LO_LIMIT_IF_AFE_ADC_INTR2_MASK 	 0x80U
#define CH7_LO_LIMIT_IF_AFE_ADC_INTR2_POS  	 7U

#define AFE_ADC_INTR3_ADDR    	 0x513U
#define AFE_ADC_INTR3_DEFAULT 	 0x00U

#define TMON_ERR_IF_AFE_ADC_INTR3_ADDR 	 0x513U // Discrepancy between temperature sensor a... 
#define TMON_ERR_IF_AFE_ADC_INTR3_MASK 	 0x02U
#define TMON_ERR_IF_AFE_ADC_INTR3_POS  	 1U

#define REFLIMSCL3_IF_AFE_ADC_INTR3_ADDR 	 0x513U // Returned value from ADC BIST test (divid... 
#define REFLIMSCL3_IF_AFE_ADC_INTR3_MASK 	 0x08U
#define REFLIMSCL3_IF_AFE_ADC_INTR3_POS  	 3U

#define REFLIMSCL2_IF_AFE_ADC_INTR3_ADDR 	 0x513U // Returned value from ADC BIST test (divid... 
#define REFLIMSCL2_IF_AFE_ADC_INTR3_MASK 	 0x10U
#define REFLIMSCL2_IF_AFE_ADC_INTR3_POS  	 4U

#define REFLIMSCL1_IF_AFE_ADC_INTR3_ADDR 	 0x513U // Returned value from ADC BIST test (divid... 
#define REFLIMSCL1_IF_AFE_ADC_INTR3_MASK 	 0x20U
#define REFLIMSCL1_IF_AFE_ADC_INTR3_POS  	 5U

#define REFLIM_IF_AFE_ADC_INTR3_ADDR 	 0x513U // Returned value from ADC BIST test (divid... 
#define REFLIM_IF_AFE_ADC_INTR3_MASK 	 0x40U
#define REFLIM_IF_AFE_ADC_INTR3_POS  	 6U

#define AFE_ADC_LIMIT0_0_ADDR    	 0x514U
#define AFE_ADC_LIMIT0_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L0_AFE_ADC_LIMIT0_0_ADDR 	 0x514U // ADC Output Register set 0 - low limit th... 
#define CHLOLIMIT_L0_AFE_ADC_LIMIT0_0_MASK 	 0xFFU
#define CHLOLIMIT_L0_AFE_ADC_LIMIT0_0_POS  	 0U

#define AFE_ADC_LIMIT0_1_ADDR    	 0x515U
#define AFE_ADC_LIMIT0_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H0_AFE_ADC_LIMIT0_1_ADDR 	 0x515U // ADC Output Register set 0 - low limit th... 
#define CHLOLIMIT_H0_AFE_ADC_LIMIT0_1_MASK 	 0x03U
#define CHLOLIMIT_H0_AFE_ADC_LIMIT0_1_POS  	 0U

#define CHHILIMIT_L0_AFE_ADC_LIMIT0_1_ADDR 	 0x515U // ADC Output Register set 0 - high limit t... 
#define CHHILIMIT_L0_AFE_ADC_LIMIT0_1_MASK 	 0xF0U
#define CHHILIMIT_L0_AFE_ADC_LIMIT0_1_POS  	 4U

#define AFE_ADC_LIMIT0_2_ADDR    	 0x516U
#define AFE_ADC_LIMIT0_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H0_AFE_ADC_LIMIT0_2_ADDR 	 0x516U // ADC Output Register set 0 - high limit t... 
#define CHHILIMIT_H0_AFE_ADC_LIMIT0_2_MASK 	 0x3FU
#define CHHILIMIT_H0_AFE_ADC_LIMIT0_2_POS  	 0U

#define AFE_ADC_LIMIT0_3_ADDR    	 0x517U
#define AFE_ADC_LIMIT0_3_DEFAULT 	 0x03U

#define CH_SEL0_AFE_ADC_LIMIT0_3_ADDR 	 0x517U // ADC Input Select for ADC Output Register... 
#define CH_SEL0_AFE_ADC_LIMIT0_3_MASK 	 0x0FU
#define CH_SEL0_AFE_ADC_LIMIT0_3_POS  	 0U

#define DIV_SEL0_AFE_ADC_LIMIT0_3_ADDR 	 0x517U // ADC channel 0 divider setting  
#define DIV_SEL0_AFE_ADC_LIMIT0_3_MASK 	 0x30U
#define DIV_SEL0_AFE_ADC_LIMIT0_3_POS  	 4U

#define AFE_ADC_LIMIT1_0_ADDR    	 0x518U
#define AFE_ADC_LIMIT1_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L1_AFE_ADC_LIMIT1_0_ADDR 	 0x518U // ADC Output Register set 1 - low limit th... 
#define CHLOLIMIT_L1_AFE_ADC_LIMIT1_0_MASK 	 0xFFU
#define CHLOLIMIT_L1_AFE_ADC_LIMIT1_0_POS  	 0U

#define AFE_ADC_LIMIT1_1_ADDR    	 0x519U
#define AFE_ADC_LIMIT1_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H1_AFE_ADC_LIMIT1_1_ADDR 	 0x519U // ADC Output Register set 1 - low limit th... 
#define CHLOLIMIT_H1_AFE_ADC_LIMIT1_1_MASK 	 0x03U
#define CHLOLIMIT_H1_AFE_ADC_LIMIT1_1_POS  	 0U

#define CHHILIMIT_L1_AFE_ADC_LIMIT1_1_ADDR 	 0x519U // ADC Output Register set 1 - high limit t... 
#define CHHILIMIT_L1_AFE_ADC_LIMIT1_1_MASK 	 0xF0U
#define CHHILIMIT_L1_AFE_ADC_LIMIT1_1_POS  	 4U

#define AFE_ADC_LIMIT1_2_ADDR    	 0x51AU
#define AFE_ADC_LIMIT1_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H1_AFE_ADC_LIMIT1_2_ADDR 	 0x51AU // ADC Output Register set 1 - high limit t... 
#define CHHILIMIT_H1_AFE_ADC_LIMIT1_2_MASK 	 0x3FU
#define CHHILIMIT_H1_AFE_ADC_LIMIT1_2_POS  	 0U

#define AFE_ADC_LIMIT1_3_ADDR    	 0x51BU
#define AFE_ADC_LIMIT1_3_DEFAULT 	 0x03U

#define CH_SEL1_AFE_ADC_LIMIT1_3_ADDR 	 0x51BU // ADC Input Select for ADC Output Register... 
#define CH_SEL1_AFE_ADC_LIMIT1_3_MASK 	 0x0FU
#define CH_SEL1_AFE_ADC_LIMIT1_3_POS  	 0U

#define DIV_SEL1_AFE_ADC_LIMIT1_3_ADDR 	 0x51BU // ADC channel 1 divider setting  
#define DIV_SEL1_AFE_ADC_LIMIT1_3_MASK 	 0x30U
#define DIV_SEL1_AFE_ADC_LIMIT1_3_POS  	 4U

#define AFE_ADC_LIMIT2_0_ADDR    	 0x51CU
#define AFE_ADC_LIMIT2_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L2_AFE_ADC_LIMIT2_0_ADDR 	 0x51CU // ADC Output Register set 2 - low limit th... 
#define CHLOLIMIT_L2_AFE_ADC_LIMIT2_0_MASK 	 0xFFU
#define CHLOLIMIT_L2_AFE_ADC_LIMIT2_0_POS  	 0U

#define AFE_ADC_LIMIT2_1_ADDR    	 0x51DU
#define AFE_ADC_LIMIT2_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H2_AFE_ADC_LIMIT2_1_ADDR 	 0x51DU // ADC Output Register set 2 - low limit th... 
#define CHLOLIMIT_H2_AFE_ADC_LIMIT2_1_MASK 	 0x03U
#define CHLOLIMIT_H2_AFE_ADC_LIMIT2_1_POS  	 0U

#define CHHILIMIT_L2_AFE_ADC_LIMIT2_1_ADDR 	 0x51DU // ADC Output Register set 2 - high limit t... 
#define CHHILIMIT_L2_AFE_ADC_LIMIT2_1_MASK 	 0xF0U
#define CHHILIMIT_L2_AFE_ADC_LIMIT2_1_POS  	 4U

#define AFE_ADC_LIMIT2_2_ADDR    	 0x51EU
#define AFE_ADC_LIMIT2_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H2_AFE_ADC_LIMIT2_2_ADDR 	 0x51EU // ADC Output Register set 2 - high limit t... 
#define CHHILIMIT_H2_AFE_ADC_LIMIT2_2_MASK 	 0x3FU
#define CHHILIMIT_H2_AFE_ADC_LIMIT2_2_POS  	 0U

#define AFE_ADC_LIMIT2_3_ADDR    	 0x51FU
#define AFE_ADC_LIMIT2_3_DEFAULT 	 0x03U

#define CH_SEL2_AFE_ADC_LIMIT2_3_ADDR 	 0x51FU // ADC Input Select for ADC Output Register... 
#define CH_SEL2_AFE_ADC_LIMIT2_3_MASK 	 0x0FU
#define CH_SEL2_AFE_ADC_LIMIT2_3_POS  	 0U

#define DIV_SEL2_AFE_ADC_LIMIT2_3_ADDR 	 0x51FU // ADC channel 2 divider setting  
#define DIV_SEL2_AFE_ADC_LIMIT2_3_MASK 	 0x30U
#define DIV_SEL2_AFE_ADC_LIMIT2_3_POS  	 4U

#define AFE_ADC_LIMIT3_0_ADDR    	 0x520U
#define AFE_ADC_LIMIT3_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L3_AFE_ADC_LIMIT3_0_ADDR 	 0x520U // ADC Output Register set 3 - low limit th... 
#define CHLOLIMIT_L3_AFE_ADC_LIMIT3_0_MASK 	 0xFFU
#define CHLOLIMIT_L3_AFE_ADC_LIMIT3_0_POS  	 0U

#define AFE_ADC_LIMIT3_1_ADDR    	 0x521U
#define AFE_ADC_LIMIT3_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H3_AFE_ADC_LIMIT3_1_ADDR 	 0x521U // ADC Output Register set 3 - low limit th... 
#define CHLOLIMIT_H3_AFE_ADC_LIMIT3_1_MASK 	 0x03U
#define CHLOLIMIT_H3_AFE_ADC_LIMIT3_1_POS  	 0U

#define CHHILIMIT_L3_AFE_ADC_LIMIT3_1_ADDR 	 0x521U // ADC Output Register set 3 - high limit t... 
#define CHHILIMIT_L3_AFE_ADC_LIMIT3_1_MASK 	 0xF0U
#define CHHILIMIT_L3_AFE_ADC_LIMIT3_1_POS  	 4U

#define AFE_ADC_LIMIT3_2_ADDR    	 0x522U
#define AFE_ADC_LIMIT3_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H3_AFE_ADC_LIMIT3_2_ADDR 	 0x522U // ADC Output Register set 3 - high limit t... 
#define CHHILIMIT_H3_AFE_ADC_LIMIT3_2_MASK 	 0x3FU
#define CHHILIMIT_H3_AFE_ADC_LIMIT3_2_POS  	 0U

#define AFE_ADC_LIMIT3_3_ADDR    	 0x523U
#define AFE_ADC_LIMIT3_3_DEFAULT 	 0x03U

#define CH_SEL3_AFE_ADC_LIMIT3_3_ADDR 	 0x523U // ADC Input Select for ADC Output Register... 
#define CH_SEL3_AFE_ADC_LIMIT3_3_MASK 	 0x0FU
#define CH_SEL3_AFE_ADC_LIMIT3_3_POS  	 0U

#define DIV_SEL3_AFE_ADC_LIMIT3_3_ADDR 	 0x523U // ADC channel 3 divider setting  
#define DIV_SEL3_AFE_ADC_LIMIT3_3_MASK 	 0x30U
#define DIV_SEL3_AFE_ADC_LIMIT3_3_POS  	 4U

#define AFE_ADC_LIMIT4_0_ADDR    	 0x524U
#define AFE_ADC_LIMIT4_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L4_AFE_ADC_LIMIT4_0_ADDR 	 0x524U // ADC Output Register set 4 - low limit th... 
#define CHLOLIMIT_L4_AFE_ADC_LIMIT4_0_MASK 	 0xFFU
#define CHLOLIMIT_L4_AFE_ADC_LIMIT4_0_POS  	 0U

#define AFE_ADC_LIMIT4_1_ADDR    	 0x525U
#define AFE_ADC_LIMIT4_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H4_AFE_ADC_LIMIT4_1_ADDR 	 0x525U // ADC Output Register set 4 - low limit th... 
#define CHLOLIMIT_H4_AFE_ADC_LIMIT4_1_MASK 	 0x03U
#define CHLOLIMIT_H4_AFE_ADC_LIMIT4_1_POS  	 0U

#define CHHILIMIT_L4_AFE_ADC_LIMIT4_1_ADDR 	 0x525U // ADC Output Register set 4 - high limit t... 
#define CHHILIMIT_L4_AFE_ADC_LIMIT4_1_MASK 	 0xF0U
#define CHHILIMIT_L4_AFE_ADC_LIMIT4_1_POS  	 4U

#define AFE_ADC_LIMIT4_2_ADDR    	 0x526U
#define AFE_ADC_LIMIT4_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H4_AFE_ADC_LIMIT4_2_ADDR 	 0x526U // ADC Output Register set 4 - high limit t... 
#define CHHILIMIT_H4_AFE_ADC_LIMIT4_2_MASK 	 0x3FU
#define CHHILIMIT_H4_AFE_ADC_LIMIT4_2_POS  	 0U

#define AFE_ADC_LIMIT4_3_ADDR    	 0x527U
#define AFE_ADC_LIMIT4_3_DEFAULT 	 0x03U

#define CH_SEL4_AFE_ADC_LIMIT4_3_ADDR 	 0x527U // ADC Input Select for ADC Output Register... 
#define CH_SEL4_AFE_ADC_LIMIT4_3_MASK 	 0x0FU
#define CH_SEL4_AFE_ADC_LIMIT4_3_POS  	 0U

#define DIV_SEL4_AFE_ADC_LIMIT4_3_ADDR 	 0x527U // ADC channel 4 divider setting  
#define DIV_SEL4_AFE_ADC_LIMIT4_3_MASK 	 0x30U
#define DIV_SEL4_AFE_ADC_LIMIT4_3_POS  	 4U

#define AFE_ADC_LIMIT5_0_ADDR    	 0x528U
#define AFE_ADC_LIMIT5_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L5_AFE_ADC_LIMIT5_0_ADDR 	 0x528U // ADC Output Register set 5 - low limit th... 
#define CHLOLIMIT_L5_AFE_ADC_LIMIT5_0_MASK 	 0xFFU
#define CHLOLIMIT_L5_AFE_ADC_LIMIT5_0_POS  	 0U

#define AFE_ADC_LIMIT5_1_ADDR    	 0x529U
#define AFE_ADC_LIMIT5_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H5_AFE_ADC_LIMIT5_1_ADDR 	 0x529U // ADC Output Register set 5 - low limit th... 
#define CHLOLIMIT_H5_AFE_ADC_LIMIT5_1_MASK 	 0x03U
#define CHLOLIMIT_H5_AFE_ADC_LIMIT5_1_POS  	 0U

#define CHHILIMIT_L5_AFE_ADC_LIMIT5_1_ADDR 	 0x529U // ADC Output Register set 5 - high limit t... 
#define CHHILIMIT_L5_AFE_ADC_LIMIT5_1_MASK 	 0xF0U
#define CHHILIMIT_L5_AFE_ADC_LIMIT5_1_POS  	 4U

#define AFE_ADC_LIMIT5_2_ADDR    	 0x52AU
#define AFE_ADC_LIMIT5_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H5_AFE_ADC_LIMIT5_2_ADDR 	 0x52AU // ADC Output Register set 5 - high limit t... 
#define CHHILIMIT_H5_AFE_ADC_LIMIT5_2_MASK 	 0x3FU
#define CHHILIMIT_H5_AFE_ADC_LIMIT5_2_POS  	 0U

#define AFE_ADC_LIMIT5_3_ADDR    	 0x52BU
#define AFE_ADC_LIMIT5_3_DEFAULT 	 0x03U

#define CH_SEL5_AFE_ADC_LIMIT5_3_ADDR 	 0x52BU // ADC Input Select for ADC Output Register... 
#define CH_SEL5_AFE_ADC_LIMIT5_3_MASK 	 0x0FU
#define CH_SEL5_AFE_ADC_LIMIT5_3_POS  	 0U

#define DIV_SEL5_AFE_ADC_LIMIT5_3_ADDR 	 0x52BU // ADC channel 5 divider setting  
#define DIV_SEL5_AFE_ADC_LIMIT5_3_MASK 	 0x30U
#define DIV_SEL5_AFE_ADC_LIMIT5_3_POS  	 4U

#define AFE_ADC_LIMIT6_0_ADDR    	 0x52CU
#define AFE_ADC_LIMIT6_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L6_AFE_ADC_LIMIT6_0_ADDR 	 0x52CU // ADC Output Register set 6 - low limit th... 
#define CHLOLIMIT_L6_AFE_ADC_LIMIT6_0_MASK 	 0xFFU
#define CHLOLIMIT_L6_AFE_ADC_LIMIT6_0_POS  	 0U

#define AFE_ADC_LIMIT6_1_ADDR    	 0x52DU
#define AFE_ADC_LIMIT6_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H6_AFE_ADC_LIMIT6_1_ADDR 	 0x52DU // ADC Output Register set 6 - low limit th... 
#define CHLOLIMIT_H6_AFE_ADC_LIMIT6_1_MASK 	 0x03U
#define CHLOLIMIT_H6_AFE_ADC_LIMIT6_1_POS  	 0U

#define CHHILIMIT_L6_AFE_ADC_LIMIT6_1_ADDR 	 0x52DU // ADC Output Register set 6 - high limit t... 
#define CHHILIMIT_L6_AFE_ADC_LIMIT6_1_MASK 	 0xF0U
#define CHHILIMIT_L6_AFE_ADC_LIMIT6_1_POS  	 4U

#define AFE_ADC_LIMIT6_2_ADDR    	 0x52EU
#define AFE_ADC_LIMIT6_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H6_AFE_ADC_LIMIT6_2_ADDR 	 0x52EU // ADC Output Register set 5 - high limit t... 
#define CHHILIMIT_H6_AFE_ADC_LIMIT6_2_MASK 	 0x3FU
#define CHHILIMIT_H6_AFE_ADC_LIMIT6_2_POS  	 0U

#define AFE_ADC_LIMIT6_3_ADDR    	 0x52FU
#define AFE_ADC_LIMIT6_3_DEFAULT 	 0x03U

#define CH_SEL6_AFE_ADC_LIMIT6_3_ADDR 	 0x52FU // ADC Input Select for ADC Output Register... 
#define CH_SEL6_AFE_ADC_LIMIT6_3_MASK 	 0x0FU
#define CH_SEL6_AFE_ADC_LIMIT6_3_POS  	 0U

#define DIV_SEL6_AFE_ADC_LIMIT6_3_ADDR 	 0x52FU // ADC channel 6 divider setting  
#define DIV_SEL6_AFE_ADC_LIMIT6_3_MASK 	 0x30U
#define DIV_SEL6_AFE_ADC_LIMIT6_3_POS  	 4U

#define AFE_ADC_LIMIT7_0_ADDR    	 0x530U
#define AFE_ADC_LIMIT7_0_DEFAULT 	 0x00U

#define CHLOLIMIT_L7_AFE_ADC_LIMIT7_0_ADDR 	 0x530U // ADC Output Register set 7 - low limit th... 
#define CHLOLIMIT_L7_AFE_ADC_LIMIT7_0_MASK 	 0xFFU
#define CHLOLIMIT_L7_AFE_ADC_LIMIT7_0_POS  	 0U

#define AFE_ADC_LIMIT7_1_ADDR    	 0x531U
#define AFE_ADC_LIMIT7_1_DEFAULT 	 0xF0U

#define CHLOLIMIT_H7_AFE_ADC_LIMIT7_1_ADDR 	 0x531U // ADC Output Register set 7- low limit thr... 
#define CHLOLIMIT_H7_AFE_ADC_LIMIT7_1_MASK 	 0x03U
#define CHLOLIMIT_H7_AFE_ADC_LIMIT7_1_POS  	 0U

#define CHHILIMIT_L7_AFE_ADC_LIMIT7_1_ADDR 	 0x531U // ADC Output Register set 7 - high limit t... 
#define CHHILIMIT_L7_AFE_ADC_LIMIT7_1_MASK 	 0xF0U
#define CHHILIMIT_L7_AFE_ADC_LIMIT7_1_POS  	 4U

#define AFE_ADC_LIMIT7_2_ADDR    	 0x532U
#define AFE_ADC_LIMIT7_2_DEFAULT 	 0x3FU

#define CHHILIMIT_H7_AFE_ADC_LIMIT7_2_ADDR 	 0x532U // ADC Output Register set 7 - high limit t... 
#define CHHILIMIT_H7_AFE_ADC_LIMIT7_2_MASK 	 0x3FU
#define CHHILIMIT_H7_AFE_ADC_LIMIT7_2_POS  	 0U

#define AFE_ADC_LIMIT7_3_ADDR    	 0x533U
#define AFE_ADC_LIMIT7_3_DEFAULT 	 0x03U

#define CH_SEL7_AFE_ADC_LIMIT7_3_ADDR 	 0x533U // ADC Input Select for ADC Output Register... 
#define CH_SEL7_AFE_ADC_LIMIT7_3_MASK 	 0x0FU
#define CH_SEL7_AFE_ADC_LIMIT7_3_POS  	 0U

#define DIV_SEL7_AFE_ADC_LIMIT7_3_ADDR 	 0x533U // ADC channel 7 divider setting  
#define DIV_SEL7_AFE_ADC_LIMIT7_3_MASK 	 0x30U
#define DIV_SEL7_AFE_ADC_LIMIT7_3_POS  	 4U

#define AFE_ADC_RR_CTRL0_ADDR    	 0x534U
#define AFE_ADC_RR_CTRL0_DEFAULT 	 0x00U

#define ADC_RR_RUN_AFE_ADC_RR_CTRL0_ADDR 	 0x534U // Enable ADC round robin operation  
#define ADC_RR_RUN_AFE_ADC_RR_CTRL0_MASK 	 0x01U
#define ADC_RR_RUN_AFE_ADC_RR_CTRL0_POS  	 0U

#define AFE_ADC_CTRL_4_ADDR    	 0x53EU
#define AFE_ADC_CTRL_4_DEFAULT 	 0x00U

#define ADC_PIN_EN_AFE_ADC_CTRL_4_ADDR 	 0x53EU // Connect selected MFP pins to ADC input m... 
#define ADC_PIN_EN_AFE_ADC_CTRL_4_MASK 	 0x07U
#define ADC_PIN_EN_AFE_ADC_CTRL_4_POS  	 0U

#define MISC_UART_PT_0_ADDR    	 0x548U
#define MISC_UART_PT_0_DEFAULT 	 0xDCU

#define BITLEN_PT_1_L_MISC_UART_PT_0_ADDR 	 0x548U // Low byte of custom (manually configured)... 
#define BITLEN_PT_1_L_MISC_UART_PT_0_MASK 	 0xFFU
#define BITLEN_PT_1_L_MISC_UART_PT_0_POS  	 0U

#define MISC_UART_PT_1_ADDR    	 0x549U
#define MISC_UART_PT_1_DEFAULT 	 0x05U

#define BITLEN_PT_1_H_MISC_UART_PT_1_ADDR 	 0x549U // High byte of custom (manually configured... 
#define BITLEN_PT_1_H_MISC_UART_PT_1_MASK 	 0x3FU
#define BITLEN_PT_1_H_MISC_UART_PT_1_POS  	 0U

#define MISC_UART_PT_2_ADDR    	 0x54AU
#define MISC_UART_PT_2_DEFAULT 	 0xDCU

#define BITLEN_PT_2_L_MISC_UART_PT_2_ADDR 	 0x54AU // Low byte of custom (manually configured)... 
#define BITLEN_PT_2_L_MISC_UART_PT_2_MASK 	 0xFFU
#define BITLEN_PT_2_L_MISC_UART_PT_2_POS  	 0U

#define MISC_UART_PT_3_ADDR    	 0x54BU
#define MISC_UART_PT_3_DEFAULT 	 0x05U

#define BITLEN_PT_2_H_MISC_UART_PT_3_ADDR 	 0x54BU // High byte of custom (manually configured... 
#define BITLEN_PT_2_H_MISC_UART_PT_3_MASK 	 0x3FU
#define BITLEN_PT_2_H_MISC_UART_PT_3_POS  	 0U

#define MISC_I2C_PT_4_ADDR    	 0x550U
#define MISC_I2C_PT_4_DEFAULT 	 0x00U

#define SRC_A_1_MISC_I2C_PT_4_ADDR 	 0x550U // I2C address translator source A for pass... 
#define SRC_A_1_MISC_I2C_PT_4_MASK 	 0xFEU
#define SRC_A_1_MISC_I2C_PT_4_POS  	 1U

#define MISC_I2C_PT_5_ADDR    	 0x551U
#define MISC_I2C_PT_5_DEFAULT 	 0x00U

#define DST_A_1_MISC_I2C_PT_5_ADDR 	 0x551U // I2C address translator destination A for... 
#define DST_A_1_MISC_I2C_PT_5_MASK 	 0xFEU
#define DST_A_1_MISC_I2C_PT_5_POS  	 1U

#define MISC_I2C_PT_6_ADDR    	 0x552U
#define MISC_I2C_PT_6_DEFAULT 	 0x00U

#define SRC_B_1_MISC_I2C_PT_6_ADDR 	 0x552U // I2C address translator source B for pass... 
#define SRC_B_1_MISC_I2C_PT_6_MASK 	 0xFEU
#define SRC_B_1_MISC_I2C_PT_6_POS  	 1U

#define MISC_I2C_PT_7_ADDR    	 0x553U
#define MISC_I2C_PT_7_DEFAULT 	 0x00U

#define DST_B_1_MISC_I2C_PT_7_ADDR 	 0x553U // I2C address translator destination B for... 
#define DST_B_1_MISC_I2C_PT_7_MASK 	 0xFEU
#define DST_B_1_MISC_I2C_PT_7_POS  	 1U

#define MISC_I2C_PT_8_ADDR    	 0x554U
#define MISC_I2C_PT_8_DEFAULT 	 0x00U

#define SRC_A_2_MISC_I2C_PT_8_ADDR 	 0x554U // I2C address translator source A for pass... 
#define SRC_A_2_MISC_I2C_PT_8_MASK 	 0xFEU
#define SRC_A_2_MISC_I2C_PT_8_POS  	 1U

#define MISC_I2C_PT_9_ADDR    	 0x555U
#define MISC_I2C_PT_9_DEFAULT 	 0x00U

#define DST_A_2_MISC_I2C_PT_9_ADDR 	 0x555U // I2C address translator destination A for... 
#define DST_A_2_MISC_I2C_PT_9_MASK 	 0xFEU
#define DST_A_2_MISC_I2C_PT_9_POS  	 1U

#define MISC_I2C_PT_10_ADDR    	 0x556U
#define MISC_I2C_PT_10_DEFAULT 	 0x00U

#define SRC_B_2_MISC_I2C_PT_10_ADDR 	 0x556U // I2C address translator source B for pass... 
#define SRC_B_2_MISC_I2C_PT_10_MASK 	 0xFEU
#define SRC_B_2_MISC_I2C_PT_10_POS  	 1U

#define MISC_I2C_PT_11_ADDR    	 0x557U
#define MISC_I2C_PT_11_DEFAULT 	 0x00U

#define DST_B_2_MISC_I2C_PT_11_ADDR 	 0x557U // I2C address translator destination B for... 
#define DST_B_2_MISC_I2C_PT_11_MASK 	 0xFEU
#define DST_B_2_MISC_I2C_PT_11_POS  	 1U

#define MISC_HS_VS_Z_ADDR    	 0x55FU
#define MISC_HS_VS_Z_DEFAULT 	 0x00U

#define HS_POL_Z_MISC_HS_VS_Z_ADDR 	 0x55FU // Detected HS polarity on video pipeline Z... 
#define HS_POL_Z_MISC_HS_VS_Z_MASK 	 0x01U
#define HS_POL_Z_MISC_HS_VS_Z_POS  	 0U

#define VS_POL_Z_MISC_HS_VS_Z_ADDR 	 0x55FU // Detected VS polarity on video pipeline Z... 
#define VS_POL_Z_MISC_HS_VS_Z_MASK 	 0x02U
#define VS_POL_Z_MISC_HS_VS_Z_POS  	 1U

#define HS_DET_Z_MISC_HS_VS_Z_ADDR 	 0x55FU // HS activity is detected on video pipelin... 
#define HS_DET_Z_MISC_HS_VS_Z_MASK 	 0x10U
#define HS_DET_Z_MISC_HS_VS_Z_POS  	 4U

#define VS_DET_Z_MISC_HS_VS_Z_ADDR 	 0x55FU // VS activity is detected on video pipelin... 
#define VS_DET_Z_MISC_HS_VS_Z_MASK 	 0x20U
#define VS_DET_Z_MISC_HS_VS_Z_POS  	 5U

#define DE_DET_Z_MISC_HS_VS_Z_ADDR 	 0x55FU // DE activity is detected on video pipelin... 
#define DE_DET_Z_MISC_HS_VS_Z_MASK 	 0x40U
#define DE_DET_Z_MISC_HS_VS_Z_POS  	 6U

#define MISC_UNLOCK_KEY_ADDR    	 0x56EU
#define MISC_UNLOCK_KEY_DEFAULT 	 0xBBU

#define UNLOCK_KEY_MISC_UNLOCK_KEY_ADDR 	 0x56EU // Register must be at unlock value to enab... 
#define UNLOCK_KEY_MISC_UNLOCK_KEY_MASK 	 0xFFU
#define UNLOCK_KEY_MISC_UNLOCK_KEY_POS  	 0U

#define MISC_PIO_SLEW_0_ADDR    	 0x56FU
#define MISC_PIO_SLEW_0_DEFAULT 	 0x3EU

#define PIO00_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x56FU // Slew rate setting for MFP0 pin. 00 value... 
#define PIO00_SLEW_MISC_PIO_SLEW_0_MASK 	 0x03U
#define PIO00_SLEW_MISC_PIO_SLEW_0_POS  	 0U

#define PIO01_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x56FU // Slew rate setting for MFP1 pin. 00 value... 
#define PIO01_SLEW_MISC_PIO_SLEW_0_MASK 	 0x0CU
#define PIO01_SLEW_MISC_PIO_SLEW_0_POS  	 2U

#define PIO02_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x56FU // Slew rate setting for MFP2 pin. 00 value... 
#define PIO02_SLEW_MISC_PIO_SLEW_0_MASK 	 0x30U
#define PIO02_SLEW_MISC_PIO_SLEW_0_POS  	 4U

#define MISC_PIO_SLEW_1_ADDR    	 0x570U
#define MISC_PIO_SLEW_1_DEFAULT 	 0x3CU

#define PIO05_SLEW_MISC_PIO_SLEW_1_ADDR 	 0x570U // Slew rate setting for MFP3 pin. 00 value... 
#define PIO05_SLEW_MISC_PIO_SLEW_1_MASK 	 0x0CU
#define PIO05_SLEW_MISC_PIO_SLEW_1_POS  	 2U

#define PIO06_SLEW_MISC_PIO_SLEW_1_ADDR 	 0x570U // Slew rate setting for MFP4 pin. 00 value... 
#define PIO06_SLEW_MISC_PIO_SLEW_1_MASK 	 0x30U
#define PIO06_SLEW_MISC_PIO_SLEW_1_POS  	 4U

#define MISC_PIO_SLEW_2_ADDR    	 0x571U
#define MISC_PIO_SLEW_2_DEFAULT 	 0xFCU

#define PIO010_SLEW_MISC_PIO_SLEW_2_ADDR 	 0x571U // Slew rate setting for MFP7 pin. 00 value... 
#define PIO010_SLEW_MISC_PIO_SLEW_2_MASK 	 0x30U
#define PIO010_SLEW_MISC_PIO_SLEW_2_POS  	 4U

#define PIO011_SLEW_MISC_PIO_SLEW_2_ADDR 	 0x571U // Slew rate setting for MFP8 pin. 00 value... 
#define PIO011_SLEW_MISC_PIO_SLEW_2_MASK 	 0xC0U
#define PIO011_SLEW_MISC_PIO_SLEW_2_POS  	 6U

#define MIPI_RX_EXT3_EXT4_ADDR    	 0x584U
#define MIPI_RX_EXT3_EXT4_DEFAULT 	 0x00U

#define CTRL1_FS_CNT_L_MIPI_RX_EXT3_EXT4_ADDR 	 0x584U // Frame start counter value of the virtual... 
#define CTRL1_FS_CNT_L_MIPI_RX_EXT3_EXT4_MASK 	 0xFFU
#define CTRL1_FS_CNT_L_MIPI_RX_EXT3_EXT4_POS  	 0U

#define MIPI_RX_EXT3_EXT5_ADDR    	 0x585U
#define MIPI_RX_EXT3_EXT5_DEFAULT 	 0x00U

#define CTRL1_FS_CNT_H_MIPI_RX_EXT3_EXT5_ADDR 	 0x585U // Frame start counter value of the virtual... 
#define CTRL1_FS_CNT_H_MIPI_RX_EXT3_EXT5_MASK 	 0xFFU
#define CTRL1_FS_CNT_H_MIPI_RX_EXT3_EXT5_POS  	 0U

#define MIPI_RX_EXT3_EXT6_ADDR    	 0x586U
#define MIPI_RX_EXT3_EXT6_DEFAULT 	 0x00U

#define CTRL1_FE_CNT_L_MIPI_RX_EXT3_EXT6_ADDR 	 0x586U // Frame end counter value of the virtual c... 
#define CTRL1_FE_CNT_L_MIPI_RX_EXT3_EXT6_MASK 	 0xFFU
#define CTRL1_FE_CNT_L_MIPI_RX_EXT3_EXT6_POS  	 0U

#define MIPI_RX_EXT3_EXT7_ADDR    	 0x587U
#define MIPI_RX_EXT3_EXT7_DEFAULT 	 0x00U

#define CTRL1_FE_CNT_H_MIPI_RX_EXT3_EXT7_ADDR 	 0x587U // Frame end counter value of the virtual c... 
#define CTRL1_FE_CNT_H_MIPI_RX_EXT3_EXT7_MASK 	 0xFFU
#define CTRL1_FE_CNT_H_MIPI_RX_EXT3_EXT7_POS  	 0U

#define MIPI_RX_EXT3_EXT8_ADDR    	 0x588U
#define MIPI_RX_EXT3_EXT8_DEFAULT 	 0x00U

#define CTRL1_FS_VC_SEL_MIPI_RX_EXT3_EXT8_ADDR 	 0x588U // Selected virtual channel for frame start... 
#define CTRL1_FS_VC_SEL_MIPI_RX_EXT3_EXT8_MASK 	 0x0FU
#define CTRL1_FS_VC_SEL_MIPI_RX_EXT3_EXT8_POS  	 0U

#define SPI_CC_WR_SPI_CC_WR__ADDR    	 0x1300U
#define SPI_CC_WR_SPI_CC_WR__DEFAULT 	 0x00U

#define SPI_CC_RD_SPI_CC_RD__ADDR    	 0x1380U
#define SPI_CC_RD_SPI_CC_RD__DEFAULT 	 0x00U

#define RLMS_A_RLMS4_ADDR    	 0x1404U
#define RLMS_A_RLMS4_DEFAULT 	 0x4BU

#define EOM_EN_RLMS_A_RLMS4_ADDR 	 0x1404U // Eye-opening monitor enable  
#define EOM_EN_RLMS_A_RLMS4_MASK 	 0x01U
#define EOM_EN_RLMS_A_RLMS4_POS  	 0U

#define EOM_PER_MODE_RLMS_A_RLMS4_ADDR 	 0x1404U // Eye-opening monitor periodic mode enable... 
#define EOM_PER_MODE_RLMS_A_RLMS4_MASK 	 0x02U
#define EOM_PER_MODE_RLMS_A_RLMS4_POS  	 1U

#define EOM_CHK_THR_RLMS_A_RLMS4_ADDR 	 0x1404U // Eye-opening monitor number of error bits... 
#define EOM_CHK_THR_RLMS_A_RLMS4_MASK 	 0x0CU
#define EOM_CHK_THR_RLMS_A_RLMS4_POS  	 2U

#define EOM_CHK_AMOUNT_RLMS_A_RLMS4_ADDR 	 0x1404U // A factor (N) used to select the order of... 
#define EOM_CHK_AMOUNT_RLMS_A_RLMS4_MASK 	 0xF0U
#define EOM_CHK_AMOUNT_RLMS_A_RLMS4_POS  	 4U

#define RLMS_A_RLMS5_ADDR    	 0x1405U
#define RLMS_A_RLMS5_DEFAULT 	 0x10U

#define EOM_MIN_THR_RLMS_A_RLMS5_ADDR 	 0x1405U // The EOM minimum threshold as defined by ... 
#define EOM_MIN_THR_RLMS_A_RLMS5_MASK 	 0x7FU
#define EOM_MIN_THR_RLMS_A_RLMS5_POS  	 0U

#define EOM_MAN_TRG_REQ_RLMS_A_RLMS5_ADDR 	 0x1405U // Eye-opening monitor manual trigger. For ... 
#define EOM_MAN_TRG_REQ_RLMS_A_RLMS5_MASK 	 0x80U
#define EOM_MAN_TRG_REQ_RLMS_A_RLMS5_POS  	 7U

#define RLMS_A_RLMS6_ADDR    	 0x1406U
#define RLMS_A_RLMS6_DEFAULT 	 0x80U

#define EOM_PV_MODE_RLMS_A_RLMS6_ADDR 	 0x1406U // Eye-opening is measured vertically or ho... 
#define EOM_PV_MODE_RLMS_A_RLMS6_MASK 	 0x80U
#define EOM_PV_MODE_RLMS_A_RLMS6_POS  	 7U

#define RLMS_A_RLMS7_ADDR    	 0x1407U
#define RLMS_A_RLMS7_DEFAULT 	 0x00U

#define EOM_RLMS_A_RLMS7_ADDR 	 0x1407U // Last completed EOM observation  
#define EOM_RLMS_A_RLMS7_MASK 	 0x7FU
#define EOM_RLMS_A_RLMS7_POS  	 0U

#define EOM_DONE_RLMS_A_RLMS7_ADDR 	 0x1407U // Eye-opening monitor measurement done  
#define EOM_DONE_RLMS_A_RLMS7_MASK 	 0x80U
#define EOM_DONE_RLMS_A_RLMS7_POS  	 7U

#define RLMS_A_RLMS17_ADDR    	 0x1417U
#define RLMS_A_RLMS17_DEFAULT 	 0x00U

#define AGCEN_RLMS_A_RLMS17_ADDR 	 0x1417U // AGC adapt enable  
#define AGCEN_RLMS_A_RLMS17_MASK 	 0x01U
#define AGCEN_RLMS_A_RLMS17_POS  	 0U

#define BSTEN_RLMS_A_RLMS17_ADDR 	 0x1417U // Frequency boost adapt enable (Disabled b... 
#define BSTEN_RLMS_A_RLMS17_MASK 	 0x02U
#define BSTEN_RLMS_A_RLMS17_POS  	 1U

#define BSTENOV_RLMS_A_RLMS17_ADDR 	 0x1417U // When 1, BSTEn from is set from registers... 
#define BSTENOV_RLMS_A_RLMS17_MASK 	 0x04U
#define BSTENOV_RLMS_A_RLMS17_POS  	 2U

#define DFE1EN_RLMS_A_RLMS17_ADDR 	 0x1417U // DFE1 coefficient adapt enable  
#define DFE1EN_RLMS_A_RLMS17_MASK 	 0x08U
#define DFE1EN_RLMS_A_RLMS17_POS  	 3U

#define DFE2EN_RLMS_A_RLMS17_ADDR 	 0x1417U // DFE2 coefficient adapt enable  
#define DFE2EN_RLMS_A_RLMS17_MASK 	 0x10U
#define DFE2EN_RLMS_A_RLMS17_POS  	 4U

#define DFE3EN_RLMS_A_RLMS17_ADDR 	 0x1417U // DFE3 coefficient adapt enable  
#define DFE3EN_RLMS_A_RLMS17_MASK 	 0x20U
#define DFE3EN_RLMS_A_RLMS17_POS  	 5U

#define DFE4EN_RLMS_A_RLMS17_ADDR 	 0x1417U // DFE4 coefficient adapt enable  
#define DFE4EN_RLMS_A_RLMS17_MASK 	 0x40U
#define DFE4EN_RLMS_A_RLMS17_POS  	 6U

#define DFE5EN_RLMS_A_RLMS17_ADDR 	 0x1417U // DFE5 coefficient adapt enable  
#define DFE5EN_RLMS_A_RLMS17_MASK 	 0x80U
#define DFE5EN_RLMS_A_RLMS17_POS  	 7U

#define RLMS_A_RLMS1C_ADDR    	 0x141CU
#define RLMS_A_RLMS1C_DEFAULT 	 0x00U

#define AGCMUL_RLMS_A_RLMS1C_ADDR 	 0x141CU // AGC adapt gain LSB  
#define AGCMUL_RLMS_A_RLMS1C_MASK 	 0xFFU
#define AGCMUL_RLMS_A_RLMS1C_POS  	 0U

#define RLMS_A_RLMS1D_ADDR    	 0x141DU
#define RLMS_A_RLMS1D_DEFAULT 	 0x02U

#define AGCMUH_RLMS_A_RLMS1D_ADDR 	 0x141DU // AGC adapt gain MSB  
#define AGCMUH_RLMS_A_RLMS1D_MASK 	 0x3FU
#define AGCMUH_RLMS_A_RLMS1D_POS  	 0U

#define RLMS_A_RLMS1F_ADDR    	 0x141FU
#define RLMS_A_RLMS1F_DEFAULT 	 0x00U

#define AGCINIT_RLMS_A_RLMS1F_ADDR 	 0x141FU // AGC initial value  
#define AGCINIT_RLMS_A_RLMS1F_MASK 	 0xFFU
#define AGCINIT_RLMS_A_RLMS1F_POS  	 0U

#define RLMS_A_RLMS32_ADDR    	 0x1432U
#define RLMS_A_RLMS32_DEFAULT 	 0x7FU

#define OSNMODE_RLMS_A_RLMS32_ADDR 	 0x1432U // GMSL2 OSN mode  
#define OSNMODE_RLMS_A_RLMS32_MASK 	 0x80U
#define OSNMODE_RLMS_A_RLMS32_POS  	 7U

#define RLMS_A_RLMS3A_ADDR    	 0x143AU
#define RLMS_A_RLMS3A_DEFAULT 	 0x00U

#define EYEMONVALCNTL_RLMS_A_RLMS3A_ADDR 	 0x143AU // Eye monitor valid (hit) count (read-only... 
#define EYEMONVALCNTL_RLMS_A_RLMS3A_MASK 	 0xFFU
#define EYEMONVALCNTL_RLMS_A_RLMS3A_POS  	 0U

#define RLMS_A_RLMS3B_ADDR    	 0x143BU
#define RLMS_A_RLMS3B_DEFAULT 	 0x00U

#define EYEMONVALCNTH_RLMS_A_RLMS3B_ADDR 	 0x143BU // Eye monitor valid (hit) count (read-only... 
#define EYEMONVALCNTH_RLMS_A_RLMS3B_MASK 	 0xFFU
#define EYEMONVALCNTH_RLMS_A_RLMS3B_POS  	 0U

#define RLMS_A_RLMS64_ADDR    	 0x1464U
#define RLMS_A_RLMS64_DEFAULT 	 0x90U

#define TXSSCMODE_RLMS_A_RLMS64_ADDR 	 0x1464U // Tx spread-spectrum mode  
#define TXSSCMODE_RLMS_A_RLMS64_MASK 	 0x03U
#define TXSSCMODE_RLMS_A_RLMS64_POS  	 0U

#define RLMS_A_RLMS70_ADDR    	 0x1470U
#define RLMS_A_RLMS70_DEFAULT 	 0x01U

#define TXSSCFRQCTRL_RLMS_A_RLMS70_ADDR 	 0x1470U // Tx SSC modulation frequency deviation co... 
#define TXSSCFRQCTRL_RLMS_A_RLMS70_MASK 	 0x7FU
#define TXSSCFRQCTRL_RLMS_A_RLMS70_POS  	 0U

#define RLMS_A_RLMS71_ADDR    	 0x1471U
#define RLMS_A_RLMS71_DEFAULT 	 0x02U

#define TXSSCEN_RLMS_A_RLMS71_ADDR 	 0x1471U // Tx spread spectrum enable  
#define TXSSCEN_RLMS_A_RLMS71_MASK 	 0x01U
#define TXSSCEN_RLMS_A_RLMS71_POS  	 0U

#define TXSSCCENSPRST_RLMS_A_RLMS71_ADDR 	 0x1471U // Tx SSC center spread starting phase  
#define TXSSCCENSPRST_RLMS_A_RLMS71_MASK 	 0x7EU
#define TXSSCCENSPRST_RLMS_A_RLMS71_POS  	 1U

#define RLMS_A_RLMS72_ADDR    	 0x1472U
#define RLMS_A_RLMS72_DEFAULT 	 0xCFU

#define TXSSCPRESCLL_RLMS_A_RLMS72_ADDR 	 0x1472U // Tx SSC frequency prescaler bits 7:0. Dec... 
#define TXSSCPRESCLL_RLMS_A_RLMS72_MASK 	 0xFFU
#define TXSSCPRESCLL_RLMS_A_RLMS72_POS  	 0U

#define RLMS_A_RLMS73_ADDR    	 0x1473U
#define RLMS_A_RLMS73_DEFAULT 	 0x00U

#define TXSSCPRESCLH_RLMS_A_RLMS73_ADDR 	 0x1473U // Tx SSC frequency prescaler bits 10:8. De... 
#define TXSSCPRESCLH_RLMS_A_RLMS73_MASK 	 0x07U
#define TXSSCPRESCLH_RLMS_A_RLMS73_POS  	 0U

#define RLMS_A_RLMS74_ADDR    	 0x1474U
#define RLMS_A_RLMS74_DEFAULT 	 0x00U

#define TXSSCPHL_RLMS_A_RLMS74_ADDR 	 0x1474U // Tx SSC phase accumulator increment bits ... 
#define TXSSCPHL_RLMS_A_RLMS74_MASK 	 0xFFU
#define TXSSCPHL_RLMS_A_RLMS74_POS  	 0U

#define RLMS_A_RLMS75_ADDR    	 0x1475U
#define RLMS_A_RLMS75_DEFAULT 	 0x00U

#define TXSSCPHH_RLMS_A_RLMS75_ADDR 	 0x1475U // Tx SSC phase accumulator increment bits ... 
#define TXSSCPHH_RLMS_A_RLMS75_MASK 	 0x7FU
#define TXSSCPHH_RLMS_A_RLMS75_POS  	 0U

#define RLMS_A_RLMS76_ADDR    	 0x1476U
#define RLMS_A_RLMS76_DEFAULT 	 0x00U

#define TXSSCPHQUAD_RLMS_A_RLMS76_ADDR 	 0x1476U // Tx SSC phase starting phase quadrant  
#define TXSSCPHQUAD_RLMS_A_RLMS76_MASK 	 0x03U
#define TXSSCPHQUAD_RLMS_A_RLMS76_POS  	 0U

#define RLMS_A_RLMSA8_ADDR    	 0x14A8U
#define RLMS_A_RLMSA8_DEFAULT 	 0x00U

#define FW_PHY_RSTB_RLMS_A_RLMSA8_ADDR 	 0x14A8U // Override PHY controller output  
#define FW_PHY_RSTB_RLMS_A_RLMSA8_MASK 	 0x20U
#define FW_PHY_RSTB_RLMS_A_RLMSA8_POS  	 5U

#define FW_PHY_PU_TX_RLMS_A_RLMSA8_ADDR 	 0x14A8U // Override PHY controller output  
#define FW_PHY_PU_TX_RLMS_A_RLMSA8_MASK 	 0x40U
#define FW_PHY_PU_TX_RLMS_A_RLMSA8_POS  	 6U

#define FW_PHY_CTRL_RLMS_A_RLMSA8_ADDR 	 0x14A8U // PHY controller firmware mode enable. Oth... 
#define FW_PHY_CTRL_RLMS_A_RLMSA8_MASK 	 0x80U
#define FW_PHY_CTRL_RLMS_A_RLMSA8_POS  	 7U

#define RLMS_A_RLMSA9_ADDR    	 0x14A9U
#define RLMS_A_RLMSA9_DEFAULT 	 0x00U

#define FW_RXD_EN_RLMS_A_RLMSA9_ADDR 	 0x14A9U // Override PHY controller output  
#define FW_RXD_EN_RLMS_A_RLMSA9_MASK 	 0x08U
#define FW_RXD_EN_RLMS_A_RLMSA9_POS  	 3U

#define FW_TXD_EN_RLMS_A_RLMSA9_ADDR 	 0x14A9U // Override PHY controller output  
#define FW_TXD_EN_RLMS_A_RLMSA9_MASK 	 0x10U
#define FW_TXD_EN_RLMS_A_RLMSA9_POS  	 4U

#define FW_TXD_SQUELCH_RLMS_A_RLMSA9_ADDR 	 0x14A9U // Override PHY controller output  
#define FW_TXD_SQUELCH_RLMS_A_RLMSA9_MASK 	 0x20U
#define FW_TXD_SQUELCH_RLMS_A_RLMSA9_POS  	 5U

#define FW_REPCAL_RSTB_RLMS_A_RLMSA9_ADDR 	 0x14A9U // Override PHY controller output  
#define FW_REPCAL_RSTB_RLMS_A_RLMSA9_MASK 	 0x80U
#define FW_REPCAL_RSTB_RLMS_A_RLMSA9_POS  	 7U

#define RLMS_A_RLMSAA_ADDR    	 0x14AAU
#define RLMS_A_RLMSAA_DEFAULT 	 0x90U

#define ROR_CLK_DET_RLMS_A_RLMSAA_ADDR 	 0x14AAU // In SER, indicates ROR clock is detected.... 
#define ROR_CLK_DET_RLMS_A_RLMSAA_MASK 	 0x20U
#define ROR_CLK_DET_RLMS_A_RLMSAA_POS  	 5U

#define RLMS_A_RLMSCE_ADDR    	 0x14CEU
#define RLMS_A_RLMSCE_DEFAULT 	 0x01U

#define ENFFE_RLMS_A_RLMSCE_ADDR 	 0x14CEU // ffe enable  
#define ENFFE_RLMS_A_RLMSCE_MASK 	 0x01U
#define ENFFE_RLMS_A_RLMSCE_POS  	 0U

#define ENMINUS_MAN_RLMS_A_RLMSCE_ADDR 	 0x14CEU // enminus manual control  
#define ENMINUS_MAN_RLMS_A_RLMSCE_MASK 	 0x08U
#define ENMINUS_MAN_RLMS_A_RLMSCE_POS  	 3U

#define ENMINUS_REG_RLMS_A_RLMSCE_ADDR 	 0x14CEU // value to use if manual control enabled w... 
#define ENMINUS_REG_RLMS_A_RLMSCE_MASK 	 0x10U
#define ENMINUS_REG_RLMS_A_RLMSCE_POS  	 4U

#define DPLL_REF_DPLL_0_ADDR    	 0x1A00U
#define DPLL_REF_DPLL_0_DEFAULT 	 0xF5U

#define CONFIG_SOFT_RST_N_DPLL_REF_DPLL_0_ADDR 	 0x1A00U // Setting this to 1 resets the PLL functio... 
#define CONFIG_SOFT_RST_N_DPLL_REF_DPLL_0_MASK 	 0x01U
#define CONFIG_SOFT_RST_N_DPLL_REF_DPLL_0_POS  	 0U

#define DPLL_REF_DPLL_3_ADDR    	 0x1A03U
#define DPLL_REF_DPLL_3_DEFAULT 	 0x82U

#define CONFIG_SPREAD_BIT_RATIO_DPLL_REF_DPLL_3_ADDR 	 0x1A03U // Controls the magnitude of the triangle w... 
#define CONFIG_SPREAD_BIT_RATIO_DPLL_REF_DPLL_3_MASK 	 0x07U
#define CONFIG_SPREAD_BIT_RATIO_DPLL_REF_DPLL_3_POS  	 0U

#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_REF_DPLL_3_ADDR 	 0x1A03U // Enable all DPLL divider values to come f... 
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_REF_DPLL_3_MASK 	 0x10U
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_REF_DPLL_3_POS  	 4U

#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_REF_DPLL_3_ADDR 	 0x1A03U // When 1, config_sel_clock_out is used to ... 
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_REF_DPLL_3_MASK 	 0x80U
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_REF_DPLL_3_POS  	 7U

#define DPLL_REF_DPLL_7_ADDR    	 0x1A07U
#define DPLL_REF_DPLL_7_DEFAULT 	 0x04U

#define CONFIG_DIV_IN_DPLL_REF_DPLL_7_ADDR 	 0x1A07U // Sets the divide value of the input divid... 
#define CONFIG_DIV_IN_DPLL_REF_DPLL_7_MASK 	 0x7CU
#define CONFIG_DIV_IN_DPLL_REF_DPLL_7_POS  	 2U

#define CONFIG_DIV_FB_L_DPLL_REF_DPLL_7_ADDR 	 0x1A07U // Sets the DPLL feedback divider value (bi... 
#define CONFIG_DIV_FB_L_DPLL_REF_DPLL_7_MASK 	 0x80U
#define CONFIG_DIV_FB_L_DPLL_REF_DPLL_7_POS  	 7U

#define DPLL_REF_DPLL_8_ADDR    	 0x1A08U
#define DPLL_REF_DPLL_8_DEFAULT 	 0x14U

#define CONFIG_DIV_FB_H_DPLL_REF_DPLL_8_ADDR 	 0x1A08U // Sets the DPLL feedback divider value (bi... 
#define CONFIG_DIV_FB_H_DPLL_REF_DPLL_8_MASK 	 0xFFU
#define CONFIG_DIV_FB_H_DPLL_REF_DPLL_8_POS  	 0U

#define DPLL_REF_DPLL_9_ADDR    	 0x1A09U
#define DPLL_REF_DPLL_9_DEFAULT 	 0x40U

#define CONFIG_DIV_FB_EXP_DPLL_REF_DPLL_9_ADDR 	 0x1A09U // Sets the feedback exponential divider va... 
#define CONFIG_DIV_FB_EXP_DPLL_REF_DPLL_9_MASK 	 0x07U
#define CONFIG_DIV_FB_EXP_DPLL_REF_DPLL_9_POS  	 0U

#define CONFIG_DIV_OUT_L_DPLL_REF_DPLL_9_ADDR 	 0x1A09U // Sets the DPLL output divider value (bits... 
#define CONFIG_DIV_OUT_L_DPLL_REF_DPLL_9_MASK 	 0xF8U
#define CONFIG_DIV_OUT_L_DPLL_REF_DPLL_9_POS  	 3U

#define DPLL_REF_DPLL_10_ADDR    	 0x1A0AU
#define DPLL_REF_DPLL_10_DEFAULT 	 0x81U

#define CONFIG_DIV_OUT_H_DPLL_REF_DPLL_10_ADDR 	 0x1A0AU // Sets the DPLL output divider value (bits... 
#define CONFIG_DIV_OUT_H_DPLL_REF_DPLL_10_MASK 	 0x0FU
#define CONFIG_DIV_OUT_H_DPLL_REF_DPLL_10_POS  	 0U

#define CONFIG_DIV_OUT_EXP_DPLL_REF_DPLL_10_ADDR 	 0x1A0AU // Sets the output exponential divider valu... 
#define CONFIG_DIV_OUT_EXP_DPLL_REF_DPLL_10_MASK 	 0x70U
#define CONFIG_DIV_OUT_EXP_DPLL_REF_DPLL_10_POS  	 4U

#define CONFIG_ALLOW_COARSE_CHANGE_DPLL_REF_DPLL_10_ADDR 	 0x1A0AU // When set to 1, the coarse tuning DAC is ... 
#define CONFIG_ALLOW_COARSE_CHANGE_DPLL_REF_DPLL_10_MASK 	 0x80U
#define CONFIG_ALLOW_COARSE_CHANGE_DPLL_REF_DPLL_10_POS  	 7U

#define EFUSE_EFUSE80_ADDR    	 0x1C50U
#define EFUSE_EFUSE80_DEFAULT 	 0x00U

#define SERIAL_NUMBER_0_EFUSE_EFUSE80_ADDR 	 0x1C50U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_0_EFUSE_EFUSE80_MASK 	 0xFFU
#define SERIAL_NUMBER_0_EFUSE_EFUSE80_POS  	 0U

#define EFUSE_EFUSE81_ADDR    	 0x1C51U
#define EFUSE_EFUSE81_DEFAULT 	 0x00U

#define SERIAL_NUMBER_1_EFUSE_EFUSE81_ADDR 	 0x1C51U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_1_EFUSE_EFUSE81_MASK 	 0xFFU
#define SERIAL_NUMBER_1_EFUSE_EFUSE81_POS  	 0U

#define EFUSE_EFUSE82_ADDR    	 0x1C52U
#define EFUSE_EFUSE82_DEFAULT 	 0x00U

#define SERIAL_NUMBER_2_EFUSE_EFUSE82_ADDR 	 0x1C52U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_2_EFUSE_EFUSE82_MASK 	 0xFFU
#define SERIAL_NUMBER_2_EFUSE_EFUSE82_POS  	 0U

#define EFUSE_EFUSE83_ADDR    	 0x1C53U
#define EFUSE_EFUSE83_DEFAULT 	 0x00U

#define SERIAL_NUMBER_3_EFUSE_EFUSE83_ADDR 	 0x1C53U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_3_EFUSE_EFUSE83_MASK 	 0xFFU
#define SERIAL_NUMBER_3_EFUSE_EFUSE83_POS  	 0U

#define EFUSE_EFUSE84_ADDR    	 0x1C54U
#define EFUSE_EFUSE84_DEFAULT 	 0x00U

#define SERIAL_NUMBER_4_EFUSE_EFUSE84_ADDR 	 0x1C54U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_4_EFUSE_EFUSE84_MASK 	 0xFFU
#define SERIAL_NUMBER_4_EFUSE_EFUSE84_POS  	 0U

#define EFUSE_EFUSE85_ADDR    	 0x1C55U
#define EFUSE_EFUSE85_DEFAULT 	 0x00U

#define SERIAL_NUMBER_5_EFUSE_EFUSE85_ADDR 	 0x1C55U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_5_EFUSE_EFUSE85_MASK 	 0xFFU
#define SERIAL_NUMBER_5_EFUSE_EFUSE85_POS  	 0U

#define EFUSE_EFUSE86_ADDR    	 0x1C56U
#define EFUSE_EFUSE86_DEFAULT 	 0x00U

#define SERIAL_NUMBER_6_EFUSE_EFUSE86_ADDR 	 0x1C56U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_6_EFUSE_EFUSE86_MASK 	 0xFFU
#define SERIAL_NUMBER_6_EFUSE_EFUSE86_POS  	 0U

#define EFUSE_EFUSE87_ADDR    	 0x1C57U
#define EFUSE_EFUSE87_DEFAULT 	 0x00U

#define SERIAL_NUMBER_7_EFUSE_EFUSE87_ADDR 	 0x1C57U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_7_EFUSE_EFUSE87_MASK 	 0xFFU
#define SERIAL_NUMBER_7_EFUSE_EFUSE87_POS  	 0U

#define EFUSE_EFUSE88_ADDR    	 0x1C58U
#define EFUSE_EFUSE88_DEFAULT 	 0x00U

#define SERIAL_NUMBER_8_EFUSE_EFUSE88_ADDR 	 0x1C58U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_8_EFUSE_EFUSE88_MASK 	 0xFFU
#define SERIAL_NUMBER_8_EFUSE_EFUSE88_POS  	 0U

#define EFUSE_EFUSE89_ADDR    	 0x1C59U
#define EFUSE_EFUSE89_DEFAULT 	 0x00U

#define SERIAL_NUMBER_9_EFUSE_EFUSE89_ADDR 	 0x1C59U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_9_EFUSE_EFUSE89_MASK 	 0xFFU
#define SERIAL_NUMBER_9_EFUSE_EFUSE89_POS  	 0U

#define EFUSE_EFUSE90_ADDR    	 0x1C5AU
#define EFUSE_EFUSE90_DEFAULT 	 0x00U

#define SERIAL_NUMBER_10_EFUSE_EFUSE90_ADDR 	 0x1C5AU // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_10_EFUSE_EFUSE90_MASK 	 0xFFU
#define SERIAL_NUMBER_10_EFUSE_EFUSE90_POS  	 0U

#define EFUSE_EFUSE91_ADDR    	 0x1C5BU
#define EFUSE_EFUSE91_DEFAULT 	 0x00U

#define SERIAL_NUMBER_11_EFUSE_EFUSE91_ADDR 	 0x1C5BU // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_11_EFUSE_EFUSE91_MASK 	 0xFFU
#define SERIAL_NUMBER_11_EFUSE_EFUSE91_POS  	 0U

#define EFUSE_EFUSE92_ADDR    	 0x1C5CU
#define EFUSE_EFUSE92_DEFAULT 	 0x00U

#define SERIAL_NUMBER_12_EFUSE_EFUSE92_ADDR 	 0x1C5CU // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_12_EFUSE_EFUSE92_MASK 	 0xFFU
#define SERIAL_NUMBER_12_EFUSE_EFUSE92_POS  	 0U

#define EFUSE_EFUSE93_ADDR    	 0x1C5DU
#define EFUSE_EFUSE93_DEFAULT 	 0x00U

#define SERIAL_NUMBER_13_EFUSE_EFUSE93_ADDR 	 0x1C5DU // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_13_EFUSE_EFUSE93_MASK 	 0xFFU
#define SERIAL_NUMBER_13_EFUSE_EFUSE93_POS  	 0U

#define EFUSE_EFUSE94_ADDR    	 0x1C5EU
#define EFUSE_EFUSE94_DEFAULT 	 0x00U

#define SERIAL_NUMBER_14_EFUSE_EFUSE94_ADDR 	 0x1C5EU // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_14_EFUSE_EFUSE94_MASK 	 0xFFU
#define SERIAL_NUMBER_14_EFUSE_EFUSE94_POS  	 0U

#define EFUSE_EFUSE95_ADDR    	 0x1C5FU
#define EFUSE_EFUSE95_DEFAULT 	 0x00U

#define SERIAL_NUMBER_15_EFUSE_EFUSE95_ADDR 	 0x1C5FU // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_15_EFUSE_EFUSE95_MASK 	 0xFFU
#define SERIAL_NUMBER_15_EFUSE_EFUSE95_POS  	 0U

#define EFUSE_EFUSE96_ADDR    	 0x1C60U
#define EFUSE_EFUSE96_DEFAULT 	 0x00U

#define SERIAL_NUMBER_16_EFUSE_EFUSE96_ADDR 	 0x1C60U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_16_EFUSE_EFUSE96_MASK 	 0xFFU
#define SERIAL_NUMBER_16_EFUSE_EFUSE96_POS  	 0U

#define EFUSE_EFUSE97_ADDR    	 0x1C61U
#define EFUSE_EFUSE97_DEFAULT 	 0x00U

#define SERIAL_NUMBER_17_EFUSE_EFUSE97_ADDR 	 0x1C61U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_17_EFUSE_EFUSE97_MASK 	 0xFFU
#define SERIAL_NUMBER_17_EFUSE_EFUSE97_POS  	 0U

#define EFUSE_EFUSE98_ADDR    	 0x1C62U
#define EFUSE_EFUSE98_DEFAULT 	 0x00U

#define SERIAL_NUMBER_18_EFUSE_EFUSE98_ADDR 	 0x1C62U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_18_EFUSE_EFUSE98_MASK 	 0xFFU
#define SERIAL_NUMBER_18_EFUSE_EFUSE98_POS  	 0U

#define EFUSE_EFUSE99_ADDR    	 0x1C63U
#define EFUSE_EFUSE99_DEFAULT 	 0x00U

#define SERIAL_NUMBER_19_EFUSE_EFUSE99_ADDR 	 0x1C63U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_19_EFUSE_EFUSE99_MASK 	 0xFFU
#define SERIAL_NUMBER_19_EFUSE_EFUSE99_POS  	 0U

#define EFUSE_EFUSE100_ADDR    	 0x1C64U
#define EFUSE_EFUSE100_DEFAULT 	 0x00U

#define SERIAL_NUMBER_20_EFUSE_EFUSE100_ADDR 	 0x1C64U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_20_EFUSE_EFUSE100_MASK 	 0xFFU
#define SERIAL_NUMBER_20_EFUSE_EFUSE100_POS  	 0U

#define EFUSE_EFUSE101_ADDR    	 0x1C65U
#define EFUSE_EFUSE101_DEFAULT 	 0x00U

#define SERIAL_NUMBER_21_EFUSE_EFUSE101_ADDR 	 0x1C65U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_21_EFUSE_EFUSE101_MASK 	 0xFFU
#define SERIAL_NUMBER_21_EFUSE_EFUSE101_POS  	 0U

#define EFUSE_EFUSE102_ADDR    	 0x1C66U
#define EFUSE_EFUSE102_DEFAULT 	 0x00U

#define SERIAL_NUMBER_22_EFUSE_EFUSE102_ADDR 	 0x1C66U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_22_EFUSE_EFUSE102_MASK 	 0xFFU
#define SERIAL_NUMBER_22_EFUSE_EFUSE102_POS  	 0U

#define EFUSE_EFUSE103_ADDR    	 0x1C67U
#define EFUSE_EFUSE103_DEFAULT 	 0x00U

#define SERIAL_NUMBER_23_EFUSE_EFUSE103_ADDR 	 0x1C67U // Serial Number. Can only be read through ... 
#define SERIAL_NUMBER_23_EFUSE_EFUSE103_MASK 	 0xFFU
#define SERIAL_NUMBER_23_EFUSE_EFUSE103_POS  	 0U

#define FUNC_SAFE_REGCRC0_ADDR    	 0x1D00U
#define FUNC_SAFE_REGCRC0_DEFAULT 	 0x00U

#define RESET_CRC_FUNC_SAFE_REGCRC0_ADDR 	 0x1D00U // Reset CRC value to 16'FFFF.  
#define RESET_CRC_FUNC_SAFE_REGCRC0_MASK 	 0x01U
#define RESET_CRC_FUNC_SAFE_REGCRC0_POS  	 0U

#define CHECK_CRC_FUNC_SAFE_REGCRC0_ADDR 	 0x1D00U // Upon calculation of register CRC, compar... 
#define CHECK_CRC_FUNC_SAFE_REGCRC0_MASK 	 0x02U
#define CHECK_CRC_FUNC_SAFE_REGCRC0_POS  	 1U

#define PERIODIC_COMPUTE_FUNC_SAFE_REGCRC0_ADDR 	 0x1D00U // Check register CRC on periodic basis, ba... 
#define PERIODIC_COMPUTE_FUNC_SAFE_REGCRC0_MASK 	 0x04U
#define PERIODIC_COMPUTE_FUNC_SAFE_REGCRC0_POS  	 2U

#define I2C_WR_COMPUTE_FUNC_SAFE_REGCRC0_ADDR 	 0x1D00U // Compute register CRC after every i2c reg... 
#define I2C_WR_COMPUTE_FUNC_SAFE_REGCRC0_MASK 	 0x08U
#define I2C_WR_COMPUTE_FUNC_SAFE_REGCRC0_POS  	 3U

#define GEN_ROLLING_CRC_FUNC_SAFE_REGCRC0_ADDR 	 0x1D00U // Calculate register CRC using additional ... 
#define GEN_ROLLING_CRC_FUNC_SAFE_REGCRC0_MASK 	 0x10U
#define GEN_ROLLING_CRC_FUNC_SAFE_REGCRC0_POS  	 4U

#define FUNC_SAFE_REGCRC1_ADDR    	 0x1D01U
#define FUNC_SAFE_REGCRC1_DEFAULT 	 0x00U

#define CRC_PERIOD_FUNC_SAFE_REGCRC1_ADDR 	 0x1D01U // Period for register CRC recomputation.  
#define CRC_PERIOD_FUNC_SAFE_REGCRC1_MASK 	 0xFFU
#define CRC_PERIOD_FUNC_SAFE_REGCRC1_POS  	 0U

#define FUNC_SAFE_REGCRC2_ADDR    	 0x1D02U
#define FUNC_SAFE_REGCRC2_DEFAULT 	 0x00U

#define REGCRC_LSB_FUNC_SAFE_REGCRC2_ADDR 	 0x1D02U // Calculated register CRC value (LSB)  
#define REGCRC_LSB_FUNC_SAFE_REGCRC2_MASK 	 0xFFU
#define REGCRC_LSB_FUNC_SAFE_REGCRC2_POS  	 0U

#define FUNC_SAFE_REGCRC3_ADDR    	 0x1D03U
#define FUNC_SAFE_REGCRC3_DEFAULT 	 0x00U

#define REGCRC_MSB_FUNC_SAFE_REGCRC3_ADDR 	 0x1D03U // Calculated register CRC value (MSB)  
#define REGCRC_MSB_FUNC_SAFE_REGCRC3_MASK 	 0xFFU
#define REGCRC_MSB_FUNC_SAFE_REGCRC3_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC0_ADDR    	 0x1D08U
#define FUNC_SAFE_I2C_UART_CRC0_DEFAULT 	 0x00U

#define RESET_MSGCNTR_FUNC_SAFE_I2C_UART_CRC0_ADDR 	 0x1D08U // Reset Message Counter Value to 0. Self-c... 
#define RESET_MSGCNTR_FUNC_SAFE_I2C_UART_CRC0_MASK 	 0x01U
#define RESET_MSGCNTR_FUNC_SAFE_I2C_UART_CRC0_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC1_ADDR    	 0x1D09U
#define FUNC_SAFE_I2C_UART_CRC1_DEFAULT 	 0x00U

#define RESET_CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_ADDR 	 0x1D09U // Reset CRC Error Count to 0. Self-clearin... 
#define RESET_CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_MASK 	 0x01U
#define RESET_CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_POS  	 0U

#define RESET_MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_ADDR 	 0x1D09U // Reset message counter error count to 0. ... 
#define RESET_MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_MASK 	 0x02U
#define RESET_MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_POS  	 1U

#define FUNC_SAFE_I2C_UART_CRC2_ADDR    	 0x1D0AU
#define FUNC_SAFE_I2C_UART_CRC2_DEFAULT 	 0x00U

#define CRC_VAL_FUNC_SAFE_I2C_UART_CRC2_ADDR 	 0x1D0AU // Calculated CRC value for the last write ... 
#define CRC_VAL_FUNC_SAFE_I2C_UART_CRC2_MASK 	 0xFFU
#define CRC_VAL_FUNC_SAFE_I2C_UART_CRC2_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC3_ADDR    	 0x1D0BU
#define FUNC_SAFE_I2C_UART_CRC3_DEFAULT 	 0x00U

#define MSGCNTR_LSB_FUNC_SAFE_I2C_UART_CRC3_ADDR 	 0x1D0BU // Bits 7:0 of current message counter valu... 
#define MSGCNTR_LSB_FUNC_SAFE_I2C_UART_CRC3_MASK 	 0xFFU
#define MSGCNTR_LSB_FUNC_SAFE_I2C_UART_CRC3_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC4_ADDR    	 0x1D0CU
#define FUNC_SAFE_I2C_UART_CRC4_DEFAULT 	 0x00U

#define MSGCNTR_MSB_FUNC_SAFE_I2C_UART_CRC4_ADDR 	 0x1D0CU // Bits 15:8 of current message counter val... 
#define MSGCNTR_MSB_FUNC_SAFE_I2C_UART_CRC4_MASK 	 0xFFU
#define MSGCNTR_MSB_FUNC_SAFE_I2C_UART_CRC4_POS  	 0U

#define FUNC_SAFE_FS_INTR0_ADDR    	 0x1D12U
#define FUNC_SAFE_FS_INTR0_DEFAULT 	 0xE0U

#define REG_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x1D12U // Enable reporting register CRC at ERRB pi... 
#define REG_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x01U
#define REG_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 0U

#define MEM_ECC_ERR1_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x1D12U // Enable reporting of memory ECC 1-bit cor... 
#define MEM_ECC_ERR1_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x10U
#define MEM_ECC_ERR1_OEN_FUNC_SAFE_FS_INTR0_POS  	 4U

#define MEM_ECC_ERR2_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x1D12U // Enable reporting of memory ECC 2-bit unc... 
#define MEM_ECC_ERR2_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x20U
#define MEM_ECC_ERR2_OEN_FUNC_SAFE_FS_INTR0_POS  	 5U

#define I2C_UART_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x1D12U // Enable reporting of I2C/UART CRC errors ... 
#define I2C_UART_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x40U
#define I2C_UART_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 6U

#define I2C_UART_MSGCNTR_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x1D12U // Enable reporting of I2C/UART message cou... 
#define I2C_UART_MSGCNTR_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x80U
#define I2C_UART_MSGCNTR_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 7U

#define FUNC_SAFE_FS_INTR1_ADDR    	 0x1D13U
#define FUNC_SAFE_FS_INTR1_DEFAULT 	 0x00U

#define REG_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_ADDR 	 0x1D13U // An error occurred on the register CRC ca... 
#define REG_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_MASK 	 0x01U
#define REG_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_POS  	 0U

#define MEM_ECC_ERR1_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x1D13U // Error flag for 1-bit correctable memory ... 
#define MEM_ECC_ERR1_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x10U
#define MEM_ECC_ERR1_INT_FUNC_SAFE_FS_INTR1_POS  	 4U

#define MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x1D13U // Error flag for 2-bit uncorrectable memor... 
#define MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x20U
#define MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_POS  	 5U

#define I2C_UART_CRC_ERR_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x1D13U // I2C/UART CRC error flag. Asserted when C... 
#define I2C_UART_CRC_ERR_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x40U
#define I2C_UART_CRC_ERR_INT_FUNC_SAFE_FS_INTR1_POS  	 6U

#define I2C_UART_MSGCNTR_ERR_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x1D13U // I2C/UART message counter error flag. Ass... 
#define I2C_UART_MSGCNTR_ERR_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x80U
#define I2C_UART_MSGCNTR_ERR_INT_FUNC_SAFE_FS_INTR1_POS  	 7U

#define FUNC_SAFE_MEM_ECC0_ADDR    	 0x1D14U
#define FUNC_SAFE_MEM_ECC0_DEFAULT 	 0x00U

#define RESET_MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC0_ADDR 	 0x1D14U // Reset memory ECC 1-bit error count to 0.... 
#define RESET_MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC0_MASK 	 0x01U
#define RESET_MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC0_POS  	 0U

#define RESET_MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC0_ADDR 	 0x1D14U // Reset memory ECC 2-bit error count to 0.... 
#define RESET_MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC0_MASK 	 0x02U
#define RESET_MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC0_POS  	 1U

#define FUNC_SAFE_REG_POST0_ADDR    	 0x1D20U
#define FUNC_SAFE_REG_POST0_DEFAULT 	 0x00U

#define POST_LBIST_PASSED_FUNC_SAFE_REG_POST0_ADDR 	 0x1D20U // LBIST passed during POST (power-on self ... 
#define POST_LBIST_PASSED_FUNC_SAFE_REG_POST0_MASK 	 0x20U
#define POST_LBIST_PASSED_FUNC_SAFE_REG_POST0_POS  	 5U

#define POST_MBIST_PASSED_FUNC_SAFE_REG_POST0_ADDR 	 0x1D20U // MBIST passed during POST (power-on self ... 
#define POST_MBIST_PASSED_FUNC_SAFE_REG_POST0_MASK 	 0x40U
#define POST_MBIST_PASSED_FUNC_SAFE_REG_POST0_POS  	 6U

#define POST_DONE_FUNC_SAFE_REG_POST0_ADDR 	 0x1D20U // POST (power-on self test LBIST and/or MB... 
#define POST_DONE_FUNC_SAFE_REG_POST0_MASK 	 0x80U
#define POST_DONE_FUNC_SAFE_REG_POST0_POS  	 7U

#define FUNC_SAFE_REGADCBIST0_ADDR    	 0x1D28U
#define FUNC_SAFE_REGADCBIST0_DEFAULT 	 0x00U

#define RUN_TMON_CAL_FUNC_SAFE_REGADCBIST0_ADDR 	 0x1D28U // Initiate temperature sensor measurement.... 
#define RUN_TMON_CAL_FUNC_SAFE_REGADCBIST0_MASK 	 0x01U
#define RUN_TMON_CAL_FUNC_SAFE_REGADCBIST0_POS  	 0U

#define RUN_ACCURACY_FUNC_SAFE_REGADCBIST0_ADDR 	 0x1D28U // Run ADC accuracy testing. Self-clearing.... 
#define RUN_ACCURACY_FUNC_SAFE_REGADCBIST0_MASK 	 0x04U
#define RUN_ACCURACY_FUNC_SAFE_REGADCBIST0_POS  	 2U

#define MUXVER_EN_FUNC_SAFE_REGADCBIST0_ADDR 	 0x1D28U // Enable MUX manual GPIO test  
#define MUXVER_EN_FUNC_SAFE_REGADCBIST0_MASK 	 0x10U
#define MUXVER_EN_FUNC_SAFE_REGADCBIST0_POS  	 4U

#define RR_ACCURACY_FUNC_SAFE_REGADCBIST0_ADDR 	 0x1D28U // Run ADC accuracy in round robin state fa... 
#define RR_ACCURACY_FUNC_SAFE_REGADCBIST0_MASK 	 0x80U
#define RR_ACCURACY_FUNC_SAFE_REGADCBIST0_POS  	 7U

#define FUNC_SAFE_REGADCBIST3_ADDR    	 0x1D31U
#define FUNC_SAFE_REGADCBIST3_DEFAULT 	 0x0FU

#define REFLIM_FUNC_SAFE_REGADCBIST3_ADDR 	 0x1D31U // ADC reference limit for testing code. Se... 
#define REFLIM_FUNC_SAFE_REGADCBIST3_MASK 	 0xFFU
#define REFLIM_FUNC_SAFE_REGADCBIST3_POS  	 0U

#define FUNC_SAFE_REGADCBIST4_ADDR    	 0x1D32U
#define FUNC_SAFE_REGADCBIST4_DEFAULT 	 0x0FU

#define REFLIMSCL1_FUNC_SAFE_REGADCBIST4_ADDR 	 0x1D32U // Control the accuracy for 1/2 scale measu... 
#define REFLIMSCL1_FUNC_SAFE_REGADCBIST4_MASK 	 0xFFU
#define REFLIMSCL1_FUNC_SAFE_REGADCBIST4_POS  	 0U

#define FUNC_SAFE_REGADCBIST5_ADDR    	 0x1D33U
#define FUNC_SAFE_REGADCBIST5_DEFAULT 	 0x07U

#define REFLIMSCL2_FUNC_SAFE_REGADCBIST5_ADDR 	 0x1D33U // Control the accuracy for 1/4 scale measu... 
#define REFLIMSCL2_FUNC_SAFE_REGADCBIST5_MASK 	 0xFFU
#define REFLIMSCL2_FUNC_SAFE_REGADCBIST5_POS  	 0U

#define FUNC_SAFE_REGADCBIST6_ADDR    	 0x1D34U
#define FUNC_SAFE_REGADCBIST6_DEFAULT 	 0x07U

#define REFLIMSCL3_FUNC_SAFE_REGADCBIST6_ADDR 	 0x1D34U // Control the accuracy for 1/8 scale measu... 
#define REFLIMSCL3_FUNC_SAFE_REGADCBIST6_MASK 	 0xFFU
#define REFLIMSCL3_FUNC_SAFE_REGADCBIST6_POS  	 0U

#define FUNC_SAFE_REGADCBIST7_ADDR    	 0x1D35U
#define FUNC_SAFE_REGADCBIST7_DEFAULT 	 0x03U

#define TLIMIT_FUNC_SAFE_REGADCBIST7_ADDR 	 0x1D35U // Control accuracy for alternate temperatu... 
#define TLIMIT_FUNC_SAFE_REGADCBIST7_MASK 	 0xFFU
#define TLIMIT_FUNC_SAFE_REGADCBIST7_POS  	 0U

#define FUNC_SAFE_REGADCBIST9_ADDR    	 0x1D37U
#define FUNC_SAFE_REGADCBIST9_DEFAULT 	 0x00U

#define MUXV_CTRL_FUNC_SAFE_REGADCBIST9_ADDR 	 0x1D37U // Value to drive GPIO during MUX testing. ... 
#define MUXV_CTRL_FUNC_SAFE_REGADCBIST9_MASK 	 0xFFU
#define MUXV_CTRL_FUNC_SAFE_REGADCBIST9_POS  	 0U

#define FUNC_SAFE_REGADCBIST12_ADDR    	 0x1D3AU
#define FUNC_SAFE_REGADCBIST12_DEFAULT 	 0xFFU

#define TMONCAL_OOD_WAIT_B2_FUNC_SAFE_REGADCBIST12_ADDR 	 0x1D3AU // Count value to set time before temperatu... 
#define TMONCAL_OOD_WAIT_B2_FUNC_SAFE_REGADCBIST12_MASK 	 0xFFU
#define TMONCAL_OOD_WAIT_B2_FUNC_SAFE_REGADCBIST12_POS  	 0U

#define FUNC_SAFE_REGADCBIST13_ADDR    	 0x1D3BU
#define FUNC_SAFE_REGADCBIST13_DEFAULT 	 0xFFU

#define T_EST_OUT_B0_FUNC_SAFE_REGADCBIST13_ADDR 	 0x1D3BU // Bits 7:0 of temperature sensor measureme... 
#define T_EST_OUT_B0_FUNC_SAFE_REGADCBIST13_MASK 	 0xFFU
#define T_EST_OUT_B0_FUNC_SAFE_REGADCBIST13_POS  	 0U

#define FUNC_SAFE_REGADCBIST14_ADDR    	 0x1D3CU
#define FUNC_SAFE_REGADCBIST14_DEFAULT 	 0xC3U

#define ALT_T_EST_OUT_B1_FUNC_SAFE_REGADCBIST14_ADDR 	 0x1D3CU // Bits 9:8 of temperature sensor measureme... 
#define ALT_T_EST_OUT_B1_FUNC_SAFE_REGADCBIST14_MASK 	 0x03U
#define ALT_T_EST_OUT_B1_FUNC_SAFE_REGADCBIST14_POS  	 0U

#define T_EST_OUT_B1_FUNC_SAFE_REGADCBIST14_ADDR 	 0x1D3CU // Bits 9:8 of temperature sensor measureme... 
#define T_EST_OUT_B1_FUNC_SAFE_REGADCBIST14_MASK 	 0xC0U
#define T_EST_OUT_B1_FUNC_SAFE_REGADCBIST14_POS  	 6U

#define FUNC_SAFE_REGADCBIST15_ADDR    	 0x1D3DU
#define FUNC_SAFE_REGADCBIST15_DEFAULT 	 0xFFU

#define ALT_T_EST_OUT_B0_FUNC_SAFE_REGADCBIST15_ADDR 	 0x1D3DU // Bits 7:0 of alternate temperature sensor... 
#define ALT_T_EST_OUT_B0_FUNC_SAFE_REGADCBIST15_MASK 	 0xFFU
#define ALT_T_EST_OUT_B0_FUNC_SAFE_REGADCBIST15_POS  	 0U

#define FUNC_SAFE_CC_RTTN_ERR_ADDR    	 0x1D5FU
#define FUNC_SAFE_CC_RTTN_ERR_DEFAULT 	 0x00U

#define INJECT_RTTN_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_ADDR 	 0x1D5FU // Set this bit before going into sleep mod... 
#define INJECT_RTTN_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_MASK 	 0x01U
#define INJECT_RTTN_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_POS  	 0U

#define INJECT_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_ADDR 	 0x1D5FU // Set this bit before reading efuse values... 
#define INJECT_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_MASK 	 0x02U
#define INJECT_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_POS  	 1U

#define RESET_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_ADDR 	 0x1D5FU // Reset efuse CRC error status to 0. Self-... 
#define RESET_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_MASK 	 0x04U
#define RESET_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_POS  	 2U

#endif
