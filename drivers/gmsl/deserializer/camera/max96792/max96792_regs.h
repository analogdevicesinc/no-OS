/******************************************************************************* 
Copyright (c) 2025 - Analog Devices Inc. All Rights Reserved. 
This software is proprietary & confidential to Analog Devices, Inc. 
and its licensors. 
****************************************************************************** 
 * \file: max96792_regs.h 
 * \brief: Header file of register fields for GMSL device MAX96792 
 * Developed by: Automotive Software and Systems team 
*****************************************************************************/ 
/*================================= INCLUDES ================================*/ 
/*================================= DEFINES =================================*/ 
#ifndef MAX96792_REGS_H
#define MAX96792_REGS_H

#define DEV_CTRL3_LINK_A_LOCK_ADDR 	 (0x13U)  
#define DEV_CTRL3_LINK_A_LOCK_MASK 	 (0x08U)

#define DEV_CTRL9_LINK_B_LOCK_ADDR 	 (0x5009U)  
#define DEV_CTRL9_LINK_B_LOCK_MASK 	 (0x08U)
#define REMAP_SRC_DST_REG_DISPLACEMENT          (0x40U)
#define MAP_DPHY_DEST_MIPI_TX_MIPI_MASK         (0x03U)
#define MAP_DPHY_DEST_MIPI_TX_NO_OF_VCS_FOR_REG (0x04U)
#define MAP_EN_SRC_DST_NO_OF_VCS_FOR_REG        (0x08U)
#define TUN_EN_MIPI_TX_MIPI_REG_DISPLACEMENT    (0x40U)
#define MIPI_TX_REG_OFFSET                      (0x40U)
#define MIPI_TX_0_MIPI_TX4_DESKEW_MASK          (0x81U)
#define MIPI_TX_ALT_MEM_MAP_MASK                (0X17U)
#define NO_OF_LANES_POLARITY                    (3U)
#define VALUE_2                                 (2U)
#define DISABLE_INITIAL_DESKEW                  (0x07U)
#define DISABLE_PERIODIC_DESKEW                 (0x01U)
#define VALUE_100                               (100U)
#define DEFAULT_MIPI_CLK                        (1500U)

#define DEV_REG0_ADDR    	 0x00U
#define DEV_REG0_DEFAULT 	 0x90U

#define CFG_BLOCK_DEV_REG0_ADDR 	 0x00U // Configuration Block  
#define CFG_BLOCK_DEV_REG0_MASK 	 0x01U
#define CFG_BLOCK_DEV_REG0_POS  	 0U

#define DEV_ADDR_DEV_REG0_ADDR 	 0x00U // Device I2C Address  
#define DEV_ADDR_DEV_REG0_MASK 	 0xFEU
#define DEV_ADDR_DEV_REG0_POS  	 1U

#define DEV_REG1_ADDR    	 0x01U
#define DEV_REG1_DEFAULT 	 0x02U

#define RX_RATE_DEV_REG1_ADDR 	 0x01U // Link A Receiver Rate  
#define RX_RATE_DEV_REG1_MASK 	 0x03U
#define RX_RATE_DEV_REG1_POS  	 0U

#define TX_RATE_DEV_REG1_ADDR 	 0x01U // Link A Transmitter Rate  
#define TX_RATE_DEV_REG1_MASK 	 0x0CU
#define TX_RATE_DEV_REG1_POS  	 2U

#define DIS_REM_CC_DEV_REG1_ADDR 	 0x01U // Disable access to Link A serializer cont... 
#define DIS_REM_CC_DEV_REG1_MASK 	 0x10U
#define DIS_REM_CC_DEV_REG1_POS  	 4U

#define DIS_LOCAL_CC_DEV_REG1_ADDR 	 0x01U // Disable control channel connection to RX... 
#define DIS_LOCAL_CC_DEV_REG1_MASK 	 0x20U
#define DIS_LOCAL_CC_DEV_REG1_POS  	 5U

#define IIC_1_EN_DEV_REG1_ADDR 	 0x01U // Enable pass-through I2C Channel 1 (SDA1/... 
#define IIC_1_EN_DEV_REG1_MASK 	 0x40U
#define IIC_1_EN_DEV_REG1_POS  	 6U

#define IIC_2_EN_DEV_REG1_ADDR 	 0x01U // Enable pass-through I2C Channel 2 (SDA2/... 
#define IIC_2_EN_DEV_REG1_MASK 	 0x80U
#define IIC_2_EN_DEV_REG1_POS  	 7U

#define DEV_REG2_ADDR    	 0x02U
#define DEV_REG2_DEFAULT 	 0x63U

#define VID_EN_Y_DEV_REG2_ADDR 	 0x02U // Enable data transmission through video p... 
#define VID_EN_Y_DEV_REG2_MASK 	 0x20U
#define VID_EN_Y_DEV_REG2_POS  	 5U

#define VID_EN_Z_DEV_REG2_ADDR 	 0x02U // Enable data transmission through video p... 
#define VID_EN_Z_DEV_REG2_MASK 	 0x40U
#define VID_EN_Z_DEV_REG2_POS  	 6U

#define DEV_REG3_ADDR    	 0x03U
#define DEV_REG3_DEFAULT 	 0x53U

#define DIS_REM_CC_B_DEV_REG3_ADDR 	 0x03U // Disable access to Link B serializer cont... 
#define DIS_REM_CC_B_DEV_REG3_MASK 	 0x04U
#define DIS_REM_CC_B_DEV_REG3_POS  	 2U

#define UART_1_EN_DEV_REG3_ADDR 	 0x03U // Enable pass-through UART Channel 1 (SDA1... 
#define UART_1_EN_DEV_REG3_MASK 	 0x10U
#define UART_1_EN_DEV_REG3_POS  	 4U

#define UART_2_EN_DEV_REG3_ADDR 	 0x03U // Enable pass-through UART Channel 2 (SDA2... 
#define UART_2_EN_DEV_REG3_MASK 	 0x20U
#define UART_2_EN_DEV_REG3_POS  	 5U

#define UART_PT_SWAP_DEV_REG3_ADDR 	 0x03U // Swap I2C/UART pass-through 1 pin assignm... 
#define UART_PT_SWAP_DEV_REG3_MASK 	 0x40U
#define UART_PT_SWAP_DEV_REG3_POS  	 6U

#define LOCK_CFG_DEV_REG3_ADDR 	 0x03U // Configure LOCK pin behavior  
#define LOCK_CFG_DEV_REG3_MASK 	 0x80U
#define LOCK_CFG_DEV_REG3_POS  	 7U

#define DEV_REG4_ADDR    	 0x04U
#define DEV_REG4_DEFAULT 	 0xC2U

#define RX_RATE_B_DEV_REG4_ADDR 	 0x04U // Link B Receiver Rate  
#define RX_RATE_B_DEV_REG4_MASK 	 0x03U
#define RX_RATE_B_DEV_REG4_POS  	 0U

#define TX_RATE_B_DEV_REG4_ADDR 	 0x04U // Link B Transmitter Rate  
#define TX_RATE_B_DEV_REG4_MASK 	 0x0CU
#define TX_RATE_B_DEV_REG4_POS  	 2U

#define GMSL3_A_DEV_REG4_ADDR 	 0x04U // GMSL3 or 2 selection for Link A.  
#define GMSL3_A_DEV_REG4_MASK 	 0x40U
#define GMSL3_A_DEV_REG4_POS  	 6U

#define GMSL3_B_DEV_REG4_ADDR 	 0x04U // GMSL3 or 2 selection for Link B.  
#define GMSL3_B_DEV_REG4_MASK 	 0x80U
#define GMSL3_B_DEV_REG4_POS  	 7U

#define DEV_REG5_ADDR    	 0x05U
#define DEV_REG5_DEFAULT 	 0xC0U

#define PU_LF0_DEV_REG5_ADDR 	 0x05U // Power up line-fault monitor 0  
#define PU_LF0_DEV_REG5_MASK 	 0x01U
#define PU_LF0_DEV_REG5_POS  	 0U

#define PU_LF1_DEV_REG5_ADDR 	 0x05U // Power up line-fault monitor 1  
#define PU_LF1_DEV_REG5_MASK 	 0x02U
#define PU_LF1_DEV_REG5_POS  	 1U

#define PU_LF2_DEV_REG5_ADDR 	 0x05U // Power up line-fault monitor 2  
#define PU_LF2_DEV_REG5_MASK 	 0x04U
#define PU_LF2_DEV_REG5_POS  	 2U

#define PU_LF3_DEV_REG5_ADDR 	 0x05U // Power up line-fault monitor 3  
#define PU_LF3_DEV_REG5_MASK 	 0x08U
#define PU_LF3_DEV_REG5_POS  	 3U

#define LOCK_ALT_EN_DEV_REG5_ADDR 	 0x05U // Enable LOCK alternate output  
#define LOCK_ALT_EN_DEV_REG5_MASK 	 0x20U
#define LOCK_ALT_EN_DEV_REG5_POS  	 5U

#define ERRB_EN_DEV_REG5_ADDR 	 0x05U // Enable ERRB output  
#define ERRB_EN_DEV_REG5_MASK 	 0x40U
#define ERRB_EN_DEV_REG5_POS  	 6U

#define LOCK_EN_DEV_REG5_ADDR 	 0x05U // Enable LOCK output  
#define LOCK_EN_DEV_REG5_MASK 	 0x80U
#define LOCK_EN_DEV_REG5_POS  	 7U

#define DEV_REG6_ADDR    	 0x06U
#define DEV_REG6_DEFAULT 	 0xC0U

#define I2CSEL_DEV_REG6_ADDR 	 0x06U // I2C/UART selection  
#define I2CSEL_DEV_REG6_MASK 	 0x10U
#define I2CSEL_DEV_REG6_POS  	 4U

#define DEV_REG7_ADDR    	 0x07U
#define DEV_REG7_DEFAULT 	 0x27U

#define CMP_VTERM_STATUS_DEV_REG7_ADDR 	 0x07U // VTERM supply undervoltage comparator sta... 
#define CMP_VTERM_STATUS_DEV_REG7_MASK 	 0x80U
#define CMP_VTERM_STATUS_DEV_REG7_POS  	 7U

#define DEV_REG13_ADDR    	 0x0DU
#define DEV_REG13_DEFAULT 	 0xB6U

#define DEV_ID_DEV_REG13_ADDR 	 0x0DU // Device Identifier  
#define DEV_ID_DEV_REG13_MASK 	 0xFFU
#define DEV_ID_DEV_REG13_POS  	 0U

#define DEV_REG14_ADDR    	 0x0EU
#define DEV_REG14_DEFAULT 	 0x03U

#define DEV_REV_DEV_REG14_ADDR 	 0x0EU // Device Revision. Refer to the device Err... 
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

#define DEV_REG27_ADDR    	 0x27U
#define DEV_REG27_DEFAULT 	 0x22U

#define LF_2_DEV_REG27_ADDR 	 0x27U // Line-fault status of wire connected to L... 
#define LF_2_DEV_REG27_MASK 	 0x07U
#define LF_2_DEV_REG27_POS  	 0U

#define LF_3_DEV_REG27_ADDR 	 0x27U // Line-fault status of wire connected to L... 
#define LF_3_DEV_REG27_MASK 	 0x70U
#define LF_3_DEV_REG27_POS  	 4U

#define DEV_IO_CHK0_ADDR    	 0x38U
#define DEV_IO_CHK0_DEFAULT 	 0x00U

#define PIN_DRV_EN_0_DEV_IO_CHK0_ADDR 	 0x38U // Bits 1:0: Set source clock frequency for... 
#define PIN_DRV_EN_0_DEV_IO_CHK0_MASK 	 0xFFU
#define PIN_DRV_EN_0_DEV_IO_CHK0_POS  	 0U

#define TCTRL_PWR0_ADDR    	 0x08U
#define TCTRL_PWR0_DEFAULT 	 0x00U

#define CMP_STATUS_TCTRL_PWR0_ADDR 	 0x08U // VDD18, VDDIO, and CAP_VDD supply voltage... 
#define CMP_STATUS_TCTRL_PWR0_MASK 	 0x1FU
#define CMP_STATUS_TCTRL_PWR0_POS  	 0U

#define VDDBAD_STATUS_TCTRL_PWR0_ADDR 	 0x08U // Switched 1V supply comparator status bit... 
#define VDDBAD_STATUS_TCTRL_PWR0_MASK 	 0xE0U
#define VDDBAD_STATUS_TCTRL_PWR0_POS  	 5U

#define TCTRL_PWR1_ADDR    	 0x09U
#define TCTRL_PWR1_DEFAULT 	 0x00U

#define PORZ_STATUS_TCTRL_PWR1_ADDR 	 0x09U // Power-on-reset status bits  
#define PORZ_STATUS_TCTRL_PWR1_MASK 	 0x3FU
#define PORZ_STATUS_TCTRL_PWR1_POS  	 0U

#define TCTRL_PWR4_ADDR    	 0x0CU
#define TCTRL_PWR4_DEFAULT 	 0x15U

#define WAKE_EN_A_TCTRL_PWR4_ADDR 	 0x0CU // Enable wake-up by remote chip connected ... 
#define WAKE_EN_A_TCTRL_PWR4_MASK 	 0x10U
#define WAKE_EN_A_TCTRL_PWR4_POS  	 4U

#define WAKE_EN_B_TCTRL_PWR4_ADDR 	 0x0CU // Enable wake-up by remote chip connected ... 
#define WAKE_EN_B_TCTRL_PWR4_MASK 	 0x20U
#define WAKE_EN_B_TCTRL_PWR4_POS  	 5U

#define DIS_LOCAL_WAKE_TCTRL_PWR4_ADDR 	 0x0CU // Disable wake-up by local μC from SDA_RX ... 
#define DIS_LOCAL_WAKE_TCTRL_PWR4_MASK 	 0x40U
#define DIS_LOCAL_WAKE_TCTRL_PWR4_POS  	 6U

#define TCTRL_CTRL0_ADDR    	 0x10U
#define TCTRL_CTRL0_DEFAULT 	 0x11U

#define LINK_CFG_TCTRL_CTRL0_ADDR 	 0x10U // AUTO_LINK and this bitfield selects the ... 
#define LINK_CFG_TCTRL_CTRL0_MASK 	 0x03U
#define LINK_CFG_TCTRL_CTRL0_POS  	 0U

#define REG_ENABLE_TCTRL_CTRL0_ADDR 	 0x10U // Enables VDD LDO regulator. See Programmi... 
#define REG_ENABLE_TCTRL_CTRL0_MASK 	 0x04U
#define REG_ENABLE_TCTRL_CTRL0_POS  	 2U

#define SLEEP_TCTRL_CTRL0_ADDR 	 0x10U // Activates sleep mode  
#define SLEEP_TCTRL_CTRL0_MASK 	 0x08U
#define SLEEP_TCTRL_CTRL0_POS  	 3U

#define AUTO_LINK_TCTRL_CTRL0_ADDR 	 0x10U // Automatically selects link configuration... 
#define AUTO_LINK_TCTRL_CTRL0_MASK 	 0x10U
#define AUTO_LINK_TCTRL_CTRL0_POS  	 4U

#define RESET_ONESHOT_TCTRL_CTRL0_ADDR 	 0x10U // One-Shot Link Reset  
#define RESET_ONESHOT_TCTRL_CTRL0_MASK 	 0x20U
#define RESET_ONESHOT_TCTRL_CTRL0_POS  	 5U

#define RESET_LINK_TCTRL_CTRL0_ADDR 	 0x10U // Link Reset  
#define RESET_LINK_TCTRL_CTRL0_MASK 	 0x40U
#define RESET_LINK_TCTRL_CTRL0_POS  	 6U

#define RESET_ALL_TCTRL_CTRL0_ADDR 	 0x10U // Device Reset  
#define RESET_ALL_TCTRL_CTRL0_MASK 	 0x80U
#define RESET_ALL_TCTRL_CTRL0_POS  	 7U

#define TCTRL_CTRL1_ADDR    	 0x11U
#define TCTRL_CTRL1_DEFAULT 	 0x0AU

#define CXTP_A_TCTRL_CTRL1_ADDR 	 0x11U // Coax/twisted-pair cable select for Link ... 
#define CXTP_A_TCTRL_CTRL1_MASK 	 0x01U
#define CXTP_A_TCTRL_CTRL1_POS  	 0U

#define CXTP_B_TCTRL_CTRL1_ADDR 	 0x11U // Coax/twisted-pair cable select for Link ... 
#define CXTP_B_TCTRL_CTRL1_MASK 	 0x04U
#define CXTP_B_TCTRL_CTRL1_POS  	 2U

#define TCTRL_CTRL2_ADDR    	 0x12U
#define TCTRL_CTRL2_DEFAULT 	 0x04U

#define LDO_TEST_TCTRL_CTRL2_ADDR 	 0x12U // Enable regulator manual mode to allow re... 
#define LDO_TEST_TCTRL_CTRL2_MASK 	 0x10U
#define LDO_TEST_TCTRL_CTRL2_POS  	 4U

#define RESET_ONESHOT_B_TCTRL_CTRL2_ADDR 	 0x12U // Reset entire data path on Link B (keep r... 
#define RESET_ONESHOT_B_TCTRL_CTRL2_MASK 	 0x20U
#define RESET_ONESHOT_B_TCTRL_CTRL2_POS  	 5U

#define TCTRL_CTRL3_ADDR    	 0x13U
#define TCTRL_CTRL3_DEFAULT 	 0x10U

#define RESET_LINK_B_TCTRL_CTRL3_ADDR 	 0x13U // Reset entire data path on Link B (keep r... 
#define RESET_LINK_B_TCTRL_CTRL3_MASK 	 0x01U
#define RESET_LINK_B_TCTRL_CTRL3_POS  	 0U

#define CMU_LOCKED_TCTRL_CTRL3_ADDR 	 0x13U // Clock multiplier unit (CMU) locked  
#define CMU_LOCKED_TCTRL_CTRL3_MASK 	 0x02U
#define CMU_LOCKED_TCTRL_CTRL3_POS  	 1U

#define ERROR_TCTRL_CTRL3_ADDR 	 0x13U // Reflects global error status  
#define ERROR_TCTRL_CTRL3_MASK 	 0x04U
#define ERROR_TCTRL_CTRL3_POS  	 2U

#define LOCKED_TCTRL_CTRL3_ADDR 	 0x13U // GMSL link locked (bidirectional). For Li... 
#define LOCKED_TCTRL_CTRL3_MASK 	 0x08U
#define LOCKED_TCTRL_CTRL3_POS  	 3U

#define LINK_MODE_TCTRL_CTRL3_ADDR 	 0x13U // Active link mode status  
#define LINK_MODE_TCTRL_CTRL3_MASK 	 0x30U
#define LINK_MODE_TCTRL_CTRL3_POS  	 4U

#define TCTRL_INTR0_ADDR    	 0x18U
#define TCTRL_INTR0_DEFAULT 	 0xA0U

#define DEC_ERR_THR_TCTRL_INTR0_ADDR 	 0x18U // Decoding and Idle-Error Reporting Thresh... 
#define DEC_ERR_THR_TCTRL_INTR0_MASK 	 0x07U
#define DEC_ERR_THR_TCTRL_INTR0_POS  	 0U

#define AUTO_ERR_RST_EN_TCTRL_INTR0_ADDR 	 0x18U // Automatically resets DEC_ERR_A (0x22), D... 
#define AUTO_ERR_RST_EN_TCTRL_INTR0_MASK 	 0x08U
#define AUTO_ERR_RST_EN_TCTRL_INTR0_POS  	 3U

#define TCTRL_INTR1_ADDR    	 0x19U
#define TCTRL_INTR1_DEFAULT 	 0x00U

#define PKT_CNT_THR_TCTRL_INTR1_ADDR 	 0x19U // Packet-count reporting threshold  
#define PKT_CNT_THR_TCTRL_INTR1_MASK 	 0x07U
#define PKT_CNT_THR_TCTRL_INTR1_POS  	 0U

#define AUTO_CNT_RST_EN_TCTRL_INTR1_ADDR 	 0x19U // Automatically resets PKT_CNT bitfield (r... 
#define AUTO_CNT_RST_EN_TCTRL_INTR1_MASK 	 0x08U
#define AUTO_CNT_RST_EN_TCTRL_INTR1_POS  	 3U

#define PKT_CNT_EXP_TCTRL_INTR1_ADDR 	 0x19U // Packet-Count Multiplier Exponent  
#define PKT_CNT_EXP_TCTRL_INTR1_MASK 	 0xF0U
#define PKT_CNT_EXP_TCTRL_INTR1_POS  	 4U

#define TCTRL_INTR2_ADDR    	 0x1AU
#define TCTRL_INTR2_DEFAULT 	 0x0BU

#define DEC_ERR_OEN_A_TCTRL_INTR2_ADDR 	 0x1AU // Enable reporting of decoding errors (DEC... 
#define DEC_ERR_OEN_A_TCTRL_INTR2_MASK 	 0x01U
#define DEC_ERR_OEN_A_TCTRL_INTR2_POS  	 0U

#define DEC_ERR_OEN_B_TCTRL_INTR2_ADDR 	 0x1AU // Enables reporting of decoding errors (DE... 
#define DEC_ERR_OEN_B_TCTRL_INTR2_MASK 	 0x02U
#define DEC_ERR_OEN_B_TCTRL_INTR2_POS  	 1U

#define IDLE_ERR_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enables reporting of idle-word errors (I... 
#define IDLE_ERR_OEN_TCTRL_INTR2_MASK 	 0x04U
#define IDLE_ERR_OEN_TCTRL_INTR2_POS  	 2U

#define LFLT_INT_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enables reporting of line-fault interrup... 
#define LFLT_INT_OEN_TCTRL_INTR2_MASK 	 0x08U
#define LFLT_INT_OEN_TCTRL_INTR2_POS  	 3U

#define REM_ERR_OEN_TCTRL_INTR2_ADDR 	 0x1AU // Enables reporting of remote-error status... 
#define REM_ERR_OEN_TCTRL_INTR2_MASK 	 0x20U
#define REM_ERR_OEN_TCTRL_INTR2_POS  	 5U

#define TCTRL_INTR3_ADDR    	 0x1BU
#define TCTRL_INTR3_DEFAULT 	 0x00U

#define DEC_ERR_FLAG_A_TCTRL_INTR3_ADDR 	 0x1BU // Decoding Error Flag for Link A  
#define DEC_ERR_FLAG_A_TCTRL_INTR3_MASK 	 0x01U
#define DEC_ERR_FLAG_A_TCTRL_INTR3_POS  	 0U

#define DEC_ERR_FLAG_B_TCTRL_INTR3_ADDR 	 0x1BU // Decoding Error Flag for Link B  
#define DEC_ERR_FLAG_B_TCTRL_INTR3_MASK 	 0x02U
#define DEC_ERR_FLAG_B_TCTRL_INTR3_POS  	 1U

#define IDLE_ERR_FLAG_TCTRL_INTR3_ADDR 	 0x1BU // Idle-Word-Error Flag for Link A  
#define IDLE_ERR_FLAG_TCTRL_INTR3_MASK 	 0x04U
#define IDLE_ERR_FLAG_TCTRL_INTR3_POS  	 2U

#define LFLT_INT_TCTRL_INTR3_ADDR 	 0x1BU // Line-Fault Interrupt  
#define LFLT_INT_TCTRL_INTR3_MASK 	 0x08U
#define LFLT_INT_TCTRL_INTR3_POS  	 3U

#define REM_ERR_FLAG_TCTRL_INTR3_ADDR 	 0x1BU // Received remote-side error status (inver... 
#define REM_ERR_FLAG_TCTRL_INTR3_MASK 	 0x20U
#define REM_ERR_FLAG_TCTRL_INTR3_POS  	 5U

#define TCTRL_INTR4_ADDR    	 0x1CU
#define TCTRL_INTR4_DEFAULT 	 0x09U

#define WM_ERR_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of watermark errors (WM... 
#define WM_ERR_OEN_TCTRL_INTR4_MASK 	 0x01U
#define WM_ERR_OEN_TCTRL_INTR4_POS  	 0U

#define PKT_CNT_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of Link A packet count ... 
#define PKT_CNT_OEN_TCTRL_INTR4_MASK 	 0x02U
#define PKT_CNT_OEN_TCTRL_INTR4_POS  	 1U

#define RT_CNT_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of Link A combined ARQ ... 
#define RT_CNT_OEN_TCTRL_INTR4_MASK 	 0x04U
#define RT_CNT_OEN_TCTRL_INTR4_POS  	 2U

#define MAX_RT_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of Link A combined ARQ ... 
#define MAX_RT_OEN_TCTRL_INTR4_MASK 	 0x08U
#define MAX_RT_OEN_TCTRL_INTR4_POS  	 3U

#define FEC_RX_ERR_OEN_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of Link A FEC receive e... 
#define FEC_RX_ERR_OEN_TCTRL_INTR4_MASK 	 0x20U
#define FEC_RX_ERR_OEN_TCTRL_INTR4_POS  	 5U

#define EOM_ERR_OEN_A_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of eye-opening monitor ... 
#define EOM_ERR_OEN_A_TCTRL_INTR4_MASK 	 0x40U
#define EOM_ERR_OEN_A_TCTRL_INTR4_POS  	 6U

#define EOM_ERR_OEN_B_TCTRL_INTR4_ADDR 	 0x1CU // Enable reporting of eye-opening monitor ... 
#define EOM_ERR_OEN_B_TCTRL_INTR4_MASK 	 0x80U
#define EOM_ERR_OEN_B_TCTRL_INTR4_POS  	 7U

#define TCTRL_INTR5_ADDR    	 0x1DU
#define TCTRL_INTR5_DEFAULT 	 0x00U

#define WM_ERR_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Watermark Error Flag  
#define WM_ERR_FLAG_TCTRL_INTR5_MASK 	 0x01U
#define WM_ERR_FLAG_TCTRL_INTR5_POS  	 0U

#define PKT_CNT_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Packet Count Flag.  For Link A only.  
#define PKT_CNT_FLAG_TCTRL_INTR5_MASK 	 0x02U
#define PKT_CNT_FLAG_TCTRL_INTR5_POS  	 1U

#define RT_CNT_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Combined ARQ Retransmission Event Flag. ... 
#define RT_CNT_FLAG_TCTRL_INTR5_MASK 	 0x04U
#define RT_CNT_FLAG_TCTRL_INTR5_POS  	 2U

#define MAX_RT_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // Combined ARQ maximum retransmission limi... 
#define MAX_RT_FLAG_TCTRL_INTR5_MASK 	 0x08U
#define MAX_RT_FLAG_TCTRL_INTR5_POS  	 3U

#define FEC_RX_ERR_FLAG_TCTRL_INTR5_ADDR 	 0x1DU // FEC Receive Errors Flag for Link A  
#define FEC_RX_ERR_FLAG_TCTRL_INTR5_MASK 	 0x20U
#define FEC_RX_ERR_FLAG_TCTRL_INTR5_POS  	 5U

#define EOM_ERR_FLAG_A_TCTRL_INTR5_ADDR 	 0x1DU // Eye-opening is below configured threshol... 
#define EOM_ERR_FLAG_A_TCTRL_INTR5_MASK 	 0x40U
#define EOM_ERR_FLAG_A_TCTRL_INTR5_POS  	 6U

#define EOM_ERR_FLAG_B_TCTRL_INTR5_ADDR 	 0x1DU // Eye-opening is below configured threshol... 
#define EOM_ERR_FLAG_B_TCTRL_INTR5_MASK 	 0x80U
#define EOM_ERR_FLAG_B_TCTRL_INTR5_POS  	 7U

#define TCTRL_INTR6_ADDR    	 0x1EU
#define TCTRL_INTR6_DEFAULT 	 0x1CU

#define VID_PXL_CRC_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of GMSL link packet CRC... 
#define VID_PXL_CRC_ERR_OEN_TCTRL_INTR6_MASK 	 0x01U
#define VID_PXL_CRC_ERR_OEN_TCTRL_INTR6_POS  	 0U

#define VPRBS_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of video PRBS errors (V... 
#define VPRBS_ERR_OEN_TCTRL_INTR6_MASK 	 0x04U
#define VPRBS_ERR_OEN_TCTRL_INTR6_POS  	 2U

#define LCRC_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of video-line CRC error... 
#define LCRC_ERR_OEN_TCTRL_INTR6_MASK 	 0x08U
#define LCRC_ERR_OEN_TCTRL_INTR6_POS  	 3U

#define FSYNC_ERR_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of frame-sync errors (F... 
#define FSYNC_ERR_OEN_TCTRL_INTR6_MASK 	 0x10U
#define FSYNC_ERR_OEN_TCTRL_INTR6_POS  	 4U

#define VDDBAD_INT_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of VDDBAD interrupt (VD... 
#define VDDBAD_INT_OEN_TCTRL_INTR6_MASK 	 0x20U
#define VDDBAD_INT_OEN_TCTRL_INTR6_POS  	 5U

#define VDDCMP_INT_OEN_TCTRL_INTR6_ADDR 	 0x1EU // Enable reporting of VDDCMP interrupt (VD... 
#define VDDCMP_INT_OEN_TCTRL_INTR6_MASK 	 0x80U
#define VDDCMP_INT_OEN_TCTRL_INTR6_POS  	 7U

#define TCTRL_INTR7_ADDR    	 0x1FU
#define TCTRL_INTR7_DEFAULT 	 0x00U

#define VID_PXL_CRC_ERR_TCTRL_INTR7_ADDR 	 0x1FU // GMSL link packet CRC error flag for vide... 
#define VID_PXL_CRC_ERR_TCTRL_INTR7_MASK 	 0x01U
#define VID_PXL_CRC_ERR_TCTRL_INTR7_POS  	 0U

#define VPRBS_ERR_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // Video PRBS error flag  
#define VPRBS_ERR_FLAG_TCTRL_INTR7_MASK 	 0x04U
#define VPRBS_ERR_FLAG_TCTRL_INTR7_POS  	 2U

#define LCRC_ERR_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // Video-line CRC error flag  
#define LCRC_ERR_FLAG_TCTRL_INTR7_MASK 	 0x08U
#define LCRC_ERR_FLAG_TCTRL_INTR7_POS  	 3U

#define FSYNC_ERR_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // Frame-sync error flag  
#define FSYNC_ERR_FLAG_TCTRL_INTR7_MASK 	 0x10U
#define FSYNC_ERR_FLAG_TCTRL_INTR7_POS  	 4U

#define VDDBAD_INT_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // VDD status interrupt. This is a flag for... 
#define VDDBAD_INT_FLAG_TCTRL_INTR7_MASK 	 0x20U
#define VDDBAD_INT_FLAG_TCTRL_INTR7_POS  	 5U

#define VDDCMP_INT_FLAG_TCTRL_INTR7_ADDR 	 0x1FU // VDDCMP interrupt flag. This is a combine... 
#define VDDCMP_INT_FLAG_TCTRL_INTR7_MASK 	 0x80U
#define VDDCMP_INT_FLAG_TCTRL_INTR7_POS  	 7U

#define TCTRL_INTR8_ADDR    	 0x20U
#define TCTRL_INTR8_DEFAULT 	 0xFFU

#define ERR_TX_ID_TCTRL_INTR8_ADDR 	 0x20U // GPIO ID used for transmitting ERR_TX  
#define ERR_TX_ID_TCTRL_INTR8_MASK 	 0x1FU
#define ERR_TX_ID_TCTRL_INTR8_POS  	 0U

#define ERR_TX_EN_B_TCTRL_INTR8_ADDR 	 0x20U // Transmit local-error status (inverse of ... 
#define ERR_TX_EN_B_TCTRL_INTR8_MASK 	 0x20U
#define ERR_TX_EN_B_TCTRL_INTR8_POS  	 5U

#define ERR_TX_EN_TCTRL_INTR8_ADDR 	 0x20U // Transmit local error status (inverse of ... 
#define ERR_TX_EN_TCTRL_INTR8_MASK 	 0x80U
#define ERR_TX_EN_TCTRL_INTR8_POS  	 7U

#define TCTRL_INTR9_ADDR    	 0x21U
#define TCTRL_INTR9_DEFAULT 	 0xFFU

#define ERR_RX_ID_TCTRL_INTR9_ADDR 	 0x21U // GPIO ID used for receiving ERR_RX  
#define ERR_RX_ID_TCTRL_INTR9_MASK 	 0x1FU
#define ERR_RX_ID_TCTRL_INTR9_POS  	 0U

#define ERR_RX_EN_B_TCTRL_INTR9_ADDR 	 0x21U // Receive Link B remote-error status (inve... 
#define ERR_RX_EN_B_TCTRL_INTR9_MASK 	 0x20U
#define ERR_RX_EN_B_TCTRL_INTR9_POS  	 5U

#define ERR_RX_EN_TCTRL_INTR9_ADDR 	 0x21U // Receive Link A remote-error status (inve... 
#define ERR_RX_EN_TCTRL_INTR9_MASK 	 0x80U
#define ERR_RX_EN_TCTRL_INTR9_POS  	 7U

#define TCTRL_CNT0_ADDR    	 0x22U
#define TCTRL_CNT0_DEFAULT 	 0x00U

#define DEC_ERR_A_TCTRL_CNT0_ADDR 	 0x22U // Number of decoding (disparity) errors de... 
#define DEC_ERR_A_TCTRL_CNT0_MASK 	 0xFFU
#define DEC_ERR_A_TCTRL_CNT0_POS  	 0U

#define TCTRL_CNT1_ADDR    	 0x23U
#define TCTRL_CNT1_DEFAULT 	 0x00U

#define DEC_ERR_B_TCTRL_CNT1_ADDR 	 0x23U // Number of decoding (disparity) errors de... 
#define DEC_ERR_B_TCTRL_CNT1_MASK 	 0xFFU
#define DEC_ERR_B_TCTRL_CNT1_POS  	 0U

#define TCTRL_CNT2_ADDR    	 0x24U
#define TCTRL_CNT2_DEFAULT 	 0x00U

#define IDLE_ERR_TCTRL_CNT2_ADDR 	 0x24U // Number of idle-word errors detected for ... 
#define IDLE_ERR_TCTRL_CNT2_MASK 	 0xFFU
#define IDLE_ERR_TCTRL_CNT2_POS  	 0U

#define TCTRL_CNT3_ADDR    	 0x25U
#define TCTRL_CNT3_DEFAULT 	 0x00U

#define PKT_CNT_TCTRL_CNT3_ADDR 	 0x25U // Number of received packets of a selected... 
#define PKT_CNT_TCTRL_CNT3_MASK 	 0xFFU
#define PKT_CNT_TCTRL_CNT3_POS  	 0U

#define GMSL_TX0_ADDR    	 0x28U
#define GMSL_TX0_DEFAULT 	 0x60U

#define RX_FEC_EN_GMSL_TX0_ADDR 	 0x28U // Enable FEC on Link A in forward directio... 
#define RX_FEC_EN_GMSL_TX0_MASK 	 0x02U
#define RX_FEC_EN_GMSL_TX0_POS  	 1U

#define GMSL_TX1_ADDR    	 0x29U
#define GMSL_TX1_DEFAULT 	 0x08U

#define ERRG_EN_A_GMSL_TX1_ADDR 	 0x29U // Error generator enable for Link A (rever... 
#define ERRG_EN_A_GMSL_TX1_MASK 	 0x10U
#define ERRG_EN_A_GMSL_TX1_POS  	 4U

#define LINK_PRBS_GEN_GMSL_TX1_ADDR 	 0x29U // Enable link PRBS-7 generator  
#define LINK_PRBS_GEN_GMSL_TX1_MASK 	 0x80U
#define LINK_PRBS_GEN_GMSL_TX1_POS  	 7U

#define GMSL_TX2_ADDR    	 0x2AU
#define GMSL_TX2_DEFAULT 	 0x20U

#define ERRG_PER_GMSL_TX2_ADDR 	 0x2AU // Error generator error-distribution selec... 
#define ERRG_PER_GMSL_TX2_MASK 	 0x01U
#define ERRG_PER_GMSL_TX2_POS  	 0U

#define ERRG_BURST_GMSL_TX2_ADDR 	 0x2AU // Error generator burst-error length  
#define ERRG_BURST_GMSL_TX2_MASK 	 0x0EU
#define ERRG_BURST_GMSL_TX2_POS  	 1U

#define ERRG_RATE_GMSL_TX2_ADDR 	 0x2AU // Error generator average bit-error rate  
#define ERRG_RATE_GMSL_TX2_MASK 	 0x30U
#define ERRG_RATE_GMSL_TX2_POS  	 4U

#define ERRG_CNT_GMSL_TX2_ADDR 	 0x2AU // Number of errors to be generated  
#define ERRG_CNT_GMSL_TX2_MASK 	 0xC0U
#define ERRG_CNT_GMSL_TX2_POS  	 6U

#define GMSL_TX3_ADDR    	 0x2BU
#define GMSL_TX3_DEFAULT 	 0x44U

#define RX_FEC_ACTIVE_GMSL_TX3_ADDR 	 0x2BU // FEC is active  
#define RX_FEC_ACTIVE_GMSL_TX3_MASK 	 0x20U
#define RX_FEC_ACTIVE_GMSL_TX3_POS  	 5U

#define GMSL_RX0_ADDR    	 0x2CU
#define GMSL_RX0_DEFAULT 	 0x00U

#define PKT_CNT_SEL_GMSL_RX0_ADDR 	 0x2CU // Select the type of received packets to c... 
#define PKT_CNT_SEL_GMSL_RX0_MASK 	 0x0FU
#define PKT_CNT_SEL_GMSL_RX0_POS  	 0U

#define PKT_CNT_LBW_GMSL_RX0_ADDR 	 0x2CU // Select the subtype of low-bandwidth (LBW... 
#define PKT_CNT_LBW_GMSL_RX0_MASK 	 0xC0U
#define PKT_CNT_LBW_GMSL_RX0_POS  	 6U

#define GMSL_RX1_ADDR    	 0x2DU
#define GMSL_RX1_DEFAULT 	 0x28U

#define LINK_PRBS_CHK_GMSL_RX1_ADDR 	 0x2DU // Enable link PRBS-7 checker  
#define LINK_PRBS_CHK_GMSL_RX1_MASK 	 0x80U
#define LINK_PRBS_CHK_GMSL_RX1_POS  	 7U

#define GMSL_RX3_ADDR    	 0x2FU
#define GMSL_RX3_DEFAULT 	 0x00U

#define LINK_PRBS_CHK_PAM4_GMSL_RX3_ADDR 	 0x2FU // Enable link PRBS-7 checker for PAM4 mode... 
#define LINK_PRBS_CHK_PAM4_GMSL_RX3_MASK 	 0x10U
#define LINK_PRBS_CHK_PAM4_GMSL_RX3_POS  	 4U

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

#define GPIO_TX_WNDW_GMSL_GPIOB_ADDR 	 0x31U // Wait time after a GPIO transition to cre... 
#define GPIO_TX_WNDW_GMSL_GPIOB_MASK 	 0xC0U
#define GPIO_TX_WNDW_GMSL_GPIOB_POS  	 6U

#define CC_I2C_0_ADDR    	 0x40U
#define CC_I2C_0_DEFAULT 	 0x26U

#define SLV_TO_CC_I2C_0_ADDR 	 0x40U // I2C-to-I2C subordinate timeout setting.  
#define SLV_TO_CC_I2C_0_MASK 	 0x07U
#define SLV_TO_CC_I2C_0_POS  	 0U

#define SLV_SH_CC_I2C_0_ADDR 	 0x40U // I2C-to-I2C subordinate-setup and hold-ti... 
#define SLV_SH_CC_I2C_0_MASK 	 0x30U
#define SLV_SH_CC_I2C_0_POS  	 4U

#define CC_I2C_1_ADDR    	 0x41U
#define CC_I2C_1_DEFAULT 	 0x56U

#define MST_TO_CC_I2C_1_ADDR 	 0x41U // I2C-to-I2C main timeout setting.  
#define MST_TO_CC_I2C_1_MASK 	 0x07U
#define MST_TO_CC_I2C_1_POS  	 0U

#define MST_BT_CC_I2C_1_ADDR 	 0x41U // I2C-to-I2C main bit rate setting.  
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

#define CC_I2C_7_ADDR    	 0x47U
#define CC_I2C_7_DEFAULT 	 0x00U

#define REM_ACK_RECVED_CC_I2C_7_ADDR 	 0x47U // I2C Ack Bit for any I2C byte is received... 
#define REM_ACK_RECVED_CC_I2C_7_MASK 	 0x01U
#define REM_ACK_RECVED_CC_I2C_7_POS  	 0U

#define REM_ACK_ACKED_CC_I2C_7_ADDR 	 0x47U // Inverse of the I2C Ack Bit received from... 
#define REM_ACK_ACKED_CC_I2C_7_MASK 	 0x02U
#define REM_ACK_ACKED_CC_I2C_7_POS  	 1U

#define I2C_TIMED_OUT_CC_I2C_7_ADDR 	 0x47U // Internal I2C-to-I2C subordinate or main ... 
#define I2C_TIMED_OUT_CC_I2C_7_MASK 	 0x04U
#define I2C_TIMED_OUT_CC_I2C_7_POS  	 2U

#define UART_TX_OVERFLOW_CC_I2C_7_ADDR 	 0x47U // Flag to indicate overflow of the UART FI... 
#define UART_TX_OVERFLOW_CC_I2C_7_MASK 	 0x40U
#define UART_TX_OVERFLOW_CC_I2C_7_POS  	 6U

#define UART_RX_OVERFLOW_CC_I2C_7_ADDR 	 0x47U // Flag to indicate overflow of the UART FI... 
#define UART_RX_OVERFLOW_CC_I2C_7_MASK 	 0x80U
#define UART_RX_OVERFLOW_CC_I2C_7_POS  	 7U

#define CC_UART_0_ADDR    	 0x48U
#define CC_UART_0_DEFAULT 	 0x42U

#define BYPASS_EN_CC_UART_0_ADDR 	 0x48U // Enable UART Soft-Bypass Mode  
#define BYPASS_EN_CC_UART_0_MASK 	 0x01U
#define BYPASS_EN_CC_UART_0_POS  	 0U

#define BYPASS_TO_CC_UART_0_ADDR 	 0x48U // UART Soft-Bypass Timeout Duration  
#define BYPASS_TO_CC_UART_0_MASK 	 0x06U
#define BYPASS_TO_CC_UART_0_POS  	 1U

#define BYPASS_DIS_PAR_CC_UART_0_ADDR 	 0x48U // Selects whether or not to receive and se... 
#define BYPASS_DIS_PAR_CC_UART_0_MASK 	 0x08U
#define BYPASS_DIS_PAR_CC_UART_0_POS  	 3U

#define LOC_MS_EN_CC_UART_0_ADDR 	 0x48U // Enables UART Bypass mode control by loca... 
#define LOC_MS_EN_CC_UART_0_MASK 	 0x10U
#define LOC_MS_EN_CC_UART_0_POS  	 4U

#define REM_MS_EN_CC_UART_0_ADDR 	 0x48U // Enables UART Bypass Mode Control by Remo... 
#define REM_MS_EN_CC_UART_0_MASK 	 0x20U
#define REM_MS_EN_CC_UART_0_POS  	 5U

#define CC_UART_1_ADDR    	 0x49U
#define CC_UART_1_DEFAULT 	 0x96U

#define BITLEN_LSB_CC_UART_1_ADDR 	 0x49U // UART detected bit length, low 8 bits. De... 
#define BITLEN_LSB_CC_UART_1_MASK 	 0xFFU
#define BITLEN_LSB_CC_UART_1_POS  	 0U

#define CC_UART_2_ADDR    	 0x4AU
#define CC_UART_2_DEFAULT 	 0x80U

#define BITLEN_MSB_CC_UART_2_ADDR 	 0x4AU // UART detected bit length, high 6 bits. D... 
#define BITLEN_MSB_CC_UART_2_MASK 	 0x3FU
#define BITLEN_MSB_CC_UART_2_POS  	 0U

#define OUT_DELAY_CC_UART_2_ADDR 	 0x4AU // UART initial output delay  
#define OUT_DELAY_CC_UART_2_MASK 	 0xC0U
#define OUT_DELAY_CC_UART_2_POS  	 6U

#define CC_I2C_PT_0_ADDR    	 0x4CU
#define CC_I2C_PT_0_DEFAULT 	 0x26U

#define SLV_TO_PT_CC_I2C_PT_0_ADDR 	 0x4CU // Pass-Through 1 and 2 I2C-to-I2C Subordin... 
#define SLV_TO_PT_CC_I2C_PT_0_MASK 	 0x07U
#define SLV_TO_PT_CC_I2C_PT_0_POS  	 0U

#define SLV_SH_PT_CC_I2C_PT_0_ADDR 	 0x4CU // Pass-Through 1 and 2 I2C-to-I2C Subordin... 
#define SLV_SH_PT_CC_I2C_PT_0_MASK 	 0x30U
#define SLV_SH_PT_CC_I2C_PT_0_POS  	 4U

#define CC_I2C_PT_1_ADDR    	 0x4DU
#define CC_I2C_PT_1_DEFAULT 	 0x56U

#define MST_TO_PT_CC_I2C_PT_1_ADDR 	 0x4DU // Pass-Through 1 and 2 I2C-to-I2C Main Tim... 
#define MST_TO_PT_CC_I2C_PT_1_MASK 	 0x07U
#define MST_TO_PT_CC_I2C_PT_1_POS  	 0U

#define MST_BT_PT_CC_I2C_PT_1_ADDR 	 0x4DU // Pass-Through 1 and 2 I2C-to-I2C Main Bit... 
#define MST_BT_PT_CC_I2C_PT_1_MASK 	 0x70U
#define MST_BT_PT_CC_I2C_PT_1_POS  	 4U

#define CC_I2C_PT_2_ADDR    	 0x4EU
#define CC_I2C_PT_2_DEFAULT 	 0x00U

#define I2C_TIMED_OUT_1_CC_I2C_PT_2_ADDR 	 0x4EU // In pass-through I2C channel 1, internal ... 
#define I2C_TIMED_OUT_1_CC_I2C_PT_2_MASK 	 0x04U
#define I2C_TIMED_OUT_1_CC_I2C_PT_2_POS  	 2U

#define I2C_TIMED_OUT_2_CC_I2C_PT_2_ADDR 	 0x4EU // In pass-through I2C channel 2, internal ... 
#define I2C_TIMED_OUT_2_CC_I2C_PT_2_MASK 	 0x40U
#define I2C_TIMED_OUT_2_CC_I2C_PT_2_POS  	 6U

#define CC_UART_PT_0_ADDR    	 0x4FU
#define CC_UART_PT_0_DEFAULT 	 0x88U

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

#define CFGH_VIDEO_X_RX0_ADDR    	 0x50U
#define CFGH_VIDEO_X_RX0_DEFAULT 	 0x00U

#define STR_SEL_CFGH_VIDEO_X_RX0_ADDR 	 0x50U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_CFGH_VIDEO_X_RX0_MASK 	 0x03U
#define STR_SEL_CFGH_VIDEO_X_RX0_POS  	 0U

#define RX_CRC_EN_CFGH_VIDEO_X_RX0_ADDR 	 0x50U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGH_VIDEO_X_RX0_MASK 	 0x80U
#define RX_CRC_EN_CFGH_VIDEO_X_RX0_POS  	 7U

#define CFGH_VIDEO_Y_RX0_ADDR    	 0x51U
#define CFGH_VIDEO_Y_RX0_DEFAULT 	 0x01U

#define STR_SEL_CFGH_VIDEO_Y_RX0_ADDR 	 0x51U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_CFGH_VIDEO_Y_RX0_MASK 	 0x03U
#define STR_SEL_CFGH_VIDEO_Y_RX0_POS  	 0U

#define RX_CRC_EN_CFGH_VIDEO_Y_RX0_ADDR 	 0x51U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGH_VIDEO_Y_RX0_MASK 	 0x80U
#define RX_CRC_EN_CFGH_VIDEO_Y_RX0_POS  	 7U

#define CFGH_VIDEO_Z_RX0_ADDR    	 0x52U
#define CFGH_VIDEO_Z_RX0_DEFAULT 	 0x02U

#define STR_SEL_CFGH_VIDEO_Z_RX0_ADDR 	 0x52U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_CFGH_VIDEO_Z_RX0_MASK 	 0x03U
#define STR_SEL_CFGH_VIDEO_Z_RX0_POS  	 0U

#define RX_CRC_EN_CFGH_VIDEO_Z_RX0_ADDR 	 0x52U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGH_VIDEO_Z_RX0_MASK 	 0x80U
#define RX_CRC_EN_CFGH_VIDEO_Z_RX0_POS  	 7U

#define CFGH_VIDEO_U_RX0_ADDR    	 0x53U
#define CFGH_VIDEO_U_RX0_DEFAULT 	 0x03U

#define STR_SEL_CFGH_VIDEO_U_RX0_ADDR 	 0x53U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_CFGH_VIDEO_U_RX0_MASK 	 0x03U
#define STR_SEL_CFGH_VIDEO_U_RX0_POS  	 0U

#define RX_CRC_EN_CFGH_VIDEO_U_RX0_ADDR 	 0x53U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGH_VIDEO_U_RX0_MASK 	 0x80U
#define RX_CRC_EN_CFGH_VIDEO_U_RX0_POS  	 7U

#define CFGI_INFOFR_TR0_ADDR    	 0x60U
#define CFGI_INFOFR_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_CFGI_INFOFR_TR0_ADDR 	 0x60U // Adjust the priority used for this channe... 
#define PRIO_CFG_CFGI_INFOFR_TR0_MASK 	 0x03U
#define PRIO_CFG_CFGI_INFOFR_TR0_POS  	 0U

#define PRIO_VAL_CFGI_INFOFR_TR0_ADDR 	 0x60U // Sets the priority for this channel's pac... 
#define PRIO_VAL_CFGI_INFOFR_TR0_MASK 	 0x0CU
#define PRIO_VAL_CFGI_INFOFR_TR0_POS  	 2U

#define RX_CRC_EN_CFGI_INFOFR_TR0_ADDR 	 0x60U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGI_INFOFR_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGI_INFOFR_TR0_POS  	 6U

#define TX_CRC_EN_CFGI_INFOFR_TR0_ADDR 	 0x60U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGI_INFOFR_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGI_INFOFR_TR0_POS  	 7U

#define CFGI_INFOFR_TR1_ADDR    	 0x61U
#define CFGI_INFOFR_TR1_DEFAULT 	 0xB0U

#define BW_VAL_CFGI_INFOFR_TR1_ADDR 	 0x61U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_CFGI_INFOFR_TR1_MASK 	 0x3FU
#define BW_VAL_CFGI_INFOFR_TR1_POS  	 0U

#define BW_MULT_CFGI_INFOFR_TR1_ADDR 	 0x61U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_CFGI_INFOFR_TR1_MASK 	 0xC0U
#define BW_MULT_CFGI_INFOFR_TR1_POS  	 6U

#define CFGI_INFOFR_TR3_ADDR    	 0x63U
#define CFGI_INFOFR_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGI_INFOFR_TR3_ADDR 	 0x63U // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGI_INFOFR_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGI_INFOFR_TR3_POS  	 0U

#define CFGI_INFOFR_TR4_ADDR    	 0x64U
#define CFGI_INFOFR_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGI_INFOFR_TR4_ADDR 	 0x64U // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGI_INFOFR_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGI_INFOFR_TR4_POS  	 0U

#define CFGL_SPI_TR0_ADDR    	 0x68U
#define CFGL_SPI_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_CFGL_SPI_TR0_ADDR 	 0x68U // Adjust the priority used for this channe... 
#define PRIO_CFG_CFGL_SPI_TR0_MASK 	 0x03U
#define PRIO_CFG_CFGL_SPI_TR0_POS  	 0U

#define PRIO_VAL_CFGL_SPI_TR0_ADDR 	 0x68U // Sets the priority for this channel's pac... 
#define PRIO_VAL_CFGL_SPI_TR0_MASK 	 0x0CU
#define PRIO_VAL_CFGL_SPI_TR0_POS  	 2U

#define RX_CRC_EN_CFGL_SPI_TR0_ADDR 	 0x68U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGL_SPI_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGL_SPI_TR0_POS  	 6U

#define TX_CRC_EN_CFGL_SPI_TR0_ADDR 	 0x68U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGL_SPI_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGL_SPI_TR0_POS  	 7U

#define CFGL_SPI_TR1_ADDR    	 0x69U
#define CFGL_SPI_TR1_DEFAULT 	 0xB0U

#define BW_VAL_CFGL_SPI_TR1_ADDR 	 0x69U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_CFGL_SPI_TR1_MASK 	 0x3FU
#define BW_VAL_CFGL_SPI_TR1_POS  	 0U

#define BW_MULT_CFGL_SPI_TR1_ADDR 	 0x69U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_CFGL_SPI_TR1_MASK 	 0xC0U
#define BW_MULT_CFGL_SPI_TR1_POS  	 6U

#define CFGL_SPI_TR3_ADDR    	 0x6BU
#define CFGL_SPI_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGL_SPI_TR3_ADDR 	 0x6BU // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGL_SPI_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGL_SPI_TR3_POS  	 0U

#define CFGL_SPI_TR4_ADDR    	 0x6CU
#define CFGL_SPI_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGL_SPI_TR4_ADDR 	 0x6CU // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGL_SPI_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGL_SPI_TR4_POS  	 0U

#define CFGL_SPI_ARQ0_ADDR    	 0x6DU
#define CFGL_SPI_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGL_SPI_ARQ0_ADDR 	 0x6DU // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGL_SPI_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGL_SPI_ARQ0_POS  	 2U

#define EN_CFGL_SPI_ARQ0_ADDR 	 0x6DU // Enable ARQ  
#define EN_CFGL_SPI_ARQ0_MASK 	 0x08U
#define EN_CFGL_SPI_ARQ0_POS  	 3U

#define ACK_SRC_ID_CFGL_SPI_ARQ0_ADDR 	 0x6DU // Select what to use as SRC_ID in transmit... 
#define ACK_SRC_ID_CFGL_SPI_ARQ0_MASK 	 0x10U
#define ACK_SRC_ID_CFGL_SPI_ARQ0_POS  	 4U

#define MATCH_SRC_ID_CFGL_SPI_ARQ0_ADDR 	 0x6DU // Ack packet source ID checking method  
#define MATCH_SRC_ID_CFGL_SPI_ARQ0_MASK 	 0x20U
#define MATCH_SRC_ID_CFGL_SPI_ARQ0_POS  	 5U

#define CFGL_SPI_ARQ1_ADDR    	 0x6EU
#define CFGL_SPI_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGL_SPI_ARQ1_ADDR 	 0x6EU // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGL_SPI_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGL_SPI_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGL_SPI_ARQ1_ADDR 	 0x6EU // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGL_SPI_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGL_SPI_ARQ1_POS  	 1U

#define MAX_RT_CFGL_SPI_ARQ1_ADDR 	 0x6EU // Maximum retransmit limit.  
#define MAX_RT_CFGL_SPI_ARQ1_MASK 	 0x70U
#define MAX_RT_CFGL_SPI_ARQ1_POS  	 4U

#define CFGL_SPI_ARQ2_ADDR    	 0x6FU
#define CFGL_SPI_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGL_SPI_ARQ2_ADDR 	 0x6FU // Total retransmission count in this chann... 
#define RT_CNT_CFGL_SPI_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGL_SPI_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGL_SPI_ARQ2_ADDR 	 0x6FU // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGL_SPI_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGL_SPI_ARQ2_POS  	 7U

#define CFGC_CC_TR0_ADDR    	 0x70U
#define CFGC_CC_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_CFGC_CC_TR0_ADDR 	 0x70U // Adjust the priority used for this channe... 
#define PRIO_CFG_CFGC_CC_TR0_MASK 	 0x03U
#define PRIO_CFG_CFGC_CC_TR0_POS  	 0U

#define PRIO_VAL_CFGC_CC_TR0_ADDR 	 0x70U // Sets the priority for this channel's pac... 
#define PRIO_VAL_CFGC_CC_TR0_MASK 	 0x0CU
#define PRIO_VAL_CFGC_CC_TR0_POS  	 2U

#define RX_CRC_EN_CFGC_CC_TR0_ADDR 	 0x70U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGC_CC_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGC_CC_TR0_POS  	 6U

#define TX_CRC_EN_CFGC_CC_TR0_ADDR 	 0x70U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGC_CC_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGC_CC_TR0_POS  	 7U

#define CFGC_CC_TR1_ADDR    	 0x71U
#define CFGC_CC_TR1_DEFAULT 	 0xB0U

#define BW_VAL_CFGC_CC_TR1_ADDR 	 0x71U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_CFGC_CC_TR1_MASK 	 0x3FU
#define BW_VAL_CFGC_CC_TR1_POS  	 0U

#define BW_MULT_CFGC_CC_TR1_ADDR 	 0x71U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_CFGC_CC_TR1_MASK 	 0xC0U
#define BW_MULT_CFGC_CC_TR1_POS  	 6U

#define CFGC_CC_TR3_ADDR    	 0x73U
#define CFGC_CC_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGC_CC_TR3_ADDR 	 0x73U // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGC_CC_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGC_CC_TR3_POS  	 0U

#define CFGC_CC_TR4_ADDR    	 0x74U
#define CFGC_CC_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGC_CC_TR4_ADDR 	 0x74U // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGC_CC_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGC_CC_TR4_POS  	 0U

#define CFGC_CC_ARQ0_ADDR    	 0x75U
#define CFGC_CC_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGC_CC_ARQ0_ADDR 	 0x75U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGC_CC_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGC_CC_ARQ0_POS  	 2U

#define EN_CFGC_CC_ARQ0_ADDR 	 0x75U // Enable ARQ  
#define EN_CFGC_CC_ARQ0_MASK 	 0x08U
#define EN_CFGC_CC_ARQ0_POS  	 3U

#define ACK_SRC_ID_CFGC_CC_ARQ0_ADDR 	 0x75U // Select what to use as SRC_ID in transmit... 
#define ACK_SRC_ID_CFGC_CC_ARQ0_MASK 	 0x10U
#define ACK_SRC_ID_CFGC_CC_ARQ0_POS  	 4U

#define MATCH_SRC_ID_CFGC_CC_ARQ0_ADDR 	 0x75U // Ack packet source ID checking method  
#define MATCH_SRC_ID_CFGC_CC_ARQ0_MASK 	 0x20U
#define MATCH_SRC_ID_CFGC_CC_ARQ0_POS  	 5U

#define CFGC_CC_ARQ1_ADDR    	 0x76U
#define CFGC_CC_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGC_CC_ARQ1_ADDR 	 0x76U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGC_CC_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGC_CC_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGC_CC_ARQ1_ADDR 	 0x76U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGC_CC_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGC_CC_ARQ1_POS  	 1U

#define MAX_RT_CFGC_CC_ARQ1_ADDR 	 0x76U // Maximum retransmit limit.  
#define MAX_RT_CFGC_CC_ARQ1_MASK 	 0x70U
#define MAX_RT_CFGC_CC_ARQ1_POS  	 4U

#define CFGC_CC_ARQ2_ADDR    	 0x77U
#define CFGC_CC_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGC_CC_ARQ2_ADDR 	 0x77U // Total retransmission count in this chann... 
#define RT_CNT_CFGC_CC_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGC_CC_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGC_CC_ARQ2_ADDR 	 0x77U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGC_CC_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGC_CC_ARQ2_POS  	 7U

#define CFGL_GPIO_TR0_ADDR    	 0x78U
#define CFGL_GPIO_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_CFGL_GPIO_TR0_ADDR 	 0x78U // Adjust the priority used for this channe... 
#define PRIO_CFG_CFGL_GPIO_TR0_MASK 	 0x03U
#define PRIO_CFG_CFGL_GPIO_TR0_POS  	 0U

#define PRIO_VAL_CFGL_GPIO_TR0_ADDR 	 0x78U // Sets the priority for this channel's pac... 
#define PRIO_VAL_CFGL_GPIO_TR0_MASK 	 0x0CU
#define PRIO_VAL_CFGL_GPIO_TR0_POS  	 2U

#define RX_CRC_EN_CFGL_GPIO_TR0_ADDR 	 0x78U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGL_GPIO_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGL_GPIO_TR0_POS  	 6U

#define TX_CRC_EN_CFGL_GPIO_TR0_ADDR 	 0x78U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGL_GPIO_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGL_GPIO_TR0_POS  	 7U

#define CFGL_GPIO_TR1_ADDR    	 0x79U
#define CFGL_GPIO_TR1_DEFAULT 	 0xB0U

#define BW_VAL_CFGL_GPIO_TR1_ADDR 	 0x79U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_CFGL_GPIO_TR1_MASK 	 0x3FU
#define BW_VAL_CFGL_GPIO_TR1_POS  	 0U

#define BW_MULT_CFGL_GPIO_TR1_ADDR 	 0x79U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_CFGL_GPIO_TR1_MASK 	 0xC0U
#define BW_MULT_CFGL_GPIO_TR1_POS  	 6U

#define CFGL_GPIO_TR3_ADDR    	 0x7BU
#define CFGL_GPIO_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGL_GPIO_TR3_ADDR 	 0x7BU // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGL_GPIO_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGL_GPIO_TR3_POS  	 0U

#define CFGL_GPIO_TR4_ADDR    	 0x7CU
#define CFGL_GPIO_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGL_GPIO_TR4_ADDR 	 0x7CU // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGL_GPIO_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGL_GPIO_TR4_POS  	 0U

#define CFGL_GPIO_ARQ0_ADDR    	 0x7DU
#define CFGL_GPIO_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGL_GPIO_ARQ0_ADDR 	 0x7DU // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGL_GPIO_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGL_GPIO_ARQ0_POS  	 2U

#define EN_CFGL_GPIO_ARQ0_ADDR 	 0x7DU // Enable ARQ  
#define EN_CFGL_GPIO_ARQ0_MASK 	 0x08U
#define EN_CFGL_GPIO_ARQ0_POS  	 3U

#define ACK_SRC_ID_CFGL_GPIO_ARQ0_ADDR 	 0x7DU // Select what to use as SRC_ID in transmit... 
#define ACK_SRC_ID_CFGL_GPIO_ARQ0_MASK 	 0x10U
#define ACK_SRC_ID_CFGL_GPIO_ARQ0_POS  	 4U

#define MATCH_SRC_ID_CFGL_GPIO_ARQ0_ADDR 	 0x7DU // Ack packet source ID checking method  
#define MATCH_SRC_ID_CFGL_GPIO_ARQ0_MASK 	 0x20U
#define MATCH_SRC_ID_CFGL_GPIO_ARQ0_POS  	 5U

#define CFGL_GPIO_ARQ1_ADDR    	 0x7EU
#define CFGL_GPIO_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGL_GPIO_ARQ1_ADDR 	 0x7EU // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGL_GPIO_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGL_GPIO_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGL_GPIO_ARQ1_ADDR 	 0x7EU // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGL_GPIO_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGL_GPIO_ARQ1_POS  	 1U

#define MAX_RT_CFGL_GPIO_ARQ1_ADDR 	 0x7EU // Maximum retransmit limit.  
#define MAX_RT_CFGL_GPIO_ARQ1_MASK 	 0x70U
#define MAX_RT_CFGL_GPIO_ARQ1_POS  	 4U

#define CFGL_GPIO_ARQ2_ADDR    	 0x7FU
#define CFGL_GPIO_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGL_GPIO_ARQ2_ADDR 	 0x7FU // Total retransmission count in this chann... 
#define RT_CNT_CFGL_GPIO_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGL_GPIO_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGL_GPIO_ARQ2_ADDR 	 0x7FU // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGL_GPIO_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGL_GPIO_ARQ2_POS  	 7U

#define CFGC_IIC_X_TR0_ADDR    	 0x80U
#define CFGC_IIC_X_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_CFGC_IIC_X_TR0_ADDR 	 0x80U // Adjust the priority used for this channe... 
#define PRIO_CFG_CFGC_IIC_X_TR0_MASK 	 0x03U
#define PRIO_CFG_CFGC_IIC_X_TR0_POS  	 0U

#define PRIO_VAL_CFGC_IIC_X_TR0_ADDR 	 0x80U // Sets the priority for this channel's pac... 
#define PRIO_VAL_CFGC_IIC_X_TR0_MASK 	 0x0CU
#define PRIO_VAL_CFGC_IIC_X_TR0_POS  	 2U

#define RX_CRC_EN_CFGC_IIC_X_TR0_ADDR 	 0x80U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGC_IIC_X_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGC_IIC_X_TR0_POS  	 6U

#define TX_CRC_EN_CFGC_IIC_X_TR0_ADDR 	 0x80U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGC_IIC_X_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGC_IIC_X_TR0_POS  	 7U

#define CFGC_IIC_X_TR1_ADDR    	 0x81U
#define CFGC_IIC_X_TR1_DEFAULT 	 0xB0U

#define BW_VAL_CFGC_IIC_X_TR1_ADDR 	 0x81U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_CFGC_IIC_X_TR1_MASK 	 0x3FU
#define BW_VAL_CFGC_IIC_X_TR1_POS  	 0U

#define BW_MULT_CFGC_IIC_X_TR1_ADDR 	 0x81U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_CFGC_IIC_X_TR1_MASK 	 0xC0U
#define BW_MULT_CFGC_IIC_X_TR1_POS  	 6U

#define CFGC_IIC_X_TR3_ADDR    	 0x83U
#define CFGC_IIC_X_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGC_IIC_X_TR3_ADDR 	 0x83U // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGC_IIC_X_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGC_IIC_X_TR3_POS  	 0U

#define CFGC_IIC_X_TR4_ADDR    	 0x84U
#define CFGC_IIC_X_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGC_IIC_X_TR4_ADDR 	 0x84U // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGC_IIC_X_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGC_IIC_X_TR4_POS  	 0U

#define CFGC_IIC_X_ARQ0_ADDR    	 0x85U
#define CFGC_IIC_X_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGC_IIC_X_ARQ0_ADDR 	 0x85U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGC_IIC_X_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGC_IIC_X_ARQ0_POS  	 2U

#define EN_CFGC_IIC_X_ARQ0_ADDR 	 0x85U // Enable ARQ  
#define EN_CFGC_IIC_X_ARQ0_MASK 	 0x08U
#define EN_CFGC_IIC_X_ARQ0_POS  	 3U

#define ACK_SRC_ID_CFGC_IIC_X_ARQ0_ADDR 	 0x85U // Select what to use as SRC_ID in transmit... 
#define ACK_SRC_ID_CFGC_IIC_X_ARQ0_MASK 	 0x10U
#define ACK_SRC_ID_CFGC_IIC_X_ARQ0_POS  	 4U

#define MATCH_SRC_ID_CFGC_IIC_X_ARQ0_ADDR 	 0x85U // Ack packet source ID checking method  
#define MATCH_SRC_ID_CFGC_IIC_X_ARQ0_MASK 	 0x20U
#define MATCH_SRC_ID_CFGC_IIC_X_ARQ0_POS  	 5U

#define CFGC_IIC_X_ARQ1_ADDR    	 0x86U
#define CFGC_IIC_X_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGC_IIC_X_ARQ1_ADDR 	 0x86U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGC_IIC_X_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGC_IIC_X_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGC_IIC_X_ARQ1_ADDR 	 0x86U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGC_IIC_X_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGC_IIC_X_ARQ1_POS  	 1U

#define MAX_RT_CFGC_IIC_X_ARQ1_ADDR 	 0x86U // Maximum retransmit limit.  
#define MAX_RT_CFGC_IIC_X_ARQ1_MASK 	 0x70U
#define MAX_RT_CFGC_IIC_X_ARQ1_POS  	 4U

#define CFGC_IIC_X_ARQ2_ADDR    	 0x87U
#define CFGC_IIC_X_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGC_IIC_X_ARQ2_ADDR 	 0x87U // Total retransmission count in this chann... 
#define RT_CNT_CFGC_IIC_X_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGC_IIC_X_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGC_IIC_X_ARQ2_ADDR 	 0x87U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGC_IIC_X_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGC_IIC_X_ARQ2_POS  	 7U

#define CFGC_IIC_Y_TR0_ADDR    	 0x88U
#define CFGC_IIC_Y_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_CFGC_IIC_Y_TR0_ADDR 	 0x88U // Adjust the priority used for this channe... 
#define PRIO_CFG_CFGC_IIC_Y_TR0_MASK 	 0x03U
#define PRIO_CFG_CFGC_IIC_Y_TR0_POS  	 0U

#define PRIO_VAL_CFGC_IIC_Y_TR0_ADDR 	 0x88U // Sets the priority for this channel's pac... 
#define PRIO_VAL_CFGC_IIC_Y_TR0_MASK 	 0x0CU
#define PRIO_VAL_CFGC_IIC_Y_TR0_POS  	 2U

#define RX_CRC_EN_CFGC_IIC_Y_TR0_ADDR 	 0x88U // When set, indicates that packets receive... 
#define RX_CRC_EN_CFGC_IIC_Y_TR0_MASK 	 0x40U
#define RX_CRC_EN_CFGC_IIC_Y_TR0_POS  	 6U

#define TX_CRC_EN_CFGC_IIC_Y_TR0_ADDR 	 0x88U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_CFGC_IIC_Y_TR0_MASK 	 0x80U
#define TX_CRC_EN_CFGC_IIC_Y_TR0_POS  	 7U

#define CFGC_IIC_Y_TR1_ADDR    	 0x89U
#define CFGC_IIC_Y_TR1_DEFAULT 	 0xB0U

#define BW_VAL_CFGC_IIC_Y_TR1_ADDR 	 0x89U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_CFGC_IIC_Y_TR1_MASK 	 0x3FU
#define BW_VAL_CFGC_IIC_Y_TR1_POS  	 0U

#define BW_MULT_CFGC_IIC_Y_TR1_ADDR 	 0x89U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_CFGC_IIC_Y_TR1_MASK 	 0xC0U
#define BW_MULT_CFGC_IIC_Y_TR1_POS  	 6U

#define CFGC_IIC_Y_TR3_ADDR    	 0x8BU
#define CFGC_IIC_Y_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_CFGC_IIC_Y_TR3_ADDR 	 0x8BU // Source identifier used in packets transm... 
#define TX_SRC_ID_CFGC_IIC_Y_TR3_MASK 	 0x07U
#define TX_SRC_ID_CFGC_IIC_Y_TR3_POS  	 0U

#define CFGC_IIC_Y_TR4_ADDR    	 0x8CU
#define CFGC_IIC_Y_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_CFGC_IIC_Y_TR4_ADDR 	 0x8CU // Receive packets from selected sources.  
#define RX_SRC_SEL_CFGC_IIC_Y_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_CFGC_IIC_Y_TR4_POS  	 0U

#define CFGC_IIC_Y_ARQ0_ADDR    	 0x8DU
#define CFGC_IIC_Y_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_CFGC_IIC_Y_ARQ0_ADDR 	 0x8DU // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_CFGC_IIC_Y_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_CFGC_IIC_Y_ARQ0_POS  	 2U

#define EN_CFGC_IIC_Y_ARQ0_ADDR 	 0x8DU // Enable ARQ  
#define EN_CFGC_IIC_Y_ARQ0_MASK 	 0x08U
#define EN_CFGC_IIC_Y_ARQ0_POS  	 3U

#define ACK_SRC_ID_CFGC_IIC_Y_ARQ0_ADDR 	 0x8DU // Select what to use as SRC_ID in transmit... 
#define ACK_SRC_ID_CFGC_IIC_Y_ARQ0_MASK 	 0x10U
#define ACK_SRC_ID_CFGC_IIC_Y_ARQ0_POS  	 4U

#define MATCH_SRC_ID_CFGC_IIC_Y_ARQ0_ADDR 	 0x8DU // Ack packet source ID checking method  
#define MATCH_SRC_ID_CFGC_IIC_Y_ARQ0_MASK 	 0x20U
#define MATCH_SRC_ID_CFGC_IIC_Y_ARQ0_POS  	 5U

#define CFGC_IIC_Y_ARQ1_ADDR    	 0x8EU
#define CFGC_IIC_Y_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_CFGC_IIC_Y_ARQ1_ADDR 	 0x8EU // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_CFGC_IIC_Y_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_CFGC_IIC_Y_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_CFGC_IIC_Y_ARQ1_ADDR 	 0x8EU // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_CFGC_IIC_Y_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_CFGC_IIC_Y_ARQ1_POS  	 1U

#define MAX_RT_CFGC_IIC_Y_ARQ1_ADDR 	 0x8EU // Maximum retransmit limit.  
#define MAX_RT_CFGC_IIC_Y_ARQ1_MASK 	 0x70U
#define MAX_RT_CFGC_IIC_Y_ARQ1_POS  	 4U

#define CFGC_IIC_Y_ARQ2_ADDR    	 0x8FU
#define CFGC_IIC_Y_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_CFGC_IIC_Y_ARQ2_ADDR 	 0x8FU // Total retransmission count in this chann... 
#define RT_CNT_CFGC_IIC_Y_ARQ2_MASK 	 0x7FU
#define RT_CNT_CFGC_IIC_Y_ARQ2_POS  	 0U

#define MAX_RT_ERR_CFGC_IIC_Y_ARQ2_ADDR 	 0x8FU // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_CFGC_IIC_Y_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_CFGC_IIC_Y_ARQ2_POS  	 7U

#define VID_RX_Y_VIDEO_RX0_ADDR    	 0x112U
#define VID_RX_Y_VIDEO_RX0_DEFAULT 	 0x32U

#define DIS_PKT_DET_VID_RX_Y_VIDEO_RX0_ADDR 	 0x112U // Disable Packet Detector.  
#define DIS_PKT_DET_VID_RX_Y_VIDEO_RX0_MASK 	 0x01U
#define DIS_PKT_DET_VID_RX_Y_VIDEO_RX0_POS  	 0U

#define LINE_CRC_EN_VID_RX_Y_VIDEO_RX0_ADDR 	 0x112U // Video-Line CRC Enable  
#define LINE_CRC_EN_VID_RX_Y_VIDEO_RX0_MASK 	 0x02U
#define LINE_CRC_EN_VID_RX_Y_VIDEO_RX0_POS  	 1U

#define LINE_CRC_SEL_VID_RX_Y_VIDEO_RX0_ADDR 	 0x112U // Line-CRC Trigger Selection  
#define LINE_CRC_SEL_VID_RX_Y_VIDEO_RX0_MASK 	 0x04U
#define LINE_CRC_SEL_VID_RX_Y_VIDEO_RX0_POS  	 2U

#define LCRC_ERR_VID_RX_Y_VIDEO_RX0_ADDR 	 0x112U // Video-Line CRC Error Flag  
#define LCRC_ERR_VID_RX_Y_VIDEO_RX0_MASK 	 0x80U
#define LCRC_ERR_VID_RX_Y_VIDEO_RX0_POS  	 7U

#define VID_RX_Y_VIDEO_RX3_ADDR    	 0x115U
#define VID_RX_Y_VIDEO_RX3_DEFAULT 	 0x40U

#define HTRACKEN_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // HS tracking enable (disable if FSYNC = 1... 
#define HTRACKEN_VID_RX_Y_VIDEO_RX3_MASK 	 0x01U
#define HTRACKEN_VID_RX_Y_VIDEO_RX3_POS  	 0U

#define VTRACKEN_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // VS tracking enable (disable if FSYNC = 1... 
#define VTRACKEN_VID_RX_Y_VIDEO_RX3_MASK 	 0x02U
#define VTRACKEN_VID_RX_Y_VIDEO_RX3_POS  	 1U

#define DTRACKEN_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // DE tracking enable (disable if FSYNC = 1... 
#define DTRACKEN_VID_RX_Y_VIDEO_RX3_MASK 	 0x04U
#define DTRACKEN_VID_RX_Y_VIDEO_RX3_POS  	 2U

#define HLOCKED_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // HS tracking locked  
#define HLOCKED_VID_RX_Y_VIDEO_RX3_MASK 	 0x08U
#define HLOCKED_VID_RX_Y_VIDEO_RX3_POS  	 3U

#define VLOCKED_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // VS tracking locked  
#define VLOCKED_VID_RX_Y_VIDEO_RX3_MASK 	 0x10U
#define VLOCKED_VID_RX_Y_VIDEO_RX3_POS  	 4U

#define DLOCKED_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // DE tracking locked  
#define DLOCKED_VID_RX_Y_VIDEO_RX3_MASK 	 0x20U
#define DLOCKED_VID_RX_Y_VIDEO_RX3_POS  	 5U

#define HD_TR_MODE_VID_RX_Y_VIDEO_RX3_ADDR 	 0x115U // Selects whether HS, DE tracking is fully... 
#define HD_TR_MODE_VID_RX_Y_VIDEO_RX3_MASK 	 0x40U
#define HD_TR_MODE_VID_RX_Y_VIDEO_RX3_POS  	 6U

#define VID_RX_Y_VIDEO_RX6_ADDR    	 0x118U
#define VID_RX_Y_VIDEO_RX6_DEFAULT 	 0x02U

#define LIM_HEART_VID_RX_Y_VIDEO_RX6_ADDR 	 0x118U // Disable heartbeat during blanking. Heart... 
#define LIM_HEART_VID_RX_Y_VIDEO_RX6_MASK 	 0x08U
#define LIM_HEART_VID_RX_Y_VIDEO_RX6_POS  	 3U

#define VID_RX_Y_VIDEO_RX8_ADDR    	 0x11AU
#define VID_RX_Y_VIDEO_RX8_DEFAULT 	 0x02U

#define VID_SEQ_ERR_VID_RX_Y_VIDEO_RX8_ADDR 	 0x11AU // Video Rx sequence error occurred. A sequ... 
#define VID_SEQ_ERR_VID_RX_Y_VIDEO_RX8_MASK 	 0x10U
#define VID_SEQ_ERR_VID_RX_Y_VIDEO_RX8_POS  	 4U

#define VID_PKT_DET_VID_RX_Y_VIDEO_RX8_ADDR 	 0x11AU // Sufficient video Rx packet throughput de... 
#define VID_PKT_DET_VID_RX_Y_VIDEO_RX8_MASK 	 0x20U
#define VID_PKT_DET_VID_RX_Y_VIDEO_RX8_POS  	 5U

#define VID_LOCK_VID_RX_Y_VIDEO_RX8_ADDR 	 0x11AU // Video pipeline locked.  
#define VID_LOCK_VID_RX_Y_VIDEO_RX8_MASK 	 0x40U
#define VID_LOCK_VID_RX_Y_VIDEO_RX8_POS  	 6U

#define VID_BLK_LEN_ERR_VID_RX_Y_VIDEO_RX8_ADDR 	 0x11AU // Video Rx video pixel data block-length e... 
#define VID_BLK_LEN_ERR_VID_RX_Y_VIDEO_RX8_MASK 	 0x80U
#define VID_BLK_LEN_ERR_VID_RX_Y_VIDEO_RX8_POS  	 7U

#define VID_RX_Y_VIDEO_RX10_ADDR    	 0x11CU
#define VID_RX_Y_VIDEO_RX10_DEFAULT 	 0x00U

#define MASK_VIDEO_DE_VID_RX_Y_VIDEO_RX10_ADDR 	 0x11CU // Masks video when DE is low  
#define MASK_VIDEO_DE_VID_RX_Y_VIDEO_RX10_MASK 	 0x40U
#define MASK_VIDEO_DE_VID_RX_Y_VIDEO_RX10_POS  	 6U

#define VID_OVERFLOW_VID_RX_Y_VIDEO_RX10_ADDR 	 0x11CU // Sticky bit for overflow detected in vide... 
#define VID_OVERFLOW_VID_RX_Y_VIDEO_RX10_MASK 	 0x80U
#define VID_OVERFLOW_VID_RX_Y_VIDEO_RX10_POS  	 7U

#define VID_RX_Z_VIDEO_RX0_ADDR    	 0x124U
#define VID_RX_Z_VIDEO_RX0_DEFAULT 	 0x32U

#define DIS_PKT_DET_VID_RX_Z_VIDEO_RX0_ADDR 	 0x124U // Disable Packet Detector.  
#define DIS_PKT_DET_VID_RX_Z_VIDEO_RX0_MASK 	 0x01U
#define DIS_PKT_DET_VID_RX_Z_VIDEO_RX0_POS  	 0U

#define LINE_CRC_EN_VID_RX_Z_VIDEO_RX0_ADDR 	 0x124U // Video-Line CRC Enable  
#define LINE_CRC_EN_VID_RX_Z_VIDEO_RX0_MASK 	 0x02U
#define LINE_CRC_EN_VID_RX_Z_VIDEO_RX0_POS  	 1U

#define LINE_CRC_SEL_VID_RX_Z_VIDEO_RX0_ADDR 	 0x124U // Line-CRC Trigger Selection  
#define LINE_CRC_SEL_VID_RX_Z_VIDEO_RX0_MASK 	 0x04U
#define LINE_CRC_SEL_VID_RX_Z_VIDEO_RX0_POS  	 2U

#define LCRC_ERR_VID_RX_Z_VIDEO_RX0_ADDR 	 0x124U // Video-Line CRC Error Flag  
#define LCRC_ERR_VID_RX_Z_VIDEO_RX0_MASK 	 0x80U
#define LCRC_ERR_VID_RX_Z_VIDEO_RX0_POS  	 7U

#define VID_RX_Z_VIDEO_RX3_ADDR    	 0x127U
#define VID_RX_Z_VIDEO_RX3_DEFAULT 	 0x40U

#define HTRACKEN_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // HS tracking enable (disable if FSYNC = 1... 
#define HTRACKEN_VID_RX_Z_VIDEO_RX3_MASK 	 0x01U
#define HTRACKEN_VID_RX_Z_VIDEO_RX3_POS  	 0U

#define VTRACKEN_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // VS tracking enable (disable if FSYNC = 1... 
#define VTRACKEN_VID_RX_Z_VIDEO_RX3_MASK 	 0x02U
#define VTRACKEN_VID_RX_Z_VIDEO_RX3_POS  	 1U

#define DTRACKEN_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // DE tracking enable (disable if FSYNC = 1... 
#define DTRACKEN_VID_RX_Z_VIDEO_RX3_MASK 	 0x04U
#define DTRACKEN_VID_RX_Z_VIDEO_RX3_POS  	 2U

#define HLOCKED_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // HS tracking locked  
#define HLOCKED_VID_RX_Z_VIDEO_RX3_MASK 	 0x08U
#define HLOCKED_VID_RX_Z_VIDEO_RX3_POS  	 3U

#define VLOCKED_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // VS tracking locked  
#define VLOCKED_VID_RX_Z_VIDEO_RX3_MASK 	 0x10U
#define VLOCKED_VID_RX_Z_VIDEO_RX3_POS  	 4U

#define DLOCKED_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // DE tracking locked  
#define DLOCKED_VID_RX_Z_VIDEO_RX3_MASK 	 0x20U
#define DLOCKED_VID_RX_Z_VIDEO_RX3_POS  	 5U

#define HD_TR_MODE_VID_RX_Z_VIDEO_RX3_ADDR 	 0x127U // Selects whether HS, DE tracking is fully... 
#define HD_TR_MODE_VID_RX_Z_VIDEO_RX3_MASK 	 0x40U
#define HD_TR_MODE_VID_RX_Z_VIDEO_RX3_POS  	 6U

#define VID_RX_Z_VIDEO_RX6_ADDR    	 0x12AU
#define VID_RX_Z_VIDEO_RX6_DEFAULT 	 0x02U

#define LIM_HEART_VID_RX_Z_VIDEO_RX6_ADDR 	 0x12AU // Disable heartbeat during blanking. Heart... 
#define LIM_HEART_VID_RX_Z_VIDEO_RX6_MASK 	 0x08U
#define LIM_HEART_VID_RX_Z_VIDEO_RX6_POS  	 3U

#define VID_RX_Z_VIDEO_RX8_ADDR    	 0x12CU
#define VID_RX_Z_VIDEO_RX8_DEFAULT 	 0x02U

#define VID_SEQ_ERR_VID_RX_Z_VIDEO_RX8_ADDR 	 0x12CU // Video Rx sequence error occurred. A sequ... 
#define VID_SEQ_ERR_VID_RX_Z_VIDEO_RX8_MASK 	 0x10U
#define VID_SEQ_ERR_VID_RX_Z_VIDEO_RX8_POS  	 4U

#define VID_PKT_DET_VID_RX_Z_VIDEO_RX8_ADDR 	 0x12CU // Sufficient video Rx packet throughput de... 
#define VID_PKT_DET_VID_RX_Z_VIDEO_RX8_MASK 	 0x20U
#define VID_PKT_DET_VID_RX_Z_VIDEO_RX8_POS  	 5U

#define VID_LOCK_VID_RX_Z_VIDEO_RX8_ADDR 	 0x12CU // Video pipeline locked.  
#define VID_LOCK_VID_RX_Z_VIDEO_RX8_MASK 	 0x40U
#define VID_LOCK_VID_RX_Z_VIDEO_RX8_POS  	 6U

#define VID_BLK_LEN_ERR_VID_RX_Z_VIDEO_RX8_ADDR 	 0x12CU // Video Rx video pixel data block-length e... 
#define VID_BLK_LEN_ERR_VID_RX_Z_VIDEO_RX8_MASK 	 0x80U
#define VID_BLK_LEN_ERR_VID_RX_Z_VIDEO_RX8_POS  	 7U

#define VID_RX_Z_VIDEO_RX10_ADDR    	 0x12EU
#define VID_RX_Z_VIDEO_RX10_DEFAULT 	 0x00U

#define MASK_VIDEO_DE_VID_RX_Z_VIDEO_RX10_ADDR 	 0x12EU // Masks video when DE is low  
#define MASK_VIDEO_DE_VID_RX_Z_VIDEO_RX10_MASK 	 0x40U
#define MASK_VIDEO_DE_VID_RX_Z_VIDEO_RX10_POS  	 6U

#define VID_OVERFLOW_VID_RX_Z_VIDEO_RX10_ADDR 	 0x12EU // Sticky bit for overflow detected in vide... 
#define VID_OVERFLOW_VID_RX_Z_VIDEO_RX10_MASK 	 0x80U
#define VID_OVERFLOW_VID_RX_Z_VIDEO_RX10_POS  	 7U

#define VIDEO_PIPE_SEL_VIDEO_PIPE_EN_ADDR    	 0x160U
#define VIDEO_PIPE_SEL_VIDEO_PIPE_EN_DEFAULT 	 0x03U

#define VIDEO_PIPE_EN_VIDEO_PIPE_SEL_VIDEO_PIPE_EN_ADDR 	 0x160U // Enables for video pipes  
#define VIDEO_PIPE_EN_VIDEO_PIPE_SEL_VIDEO_PIPE_EN_MASK 	 0x03U
#define VIDEO_PIPE_EN_VIDEO_PIPE_SEL_VIDEO_PIPE_EN_POS  	 0U

#define VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR    	 0x161U
#define VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_DEFAULT 	 0x32U

#define VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR 	 0x161U // Selects the incoming stream ID to receiv... 
#define VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK 	 0x07U
#define VIDEO_PIPE_SEL_Y_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_POS  	 0U

#define VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_ADDR 	 0x161U // Selects the incoming stream ID to receiv... 
#define VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_MASK 	 0x38U
#define VIDEO_PIPE_SEL_Z_VIDEO_PIPE_SEL_VIDEO_PIPE_SEL_POS  	 3U

#define VIDEO_PIPE_SEL_LINK_SEL_ADDR    	 0x162U
#define VIDEO_PIPE_SEL_LINK_SEL_DEFAULT 	 0x00U

#define UART_0_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_ADDR 	 0x162U // Control Channel UART Link Connection Sel... 
#define UART_0_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_MASK 	 0x01U
#define UART_0_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_POS  	 0U

#define UART_1_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_ADDR 	 0x162U // UART Pass-Through 1 Link Connection Sele... 
#define UART_1_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_MASK 	 0x02U
#define UART_1_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_POS  	 1U

#define UART_2_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_ADDR 	 0x162U // UART Pass-Through 2 Link Connection Sele... 
#define UART_2_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_MASK 	 0x04U
#define UART_2_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_POS  	 2U

#define SPI_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_ADDR 	 0x162U // Selects link connection for SPI Port  
#define SPI_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_MASK 	 0x08U
#define SPI_LINK_SELECT_VIDEO_PIPE_SEL_LINK_SEL_POS  	 3U

#define SPI_SPI_0_ADDR    	 0x170U
#define SPI_SPI_0_DEFAULT 	 0x08U

#define SPI_EN_SPI_SPI_0_ADDR 	 0x170U // Enable SPI channel  
#define SPI_EN_SPI_SPI_0_MASK 	 0x01U
#define SPI_EN_SPI_SPI_0_POS  	 0U

#define MST_SLVN_SPI_SPI_0_ADDR 	 0x170U // Selects if SPI is main or subordinate  
#define MST_SLVN_SPI_SPI_0_MASK 	 0x02U
#define MST_SLVN_SPI_SPI_0_POS  	 1U

#define SPI_CC_EN_SPI_SPI_0_ADDR 	 0x170U // Enable control-channel SPI bridge functi... 
#define SPI_CC_EN_SPI_SPI_0_MASK 	 0x04U
#define SPI_CC_EN_SPI_SPI_0_POS  	 2U

#define SPI_IGNR_ID_SPI_SPI_0_ADDR 	 0x170U // Selects if SPI should use or ignore head... 
#define SPI_IGNR_ID_SPI_SPI_0_MASK 	 0x08U
#define SPI_IGNR_ID_SPI_SPI_0_POS  	 3U

#define SPI_CC_TRG_ID_SPI_SPI_0_ADDR 	 0x170U // ID for GMSL2 header in SPI control-chann... 
#define SPI_CC_TRG_ID_SPI_SPI_0_MASK 	 0x30U
#define SPI_CC_TRG_ID_SPI_SPI_0_POS  	 4U

#define SPI_LOC_ID_SPI_SPI_0_ADDR 	 0x170U // Program to local ID if filtering packets... 
#define SPI_LOC_ID_SPI_SPI_0_MASK 	 0xC0U
#define SPI_LOC_ID_SPI_SPI_0_POS  	 6U

#define SPI_SPI_1_ADDR    	 0x171U
#define SPI_SPI_1_DEFAULT 	 0x1DU

#define SPI_BASE_PRIO_SPI_SPI_1_ADDR 	 0x171U // Starting GMSL2 request priority, advance... 
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

#define SPI_MOD3_SPI_SPI_2_ADDR 	 0x172U // Selects SPI mode 0 or 3.  
#define SPI_MOD3_SPI_SPI_2_MASK 	 0x04U
#define SPI_MOD3_SPI_SPI_2_POS  	 2U

#define SPI_MOD3_F_SPI_SPI_2_ADDR 	 0x172U // Allows the supression of an extra SCK pr... 
#define SPI_MOD3_F_SPI_SPI_2_MASK 	 0x08U
#define SPI_MOD3_F_SPI_SPI_2_POS  	 3U

#define FULL_SCK_SETUP_SPI_SPI_2_ADDR 	 0x172U // Sample MISO after half- or full-SCK peri... 
#define FULL_SCK_SETUP_SPI_SPI_2_MASK 	 0x10U
#define FULL_SCK_SETUP_SPI_SPI_2_POS  	 4U

#define REQ_HOLD_OFF_SPI_SPI_2_ADDR 	 0x172U // Hold off transmitting data across GMSL l... 
#define REQ_HOLD_OFF_SPI_SPI_2_MASK 	 0xE0U
#define REQ_HOLD_OFF_SPI_SPI_2_POS  	 5U

#define SPI_SPI_3_ADDR    	 0x173U
#define SPI_SPI_3_DEFAULT 	 0x00U

#define SPIM_SS_DLY_CLKS_SPI_SPI_3_ADDR 	 0x173U // ​​​​​​​Number of 300MHz clocks to delay ... 
#define SPIM_SS_DLY_CLKS_SPI_SPI_3_MASK 	 0xFFU
#define SPIM_SS_DLY_CLKS_SPI_SPI_3_POS  	 0U

#define SPI_SPI_4_ADDR    	 0x174U
#define SPI_SPI_4_DEFAULT 	 0x00U

#define SPIM_SCK_LO_CLKS_SPI_SPI_4_ADDR 	 0x174U // Number of 300MHz clocks for SCK low time... 
#define SPIM_SCK_LO_CLKS_SPI_SPI_4_MASK 	 0xFFU
#define SPIM_SCK_LO_CLKS_SPI_SPI_4_POS  	 0U

#define SPI_SPI_5_ADDR    	 0x175U
#define SPI_SPI_5_DEFAULT 	 0x00U

#define SPIM_SCK_HI_CLKS_SPI_SPI_5_ADDR 	 0x175U // Number of 300MHz clocks for SCK high tim... 
#define SPIM_SCK_HI_CLKS_SPI_SPI_5_MASK 	 0xFFU
#define SPIM_SCK_HI_CLKS_SPI_SPI_5_POS  	 0U

#define SPI_SPI_6_ADDR    	 0x176U
#define SPI_SPI_6_DEFAULT 	 0x00U

#define RWN_IO_EN_SPI_SPI_6_ADDR 	 0x176U // Enable GPIO for use as RO input for cont... 
#define RWN_IO_EN_SPI_SPI_6_MASK 	 0x01U
#define RWN_IO_EN_SPI_SPI_6_POS  	 0U

#define BNE_IO_EN_SPI_SPI_6_ADDR 	 0x176U // Enable GPIO for use as BNE output for SP... 
#define BNE_IO_EN_SPI_SPI_6_MASK 	 0x02U
#define BNE_IO_EN_SPI_SPI_6_POS  	 1U

#define SS_IO_EN_1_SPI_SPI_6_ADDR 	 0x176U // Enable GPIO for use as Subordinate Selec... 
#define SS_IO_EN_1_SPI_SPI_6_MASK 	 0x04U
#define SS_IO_EN_1_SPI_SPI_6_POS  	 2U

#define SS_IO_EN_2_SPI_SPI_6_ADDR 	 0x176U // Enable GPIO for use as Subordinate Selec... 
#define SS_IO_EN_2_SPI_SPI_6_MASK 	 0x08U
#define SS_IO_EN_2_SPI_SPI_6_POS  	 3U

#define SPIS_RWN_SPI_SPI_6_ADDR 	 0x176U // Alternate GPU control register to use fo... 
#define SPIS_RWN_SPI_SPI_6_MASK 	 0x10U
#define SPIS_RWN_SPI_SPI_6_POS  	 4U

#define BNE_SPI_SPI_6_ADDR 	 0x176U // Alternate GPU status register to use for... 
#define BNE_SPI_SPI_6_MASK 	 0x20U
#define BNE_SPI_SPI_6_POS  	 5U

#define SPI_SPI_7_ADDR    	 0x177U
#define SPI_SPI_7_DEFAULT 	 0x00U

#define SPIS_BYTE_CNT_SPI_SPI_7_ADDR 	 0x177U // Number of SPI data bytes available for r... 
#define SPIS_BYTE_CNT_SPI_SPI_7_MASK 	 0x1FU
#define SPIS_BYTE_CNT_SPI_SPI_7_POS  	 0U

#define RO_ALT_SPI_SPI_7_ADDR 	 0x177U // When set to 1, use the alternative pin f... 
#define RO_ALT_SPI_SPI_7_MASK 	 0x20U
#define RO_ALT_SPI_SPI_7_POS  	 5U

#define SPI_TX_OVRFLW_SPI_SPI_7_ADDR 	 0x177U // SPI Tx buffer overflow flag  
#define SPI_TX_OVRFLW_SPI_SPI_7_MASK 	 0x40U
#define SPI_TX_OVRFLW_SPI_SPI_7_POS  	 6U

#define SPI_RX_OVRFLW_SPI_SPI_7_ADDR 	 0x177U // SPI Rx buffer overflow flag  
#define SPI_RX_OVRFLW_SPI_SPI_7_MASK 	 0x80U
#define SPI_RX_OVRFLW_SPI_SPI_7_POS  	 7U

#define SPI_SPI_8_ADDR    	 0x178U
#define SPI_SPI_8_DEFAULT 	 0x00U

#define REQ_HOLD_OFF_TO_SPI_SPI_8_ADDR 	 0x178U // Timeout delay (in 100nS increments) for ... 
#define REQ_HOLD_OFF_TO_SPI_SPI_8_MASK 	 0xFFU
#define REQ_HOLD_OFF_TO_SPI_SPI_8_POS  	 0U

#define WM_WM_0_ADDR    	 0x190U
#define WM_WM_0_DEFAULT 	 0x00U

#define WM_EN_WM_WM_0_ADDR 	 0x190U // Watermark Enable  
#define WM_EN_WM_WM_0_MASK 	 0x01U
#define WM_EN_WM_WM_0_POS  	 0U

#define WM_DET_WM_WM_0_ADDR 	 0x190U // Watermark Detection/Generation  
#define WM_DET_WM_WM_0_MASK 	 0x0CU
#define WM_DET_WM_WM_0_POS  	 2U

#define WM_MODE_WM_WM_0_ADDR 	 0x190U // Watermark Mode  
#define WM_MODE_WM_WM_0_MASK 	 0x70U
#define WM_MODE_WM_WM_0_POS  	 4U

#define WM_LEN_WM_WM_0_ADDR 	 0x190U // Watermark Length  
#define WM_LEN_WM_WM_0_MASK 	 0x80U
#define WM_LEN_WM_WM_0_POS  	 7U

#define WM_WM_2_ADDR    	 0x192U
#define WM_WM_2_DEFAULT 	 0x50U

#define WM_NPFILT_WM_WM_2_ADDR 	 0x192U // Phase accumulator terminal count  
#define WM_NPFILT_WM_WM_2_MASK 	 0x03U
#define WM_NPFILT_WM_WM_2_POS  	 0U

#define VSYNCPOL_WM_WM_2_ADDR 	 0x192U // VS Polarity  
#define VSYNCPOL_WM_WM_2_MASK 	 0x04U
#define VSYNCPOL_WM_WM_2_POS  	 2U

#define HSYNCPOL_WM_WM_2_ADDR 	 0x192U // HS Polarity  
#define HSYNCPOL_WM_WM_2_MASK 	 0x08U
#define HSYNCPOL_WM_WM_2_POS  	 3U

#define WM_WM_4_ADDR    	 0x194U
#define WM_WM_4_DEFAULT 	 0x10U

#define WM_MASKMODE_WM_WM_4_ADDR 	 0x194U // Sets watermark video mask behavior for t... 
#define WM_MASKMODE_WM_WM_4_MASK 	 0x03U
#define WM_MASKMODE_WM_WM_4_POS  	 0U

#define WM_WM_5_ADDR    	 0x195U
#define WM_WM_5_DEFAULT 	 0x00U

#define WM_ERROR_WM_WM_5_ADDR 	 0x195U // Live value of active-high watermark erro... 
#define WM_ERROR_WM_WM_5_MASK 	 0x01U
#define WM_ERROR_WM_WM_5_POS  	 0U

#define WM_DETOUT_WM_WM_5_ADDR 	 0x195U // Live value of frame-based detection outp... 
#define WM_DETOUT_WM_WM_5_MASK 	 0x02U
#define WM_DETOUT_WM_WM_5_POS  	 1U

#define WM_WM_6_ADDR    	 0x196U
#define WM_WM_6_DEFAULT 	 0x00U

#define WM_TIMER_WM_WM_6_ADDR 	 0x196U // Time (in 2ms steps) the frozen-frame con... 
#define WM_TIMER_WM_WM_6_MASK 	 0xFFU
#define WM_TIMER_WM_WM_6_POS  	 0U

#define WM_WM_WREN_0_ADDR    	 0x1AEU
#define WM_WM_WREN_0_DEFAULT 	 0x00U

#define WM_WREN_L_WM_WM_WREN_0_ADDR 	 0x1AEU // Works in conjunction with WM_WREN_H to e... 
#define WM_WREN_L_WM_WM_WREN_0_MASK 	 0xFFU
#define WM_WREN_L_WM_WM_WREN_0_POS  	 0U

#define WM_WM_WREN_1_ADDR    	 0x1AFU
#define WM_WM_WREN_1_DEFAULT 	 0x00U

#define WM_WREN_H_WM_WM_WREN_1_ADDR 	 0x1AFU // Works in conjunction with WM_WREN_L to e... 
#define WM_WREN_H_WM_WM_WREN_1_MASK 	 0xFFU
#define WM_WREN_H_WM_WM_WREN_1_POS  	 0U

#define VRX_Y_CROSS_0_ADDR    	 0x1E0U
#define VRX_Y_CROSS_0_DEFAULT 	 0x00U

#define CROSS0_VRX_Y_CROSS_0_ADDR 	 0x1E0U // Maps incoming bit position set by this f... 
#define CROSS0_VRX_Y_CROSS_0_MASK 	 0x1FU
#define CROSS0_VRX_Y_CROSS_0_POS  	 0U

#define CROSS0_F_VRX_Y_CROSS_0_ADDR 	 0x1E0U // Force outgoing bit 0 to 0. Applied befor... 
#define CROSS0_F_VRX_Y_CROSS_0_MASK 	 0x20U
#define CROSS0_F_VRX_Y_CROSS_0_POS  	 5U

#define CROSS0_I_VRX_Y_CROSS_0_ADDR 	 0x1E0U // Invert outgoing bit 0  
#define CROSS0_I_VRX_Y_CROSS_0_MASK 	 0x40U
#define CROSS0_I_VRX_Y_CROSS_0_POS  	 6U

#define VRX_Y_CROSS_1_ADDR    	 0x1E1U
#define VRX_Y_CROSS_1_DEFAULT 	 0x01U

#define CROSS1_VRX_Y_CROSS_1_ADDR 	 0x1E1U // Maps incoming bit position set by this f... 
#define CROSS1_VRX_Y_CROSS_1_MASK 	 0x1FU
#define CROSS1_VRX_Y_CROSS_1_POS  	 0U

#define CROSS1_F_VRX_Y_CROSS_1_ADDR 	 0x1E1U // Force outgoing bit 1 to 0. Applied befor... 
#define CROSS1_F_VRX_Y_CROSS_1_MASK 	 0x20U
#define CROSS1_F_VRX_Y_CROSS_1_POS  	 5U

#define CROSS1_I_VRX_Y_CROSS_1_ADDR 	 0x1E1U // Invert outgoing bit 1  
#define CROSS1_I_VRX_Y_CROSS_1_MASK 	 0x40U
#define CROSS1_I_VRX_Y_CROSS_1_POS  	 6U

#define VRX_Y_CROSS_2_ADDR    	 0x1E2U
#define VRX_Y_CROSS_2_DEFAULT 	 0x02U

#define CROSS2_VRX_Y_CROSS_2_ADDR 	 0x1E2U // Maps incoming bit position set by this f... 
#define CROSS2_VRX_Y_CROSS_2_MASK 	 0x1FU
#define CROSS2_VRX_Y_CROSS_2_POS  	 0U

#define CROSS2_F_VRX_Y_CROSS_2_ADDR 	 0x1E2U // Force outgoing bit 2 to 0. Applied befor... 
#define CROSS2_F_VRX_Y_CROSS_2_MASK 	 0x20U
#define CROSS2_F_VRX_Y_CROSS_2_POS  	 5U

#define CROSS2_I_VRX_Y_CROSS_2_ADDR 	 0x1E2U // Invert outgoing bit 2  
#define CROSS2_I_VRX_Y_CROSS_2_MASK 	 0x40U
#define CROSS2_I_VRX_Y_CROSS_2_POS  	 6U

#define VRX_Y_CROSS_3_ADDR    	 0x1E3U
#define VRX_Y_CROSS_3_DEFAULT 	 0x03U

#define CROSS3_VRX_Y_CROSS_3_ADDR 	 0x1E3U // Maps incoming bit position set by this f... 
#define CROSS3_VRX_Y_CROSS_3_MASK 	 0x1FU
#define CROSS3_VRX_Y_CROSS_3_POS  	 0U

#define CROSS3_F_VRX_Y_CROSS_3_ADDR 	 0x1E3U // Force outgoing bit 3 to 0. Applied befor... 
#define CROSS3_F_VRX_Y_CROSS_3_MASK 	 0x20U
#define CROSS3_F_VRX_Y_CROSS_3_POS  	 5U

#define CROSS3_I_VRX_Y_CROSS_3_ADDR 	 0x1E3U // Invert outgoing bit 3  
#define CROSS3_I_VRX_Y_CROSS_3_MASK 	 0x40U
#define CROSS3_I_VRX_Y_CROSS_3_POS  	 6U

#define VRX_Y_CROSS_4_ADDR    	 0x1E4U
#define VRX_Y_CROSS_4_DEFAULT 	 0x04U

#define CROSS4_VRX_Y_CROSS_4_ADDR 	 0x1E4U // Maps incoming bit position set by this f... 
#define CROSS4_VRX_Y_CROSS_4_MASK 	 0x1FU
#define CROSS4_VRX_Y_CROSS_4_POS  	 0U

#define CROSS4_F_VRX_Y_CROSS_4_ADDR 	 0x1E4U // Force outgoing bit 4 to 0. Applied befor... 
#define CROSS4_F_VRX_Y_CROSS_4_MASK 	 0x20U
#define CROSS4_F_VRX_Y_CROSS_4_POS  	 5U

#define CROSS4_I_VRX_Y_CROSS_4_ADDR 	 0x1E4U // Invert outgoing bit 4  
#define CROSS4_I_VRX_Y_CROSS_4_MASK 	 0x40U
#define CROSS4_I_VRX_Y_CROSS_4_POS  	 6U

#define VRX_Y_CROSS_5_ADDR    	 0x1E5U
#define VRX_Y_CROSS_5_DEFAULT 	 0x05U

#define CROSS5_VRX_Y_CROSS_5_ADDR 	 0x1E5U // Maps incoming bit position set by this f... 
#define CROSS5_VRX_Y_CROSS_5_MASK 	 0x1FU
#define CROSS5_VRX_Y_CROSS_5_POS  	 0U

#define CROSS5_F_VRX_Y_CROSS_5_ADDR 	 0x1E5U // Force outgoing bit 5 to 0. Applied befor... 
#define CROSS5_F_VRX_Y_CROSS_5_MASK 	 0x20U
#define CROSS5_F_VRX_Y_CROSS_5_POS  	 5U

#define CROSS5_I_VRX_Y_CROSS_5_ADDR 	 0x1E5U // Invert outgoing bit 5  
#define CROSS5_I_VRX_Y_CROSS_5_MASK 	 0x40U
#define CROSS5_I_VRX_Y_CROSS_5_POS  	 6U

#define VRX_Y_CROSS_6_ADDR    	 0x1E6U
#define VRX_Y_CROSS_6_DEFAULT 	 0x06U

#define CROSS6_VRX_Y_CROSS_6_ADDR 	 0x1E6U // Maps incoming bit position set by this f... 
#define CROSS6_VRX_Y_CROSS_6_MASK 	 0x1FU
#define CROSS6_VRX_Y_CROSS_6_POS  	 0U

#define CROSS6_F_VRX_Y_CROSS_6_ADDR 	 0x1E6U // Force outgoing bit 6 to 0. Applied befor... 
#define CROSS6_F_VRX_Y_CROSS_6_MASK 	 0x20U
#define CROSS6_F_VRX_Y_CROSS_6_POS  	 5U

#define CROSS6_I_VRX_Y_CROSS_6_ADDR 	 0x1E6U // Invert outgoing bit 6  
#define CROSS6_I_VRX_Y_CROSS_6_MASK 	 0x40U
#define CROSS6_I_VRX_Y_CROSS_6_POS  	 6U

#define VRX_Y_CROSS_7_ADDR    	 0x1E7U
#define VRX_Y_CROSS_7_DEFAULT 	 0x07U

#define CROSS7_VRX_Y_CROSS_7_ADDR 	 0x1E7U // Maps incoming bit position set by this f... 
#define CROSS7_VRX_Y_CROSS_7_MASK 	 0x1FU
#define CROSS7_VRX_Y_CROSS_7_POS  	 0U

#define CROSS7_F_VRX_Y_CROSS_7_ADDR 	 0x1E7U // Force outgoing bit 7 to 0. Applied befor... 
#define CROSS7_F_VRX_Y_CROSS_7_MASK 	 0x20U
#define CROSS7_F_VRX_Y_CROSS_7_POS  	 5U

#define CROSS7_I_VRX_Y_CROSS_7_ADDR 	 0x1E7U // Invert outgoing bit 7  
#define CROSS7_I_VRX_Y_CROSS_7_MASK 	 0x40U
#define CROSS7_I_VRX_Y_CROSS_7_POS  	 6U

#define VRX_Y_CROSS_8_ADDR    	 0x1E8U
#define VRX_Y_CROSS_8_DEFAULT 	 0x08U

#define CROSS8_VRX_Y_CROSS_8_ADDR 	 0x1E8U // Maps incoming bit position set by this f... 
#define CROSS8_VRX_Y_CROSS_8_MASK 	 0x1FU
#define CROSS8_VRX_Y_CROSS_8_POS  	 0U

#define CROSS8_F_VRX_Y_CROSS_8_ADDR 	 0x1E8U // Force outgoing bit 8 to 0. Applied befor... 
#define CROSS8_F_VRX_Y_CROSS_8_MASK 	 0x20U
#define CROSS8_F_VRX_Y_CROSS_8_POS  	 5U

#define CROSS8_I_VRX_Y_CROSS_8_ADDR 	 0x1E8U // Invert outgoing bit 8  
#define CROSS8_I_VRX_Y_CROSS_8_MASK 	 0x40U
#define CROSS8_I_VRX_Y_CROSS_8_POS  	 6U

#define VRX_Y_CROSS_9_ADDR    	 0x1E9U
#define VRX_Y_CROSS_9_DEFAULT 	 0x09U

#define CROSS9_VRX_Y_CROSS_9_ADDR 	 0x1E9U // Maps incoming bit position set by this f... 
#define CROSS9_VRX_Y_CROSS_9_MASK 	 0x1FU
#define CROSS9_VRX_Y_CROSS_9_POS  	 0U

#define CROSS9_F_VRX_Y_CROSS_9_ADDR 	 0x1E9U // Force outgoing bit 9 to 0. Applied befor... 
#define CROSS9_F_VRX_Y_CROSS_9_MASK 	 0x20U
#define CROSS9_F_VRX_Y_CROSS_9_POS  	 5U

#define CROSS9_I_VRX_Y_CROSS_9_ADDR 	 0x1E9U // Invert outgoing bit 9  
#define CROSS9_I_VRX_Y_CROSS_9_MASK 	 0x40U
#define CROSS9_I_VRX_Y_CROSS_9_POS  	 6U

#define VRX_Y_CROSS_10_ADDR    	 0x1EAU
#define VRX_Y_CROSS_10_DEFAULT 	 0x0AU

#define CROSS10_VRX_Y_CROSS_10_ADDR 	 0x1EAU // Maps incoming bit position set by this f... 
#define CROSS10_VRX_Y_CROSS_10_MASK 	 0x1FU
#define CROSS10_VRX_Y_CROSS_10_POS  	 0U

#define CROSS10_F_VRX_Y_CROSS_10_ADDR 	 0x1EAU // Force outgoing bit 10 to 0. Applied befo... 
#define CROSS10_F_VRX_Y_CROSS_10_MASK 	 0x20U
#define CROSS10_F_VRX_Y_CROSS_10_POS  	 5U

#define CROSS10_I_VRX_Y_CROSS_10_ADDR 	 0x1EAU // Invert outgoing bit 10  
#define CROSS10_I_VRX_Y_CROSS_10_MASK 	 0x40U
#define CROSS10_I_VRX_Y_CROSS_10_POS  	 6U

#define VRX_Y_CROSS_11_ADDR    	 0x1EBU
#define VRX_Y_CROSS_11_DEFAULT 	 0x0BU

#define CROSS11_VRX_Y_CROSS_11_ADDR 	 0x1EBU // Maps incoming bit position set by this f... 
#define CROSS11_VRX_Y_CROSS_11_MASK 	 0x1FU
#define CROSS11_VRX_Y_CROSS_11_POS  	 0U

#define CROSS11_F_VRX_Y_CROSS_11_ADDR 	 0x1EBU // Force outgoing bit 11 to 0. Applied befo... 
#define CROSS11_F_VRX_Y_CROSS_11_MASK 	 0x20U
#define CROSS11_F_VRX_Y_CROSS_11_POS  	 5U

#define CROSS11_I_VRX_Y_CROSS_11_ADDR 	 0x1EBU // Invert outgoing bit 11  
#define CROSS11_I_VRX_Y_CROSS_11_MASK 	 0x40U
#define CROSS11_I_VRX_Y_CROSS_11_POS  	 6U

#define VRX_Y_CROSS_12_ADDR    	 0x1ECU
#define VRX_Y_CROSS_12_DEFAULT 	 0x0CU

#define CROSS12_VRX_Y_CROSS_12_ADDR 	 0x1ECU // Maps incoming bit position set by this f... 
#define CROSS12_VRX_Y_CROSS_12_MASK 	 0x1FU
#define CROSS12_VRX_Y_CROSS_12_POS  	 0U

#define CROSS12_F_VRX_Y_CROSS_12_ADDR 	 0x1ECU // Force outgoing bit 12 to 0. Applied befo... 
#define CROSS12_F_VRX_Y_CROSS_12_MASK 	 0x20U
#define CROSS12_F_VRX_Y_CROSS_12_POS  	 5U

#define CROSS12_I_VRX_Y_CROSS_12_ADDR 	 0x1ECU // Invert outgoing bit 12  
#define CROSS12_I_VRX_Y_CROSS_12_MASK 	 0x40U
#define CROSS12_I_VRX_Y_CROSS_12_POS  	 6U

#define VRX_Y_CROSS_13_ADDR    	 0x1EDU
#define VRX_Y_CROSS_13_DEFAULT 	 0x0DU

#define CROSS13_VRX_Y_CROSS_13_ADDR 	 0x1EDU // Maps incoming bit position set by this f... 
#define CROSS13_VRX_Y_CROSS_13_MASK 	 0x1FU
#define CROSS13_VRX_Y_CROSS_13_POS  	 0U

#define CROSS13_F_VRX_Y_CROSS_13_ADDR 	 0x1EDU // Force outgoing bit 13 to 0. Applied befo... 
#define CROSS13_F_VRX_Y_CROSS_13_MASK 	 0x20U
#define CROSS13_F_VRX_Y_CROSS_13_POS  	 5U

#define CROSS13_I_VRX_Y_CROSS_13_ADDR 	 0x1EDU // Invert outgoing bit 13  
#define CROSS13_I_VRX_Y_CROSS_13_MASK 	 0x40U
#define CROSS13_I_VRX_Y_CROSS_13_POS  	 6U

#define VRX_Y_CROSS_14_ADDR    	 0x1EEU
#define VRX_Y_CROSS_14_DEFAULT 	 0x0EU

#define CROSS14_VRX_Y_CROSS_14_ADDR 	 0x1EEU // Maps incoming bit position set by this f... 
#define CROSS14_VRX_Y_CROSS_14_MASK 	 0x1FU
#define CROSS14_VRX_Y_CROSS_14_POS  	 0U

#define CROSS14_F_VRX_Y_CROSS_14_ADDR 	 0x1EEU // Force outgoing bit 14 to 0. Applied befo... 
#define CROSS14_F_VRX_Y_CROSS_14_MASK 	 0x20U
#define CROSS14_F_VRX_Y_CROSS_14_POS  	 5U

#define CROSS14_I_VRX_Y_CROSS_14_ADDR 	 0x1EEU // Invert outgoing bit 14  
#define CROSS14_I_VRX_Y_CROSS_14_MASK 	 0x40U
#define CROSS14_I_VRX_Y_CROSS_14_POS  	 6U

#define VRX_Y_CROSS_15_ADDR    	 0x1EFU
#define VRX_Y_CROSS_15_DEFAULT 	 0x0FU

#define CROSS15_VRX_Y_CROSS_15_ADDR 	 0x1EFU // Maps incoming bit position set by this f... 
#define CROSS15_VRX_Y_CROSS_15_MASK 	 0x1FU
#define CROSS15_VRX_Y_CROSS_15_POS  	 0U

#define CROSS15_F_VRX_Y_CROSS_15_ADDR 	 0x1EFU // Force outgoing bit 15 to 0. Applied befo... 
#define CROSS15_F_VRX_Y_CROSS_15_MASK 	 0x20U
#define CROSS15_F_VRX_Y_CROSS_15_POS  	 5U

#define CROSS15_I_VRX_Y_CROSS_15_ADDR 	 0x1EFU // Invert outgoing bit 15  
#define CROSS15_I_VRX_Y_CROSS_15_MASK 	 0x40U
#define CROSS15_I_VRX_Y_CROSS_15_POS  	 6U

#define VRX_Y_CROSS_16_ADDR    	 0x1F0U
#define VRX_Y_CROSS_16_DEFAULT 	 0x10U

#define CROSS16_VRX_Y_CROSS_16_ADDR 	 0x1F0U // Maps incoming bit position set by this f... 
#define CROSS16_VRX_Y_CROSS_16_MASK 	 0x1FU
#define CROSS16_VRX_Y_CROSS_16_POS  	 0U

#define CROSS16_F_VRX_Y_CROSS_16_ADDR 	 0x1F0U // Force outgoing bit 16 to 0. Applied befo... 
#define CROSS16_F_VRX_Y_CROSS_16_MASK 	 0x20U
#define CROSS16_F_VRX_Y_CROSS_16_POS  	 5U

#define CROSS16_I_VRX_Y_CROSS_16_ADDR 	 0x1F0U // Invert outgoing bit 16  
#define CROSS16_I_VRX_Y_CROSS_16_MASK 	 0x40U
#define CROSS16_I_VRX_Y_CROSS_16_POS  	 6U

#define VRX_Y_CROSS_17_ADDR    	 0x1F1U
#define VRX_Y_CROSS_17_DEFAULT 	 0x11U

#define CROSS17_VRX_Y_CROSS_17_ADDR 	 0x1F1U // Maps incoming bit position set by this f... 
#define CROSS17_VRX_Y_CROSS_17_MASK 	 0x1FU
#define CROSS17_VRX_Y_CROSS_17_POS  	 0U

#define CROSS17_F_VRX_Y_CROSS_17_ADDR 	 0x1F1U // Force outgoing bit 17 to 0. Applied befo... 
#define CROSS17_F_VRX_Y_CROSS_17_MASK 	 0x20U
#define CROSS17_F_VRX_Y_CROSS_17_POS  	 5U

#define CROSS17_I_VRX_Y_CROSS_17_ADDR 	 0x1F1U // IInvert outgoing bit 17  
#define CROSS17_I_VRX_Y_CROSS_17_MASK 	 0x40U
#define CROSS17_I_VRX_Y_CROSS_17_POS  	 6U

#define VRX_Y_CROSS_18_ADDR    	 0x1F2U
#define VRX_Y_CROSS_18_DEFAULT 	 0x12U

#define CROSS18_VRX_Y_CROSS_18_ADDR 	 0x1F2U // Maps incoming bit position set by this f... 
#define CROSS18_VRX_Y_CROSS_18_MASK 	 0x1FU
#define CROSS18_VRX_Y_CROSS_18_POS  	 0U

#define CROSS18_F_VRX_Y_CROSS_18_ADDR 	 0x1F2U // Force outgoing bit 18 to 0. Applied befo... 
#define CROSS18_F_VRX_Y_CROSS_18_MASK 	 0x20U
#define CROSS18_F_VRX_Y_CROSS_18_POS  	 5U

#define CROSS18_I_VRX_Y_CROSS_18_ADDR 	 0x1F2U // Invert outgoing bit 18  
#define CROSS18_I_VRX_Y_CROSS_18_MASK 	 0x40U
#define CROSS18_I_VRX_Y_CROSS_18_POS  	 6U

#define VRX_Y_CROSS_19_ADDR    	 0x1F3U
#define VRX_Y_CROSS_19_DEFAULT 	 0x13U

#define CROSS19_VRX_Y_CROSS_19_ADDR 	 0x1F3U // Maps incoming bit position set by this f... 
#define CROSS19_VRX_Y_CROSS_19_MASK 	 0x1FU
#define CROSS19_VRX_Y_CROSS_19_POS  	 0U

#define CROSS19_F_VRX_Y_CROSS_19_ADDR 	 0x1F3U // Force outgoing bit 19 to 0. Applied befo... 
#define CROSS19_F_VRX_Y_CROSS_19_MASK 	 0x20U
#define CROSS19_F_VRX_Y_CROSS_19_POS  	 5U

#define CROSS19_I_VRX_Y_CROSS_19_ADDR 	 0x1F3U // Invert outgoing bit 19  
#define CROSS19_I_VRX_Y_CROSS_19_MASK 	 0x40U
#define CROSS19_I_VRX_Y_CROSS_19_POS  	 6U

#define VRX_Y_CROSS_20_ADDR    	 0x1F4U
#define VRX_Y_CROSS_20_DEFAULT 	 0x14U

#define CROSS20_VRX_Y_CROSS_20_ADDR 	 0x1F4U // Maps incoming bit position set by this f... 
#define CROSS20_VRX_Y_CROSS_20_MASK 	 0x1FU
#define CROSS20_VRX_Y_CROSS_20_POS  	 0U

#define CROSS20_F_VRX_Y_CROSS_20_ADDR 	 0x1F4U // Force outgoing bit 20 to 0. Applied befo... 
#define CROSS20_F_VRX_Y_CROSS_20_MASK 	 0x20U
#define CROSS20_F_VRX_Y_CROSS_20_POS  	 5U

#define CROSS20_I_VRX_Y_CROSS_20_ADDR 	 0x1F4U // Invert outgoing bit 20  
#define CROSS20_I_VRX_Y_CROSS_20_MASK 	 0x40U
#define CROSS20_I_VRX_Y_CROSS_20_POS  	 6U

#define VRX_Y_CROSS_21_ADDR    	 0x1F5U
#define VRX_Y_CROSS_21_DEFAULT 	 0x15U

#define CROSS21_VRX_Y_CROSS_21_ADDR 	 0x1F5U // Maps incoming bit position set by this f... 
#define CROSS21_VRX_Y_CROSS_21_MASK 	 0x1FU
#define CROSS21_VRX_Y_CROSS_21_POS  	 0U

#define CROSS21_F_VRX_Y_CROSS_21_ADDR 	 0x1F5U // Force outgoing bit 21 to 0. Applied befo... 
#define CROSS21_F_VRX_Y_CROSS_21_MASK 	 0x20U
#define CROSS21_F_VRX_Y_CROSS_21_POS  	 5U

#define CROSS21_I_VRX_Y_CROSS_21_ADDR 	 0x1F5U // Invert outgoing bit 21  
#define CROSS21_I_VRX_Y_CROSS_21_MASK 	 0x40U
#define CROSS21_I_VRX_Y_CROSS_21_POS  	 6U

#define VRX_Y_CROSS_22_ADDR    	 0x1F6U
#define VRX_Y_CROSS_22_DEFAULT 	 0x16U

#define CROSS22_VRX_Y_CROSS_22_ADDR 	 0x1F6U // Maps incoming bit position set by this f... 
#define CROSS22_VRX_Y_CROSS_22_MASK 	 0x1FU
#define CROSS22_VRX_Y_CROSS_22_POS  	 0U

#define CROSS22_F_VRX_Y_CROSS_22_ADDR 	 0x1F6U // Force outgoing bit 22 to 0. Applied befo... 
#define CROSS22_F_VRX_Y_CROSS_22_MASK 	 0x20U
#define CROSS22_F_VRX_Y_CROSS_22_POS  	 5U

#define CROSS22_I_VRX_Y_CROSS_22_ADDR 	 0x1F6U // Invert outgoing bit 22  
#define CROSS22_I_VRX_Y_CROSS_22_MASK 	 0x40U
#define CROSS22_I_VRX_Y_CROSS_22_POS  	 6U

#define VRX_Y_CROSS_23_ADDR    	 0x1F7U
#define VRX_Y_CROSS_23_DEFAULT 	 0x17U

#define CROSS23_VRX_Y_CROSS_23_ADDR 	 0x1F7U // Maps incoming bit position set by this f... 
#define CROSS23_VRX_Y_CROSS_23_MASK 	 0x1FU
#define CROSS23_VRX_Y_CROSS_23_POS  	 0U

#define CROSS23_F_VRX_Y_CROSS_23_ADDR 	 0x1F7U // Force outgoing bit 23 to 0. Applied befo... 
#define CROSS23_F_VRX_Y_CROSS_23_MASK 	 0x20U
#define CROSS23_F_VRX_Y_CROSS_23_POS  	 5U

#define CROSS23_I_VRX_Y_CROSS_23_ADDR 	 0x1F7U // Invert outgoing bit 23  
#define CROSS23_I_VRX_Y_CROSS_23_MASK 	 0x40U
#define CROSS23_I_VRX_Y_CROSS_23_POS  	 6U

#define VRX_Y_CROSS_HS_ADDR    	 0x1F8U
#define VRX_Y_CROSS_HS_DEFAULT 	 0x18U

#define CROSS_HS_VRX_Y_CROSS_HS_ADDR 	 0x1F8U // Map selected internal signal to HS  
#define CROSS_HS_VRX_Y_CROSS_HS_MASK 	 0x1FU
#define CROSS_HS_VRX_Y_CROSS_HS_POS  	 0U

#define CROSS_HS_F_VRX_Y_CROSS_HS_ADDR 	 0x1F8U // Force CROSS_HS to 0. Applied before inve... 
#define CROSS_HS_F_VRX_Y_CROSS_HS_MASK 	 0x20U
#define CROSS_HS_F_VRX_Y_CROSS_HS_POS  	 5U

#define CROSS_HS_I_VRX_Y_CROSS_HS_ADDR 	 0x1F8U // Invert CROSS_HS  
#define CROSS_HS_I_VRX_Y_CROSS_HS_MASK 	 0x40U
#define CROSS_HS_I_VRX_Y_CROSS_HS_POS  	 6U

#define VRX_Y_CROSS_VS_ADDR    	 0x1F9U
#define VRX_Y_CROSS_VS_DEFAULT 	 0x19U

#define CROSS_VS_VRX_Y_CROSS_VS_ADDR 	 0x1F9U // Map selected internal signal to VS  
#define CROSS_VS_VRX_Y_CROSS_VS_MASK 	 0x1FU
#define CROSS_VS_VRX_Y_CROSS_VS_POS  	 0U

#define CROSS_VS_F_VRX_Y_CROSS_VS_ADDR 	 0x1F9U // Force CROSS_VS to 0. Applied before inve... 
#define CROSS_VS_F_VRX_Y_CROSS_VS_MASK 	 0x20U
#define CROSS_VS_F_VRX_Y_CROSS_VS_POS  	 5U

#define CROSS_VS_I_VRX_Y_CROSS_VS_ADDR 	 0x1F9U // Invert CROSS_VS  
#define CROSS_VS_I_VRX_Y_CROSS_VS_MASK 	 0x40U
#define CROSS_VS_I_VRX_Y_CROSS_VS_POS  	 6U

#define VRX_Y_CROSS_DE_ADDR    	 0x1FAU
#define VRX_Y_CROSS_DE_DEFAULT 	 0x1AU

#define CROSS_DE_VRX_Y_CROSS_DE_ADDR 	 0x1FAU // Map selected internal signal to DE  
#define CROSS_DE_VRX_Y_CROSS_DE_MASK 	 0x1FU
#define CROSS_DE_VRX_Y_CROSS_DE_POS  	 0U

#define CROSS_DE_F_VRX_Y_CROSS_DE_ADDR 	 0x1FAU // Force CROSS_DE to 0. Applied before inve... 
#define CROSS_DE_F_VRX_Y_CROSS_DE_MASK 	 0x20U
#define CROSS_DE_F_VRX_Y_CROSS_DE_POS  	 5U

#define CROSS_DE_I_VRX_Y_CROSS_DE_ADDR 	 0x1FAU // Invert CROSS_DE  
#define CROSS_DE_I_VRX_Y_CROSS_DE_MASK 	 0x40U
#define CROSS_DE_I_VRX_Y_CROSS_DE_POS  	 6U

#define VRX_Y_PRBS_ERR_ADDR    	 0x1FBU
#define VRX_Y_PRBS_ERR_DEFAULT 	 0x00U

#define VPRBS_ERR_VRX_Y_PRBS_ERR_ADDR 	 0x1FBU // Video PRBS error counter, clears on read... 
#define VPRBS_ERR_VRX_Y_PRBS_ERR_MASK 	 0xFFU
#define VPRBS_ERR_VRX_Y_PRBS_ERR_POS  	 0U

#define VRX_Y_VPRBS_ADDR    	 0x1FCU
#define VRX_Y_VPRBS_DEFAULT 	 0x80U

#define VIDEO_LOCK_VRX_Y_VPRBS_ADDR 	 0x1FCU // Video channel is locked and outputting v... 
#define VIDEO_LOCK_VRX_Y_VPRBS_MASK 	 0x01U
#define VIDEO_LOCK_VRX_Y_VPRBS_POS  	 0U

#define VPRBS_CHK_EN_VRX_Y_VPRBS_ADDR 	 0x1FCU // Enable video PRBS checker  
#define VPRBS_CHK_EN_VRX_Y_VPRBS_MASK 	 0x10U
#define VPRBS_CHK_EN_VRX_Y_VPRBS_POS  	 4U

#define VPRBS_FAIL_VRX_Y_VPRBS_ADDR 	 0x1FCU // Video PRBS check pass/fail  
#define VPRBS_FAIL_VRX_Y_VPRBS_MASK 	 0x20U
#define VPRBS_FAIL_VRX_Y_VPRBS_POS  	 5U

#define PATGEN_CLK_SRC_VRX_Y_VPRBS_ADDR 	 0x1FCU // Pattern-generator clock source for video... 
#define PATGEN_CLK_SRC_VRX_Y_VPRBS_MASK 	 0x80U
#define PATGEN_CLK_SRC_VRX_Y_VPRBS_POS  	 7U

#define VRX_Y_CROSS_27_ADDR    	 0x1FDU
#define VRX_Y_CROSS_27_DEFAULT 	 0x1BU

#define CROSS27_VRX_Y_CROSS_27_ADDR 	 0x1FDU // Maps incoming bit position set by this f... 
#define CROSS27_VRX_Y_CROSS_27_MASK 	 0x1FU
#define CROSS27_VRX_Y_CROSS_27_POS  	 0U

#define CROSS27_F_VRX_Y_CROSS_27_ADDR 	 0x1FDU // Force outgoing bit 27 to 0. Applied befo... 
#define CROSS27_F_VRX_Y_CROSS_27_MASK 	 0x20U
#define CROSS27_F_VRX_Y_CROSS_27_POS  	 5U

#define CROSS27_I_VRX_Y_CROSS_27_ADDR 	 0x1FDU // Invert outgoing bit 27  
#define CROSS27_I_VRX_Y_CROSS_27_MASK 	 0x40U
#define CROSS27_I_VRX_Y_CROSS_27_POS  	 6U

#define ALT_CROSSBAR_VRX_Y_CROSS_27_ADDR 	 0x1FDU // Selects whether to use the crossbar in t... 
#define ALT_CROSSBAR_VRX_Y_CROSS_27_MASK 	 0x80U
#define ALT_CROSSBAR_VRX_Y_CROSS_27_POS  	 7U

#define VRX_Y_CROSS_28_ADDR    	 0x1FEU
#define VRX_Y_CROSS_28_DEFAULT 	 0x1CU

#define CROSS28_VRX_Y_CROSS_28_ADDR 	 0x1FEU // Maps incoming bit position set by this f... 
#define CROSS28_VRX_Y_CROSS_28_MASK 	 0x1FU
#define CROSS28_VRX_Y_CROSS_28_POS  	 0U

#define CROSS28_F_VRX_Y_CROSS_28_ADDR 	 0x1FEU // Force outgoing bit 28 to 0. Applied befo... 
#define CROSS28_F_VRX_Y_CROSS_28_MASK 	 0x20U
#define CROSS28_F_VRX_Y_CROSS_28_POS  	 5U

#define CROSS28_I_VRX_Y_CROSS_28_ADDR 	 0x1FEU // Invert outgoing bit 28  
#define CROSS28_I_VRX_Y_CROSS_28_MASK 	 0x40U
#define CROSS28_I_VRX_Y_CROSS_28_POS  	 6U

#define VRX_Y_CROSS_29_ADDR    	 0x1FFU
#define VRX_Y_CROSS_29_DEFAULT 	 0x1DU

#define CROSS29_VRX_Y_CROSS_29_ADDR 	 0x1FFU // Maps incoming bit position set by this f... 
#define CROSS29_VRX_Y_CROSS_29_MASK 	 0x1FU
#define CROSS29_VRX_Y_CROSS_29_POS  	 0U

#define CROSS29_F_VRX_Y_CROSS_29_ADDR 	 0x1FFU // Force outgoing bit 29 to 0. Applied befo... 
#define CROSS29_F_VRX_Y_CROSS_29_MASK 	 0x20U
#define CROSS29_F_VRX_Y_CROSS_29_POS  	 5U

#define CROSS29_I_VRX_Y_CROSS_29_ADDR 	 0x1FFU // Invert outgoing bit 29  
#define CROSS29_I_VRX_Y_CROSS_29_MASK 	 0x40U
#define CROSS29_I_VRX_Y_CROSS_29_POS  	 6U

#define VRX_Z_CROSS_0_ADDR    	 0x200U
#define VRX_Z_CROSS_0_DEFAULT 	 0x00U

#define CROSS0_VRX_Z_CROSS_0_ADDR 	 0x200U // Maps incoming bit position set by this f... 
#define CROSS0_VRX_Z_CROSS_0_MASK 	 0x1FU
#define CROSS0_VRX_Z_CROSS_0_POS  	 0U

#define CROSS0_F_VRX_Z_CROSS_0_ADDR 	 0x200U // Force outgoing bit 0 to 0. Applied befor... 
#define CROSS0_F_VRX_Z_CROSS_0_MASK 	 0x20U
#define CROSS0_F_VRX_Z_CROSS_0_POS  	 5U

#define CROSS0_I_VRX_Z_CROSS_0_ADDR 	 0x200U // Invert outgoing bit 0  
#define CROSS0_I_VRX_Z_CROSS_0_MASK 	 0x40U
#define CROSS0_I_VRX_Z_CROSS_0_POS  	 6U

#define VRX_Z_CROSS_1_ADDR    	 0x201U
#define VRX_Z_CROSS_1_DEFAULT 	 0x01U

#define CROSS1_VRX_Z_CROSS_1_ADDR 	 0x201U // Maps incoming bit position set by this f... 
#define CROSS1_VRX_Z_CROSS_1_MASK 	 0x1FU
#define CROSS1_VRX_Z_CROSS_1_POS  	 0U

#define CROSS1_F_VRX_Z_CROSS_1_ADDR 	 0x201U // Force outgoing bit 1 to 0. Applied befor... 
#define CROSS1_F_VRX_Z_CROSS_1_MASK 	 0x20U
#define CROSS1_F_VRX_Z_CROSS_1_POS  	 5U

#define CROSS1_I_VRX_Z_CROSS_1_ADDR 	 0x201U // Invert outgoing bit 1  
#define CROSS1_I_VRX_Z_CROSS_1_MASK 	 0x40U
#define CROSS1_I_VRX_Z_CROSS_1_POS  	 6U

#define VRX_Z_CROSS_2_ADDR    	 0x202U
#define VRX_Z_CROSS_2_DEFAULT 	 0x02U

#define CROSS2_VRX_Z_CROSS_2_ADDR 	 0x202U // Maps incoming bit position set by this f... 
#define CROSS2_VRX_Z_CROSS_2_MASK 	 0x1FU
#define CROSS2_VRX_Z_CROSS_2_POS  	 0U

#define CROSS2_F_VRX_Z_CROSS_2_ADDR 	 0x202U // Force outgoing bit 2 to 0. Applied befor... 
#define CROSS2_F_VRX_Z_CROSS_2_MASK 	 0x20U
#define CROSS2_F_VRX_Z_CROSS_2_POS  	 5U

#define CROSS2_I_VRX_Z_CROSS_2_ADDR 	 0x202U // Invert outgoing bit 2  
#define CROSS2_I_VRX_Z_CROSS_2_MASK 	 0x40U
#define CROSS2_I_VRX_Z_CROSS_2_POS  	 6U

#define VRX_Z_CROSS_3_ADDR    	 0x203U
#define VRX_Z_CROSS_3_DEFAULT 	 0x03U

#define CROSS3_VRX_Z_CROSS_3_ADDR 	 0x203U // Maps incoming bit position set by this f... 
#define CROSS3_VRX_Z_CROSS_3_MASK 	 0x1FU
#define CROSS3_VRX_Z_CROSS_3_POS  	 0U

#define CROSS3_F_VRX_Z_CROSS_3_ADDR 	 0x203U // Force outgoing bit 3 to 0. Applied befor... 
#define CROSS3_F_VRX_Z_CROSS_3_MASK 	 0x20U
#define CROSS3_F_VRX_Z_CROSS_3_POS  	 5U

#define CROSS3_I_VRX_Z_CROSS_3_ADDR 	 0x203U // Invert outgoing bit 3  
#define CROSS3_I_VRX_Z_CROSS_3_MASK 	 0x40U
#define CROSS3_I_VRX_Z_CROSS_3_POS  	 6U

#define VRX_Z_CROSS_4_ADDR    	 0x204U
#define VRX_Z_CROSS_4_DEFAULT 	 0x04U

#define CROSS4_VRX_Z_CROSS_4_ADDR 	 0x204U // Maps incoming bit position set by this f... 
#define CROSS4_VRX_Z_CROSS_4_MASK 	 0x1FU
#define CROSS4_VRX_Z_CROSS_4_POS  	 0U

#define CROSS4_F_VRX_Z_CROSS_4_ADDR 	 0x204U // Force outgoing bit 4 to 0. Applied befor... 
#define CROSS4_F_VRX_Z_CROSS_4_MASK 	 0x20U
#define CROSS4_F_VRX_Z_CROSS_4_POS  	 5U

#define CROSS4_I_VRX_Z_CROSS_4_ADDR 	 0x204U // Invert outgoing bit 4  
#define CROSS4_I_VRX_Z_CROSS_4_MASK 	 0x40U
#define CROSS4_I_VRX_Z_CROSS_4_POS  	 6U

#define VRX_Z_CROSS_5_ADDR    	 0x205U
#define VRX_Z_CROSS_5_DEFAULT 	 0x05U

#define CROSS5_VRX_Z_CROSS_5_ADDR 	 0x205U // Maps incoming bit position set by this f... 
#define CROSS5_VRX_Z_CROSS_5_MASK 	 0x1FU
#define CROSS5_VRX_Z_CROSS_5_POS  	 0U

#define CROSS5_F_VRX_Z_CROSS_5_ADDR 	 0x205U // Force outgoing bit 5 to 0. Applied befor... 
#define CROSS5_F_VRX_Z_CROSS_5_MASK 	 0x20U
#define CROSS5_F_VRX_Z_CROSS_5_POS  	 5U

#define CROSS5_I_VRX_Z_CROSS_5_ADDR 	 0x205U // Invert outgoing bit 5  
#define CROSS5_I_VRX_Z_CROSS_5_MASK 	 0x40U
#define CROSS5_I_VRX_Z_CROSS_5_POS  	 6U

#define VRX_Z_CROSS_6_ADDR    	 0x206U
#define VRX_Z_CROSS_6_DEFAULT 	 0x06U

#define CROSS6_VRX_Z_CROSS_6_ADDR 	 0x206U // Maps incoming bit position set by this f... 
#define CROSS6_VRX_Z_CROSS_6_MASK 	 0x1FU
#define CROSS6_VRX_Z_CROSS_6_POS  	 0U

#define CROSS6_F_VRX_Z_CROSS_6_ADDR 	 0x206U // Force outgoing bit 6 to 0. Applied befor... 
#define CROSS6_F_VRX_Z_CROSS_6_MASK 	 0x20U
#define CROSS6_F_VRX_Z_CROSS_6_POS  	 5U

#define CROSS6_I_VRX_Z_CROSS_6_ADDR 	 0x206U // Invert outgoing bit 6  
#define CROSS6_I_VRX_Z_CROSS_6_MASK 	 0x40U
#define CROSS6_I_VRX_Z_CROSS_6_POS  	 6U

#define VRX_Z_CROSS_7_ADDR    	 0x207U
#define VRX_Z_CROSS_7_DEFAULT 	 0x07U

#define CROSS7_VRX_Z_CROSS_7_ADDR 	 0x207U // Maps incoming bit position set by this f... 
#define CROSS7_VRX_Z_CROSS_7_MASK 	 0x1FU
#define CROSS7_VRX_Z_CROSS_7_POS  	 0U

#define CROSS7_F_VRX_Z_CROSS_7_ADDR 	 0x207U // Force outgoing bit 7 to 0. Applied befor... 
#define CROSS7_F_VRX_Z_CROSS_7_MASK 	 0x20U
#define CROSS7_F_VRX_Z_CROSS_7_POS  	 5U

#define CROSS7_I_VRX_Z_CROSS_7_ADDR 	 0x207U // Invert outgoing bit 7  
#define CROSS7_I_VRX_Z_CROSS_7_MASK 	 0x40U
#define CROSS7_I_VRX_Z_CROSS_7_POS  	 6U

#define VRX_Z_CROSS_8_ADDR    	 0x208U
#define VRX_Z_CROSS_8_DEFAULT 	 0x08U

#define CROSS8_VRX_Z_CROSS_8_ADDR 	 0x208U // Maps incoming bit position set by this f... 
#define CROSS8_VRX_Z_CROSS_8_MASK 	 0x1FU
#define CROSS8_VRX_Z_CROSS_8_POS  	 0U

#define CROSS8_F_VRX_Z_CROSS_8_ADDR 	 0x208U // Force outgoing bit 8 to 0. Applied befor... 
#define CROSS8_F_VRX_Z_CROSS_8_MASK 	 0x20U
#define CROSS8_F_VRX_Z_CROSS_8_POS  	 5U

#define CROSS8_I_VRX_Z_CROSS_8_ADDR 	 0x208U // Invert outgoing bit 8  
#define CROSS8_I_VRX_Z_CROSS_8_MASK 	 0x40U
#define CROSS8_I_VRX_Z_CROSS_8_POS  	 6U

#define VRX_Z_CROSS_9_ADDR    	 0x209U
#define VRX_Z_CROSS_9_DEFAULT 	 0x09U

#define CROSS9_VRX_Z_CROSS_9_ADDR 	 0x209U // Maps incoming bit position set by this f... 
#define CROSS9_VRX_Z_CROSS_9_MASK 	 0x1FU
#define CROSS9_VRX_Z_CROSS_9_POS  	 0U

#define CROSS9_F_VRX_Z_CROSS_9_ADDR 	 0x209U // Force outgoing bit 9 to 0. Applied befor... 
#define CROSS9_F_VRX_Z_CROSS_9_MASK 	 0x20U
#define CROSS9_F_VRX_Z_CROSS_9_POS  	 5U

#define CROSS9_I_VRX_Z_CROSS_9_ADDR 	 0x209U // Invert outgoing bit 9  
#define CROSS9_I_VRX_Z_CROSS_9_MASK 	 0x40U
#define CROSS9_I_VRX_Z_CROSS_9_POS  	 6U

#define VRX_Z_CROSS_10_ADDR    	 0x20AU
#define VRX_Z_CROSS_10_DEFAULT 	 0x0AU

#define CROSS10_VRX_Z_CROSS_10_ADDR 	 0x20AU // Maps incoming bit position set by this f... 
#define CROSS10_VRX_Z_CROSS_10_MASK 	 0x1FU
#define CROSS10_VRX_Z_CROSS_10_POS  	 0U

#define CROSS10_F_VRX_Z_CROSS_10_ADDR 	 0x20AU // Force outgoing bit 10 to 0. Applied befo... 
#define CROSS10_F_VRX_Z_CROSS_10_MASK 	 0x20U
#define CROSS10_F_VRX_Z_CROSS_10_POS  	 5U

#define CROSS10_I_VRX_Z_CROSS_10_ADDR 	 0x20AU // Invert outgoing bit 10  
#define CROSS10_I_VRX_Z_CROSS_10_MASK 	 0x40U
#define CROSS10_I_VRX_Z_CROSS_10_POS  	 6U

#define VRX_Z_CROSS_11_ADDR    	 0x20BU
#define VRX_Z_CROSS_11_DEFAULT 	 0x0BU

#define CROSS11_VRX_Z_CROSS_11_ADDR 	 0x20BU // Maps incoming bit position set by this f... 
#define CROSS11_VRX_Z_CROSS_11_MASK 	 0x1FU
#define CROSS11_VRX_Z_CROSS_11_POS  	 0U

#define CROSS11_F_VRX_Z_CROSS_11_ADDR 	 0x20BU // Force outgoing bit 11 to 0. Applied befo... 
#define CROSS11_F_VRX_Z_CROSS_11_MASK 	 0x20U
#define CROSS11_F_VRX_Z_CROSS_11_POS  	 5U

#define CROSS11_I_VRX_Z_CROSS_11_ADDR 	 0x20BU // Invert outgoing bit 11  
#define CROSS11_I_VRX_Z_CROSS_11_MASK 	 0x40U
#define CROSS11_I_VRX_Z_CROSS_11_POS  	 6U

#define VRX_Z_CROSS_12_ADDR    	 0x20CU
#define VRX_Z_CROSS_12_DEFAULT 	 0x0CU

#define CROSS12_VRX_Z_CROSS_12_ADDR 	 0x20CU // Maps incoming bit position set by this f... 
#define CROSS12_VRX_Z_CROSS_12_MASK 	 0x1FU
#define CROSS12_VRX_Z_CROSS_12_POS  	 0U

#define CROSS12_F_VRX_Z_CROSS_12_ADDR 	 0x20CU // Force outgoing bit 12 to 0. Applied befo... 
#define CROSS12_F_VRX_Z_CROSS_12_MASK 	 0x20U
#define CROSS12_F_VRX_Z_CROSS_12_POS  	 5U

#define CROSS12_I_VRX_Z_CROSS_12_ADDR 	 0x20CU // Invert outgoing bit 12  
#define CROSS12_I_VRX_Z_CROSS_12_MASK 	 0x40U
#define CROSS12_I_VRX_Z_CROSS_12_POS  	 6U

#define VRX_Z_CROSS_13_ADDR    	 0x20DU
#define VRX_Z_CROSS_13_DEFAULT 	 0x0DU

#define CROSS13_VRX_Z_CROSS_13_ADDR 	 0x20DU // Maps incoming bit position set by this f... 
#define CROSS13_VRX_Z_CROSS_13_MASK 	 0x1FU
#define CROSS13_VRX_Z_CROSS_13_POS  	 0U

#define CROSS13_F_VRX_Z_CROSS_13_ADDR 	 0x20DU // Force outgoing bit 13 to 0. Applied befo... 
#define CROSS13_F_VRX_Z_CROSS_13_MASK 	 0x20U
#define CROSS13_F_VRX_Z_CROSS_13_POS  	 5U

#define CROSS13_I_VRX_Z_CROSS_13_ADDR 	 0x20DU // Invert outgoing bit 13  
#define CROSS13_I_VRX_Z_CROSS_13_MASK 	 0x40U
#define CROSS13_I_VRX_Z_CROSS_13_POS  	 6U

#define VRX_Z_CROSS_14_ADDR    	 0x20EU
#define VRX_Z_CROSS_14_DEFAULT 	 0x0EU

#define CROSS14_VRX_Z_CROSS_14_ADDR 	 0x20EU // Maps incoming bit position set by this f... 
#define CROSS14_VRX_Z_CROSS_14_MASK 	 0x1FU
#define CROSS14_VRX_Z_CROSS_14_POS  	 0U

#define CROSS14_F_VRX_Z_CROSS_14_ADDR 	 0x20EU // Force outgoing bit 14 to 0. Applied befo... 
#define CROSS14_F_VRX_Z_CROSS_14_MASK 	 0x20U
#define CROSS14_F_VRX_Z_CROSS_14_POS  	 5U

#define CROSS14_I_VRX_Z_CROSS_14_ADDR 	 0x20EU // Invert outgoing bit 14  
#define CROSS14_I_VRX_Z_CROSS_14_MASK 	 0x40U
#define CROSS14_I_VRX_Z_CROSS_14_POS  	 6U

#define VRX_Z_CROSS_15_ADDR    	 0x20FU
#define VRX_Z_CROSS_15_DEFAULT 	 0x0FU

#define CROSS15_VRX_Z_CROSS_15_ADDR 	 0x20FU // Maps incoming bit position set by this f... 
#define CROSS15_VRX_Z_CROSS_15_MASK 	 0x1FU
#define CROSS15_VRX_Z_CROSS_15_POS  	 0U

#define CROSS15_F_VRX_Z_CROSS_15_ADDR 	 0x20FU // Force outgoing bit 15 to 0. Applied befo... 
#define CROSS15_F_VRX_Z_CROSS_15_MASK 	 0x20U
#define CROSS15_F_VRX_Z_CROSS_15_POS  	 5U

#define CROSS15_I_VRX_Z_CROSS_15_ADDR 	 0x20FU // Invert outgoing bit 15  
#define CROSS15_I_VRX_Z_CROSS_15_MASK 	 0x40U
#define CROSS15_I_VRX_Z_CROSS_15_POS  	 6U

#define VRX_Z_CROSS_16_ADDR    	 0x210U
#define VRX_Z_CROSS_16_DEFAULT 	 0x10U

#define CROSS16_VRX_Z_CROSS_16_ADDR 	 0x210U // Maps incoming bit position set by this f... 
#define CROSS16_VRX_Z_CROSS_16_MASK 	 0x1FU
#define CROSS16_VRX_Z_CROSS_16_POS  	 0U

#define CROSS16_F_VRX_Z_CROSS_16_ADDR 	 0x210U // Force outgoing bit 16 to 0. Applied befo... 
#define CROSS16_F_VRX_Z_CROSS_16_MASK 	 0x20U
#define CROSS16_F_VRX_Z_CROSS_16_POS  	 5U

#define CROSS16_I_VRX_Z_CROSS_16_ADDR 	 0x210U // Invert outgoing bit 16  
#define CROSS16_I_VRX_Z_CROSS_16_MASK 	 0x40U
#define CROSS16_I_VRX_Z_CROSS_16_POS  	 6U

#define VRX_Z_CROSS_17_ADDR    	 0x211U
#define VRX_Z_CROSS_17_DEFAULT 	 0x11U

#define CROSS17_VRX_Z_CROSS_17_ADDR 	 0x211U // Maps incoming bit position set by this f... 
#define CROSS17_VRX_Z_CROSS_17_MASK 	 0x1FU
#define CROSS17_VRX_Z_CROSS_17_POS  	 0U

#define CROSS17_F_VRX_Z_CROSS_17_ADDR 	 0x211U // Force outgoing bit 17 to 0. Applied befo... 
#define CROSS17_F_VRX_Z_CROSS_17_MASK 	 0x20U
#define CROSS17_F_VRX_Z_CROSS_17_POS  	 5U

#define CROSS17_I_VRX_Z_CROSS_17_ADDR 	 0x211U // IInvert outgoing bit 17  
#define CROSS17_I_VRX_Z_CROSS_17_MASK 	 0x40U
#define CROSS17_I_VRX_Z_CROSS_17_POS  	 6U

#define VRX_Z_CROSS_18_ADDR    	 0x212U
#define VRX_Z_CROSS_18_DEFAULT 	 0x12U

#define CROSS18_VRX_Z_CROSS_18_ADDR 	 0x212U // Maps incoming bit position set by this f... 
#define CROSS18_VRX_Z_CROSS_18_MASK 	 0x1FU
#define CROSS18_VRX_Z_CROSS_18_POS  	 0U

#define CROSS18_F_VRX_Z_CROSS_18_ADDR 	 0x212U // Force outgoing bit 18 to 0. Applied befo... 
#define CROSS18_F_VRX_Z_CROSS_18_MASK 	 0x20U
#define CROSS18_F_VRX_Z_CROSS_18_POS  	 5U

#define CROSS18_I_VRX_Z_CROSS_18_ADDR 	 0x212U // Invert outgoing bit 18  
#define CROSS18_I_VRX_Z_CROSS_18_MASK 	 0x40U
#define CROSS18_I_VRX_Z_CROSS_18_POS  	 6U

#define VRX_Z_CROSS_19_ADDR    	 0x213U
#define VRX_Z_CROSS_19_DEFAULT 	 0x13U

#define CROSS19_VRX_Z_CROSS_19_ADDR 	 0x213U // Maps incoming bit position set by this f... 
#define CROSS19_VRX_Z_CROSS_19_MASK 	 0x1FU
#define CROSS19_VRX_Z_CROSS_19_POS  	 0U

#define CROSS19_F_VRX_Z_CROSS_19_ADDR 	 0x213U // Force outgoing bit 19 to 0. Applied befo... 
#define CROSS19_F_VRX_Z_CROSS_19_MASK 	 0x20U
#define CROSS19_F_VRX_Z_CROSS_19_POS  	 5U

#define CROSS19_I_VRX_Z_CROSS_19_ADDR 	 0x213U // Invert outgoing bit 19  
#define CROSS19_I_VRX_Z_CROSS_19_MASK 	 0x40U
#define CROSS19_I_VRX_Z_CROSS_19_POS  	 6U

#define VRX_Z_CROSS_20_ADDR    	 0x214U
#define VRX_Z_CROSS_20_DEFAULT 	 0x14U

#define CROSS20_VRX_Z_CROSS_20_ADDR 	 0x214U // Maps incoming bit position set by this f... 
#define CROSS20_VRX_Z_CROSS_20_MASK 	 0x1FU
#define CROSS20_VRX_Z_CROSS_20_POS  	 0U

#define CROSS20_F_VRX_Z_CROSS_20_ADDR 	 0x214U // Force outgoing bit 20 to 0. Applied befo... 
#define CROSS20_F_VRX_Z_CROSS_20_MASK 	 0x20U
#define CROSS20_F_VRX_Z_CROSS_20_POS  	 5U

#define CROSS20_I_VRX_Z_CROSS_20_ADDR 	 0x214U // Invert outgoing bit 20  
#define CROSS20_I_VRX_Z_CROSS_20_MASK 	 0x40U
#define CROSS20_I_VRX_Z_CROSS_20_POS  	 6U

#define VRX_Z_CROSS_21_ADDR    	 0x215U
#define VRX_Z_CROSS_21_DEFAULT 	 0x15U

#define CROSS21_VRX_Z_CROSS_21_ADDR 	 0x215U // Maps incoming bit position set by this f... 
#define CROSS21_VRX_Z_CROSS_21_MASK 	 0x1FU
#define CROSS21_VRX_Z_CROSS_21_POS  	 0U

#define CROSS21_F_VRX_Z_CROSS_21_ADDR 	 0x215U // Force outgoing bit 21 to 0. Applied befo... 
#define CROSS21_F_VRX_Z_CROSS_21_MASK 	 0x20U
#define CROSS21_F_VRX_Z_CROSS_21_POS  	 5U

#define CROSS21_I_VRX_Z_CROSS_21_ADDR 	 0x215U // Invert outgoing bit 21  
#define CROSS21_I_VRX_Z_CROSS_21_MASK 	 0x40U
#define CROSS21_I_VRX_Z_CROSS_21_POS  	 6U

#define VRX_Z_CROSS_22_ADDR    	 0x216U
#define VRX_Z_CROSS_22_DEFAULT 	 0x16U

#define CROSS22_VRX_Z_CROSS_22_ADDR 	 0x216U // Maps incoming bit position set by this f... 
#define CROSS22_VRX_Z_CROSS_22_MASK 	 0x1FU
#define CROSS22_VRX_Z_CROSS_22_POS  	 0U

#define CROSS22_F_VRX_Z_CROSS_22_ADDR 	 0x216U // Force outgoing bit 22 to 0. Applied befo... 
#define CROSS22_F_VRX_Z_CROSS_22_MASK 	 0x20U
#define CROSS22_F_VRX_Z_CROSS_22_POS  	 5U

#define CROSS22_I_VRX_Z_CROSS_22_ADDR 	 0x216U // Invert outgoing bit 22  
#define CROSS22_I_VRX_Z_CROSS_22_MASK 	 0x40U
#define CROSS22_I_VRX_Z_CROSS_22_POS  	 6U

#define VRX_Z_CROSS_23_ADDR    	 0x217U
#define VRX_Z_CROSS_23_DEFAULT 	 0x17U

#define CROSS23_VRX_Z_CROSS_23_ADDR 	 0x217U // Maps incoming bit position set by this f... 
#define CROSS23_VRX_Z_CROSS_23_MASK 	 0x1FU
#define CROSS23_VRX_Z_CROSS_23_POS  	 0U

#define CROSS23_F_VRX_Z_CROSS_23_ADDR 	 0x217U // Force outgoing bit 23 to 0. Applied befo... 
#define CROSS23_F_VRX_Z_CROSS_23_MASK 	 0x20U
#define CROSS23_F_VRX_Z_CROSS_23_POS  	 5U

#define CROSS23_I_VRX_Z_CROSS_23_ADDR 	 0x217U // Invert outgoing bit 23  
#define CROSS23_I_VRX_Z_CROSS_23_MASK 	 0x40U
#define CROSS23_I_VRX_Z_CROSS_23_POS  	 6U

#define VRX_Z_CROSS_HS_ADDR    	 0x218U
#define VRX_Z_CROSS_HS_DEFAULT 	 0x18U

#define CROSS_HS_VRX_Z_CROSS_HS_ADDR 	 0x218U // Map selected internal signal to HS  
#define CROSS_HS_VRX_Z_CROSS_HS_MASK 	 0x1FU
#define CROSS_HS_VRX_Z_CROSS_HS_POS  	 0U

#define CROSS_HS_F_VRX_Z_CROSS_HS_ADDR 	 0x218U // Force CROSS_HS to 0. Applied before inve... 
#define CROSS_HS_F_VRX_Z_CROSS_HS_MASK 	 0x20U
#define CROSS_HS_F_VRX_Z_CROSS_HS_POS  	 5U

#define CROSS_HS_I_VRX_Z_CROSS_HS_ADDR 	 0x218U // Invert CROSS_HS  
#define CROSS_HS_I_VRX_Z_CROSS_HS_MASK 	 0x40U
#define CROSS_HS_I_VRX_Z_CROSS_HS_POS  	 6U

#define VRX_Z_CROSS_VS_ADDR    	 0x219U
#define VRX_Z_CROSS_VS_DEFAULT 	 0x19U

#define CROSS_VS_VRX_Z_CROSS_VS_ADDR 	 0x219U // Map selected internal signal to VS  
#define CROSS_VS_VRX_Z_CROSS_VS_MASK 	 0x1FU
#define CROSS_VS_VRX_Z_CROSS_VS_POS  	 0U

#define CROSS_VS_F_VRX_Z_CROSS_VS_ADDR 	 0x219U // Force CROSS_VS to 0. Applied before inve... 
#define CROSS_VS_F_VRX_Z_CROSS_VS_MASK 	 0x20U
#define CROSS_VS_F_VRX_Z_CROSS_VS_POS  	 5U

#define CROSS_VS_I_VRX_Z_CROSS_VS_ADDR 	 0x219U // Invert CROSS_VS  
#define CROSS_VS_I_VRX_Z_CROSS_VS_MASK 	 0x40U
#define CROSS_VS_I_VRX_Z_CROSS_VS_POS  	 6U

#define VRX_Z_CROSS_DE_ADDR    	 0x21AU
#define VRX_Z_CROSS_DE_DEFAULT 	 0x1AU

#define CROSS_DE_VRX_Z_CROSS_DE_ADDR 	 0x21AU // Map selected internal signal to DE  
#define CROSS_DE_VRX_Z_CROSS_DE_MASK 	 0x1FU
#define CROSS_DE_VRX_Z_CROSS_DE_POS  	 0U

#define CROSS_DE_F_VRX_Z_CROSS_DE_ADDR 	 0x21AU // Force CROSS_DE to 0. Applied before inve... 
#define CROSS_DE_F_VRX_Z_CROSS_DE_MASK 	 0x20U
#define CROSS_DE_F_VRX_Z_CROSS_DE_POS  	 5U

#define CROSS_DE_I_VRX_Z_CROSS_DE_ADDR 	 0x21AU // Invert CROSS_DE  
#define CROSS_DE_I_VRX_Z_CROSS_DE_MASK 	 0x40U
#define CROSS_DE_I_VRX_Z_CROSS_DE_POS  	 6U

#define VRX_Z_PRBS_ERR_ADDR    	 0x21BU
#define VRX_Z_PRBS_ERR_DEFAULT 	 0x00U

#define VPRBS_ERR_VRX_Z_PRBS_ERR_ADDR 	 0x21BU // Video PRBS error counter, clears on read... 
#define VPRBS_ERR_VRX_Z_PRBS_ERR_MASK 	 0xFFU
#define VPRBS_ERR_VRX_Z_PRBS_ERR_POS  	 0U

#define VRX_Z_VPRBS_ADDR    	 0x21CU
#define VRX_Z_VPRBS_DEFAULT 	 0x80U

#define VIDEO_LOCK_VRX_Z_VPRBS_ADDR 	 0x21CU // Video channel is locked and outputting v... 
#define VIDEO_LOCK_VRX_Z_VPRBS_MASK 	 0x01U
#define VIDEO_LOCK_VRX_Z_VPRBS_POS  	 0U

#define VPRBS_CHK_EN_VRX_Z_VPRBS_ADDR 	 0x21CU // Enable video PRBS checker  
#define VPRBS_CHK_EN_VRX_Z_VPRBS_MASK 	 0x10U
#define VPRBS_CHK_EN_VRX_Z_VPRBS_POS  	 4U

#define VPRBS_FAIL_VRX_Z_VPRBS_ADDR 	 0x21CU // Video PRBS check pass/fail  
#define VPRBS_FAIL_VRX_Z_VPRBS_MASK 	 0x20U
#define VPRBS_FAIL_VRX_Z_VPRBS_POS  	 5U

#define PATGEN_CLK_SRC_VRX_Z_VPRBS_ADDR 	 0x21CU // Pattern-generator clock source for video... 
#define PATGEN_CLK_SRC_VRX_Z_VPRBS_MASK 	 0x80U
#define PATGEN_CLK_SRC_VRX_Z_VPRBS_POS  	 7U

#define VRX_Z_CROSS_27_ADDR    	 0x21DU
#define VRX_Z_CROSS_27_DEFAULT 	 0x1BU

#define CROSS27_VRX_Z_CROSS_27_ADDR 	 0x21DU // Maps incoming bit position set by this f... 
#define CROSS27_VRX_Z_CROSS_27_MASK 	 0x1FU
#define CROSS27_VRX_Z_CROSS_27_POS  	 0U

#define CROSS27_F_VRX_Z_CROSS_27_ADDR 	 0x21DU // Force outgoing bit 27 to 0. Applied befo... 
#define CROSS27_F_VRX_Z_CROSS_27_MASK 	 0x20U
#define CROSS27_F_VRX_Z_CROSS_27_POS  	 5U

#define CROSS27_I_VRX_Z_CROSS_27_ADDR 	 0x21DU // Invert outgoing bit 27  
#define CROSS27_I_VRX_Z_CROSS_27_MASK 	 0x40U
#define CROSS27_I_VRX_Z_CROSS_27_POS  	 6U

#define ALT_CROSSBAR_VRX_Z_CROSS_27_ADDR 	 0x21DU // Selects whether to use the crossbar in t... 
#define ALT_CROSSBAR_VRX_Z_CROSS_27_MASK 	 0x80U
#define ALT_CROSSBAR_VRX_Z_CROSS_27_POS  	 7U

#define VRX_Z_CROSS_28_ADDR    	 0x21EU
#define VRX_Z_CROSS_28_DEFAULT 	 0x1CU

#define CROSS28_VRX_Z_CROSS_28_ADDR 	 0x21EU // Maps incoming bit position set by this f... 
#define CROSS28_VRX_Z_CROSS_28_MASK 	 0x1FU
#define CROSS28_VRX_Z_CROSS_28_POS  	 0U

#define CROSS28_F_VRX_Z_CROSS_28_ADDR 	 0x21EU // Force outgoing bit 28 to 0. Applied befo... 
#define CROSS28_F_VRX_Z_CROSS_28_MASK 	 0x20U
#define CROSS28_F_VRX_Z_CROSS_28_POS  	 5U

#define CROSS28_I_VRX_Z_CROSS_28_ADDR 	 0x21EU // Invert outgoing bit 28  
#define CROSS28_I_VRX_Z_CROSS_28_MASK 	 0x40U
#define CROSS28_I_VRX_Z_CROSS_28_POS  	 6U

#define VRX_Z_CROSS_29_ADDR    	 0x21FU
#define VRX_Z_CROSS_29_DEFAULT 	 0x1DU

#define CROSS29_VRX_Z_CROSS_29_ADDR 	 0x21FU // Maps incoming bit position set by this f... 
#define CROSS29_VRX_Z_CROSS_29_MASK 	 0x1FU
#define CROSS29_VRX_Z_CROSS_29_POS  	 0U

#define CROSS29_F_VRX_Z_CROSS_29_ADDR 	 0x21FU // Force outgoing bit 29 to 0. Applied befo... 
#define CROSS29_F_VRX_Z_CROSS_29_MASK 	 0x20U
#define CROSS29_F_VRX_Z_CROSS_29_POS  	 5U

#define CROSS29_I_VRX_Z_CROSS_29_ADDR 	 0x21FU // Invert outgoing bit 29  
#define CROSS29_I_VRX_Z_CROSS_29_MASK 	 0x40U
#define CROSS29_I_VRX_Z_CROSS_29_POS  	 6U

#define VRX_PATGEN_0_PATGEN_0_ADDR    	 0x240U
#define VRX_PATGEN_0_PATGEN_0_DEFAULT 	 0x03U

#define VTG_MODE_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Video-Timing Generation Mode  
#define VTG_MODE_VRX_PATGEN_0_PATGEN_0_MASK 	 0x03U
#define VTG_MODE_VRX_PATGEN_0_PATGEN_0_POS  	 0U

#define DE_INV_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Invert DE output of video-timing generat... 
#define DE_INV_VRX_PATGEN_0_PATGEN_0_MASK 	 0x04U
#define DE_INV_VRX_PATGEN_0_PATGEN_0_POS  	 2U

#define HS_INV_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Invert HSYNC output of video-timing gene... 
#define HS_INV_VRX_PATGEN_0_PATGEN_0_MASK 	 0x08U
#define HS_INV_VRX_PATGEN_0_PATGEN_0_POS  	 3U

#define VS_INV_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Invert VSYNC output of video-timing gene... 
#define VS_INV_VRX_PATGEN_0_PATGEN_0_MASK 	 0x10U
#define VS_INV_VRX_PATGEN_0_PATGEN_0_POS  	 4U

#define GEN_DE_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Enable to generate DE output according t... 
#define GEN_DE_VRX_PATGEN_0_PATGEN_0_MASK 	 0x20U
#define GEN_DE_VRX_PATGEN_0_PATGEN_0_POS  	 5U

#define GEN_HS_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Enable to generate HS output according t... 
#define GEN_HS_VRX_PATGEN_0_PATGEN_0_MASK 	 0x40U
#define GEN_HS_VRX_PATGEN_0_PATGEN_0_POS  	 6U

#define GEN_VS_VRX_PATGEN_0_PATGEN_0_ADDR 	 0x240U // Enable to generate VS output according t... 
#define GEN_VS_VRX_PATGEN_0_PATGEN_0_MASK 	 0x80U
#define GEN_VS_VRX_PATGEN_0_PATGEN_0_POS  	 7U

#define VRX_PATGEN_0_PATGEN_1_ADDR    	 0x241U
#define VRX_PATGEN_0_PATGEN_1_DEFAULT 	 0x00U

#define VS_TRIG_VRX_PATGEN_0_PATGEN_1_ADDR 	 0x241U // Select VS trigger edge  
#define VS_TRIG_VRX_PATGEN_0_PATGEN_1_MASK 	 0x01U
#define VS_TRIG_VRX_PATGEN_0_PATGEN_1_POS  	 0U

#define PATGEN_MODE_VRX_PATGEN_0_PATGEN_1_ADDR 	 0x241U // Pattern-Generator Mode  
#define PATGEN_MODE_VRX_PATGEN_0_PATGEN_1_MASK 	 0x30U
#define PATGEN_MODE_VRX_PATGEN_0_PATGEN_1_POS  	 4U

#define GRAD_MODE_VRX_PATGEN_0_PATGEN_1_ADDR 	 0x241U // Gradient Pattern-Generator Mode  
#define GRAD_MODE_VRX_PATGEN_0_PATGEN_1_MASK 	 0x80U
#define GRAD_MODE_VRX_PATGEN_0_PATGEN_1_POS  	 7U

#define VRX_PATGEN_0_VS_DLY_2_ADDR    	 0x242U
#define VRX_PATGEN_0_VS_DLY_2_DEFAULT 	 0x00U

#define VS_DLY_2_VRX_PATGEN_0_VS_DLY_2_ADDR 	 0x242U // VS delay in terms of PCLK cycles  
#define VS_DLY_2_VRX_PATGEN_0_VS_DLY_2_MASK 	 0xFFU
#define VS_DLY_2_VRX_PATGEN_0_VS_DLY_2_POS  	 0U

#define VRX_PATGEN_0_VS_DLY_1_ADDR    	 0x243U
#define VRX_PATGEN_0_VS_DLY_1_DEFAULT 	 0x00U

#define VS_DLY_1_VRX_PATGEN_0_VS_DLY_1_ADDR 	 0x243U // VS delay in terms of PCLK cycles  
#define VS_DLY_1_VRX_PATGEN_0_VS_DLY_1_MASK 	 0xFFU
#define VS_DLY_1_VRX_PATGEN_0_VS_DLY_1_POS  	 0U

#define VRX_PATGEN_0_VS_DLY_0_ADDR    	 0x244U
#define VRX_PATGEN_0_VS_DLY_0_DEFAULT 	 0x00U

#define VS_DLY_0_VRX_PATGEN_0_VS_DLY_0_ADDR 	 0x244U // VS delay in terms of PCLK cycles  
#define VS_DLY_0_VRX_PATGEN_0_VS_DLY_0_MASK 	 0xFFU
#define VS_DLY_0_VRX_PATGEN_0_VS_DLY_0_POS  	 0U

#define VRX_PATGEN_0_VS_HIGH_2_ADDR    	 0x245U
#define VRX_PATGEN_0_VS_HIGH_2_DEFAULT 	 0x00U

#define VS_HIGH_2_VRX_PATGEN_0_VS_HIGH_2_ADDR 	 0x245U // VS high period in terms of PCLK cycles (... 
#define VS_HIGH_2_VRX_PATGEN_0_VS_HIGH_2_MASK 	 0xFFU
#define VS_HIGH_2_VRX_PATGEN_0_VS_HIGH_2_POS  	 0U

#define VRX_PATGEN_0_VS_HIGH_1_ADDR    	 0x246U
#define VRX_PATGEN_0_VS_HIGH_1_DEFAULT 	 0x00U

#define VS_HIGH_1_VRX_PATGEN_0_VS_HIGH_1_ADDR 	 0x246U // VS high period in terms of PCLK cycles (... 
#define VS_HIGH_1_VRX_PATGEN_0_VS_HIGH_1_MASK 	 0xFFU
#define VS_HIGH_1_VRX_PATGEN_0_VS_HIGH_1_POS  	 0U

#define VRX_PATGEN_0_VS_HIGH_0_ADDR    	 0x247U
#define VRX_PATGEN_0_VS_HIGH_0_DEFAULT 	 0x00U

#define VS_HIGH_0_VRX_PATGEN_0_VS_HIGH_0_ADDR 	 0x247U // VS high period in terms of PCLK cycles (... 
#define VS_HIGH_0_VRX_PATGEN_0_VS_HIGH_0_MASK 	 0xFFU
#define VS_HIGH_0_VRX_PATGEN_0_VS_HIGH_0_POS  	 0U

#define VRX_PATGEN_0_VS_LOW_2_ADDR    	 0x248U
#define VRX_PATGEN_0_VS_LOW_2_DEFAULT 	 0x00U

#define VS_LOW_2_VRX_PATGEN_0_VS_LOW_2_ADDR 	 0x248U // VS low period in terms of PCLK cycles ([... 
#define VS_LOW_2_VRX_PATGEN_0_VS_LOW_2_MASK 	 0xFFU
#define VS_LOW_2_VRX_PATGEN_0_VS_LOW_2_POS  	 0U

#define VRX_PATGEN_0_VS_LOW_1_ADDR    	 0x249U
#define VRX_PATGEN_0_VS_LOW_1_DEFAULT 	 0x00U

#define VS_LOW_1_VRX_PATGEN_0_VS_LOW_1_ADDR 	 0x249U // VS low period in terms of PCLK cycles ([... 
#define VS_LOW_1_VRX_PATGEN_0_VS_LOW_1_MASK 	 0xFFU
#define VS_LOW_1_VRX_PATGEN_0_VS_LOW_1_POS  	 0U

#define VRX_PATGEN_0_VS_LOW_0_ADDR    	 0x24AU
#define VRX_PATGEN_0_VS_LOW_0_DEFAULT 	 0x00U

#define VS_LOW_0_VRX_PATGEN_0_VS_LOW_0_ADDR 	 0x24AU // VS low period in terms of PCLK cycles ([... 
#define VS_LOW_0_VRX_PATGEN_0_VS_LOW_0_MASK 	 0xFFU
#define VS_LOW_0_VRX_PATGEN_0_VS_LOW_0_POS  	 0U

#define VRX_PATGEN_0_V2H_2_ADDR    	 0x24BU
#define VRX_PATGEN_0_V2H_2_DEFAULT 	 0x00U

#define V2H_2_VRX_PATGEN_0_V2H_2_ADDR 	 0x24BU // VS edge to the rising edge of the first ... 
#define V2H_2_VRX_PATGEN_0_V2H_2_MASK 	 0xFFU
#define V2H_2_VRX_PATGEN_0_V2H_2_POS  	 0U

#define VRX_PATGEN_0_V2H_1_ADDR    	 0x24CU
#define VRX_PATGEN_0_V2H_1_DEFAULT 	 0x00U

#define V2H_1_VRX_PATGEN_0_V2H_1_ADDR 	 0x24CU // VS edge to the rising edge of the first ... 
#define V2H_1_VRX_PATGEN_0_V2H_1_MASK 	 0xFFU
#define V2H_1_VRX_PATGEN_0_V2H_1_POS  	 0U

#define VRX_PATGEN_0_V2H_0_ADDR    	 0x24DU
#define VRX_PATGEN_0_V2H_0_DEFAULT 	 0x00U

#define V2H_0_VRX_PATGEN_0_V2H_0_ADDR 	 0x24DU // VS edge to the rising edge of the first ... 
#define V2H_0_VRX_PATGEN_0_V2H_0_MASK 	 0xFFU
#define V2H_0_VRX_PATGEN_0_V2H_0_POS  	 0U

#define VRX_PATGEN_0_HS_HIGH_1_ADDR    	 0x24EU
#define VRX_PATGEN_0_HS_HIGH_1_DEFAULT 	 0x00U

#define HS_HIGH_1_VRX_PATGEN_0_HS_HIGH_1_ADDR 	 0x24EU // HS high period in terms of PCLK cycles (... 
#define HS_HIGH_1_VRX_PATGEN_0_HS_HIGH_1_MASK 	 0xFFU
#define HS_HIGH_1_VRX_PATGEN_0_HS_HIGH_1_POS  	 0U

#define VRX_PATGEN_0_HS_HIGH_0_ADDR    	 0x24FU
#define VRX_PATGEN_0_HS_HIGH_0_DEFAULT 	 0x00U

#define HS_HIGH_0_VRX_PATGEN_0_HS_HIGH_0_ADDR 	 0x24FU // HS high period in terms of PCLK cycles (... 
#define HS_HIGH_0_VRX_PATGEN_0_HS_HIGH_0_MASK 	 0xFFU
#define HS_HIGH_0_VRX_PATGEN_0_HS_HIGH_0_POS  	 0U

#define VRX_PATGEN_0_HS_LOW_1_ADDR    	 0x250U
#define VRX_PATGEN_0_HS_LOW_1_DEFAULT 	 0x00U

#define HS_LOW_1_VRX_PATGEN_0_HS_LOW_1_ADDR 	 0x250U // HS low period in terms of PCLK cycles ([... 
#define HS_LOW_1_VRX_PATGEN_0_HS_LOW_1_MASK 	 0xFFU
#define HS_LOW_1_VRX_PATGEN_0_HS_LOW_1_POS  	 0U

#define VRX_PATGEN_0_HS_LOW_0_ADDR    	 0x251U
#define VRX_PATGEN_0_HS_LOW_0_DEFAULT 	 0x00U

#define HS_LOW_0_VRX_PATGEN_0_HS_LOW_0_ADDR 	 0x251U // HS low period in terms of PCLK cycles ([... 
#define HS_LOW_0_VRX_PATGEN_0_HS_LOW_0_MASK 	 0xFFU
#define HS_LOW_0_VRX_PATGEN_0_HS_LOW_0_POS  	 0U

#define VRX_PATGEN_0_HS_CNT_1_ADDR    	 0x252U
#define VRX_PATGEN_0_HS_CNT_1_DEFAULT 	 0x00U

#define HS_CNT_1_VRX_PATGEN_0_HS_CNT_1_ADDR 	 0x252U // HS pulses per frame ([15:8])  
#define HS_CNT_1_VRX_PATGEN_0_HS_CNT_1_MASK 	 0xFFU
#define HS_CNT_1_VRX_PATGEN_0_HS_CNT_1_POS  	 0U

#define VRX_PATGEN_0_HS_CNT_0_ADDR    	 0x253U
#define VRX_PATGEN_0_HS_CNT_0_DEFAULT 	 0x00U

#define HS_CNT_0_VRX_PATGEN_0_HS_CNT_0_ADDR 	 0x253U // HS pulses per frame [7:0])  
#define HS_CNT_0_VRX_PATGEN_0_HS_CNT_0_MASK 	 0xFFU
#define HS_CNT_0_VRX_PATGEN_0_HS_CNT_0_POS  	 0U

#define VRX_PATGEN_0_V2D_2_ADDR    	 0x254U
#define VRX_PATGEN_0_V2D_2_DEFAULT 	 0x00U

#define V2D_2_VRX_PATGEN_0_V2D_2_ADDR 	 0x254U // VS edge to the rising edge of the first ... 
#define V2D_2_VRX_PATGEN_0_V2D_2_MASK 	 0xFFU
#define V2D_2_VRX_PATGEN_0_V2D_2_POS  	 0U

#define VRX_PATGEN_0_V2D_1_ADDR    	 0x255U
#define VRX_PATGEN_0_V2D_1_DEFAULT 	 0x00U

#define V2D_1_VRX_PATGEN_0_V2D_1_ADDR 	 0x255U // VS edge to the rising edge of the first ... 
#define V2D_1_VRX_PATGEN_0_V2D_1_MASK 	 0xFFU
#define V2D_1_VRX_PATGEN_0_V2D_1_POS  	 0U

#define VRX_PATGEN_0_V2D_0_ADDR    	 0x256U
#define VRX_PATGEN_0_V2D_0_DEFAULT 	 0x00U

#define V2D_0_VRX_PATGEN_0_V2D_0_ADDR 	 0x256U // VS edge to the rising edge of the first ... 
#define V2D_0_VRX_PATGEN_0_V2D_0_MASK 	 0xFFU
#define V2D_0_VRX_PATGEN_0_V2D_0_POS  	 0U

#define VRX_PATGEN_0_DE_HIGH_1_ADDR    	 0x257U
#define VRX_PATGEN_0_DE_HIGH_1_DEFAULT 	 0x00U

#define DE_HIGH_1_VRX_PATGEN_0_DE_HIGH_1_ADDR 	 0x257U // DE high period in terms of PCLK cycles (... 
#define DE_HIGH_1_VRX_PATGEN_0_DE_HIGH_1_MASK 	 0xFFU
#define DE_HIGH_1_VRX_PATGEN_0_DE_HIGH_1_POS  	 0U

#define VRX_PATGEN_0_DE_HIGH_0_ADDR    	 0x258U
#define VRX_PATGEN_0_DE_HIGH_0_DEFAULT 	 0x00U

#define DE_HIGH_0_VRX_PATGEN_0_DE_HIGH_0_ADDR 	 0x258U // DE high period in terms of PCLK cycles (... 
#define DE_HIGH_0_VRX_PATGEN_0_DE_HIGH_0_MASK 	 0xFFU
#define DE_HIGH_0_VRX_PATGEN_0_DE_HIGH_0_POS  	 0U

#define VRX_PATGEN_0_DE_LOW_1_ADDR    	 0x259U
#define VRX_PATGEN_0_DE_LOW_1_DEFAULT 	 0x00U

#define DE_LOW_1_VRX_PATGEN_0_DE_LOW_1_ADDR 	 0x259U // DE low period in terms of PCLK cycles ([... 
#define DE_LOW_1_VRX_PATGEN_0_DE_LOW_1_MASK 	 0xFFU
#define DE_LOW_1_VRX_PATGEN_0_DE_LOW_1_POS  	 0U

#define VRX_PATGEN_0_DE_LOW_0_ADDR    	 0x25AU
#define VRX_PATGEN_0_DE_LOW_0_DEFAULT 	 0x00U

#define DE_LOW_0_VRX_PATGEN_0_DE_LOW_0_ADDR 	 0x25AU // DE low period in terms of PCLK cycles ([... 
#define DE_LOW_0_VRX_PATGEN_0_DE_LOW_0_MASK 	 0xFFU
#define DE_LOW_0_VRX_PATGEN_0_DE_LOW_0_POS  	 0U

#define VRX_PATGEN_0_DE_CNT_1_ADDR    	 0x25BU
#define VRX_PATGEN_0_DE_CNT_1_DEFAULT 	 0x00U

#define DE_CNT_1_VRX_PATGEN_0_DE_CNT_1_ADDR 	 0x25BU // Active lines per frame ([15:8])  
#define DE_CNT_1_VRX_PATGEN_0_DE_CNT_1_MASK 	 0xFFU
#define DE_CNT_1_VRX_PATGEN_0_DE_CNT_1_POS  	 0U

#define VRX_PATGEN_0_DE_CNT_0_ADDR    	 0x25CU
#define VRX_PATGEN_0_DE_CNT_0_DEFAULT 	 0x00U

#define DE_CNT_0_VRX_PATGEN_0_DE_CNT_0_ADDR 	 0x25CU // Active lines per frame ([7:0])  
#define DE_CNT_0_VRX_PATGEN_0_DE_CNT_0_MASK 	 0xFFU
#define DE_CNT_0_VRX_PATGEN_0_DE_CNT_0_POS  	 0U

#define VRX_PATGEN_0_GRAD_INCR_ADDR    	 0x25DU
#define VRX_PATGEN_0_GRAD_INCR_DEFAULT 	 0x00U

#define GRAD_INCR_VRX_PATGEN_0_GRAD_INCR_ADDR 	 0x25DU // Gradient mode increment amount (incremen... 
#define GRAD_INCR_VRX_PATGEN_0_GRAD_INCR_MASK 	 0xFFU
#define GRAD_INCR_VRX_PATGEN_0_GRAD_INCR_POS  	 0U

#define VRX_PATGEN_0_CHKR_COLOR_A_L_ADDR    	 0x25EU
#define VRX_PATGEN_0_CHKR_COLOR_A_L_DEFAULT 	 0x00U

#define CHKR_COLOR_A_L_VRX_PATGEN_0_CHKR_COLOR_A_L_ADDR 	 0x25EU // Checkerboard mode Color A low byte  
#define CHKR_COLOR_A_L_VRX_PATGEN_0_CHKR_COLOR_A_L_MASK 	 0xFFU
#define CHKR_COLOR_A_L_VRX_PATGEN_0_CHKR_COLOR_A_L_POS  	 0U

#define VRX_PATGEN_0_CHKR_COLOR_A_1_ADDR    	 0x25FU
#define VRX_PATGEN_0_CHKR_COLOR_A_1_DEFAULT 	 0x00U

#define CHKR_COLOR_A_M_VRX_PATGEN_0_CHKR_COLOR_A_1_ADDR 	 0x25FU // Checkerboard mode Color A middle byte  
#define CHKR_COLOR_A_M_VRX_PATGEN_0_CHKR_COLOR_A_1_MASK 	 0xFFU
#define CHKR_COLOR_A_M_VRX_PATGEN_0_CHKR_COLOR_A_1_POS  	 0U

#define VRX_PATGEN_0_CHKR_COLOR_A_H_ADDR    	 0x260U
#define VRX_PATGEN_0_CHKR_COLOR_A_H_DEFAULT 	 0x00U

#define CHKR_COLOR_A_H_VRX_PATGEN_0_CHKR_COLOR_A_H_ADDR 	 0x260U // Checkerboard mode Color A high byte  
#define CHKR_COLOR_A_H_VRX_PATGEN_0_CHKR_COLOR_A_H_MASK 	 0xFFU
#define CHKR_COLOR_A_H_VRX_PATGEN_0_CHKR_COLOR_A_H_POS  	 0U

#define VRX_PATGEN_0_CHKR_COLOR_B_L_ADDR    	 0x261U
#define VRX_PATGEN_0_CHKR_COLOR_B_L_DEFAULT 	 0x00U

#define CHKR_COLOR_B_L_VRX_PATGEN_0_CHKR_COLOR_B_L_ADDR 	 0x261U // Checkerboard mode Color B low byte  
#define CHKR_COLOR_B_L_VRX_PATGEN_0_CHKR_COLOR_B_L_MASK 	 0xFFU
#define CHKR_COLOR_B_L_VRX_PATGEN_0_CHKR_COLOR_B_L_POS  	 0U

#define VRX_PATGEN_0_CHKR_COLOR_B_M_ADDR    	 0x262U
#define VRX_PATGEN_0_CHKR_COLOR_B_M_DEFAULT 	 0x00U

#define CHKR_COLOR_B_M_VRX_PATGEN_0_CHKR_COLOR_B_M_ADDR 	 0x262U // Checkerboard mode Color B middle byte  
#define CHKR_COLOR_B_M_VRX_PATGEN_0_CHKR_COLOR_B_M_MASK 	 0xFFU
#define CHKR_COLOR_B_M_VRX_PATGEN_0_CHKR_COLOR_B_M_POS  	 0U

#define VRX_PATGEN_0_CHKR_COLOR_B_H_ADDR    	 0x263U
#define VRX_PATGEN_0_CHKR_COLOR_B_H_DEFAULT 	 0x00U

#define CHKR_COLOR_B_H_VRX_PATGEN_0_CHKR_COLOR_B_H_ADDR 	 0x263U // Checkerboard mode Color B high byte  
#define CHKR_COLOR_B_H_VRX_PATGEN_0_CHKR_COLOR_B_H_MASK 	 0xFFU
#define CHKR_COLOR_B_H_VRX_PATGEN_0_CHKR_COLOR_B_H_POS  	 0U

#define VRX_PATGEN_0_CHKR_RPT_A_ADDR    	 0x264U
#define VRX_PATGEN_0_CHKR_RPT_A_DEFAULT 	 0x00U

#define CHKR_RPT_A_VRX_PATGEN_0_CHKR_RPT_A_ADDR 	 0x264U // Checkerboard mode Color A repeat count  
#define CHKR_RPT_A_VRX_PATGEN_0_CHKR_RPT_A_MASK 	 0xFFU
#define CHKR_RPT_A_VRX_PATGEN_0_CHKR_RPT_A_POS  	 0U

#define VRX_PATGEN_0_CHKR_RPT_B_ADDR    	 0x265U
#define VRX_PATGEN_0_CHKR_RPT_B_DEFAULT 	 0x00U

#define CHKR_RPT_B_VRX_PATGEN_0_CHKR_RPT_B_ADDR 	 0x265U // Checkerboard mode Color B repeat count  
#define CHKR_RPT_B_VRX_PATGEN_0_CHKR_RPT_B_MASK 	 0xFFU
#define CHKR_RPT_B_VRX_PATGEN_0_CHKR_RPT_B_POS  	 0U

#define VRX_PATGEN_0_CHKR_ALT_ADDR    	 0x266U
#define VRX_PATGEN_0_CHKR_ALT_DEFAULT 	 0x00U

#define CHKR_ALT_VRX_PATGEN_0_CHKR_ALT_ADDR 	 0x266U // Checkerboard mode alternate line count. ... 
#define CHKR_ALT_VRX_PATGEN_0_CHKR_ALT_MASK 	 0xFFU
#define CHKR_ALT_VRX_PATGEN_0_CHKR_ALT_POS  	 0U

#define GPIO0_0_GPIO_A_ADDR    	 0x2B0U
#define GPIO0_0_GPIO_A_DEFAULT 	 0x83U

#define GPIO_OUT_DIS_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO0_0_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO0_0_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // GPIO Tx source control  
#define GPIO_TX_EN_GPIO0_0_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO0_0_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // GPIO out source control.  
#define GPIO_RX_EN_GPIO0_0_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO0_0_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO0_0_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO0_0_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO0_0_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO0_0_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO0_0_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO0_0_GPIO_A_POS  	 5U

#define RES_CFG_GPIO0_0_GPIO_A_ADDR 	 0x2B0U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO0_0_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO0_0_GPIO_A_POS  	 7U

#define GPIO0_0_GPIO_B_ADDR    	 0x2B1U
#define GPIO0_0_GPIO_B_DEFAULT 	 0xA0U

#define GPIO_TX_ID_GPIO0_0_GPIO_B_ADDR 	 0x2B1U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO0_0_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO0_0_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO0_0_GPIO_B_ADDR 	 0x2B1U // Driver type selection  
#define OUT_TYPE_GPIO0_0_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO0_0_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO0_0_GPIO_B_ADDR 	 0x2B1U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO0_0_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO0_0_GPIO_B_POS  	 6U

#define GPIO0_0_GPIO_C_ADDR    	 0x2B2U
#define GPIO0_0_GPIO_C_DEFAULT 	 0x40U

#define GPIO_RX_ID_GPIO0_0_GPIO_C_ADDR 	 0x2B2U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO0_0_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO0_0_GPIO_C_POS  	 0U

#define GPIO_RECVED_GPIO0_0_GPIO_C_ADDR 	 0x2B2U // Received GPIO value from across the GMSL... 
#define GPIO_RECVED_GPIO0_0_GPIO_C_MASK 	 0x40U
#define GPIO_RECVED_GPIO0_0_GPIO_C_POS  	 6U

#define OVR_RES_CFG_GPIO0_0_GPIO_C_ADDR 	 0x2B2U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO0_0_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO0_0_GPIO_C_POS  	 7U

#define GPIO1_1_GPIO_A_ADDR    	 0x2B3U
#define GPIO1_1_GPIO_A_DEFAULT 	 0x84U

#define GPIO_OUT_DIS_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO1_1_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO1_1_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO1_1_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO1_1_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // GPIO out source control.  
#define GPIO_RX_EN_GPIO1_1_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO1_1_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO1_1_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO1_1_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO1_1_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO1_1_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO1_1_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO1_1_GPIO_A_POS  	 5U

#define RES_CFG_GPIO1_1_GPIO_A_ADDR 	 0x2B3U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO1_1_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO1_1_GPIO_A_POS  	 7U

#define GPIO1_1_GPIO_B_ADDR    	 0x2B4U
#define GPIO1_1_GPIO_B_DEFAULT 	 0xA1U

#define GPIO_TX_ID_GPIO1_1_GPIO_B_ADDR 	 0x2B4U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO1_1_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO1_1_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO1_1_GPIO_B_ADDR 	 0x2B4U // Driver type selection  
#define OUT_TYPE_GPIO1_1_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO1_1_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO1_1_GPIO_B_ADDR 	 0x2B4U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO1_1_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO1_1_GPIO_B_POS  	 6U

#define GPIO1_1_GPIO_C_ADDR    	 0x2B5U
#define GPIO1_1_GPIO_C_DEFAULT 	 0x41U

#define GPIO_RX_ID_GPIO1_1_GPIO_C_ADDR 	 0x2B5U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO1_1_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO1_1_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO1_1_GPIO_C_ADDR 	 0x2B5U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO1_1_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO1_1_GPIO_C_POS  	 7U

#define GPIO2_2_GPIO_A_ADDR    	 0x2B6U
#define GPIO2_2_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO2_2_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO2_2_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO2_2_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO2_2_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // GPIO out source control.  
#define GPIO_RX_EN_GPIO2_2_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO2_2_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO2_2_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO2_2_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO2_2_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO2_2_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO2_2_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO2_2_GPIO_A_POS  	 5U

#define RES_CFG_GPIO2_2_GPIO_A_ADDR 	 0x2B6U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO2_2_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO2_2_GPIO_A_POS  	 7U

#define GPIO2_2_GPIO_B_ADDR    	 0x2B7U
#define GPIO2_2_GPIO_B_DEFAULT 	 0x22U

#define GPIO_TX_ID_GPIO2_2_GPIO_B_ADDR 	 0x2B7U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO2_2_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO2_2_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO2_2_GPIO_B_ADDR 	 0x2B7U // Driver type selection  
#define OUT_TYPE_GPIO2_2_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO2_2_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO2_2_GPIO_B_ADDR 	 0x2B7U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO2_2_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO2_2_GPIO_B_POS  	 6U

#define GPIO2_2_GPIO_C_ADDR    	 0x2B8U
#define GPIO2_2_GPIO_C_DEFAULT 	 0x42U

#define GPIO_RX_ID_GPIO2_2_GPIO_C_ADDR 	 0x2B8U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO2_2_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO2_2_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO2_2_GPIO_C_ADDR 	 0x2B8U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO2_2_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO2_2_GPIO_C_POS  	 7U

#define GPIO3_3_GPIO_A_ADDR    	 0x2B9U
#define GPIO3_3_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO3_3_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO3_3_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO3_3_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO3_3_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // GPIO out source control.  
#define GPIO_RX_EN_GPIO3_3_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO3_3_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO3_3_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO3_3_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO3_3_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO3_3_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO3_3_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO3_3_GPIO_A_POS  	 5U

#define RES_CFG_GPIO3_3_GPIO_A_ADDR 	 0x2B9U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO3_3_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO3_3_GPIO_A_POS  	 7U

#define GPIO3_3_GPIO_B_ADDR    	 0x2BAU
#define GPIO3_3_GPIO_B_DEFAULT 	 0x23U

#define GPIO_TX_ID_GPIO3_3_GPIO_B_ADDR 	 0x2BAU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO3_3_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO3_3_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO3_3_GPIO_B_ADDR 	 0x2BAU // Driver type selection  
#define OUT_TYPE_GPIO3_3_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO3_3_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO3_3_GPIO_B_ADDR 	 0x2BAU // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO3_3_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO3_3_GPIO_B_POS  	 6U

#define GPIO3_3_GPIO_C_ADDR    	 0x2BBU
#define GPIO3_3_GPIO_C_DEFAULT 	 0x43U

#define GPIO_RX_ID_GPIO3_3_GPIO_C_ADDR 	 0x2BBU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO3_3_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO3_3_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO3_3_GPIO_C_ADDR 	 0x2BBU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO3_3_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO3_3_GPIO_C_POS  	 7U

#define GPIO4_4_GPIO_A_ADDR    	 0x2BCU
#define GPIO4_4_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO4_4_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO4_4_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO4_4_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO4_4_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // GPIO out source control.  
#define GPIO_RX_EN_GPIO4_4_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO4_4_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO4_4_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO4_4_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO4_4_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO4_4_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO4_4_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO4_4_GPIO_A_POS  	 5U

#define RES_CFG_GPIO4_4_GPIO_A_ADDR 	 0x2BCU // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO4_4_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO4_4_GPIO_A_POS  	 7U

#define GPIO4_4_GPIO_B_ADDR    	 0x2BDU
#define GPIO4_4_GPIO_B_DEFAULT 	 0xA4U

#define GPIO_TX_ID_GPIO4_4_GPIO_B_ADDR 	 0x2BDU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO4_4_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO4_4_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO4_4_GPIO_B_ADDR 	 0x2BDU // Driver type selection  
#define OUT_TYPE_GPIO4_4_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO4_4_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO4_4_GPIO_B_ADDR 	 0x2BDU // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO4_4_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO4_4_GPIO_B_POS  	 6U

#define GPIO4_4_GPIO_C_ADDR    	 0x2BEU
#define GPIO4_4_GPIO_C_DEFAULT 	 0x44U

#define GPIO_RX_ID_GPIO4_4_GPIO_C_ADDR 	 0x2BEU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO4_4_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO4_4_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO4_4_GPIO_C_ADDR 	 0x2BEU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO4_4_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO4_4_GPIO_C_POS  	 7U

#define GPIO5_5_GPIO_A_ADDR    	 0x2BFU
#define GPIO5_5_GPIO_A_DEFAULT 	 0x84U

#define GPIO_OUT_DIS_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO5_5_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO5_5_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO5_5_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO5_5_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // GPIO out source control.  
#define GPIO_RX_EN_GPIO5_5_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO5_5_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO5_5_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO5_5_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO5_5_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO5_5_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO5_5_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO5_5_GPIO_A_POS  	 5U

#define RES_CFG_GPIO5_5_GPIO_A_ADDR 	 0x2BFU // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO5_5_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO5_5_GPIO_A_POS  	 7U

#define GPIO5_5_GPIO_B_ADDR    	 0x2C0U
#define GPIO5_5_GPIO_B_DEFAULT 	 0xA5U

#define GPIO_TX_ID_GPIO5_5_GPIO_B_ADDR 	 0x2C0U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO5_5_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO5_5_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO5_5_GPIO_B_ADDR 	 0x2C0U // Driver type selection  
#define OUT_TYPE_GPIO5_5_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO5_5_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO5_5_GPIO_B_ADDR 	 0x2C0U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO5_5_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO5_5_GPIO_B_POS  	 6U

#define GPIO5_5_GPIO_C_ADDR    	 0x2C1U
#define GPIO5_5_GPIO_C_DEFAULT 	 0x45U

#define GPIO_RX_ID_GPIO5_5_GPIO_C_ADDR 	 0x2C1U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO5_5_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO5_5_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO5_5_GPIO_C_ADDR 	 0x2C1U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO5_5_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO5_5_GPIO_C_POS  	 7U

#define GPIO6_6_GPIO_A_ADDR    	 0x2C2U
#define GPIO6_6_GPIO_A_DEFAULT 	 0x83U

#define GPIO_OUT_DIS_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO6_6_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO6_6_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO6_6_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO6_6_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // GPIO out source control.  
#define GPIO_RX_EN_GPIO6_6_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO6_6_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO6_6_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO6_6_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO6_6_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO6_6_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO6_6_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO6_6_GPIO_A_POS  	 5U

#define RES_CFG_GPIO6_6_GPIO_A_ADDR 	 0x2C2U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO6_6_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO6_6_GPIO_A_POS  	 7U

#define GPIO6_6_GPIO_B_ADDR    	 0x2C3U
#define GPIO6_6_GPIO_B_DEFAULT 	 0xA6U

#define GPIO_TX_ID_GPIO6_6_GPIO_B_ADDR 	 0x2C3U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO6_6_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO6_6_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO6_6_GPIO_B_ADDR 	 0x2C3U // Driver type selection  
#define OUT_TYPE_GPIO6_6_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO6_6_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO6_6_GPIO_B_ADDR 	 0x2C3U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO6_6_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO6_6_GPIO_B_POS  	 6U

#define GPIO6_6_GPIO_C_ADDR    	 0x2C4U
#define GPIO6_6_GPIO_C_DEFAULT 	 0x46U

#define GPIO_RX_ID_GPIO6_6_GPIO_C_ADDR 	 0x2C4U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO6_6_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO6_6_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO6_6_GPIO_C_ADDR 	 0x2C4U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO6_6_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO6_6_GPIO_C_POS  	 7U

#define GPIO7_7_GPIO_A_ADDR    	 0x2C5U
#define GPIO7_7_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO7_7_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO7_7_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO7_7_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO7_7_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // GPIO out source control.  
#define GPIO_RX_EN_GPIO7_7_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO7_7_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO7_7_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO7_7_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO7_7_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO7_7_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO7_7_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO7_7_GPIO_A_POS  	 5U

#define RES_CFG_GPIO7_7_GPIO_A_ADDR 	 0x2C5U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO7_7_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO7_7_GPIO_A_POS  	 7U

#define GPIO7_7_GPIO_B_ADDR    	 0x2C6U
#define GPIO7_7_GPIO_B_DEFAULT 	 0xA7U

#define GPIO_TX_ID_GPIO7_7_GPIO_B_ADDR 	 0x2C6U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO7_7_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO7_7_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO7_7_GPIO_B_ADDR 	 0x2C6U // Driver type selection  
#define OUT_TYPE_GPIO7_7_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO7_7_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO7_7_GPIO_B_ADDR 	 0x2C6U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO7_7_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO7_7_GPIO_B_POS  	 6U

#define GPIO7_7_GPIO_C_ADDR    	 0x2C7U
#define GPIO7_7_GPIO_C_DEFAULT 	 0x47U

#define GPIO_RX_ID_GPIO7_7_GPIO_C_ADDR 	 0x2C7U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO7_7_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO7_7_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO7_7_GPIO_C_ADDR 	 0x2C7U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO7_7_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO7_7_GPIO_C_POS  	 7U

#define GPIO8_8_GPIO_A_ADDR    	 0x2C8U
#define GPIO8_8_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO8_8_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO8_8_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO8_8_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO8_8_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // GPIO out source control.  
#define GPIO_RX_EN_GPIO8_8_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO8_8_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO8_8_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO8_8_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO8_8_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO8_8_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO8_8_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO8_8_GPIO_A_POS  	 5U

#define RES_CFG_GPIO8_8_GPIO_A_ADDR 	 0x2C8U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO8_8_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO8_8_GPIO_A_POS  	 7U

#define GPIO8_8_GPIO_B_ADDR    	 0x2C9U
#define GPIO8_8_GPIO_B_DEFAULT 	 0xA8U

#define GPIO_TX_ID_GPIO8_8_GPIO_B_ADDR 	 0x2C9U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO8_8_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO8_8_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO8_8_GPIO_B_ADDR 	 0x2C9U // Driver type selection  
#define OUT_TYPE_GPIO8_8_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO8_8_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO8_8_GPIO_B_ADDR 	 0x2C9U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO8_8_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO8_8_GPIO_B_POS  	 6U

#define GPIO8_8_GPIO_C_ADDR    	 0x2CAU
#define GPIO8_8_GPIO_C_DEFAULT 	 0x48U

#define GPIO_RX_ID_GPIO8_8_GPIO_C_ADDR 	 0x2CAU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO8_8_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO8_8_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO8_8_GPIO_C_ADDR 	 0x2CAU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO8_8_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO8_8_GPIO_C_POS  	 7U

#define GPIO9_9_GPIO_A_ADDR    	 0x2CBU
#define GPIO9_9_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO9_9_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO9_9_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO9_9_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO9_9_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // GPIO out source control.  
#define GPIO_RX_EN_GPIO9_9_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO9_9_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO9_9_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO9_9_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO9_9_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO9_9_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO9_9_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO9_9_GPIO_A_POS  	 5U

#define RES_CFG_GPIO9_9_GPIO_A_ADDR 	 0x2CBU // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO9_9_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO9_9_GPIO_A_POS  	 7U

#define GPIO9_9_GPIO_B_ADDR    	 0x2CCU
#define GPIO9_9_GPIO_B_DEFAULT 	 0xA9U

#define GPIO_TX_ID_GPIO9_9_GPIO_B_ADDR 	 0x2CCU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO9_9_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO9_9_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO9_9_GPIO_B_ADDR 	 0x2CCU // Driver type selection  
#define OUT_TYPE_GPIO9_9_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO9_9_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO9_9_GPIO_B_ADDR 	 0x2CCU // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO9_9_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO9_9_GPIO_B_POS  	 6U

#define GPIO9_9_GPIO_C_ADDR    	 0x2CDU
#define GPIO9_9_GPIO_C_DEFAULT 	 0x49U

#define GPIO_RX_ID_GPIO9_9_GPIO_C_ADDR 	 0x2CDU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO9_9_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO9_9_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO9_9_GPIO_C_ADDR 	 0x2CDU // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO9_9_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO9_9_GPIO_C_POS  	 7U

#define GPIO10_10_GPIO_A_ADDR    	 0x2CEU
#define GPIO10_10_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO10_10_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO10_10_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO10_10_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO10_10_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // GPIO out source control.  
#define GPIO_RX_EN_GPIO10_10_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO10_10_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // GPIO pin local MFP input level  
#define GPIO_IN_GPIO10_10_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO10_10_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO10_10_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO10_10_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO10_10_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO10_10_GPIO_A_POS  	 5U

#define RES_CFG_GPIO10_10_GPIO_A_ADDR 	 0x2CEU // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO10_10_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO10_10_GPIO_A_POS  	 7U

#define GPIO10_10_GPIO_B_ADDR    	 0x2CFU
#define GPIO10_10_GPIO_B_DEFAULT 	 0xAAU

#define GPIO_TX_ID_GPIO10_10_GPIO_B_ADDR 	 0x2CFU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO10_10_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO10_10_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO10_10_GPIO_B_ADDR 	 0x2CFU // Driver type selection  
#define OUT_TYPE_GPIO10_10_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO10_10_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO10_10_GPIO_B_ADDR 	 0x2CFU // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO10_10_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO10_10_GPIO_B_POS  	 6U

#define GPIO10_10_GPIO_C_ADDR    	 0x2D0U
#define GPIO10_10_GPIO_C_DEFAULT 	 0x4AU

#define GPIO_RX_ID_GPIO10_10_GPIO_C_ADDR 	 0x2D0U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO10_10_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO10_10_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO10_10_GPIO_C_ADDR 	 0x2D0U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO10_10_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO10_10_GPIO_C_POS  	 7U

#define GPIO11_11_GPIO_A_ADDR    	 0x2D1U
#define GPIO11_11_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO11_11_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO11_11_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO11_11_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO11_11_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // GPIO out source control.  
#define GPIO_RX_EN_GPIO11_11_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO11_11_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO11_11_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO11_11_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO11_11_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO11_11_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO11_11_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO11_11_GPIO_A_POS  	 5U

#define RES_CFG_GPIO11_11_GPIO_A_ADDR 	 0x2D1U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO11_11_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO11_11_GPIO_A_POS  	 7U

#define GPIO11_11_GPIO_B_ADDR    	 0x2D2U
#define GPIO11_11_GPIO_B_DEFAULT 	 0xABU

#define GPIO_TX_ID_GPIO11_11_GPIO_B_ADDR 	 0x2D2U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO11_11_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO11_11_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO11_11_GPIO_B_ADDR 	 0x2D2U // Driver type selection  
#define OUT_TYPE_GPIO11_11_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO11_11_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO11_11_GPIO_B_ADDR 	 0x2D2U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO11_11_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO11_11_GPIO_B_POS  	 6U

#define GPIO11_11_GPIO_C_ADDR    	 0x2D3U
#define GPIO11_11_GPIO_C_DEFAULT 	 0x4BU

#define GPIO_RX_ID_GPIO11_11_GPIO_C_ADDR 	 0x2D3U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO11_11_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO11_11_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO11_11_GPIO_C_ADDR 	 0x2D3U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO11_11_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO11_11_GPIO_C_POS  	 7U

#define GPIO12_12_GPIO_A_ADDR    	 0x2D4U
#define GPIO12_12_GPIO_A_DEFAULT 	 0x81U

#define GPIO_OUT_DIS_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // Disables GPIO output driver  
#define GPIO_OUT_DIS_GPIO12_12_GPIO_A_MASK 	 0x01U
#define GPIO_OUT_DIS_GPIO12_12_GPIO_A_POS  	 0U

#define GPIO_TX_EN_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // GPIO Tx source control.  
#define GPIO_TX_EN_GPIO12_12_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_GPIO12_12_GPIO_A_POS  	 1U

#define GPIO_RX_EN_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // GPIO out source control.  
#define GPIO_RX_EN_GPIO12_12_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_GPIO12_12_GPIO_A_POS  	 2U

#define GPIO_IN_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // GPIO pin local MFP input level  
#define GPIO_IN_GPIO12_12_GPIO_A_MASK 	 0x08U
#define GPIO_IN_GPIO12_12_GPIO_A_POS  	 3U

#define GPIO_OUT_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // GPIO pin output drive value when GPIO_RX... 
#define GPIO_OUT_GPIO12_12_GPIO_A_MASK 	 0x10U
#define GPIO_OUT_GPIO12_12_GPIO_A_POS  	 4U

#define TX_COMP_EN_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // Enables jitter minimization compensation... 
#define TX_COMP_EN_GPIO12_12_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_GPIO12_12_GPIO_A_POS  	 5U

#define RES_CFG_GPIO12_12_GPIO_A_ADDR 	 0x2D4U // Pull-Up/Pull-Down Resistor Strength  
#define RES_CFG_GPIO12_12_GPIO_A_MASK 	 0x80U
#define RES_CFG_GPIO12_12_GPIO_A_POS  	 7U

#define GPIO12_12_GPIO_B_ADDR    	 0x2D5U
#define GPIO12_12_GPIO_B_DEFAULT 	 0xACU

#define GPIO_TX_ID_GPIO12_12_GPIO_B_ADDR 	 0x2D5U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_GPIO12_12_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_GPIO12_12_GPIO_B_POS  	 0U

#define OUT_TYPE_GPIO12_12_GPIO_B_ADDR 	 0x2D5U // Driver type selection  
#define OUT_TYPE_GPIO12_12_GPIO_B_MASK 	 0x20U
#define OUT_TYPE_GPIO12_12_GPIO_B_POS  	 5U

#define PULL_UPDN_SEL_GPIO12_12_GPIO_B_ADDR 	 0x2D5U // Buffer Pull-Up/Pull-Down Configuration  
#define PULL_UPDN_SEL_GPIO12_12_GPIO_B_MASK 	 0xC0U
#define PULL_UPDN_SEL_GPIO12_12_GPIO_B_POS  	 6U

#define GPIO12_12_GPIO_C_ADDR    	 0x2D6U
#define GPIO12_12_GPIO_C_DEFAULT 	 0x4CU

#define GPIO_RX_ID_GPIO12_12_GPIO_C_ADDR 	 0x2D6U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_GPIO12_12_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_GPIO12_12_GPIO_C_POS  	 0U

#define OVR_RES_CFG_GPIO12_12_GPIO_C_ADDR 	 0x2D6U // Override non-GPIO port function IO setti... 
#define OVR_RES_CFG_GPIO12_12_GPIO_C_MASK 	 0x80U
#define OVR_RES_CFG_GPIO12_12_GPIO_C_POS  	 7U

#define CMU_CMU2_ADDR    	 0x302U
#define CMU_CMU2_DEFAULT 	 0x00U

#define PFDDIV_RSHORT_CMU_CMU2_ADDR 	 0x302U // PFDDIV regulator voltage control. Contro... 
#define PFDDIV_RSHORT_CMU_CMU2_MASK 	 0x70U
#define PFDDIV_RSHORT_CMU_CMU2_POS  	 4U

#define BACKTOP_BACKTOP1_ADDR    	 0x308U
#define BACKTOP_BACKTOP1_DEFAULT 	 0x01U

#define BACKTOP_EN_BACKTOP_BACKTOP1_ADDR 	 0x308U // Backtop (line-buffer memory) write logic... 
#define BACKTOP_EN_BACKTOP_BACKTOP1_MASK 	 0x01U
#define BACKTOP_EN_BACKTOP_BACKTOP1_POS  	 0U

#define LINE_SPL2_BACKTOP_BACKTOP1_ADDR 	 0x308U // Line based distribution to line memories... 
#define LINE_SPL2_BACKTOP_BACKTOP1_MASK 	 0x08U
#define LINE_SPL2_BACKTOP_BACKTOP1_POS  	 3U

#define CSIPLLX_LOCK_BACKTOP_BACKTOP1_ADDR 	 0x308U // CSI MIPI TX PLL 0 locked (PLL for MIPI P... 
#define CSIPLLX_LOCK_BACKTOP_BACKTOP1_MASK 	 0x10U
#define CSIPLLX_LOCK_BACKTOP_BACKTOP1_POS  	 4U

#define CSIPLLY_LOCK_BACKTOP_BACKTOP1_ADDR 	 0x308U // CSI MIPI TX PLL 1 locked (PLL for MIPI P... 
#define CSIPLLY_LOCK_BACKTOP_BACKTOP1_MASK 	 0x20U
#define CSIPLLY_LOCK_BACKTOP_BACKTOP1_POS  	 5U

#define CSIPLLZ_LOCK_BACKTOP_BACKTOP1_ADDR 	 0x308U // CSI MIPI TX PLL 2 locked (PLL for MIPI P... 
#define CSIPLLZ_LOCK_BACKTOP_BACKTOP1_MASK 	 0x40U
#define CSIPLLZ_LOCK_BACKTOP_BACKTOP1_POS  	 6U

#define CSIPLLU_LOCK_BACKTOP_BACKTOP1_ADDR 	 0x308U // CSI MIPI TX PLL 3 locked (PLL for MIPI P... 
#define CSIPLLU_LOCK_BACKTOP_BACKTOP1_MASK 	 0x80U
#define CSIPLLU_LOCK_BACKTOP_BACKTOP1_POS  	 7U

#define BACKTOP_BACKTOP4_ADDR    	 0x30BU
#define BACKTOP_BACKTOP4_DEFAULT 	 0x00U

#define VS_VC2_L_BACKTOP_BACKTOP4_ADDR 	 0x30BU // For each video frame, override whether F... 
#define VS_VC2_L_BACKTOP_BACKTOP4_MASK 	 0xFFU
#define VS_VC2_L_BACKTOP_BACKTOP4_POS  	 0U

#define BACKTOP_BACKTOP5_ADDR    	 0x30CU
#define BACKTOP_BACKTOP5_DEFAULT 	 0x00U

#define VS_VC2_H_BACKTOP_BACKTOP5_ADDR 	 0x30CU // For each video frame, override whether F... 
#define VS_VC2_H_BACKTOP_BACKTOP5_MASK 	 0xFFU
#define VS_VC2_H_BACKTOP_BACKTOP5_POS  	 0U

#define BACKTOP_BACKTOP6_ADDR    	 0x30DU
#define BACKTOP_BACKTOP6_DEFAULT 	 0x00U

#define VS_VC3_L_BACKTOP_BACKTOP6_ADDR 	 0x30DU // For each video frame, override whether F... 
#define VS_VC3_L_BACKTOP_BACKTOP6_MASK 	 0xFFU
#define VS_VC3_L_BACKTOP_BACKTOP6_POS  	 0U

#define BACKTOP_BACKTOP7_ADDR    	 0x30EU
#define BACKTOP_BACKTOP7_DEFAULT 	 0x00U

#define VS_VC3_H_BACKTOP_BACKTOP7_ADDR 	 0x30EU // For each video frame, override whether F... 
#define VS_VC3_H_BACKTOP_BACKTOP7_MASK 	 0xFFU
#define VS_VC3_H_BACKTOP_BACKTOP7_POS  	 0U

#define BACKTOP_BACKTOP11_ADDR    	 0x312U
#define BACKTOP_BACKTOP11_DEFAULT 	 0x00U

#define LMO_Y_BACKTOP_BACKTOP11_ADDR 	 0x312U // Pipeline Y line memory overflow sticky r... 
#define LMO_Y_BACKTOP_BACKTOP11_MASK 	 0x02U
#define LMO_Y_BACKTOP_BACKTOP11_POS  	 1U

#define LMO_Z_BACKTOP_BACKTOP11_ADDR 	 0x312U // Pipeline Z line memory overflow sticky r... 
#define LMO_Z_BACKTOP_BACKTOP11_MASK 	 0x04U
#define LMO_Z_BACKTOP_BACKTOP11_POS  	 2U

#define CMD_OVERFLOW2_BACKTOP_BACKTOP11_ADDR 	 0x312U // Pipeline Y line memory command FIFO over... 
#define CMD_OVERFLOW2_BACKTOP_BACKTOP11_MASK 	 0x20U
#define CMD_OVERFLOW2_BACKTOP_BACKTOP11_POS  	 5U

#define CMD_OVERFLOW3_BACKTOP_BACKTOP11_ADDR 	 0x312U // Pipeline Z line memory command FIFO over... 
#define CMD_OVERFLOW3_BACKTOP_BACKTOP11_MASK 	 0x40U
#define CMD_OVERFLOW3_BACKTOP_BACKTOP11_POS  	 6U

#define BACKTOP_BACKTOP12_ADDR    	 0x313U
#define BACKTOP_BACKTOP12_DEFAULT 	 0x02U

#define CSI_OUT_EN_BACKTOP_BACKTOP12_ADDR 	 0x313U // Enable CSI output  
#define CSI_OUT_EN_BACKTOP_BACKTOP12_MASK 	 0x02U
#define CSI_OUT_EN_BACKTOP_BACKTOP12_POS  	 1U

#define BACKTOP_BACKTOP13_ADDR    	 0x314U
#define BACKTOP_BACKTOP13_DEFAULT 	 0x00U

#define SOFT_VC_Y_BACKTOP_BACKTOP13_ADDR 	 0x314U // Software-defined virtual channel number ... 
#define SOFT_VC_Y_BACKTOP_BACKTOP13_MASK 	 0xF0U
#define SOFT_VC_Y_BACKTOP_BACKTOP13_POS  	 4U

#define BACKTOP_BACKTOP14_ADDR    	 0x315U
#define BACKTOP_BACKTOP14_DEFAULT 	 0x00U

#define SOFT_VC_Z_BACKTOP_BACKTOP14_ADDR 	 0x315U // Software-defined virtual channel number ... 
#define SOFT_VC_Z_BACKTOP_BACKTOP14_MASK 	 0x0FU
#define SOFT_VC_Z_BACKTOP_BACKTOP14_POS  	 0U

#define BACKTOP_BACKTOP15_ADDR    	 0x316U
#define BACKTOP_BACKTOP15_DEFAULT 	 0x00U

#define SOFT_DT_Y_H_BACKTOP_BACKTOP15_ADDR 	 0x316U // High bits of software-defined data type ... 
#define SOFT_DT_Y_H_BACKTOP_BACKTOP15_MASK 	 0xC0U
#define SOFT_DT_Y_H_BACKTOP_BACKTOP15_POS  	 6U

#define BACKTOP_BACKTOP16_ADDR    	 0x317U
#define BACKTOP_BACKTOP16_DEFAULT 	 0x00U

#define SOFT_DT_Y_L_BACKTOP_BACKTOP16_ADDR 	 0x317U // Low bits of software-defined data type f... 
#define SOFT_DT_Y_L_BACKTOP_BACKTOP16_MASK 	 0x0FU
#define SOFT_DT_Y_L_BACKTOP_BACKTOP16_POS  	 0U

#define SOFT_DT_Z_H_BACKTOP_BACKTOP16_ADDR 	 0x317U // High bits of software-defined data type ... 
#define SOFT_DT_Z_H_BACKTOP_BACKTOP16_MASK 	 0xF0U
#define SOFT_DT_Z_H_BACKTOP_BACKTOP16_POS  	 4U

#define BACKTOP_BACKTOP17_ADDR    	 0x318U
#define BACKTOP_BACKTOP17_DEFAULT 	 0x00U

#define SOFT_DT_Z_L_BACKTOP_BACKTOP17_ADDR 	 0x318U // Low bits of software-defined data type f... 
#define SOFT_DT_Z_L_BACKTOP_BACKTOP17_MASK 	 0x03U
#define SOFT_DT_Z_L_BACKTOP_BACKTOP17_POS  	 0U

#define BACKTOP_BACKTOP18_ADDR    	 0x319U
#define BACKTOP_BACKTOP18_DEFAULT 	 0x00U

#define SOFT_BPP_Y_BACKTOP_BACKTOP18_ADDR 	 0x319U // Software-defined bpp for Pipeline Y  
#define SOFT_BPP_Y_BACKTOP_BACKTOP18_MASK 	 0x1FU
#define SOFT_BPP_Y_BACKTOP_BACKTOP18_POS  	 0U

#define SOFT_BPP_Z_H_BACKTOP_BACKTOP18_ADDR 	 0x319U // High bits of software-defined bpp for Pi... 
#define SOFT_BPP_Z_H_BACKTOP_BACKTOP18_MASK 	 0xE0U
#define SOFT_BPP_Z_H_BACKTOP_BACKTOP18_POS  	 5U

#define BACKTOP_BACKTOP19_ADDR    	 0x31AU
#define BACKTOP_BACKTOP19_DEFAULT 	 0x00U

#define SOFT_BPP_Z_L_BACKTOP_BACKTOP19_ADDR 	 0x31AU // Low bits of software-defined bpp for Pip... 
#define SOFT_BPP_Z_L_BACKTOP_BACKTOP19_MASK 	 0x03U
#define SOFT_BPP_Z_L_BACKTOP_BACKTOP19_POS  	 0U

#define BACKTOP_BACKTOP20_ADDR    	 0x31BU
#define BACKTOP_BACKTOP20_DEFAULT 	 0x00U

#define PHY0_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP20_ADDR 	 0x31BU // Low byte of software-override value for ... 
#define PHY0_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP20_MASK 	 0xFFU
#define PHY0_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP20_POS  	 0U

#define BACKTOP_BACKTOP21_ADDR    	 0x31CU
#define BACKTOP_BACKTOP21_DEFAULT 	 0x00U

#define PHY0_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP21_ADDR 	 0x31CU // High nibble of software-override value f... 
#define PHY0_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP21_MASK 	 0x0FU
#define PHY0_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP21_POS  	 0U

#define BPP8DBLY_BACKTOP_BACKTOP21_ADDR 	 0x31CU // bpp = 8 processed as 16-bit color enable... 
#define BPP8DBLY_BACKTOP_BACKTOP21_MASK 	 0x20U
#define BPP8DBLY_BACKTOP_BACKTOP21_POS  	 5U

#define BPP8DBLZ_BACKTOP_BACKTOP21_ADDR 	 0x31CU // bpp = 8 processed as 16-bit color enable... 
#define BPP8DBLZ_BACKTOP_BACKTOP21_MASK 	 0x40U
#define BPP8DBLZ_BACKTOP_BACKTOP21_POS  	 6U

#define BACKTOP_BACKTOP22_ADDR    	 0x31DU
#define BACKTOP_BACKTOP22_DEFAULT 	 0x2FU

#define PHY0_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP22_ADDR 	 0x31DU // Determines CSI PHY0 output frequency in ... 
#define PHY0_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP22_MASK 	 0x1FU
#define PHY0_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP22_POS  	 0U

#define PHY0_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP22_ADDR 	 0x31DU // CSI PHY0 software-override disable for f... 
#define PHY0_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP22_MASK 	 0x20U
#define PHY0_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP22_POS  	 5U

#define OVERRIDE_BPP_VC_DTY_BACKTOP_BACKTOP22_ADDR 	 0x31DU // Software-override enable for BPP, VC, an... 
#define OVERRIDE_BPP_VC_DTY_BACKTOP_BACKTOP22_MASK 	 0x80U
#define OVERRIDE_BPP_VC_DTY_BACKTOP_BACKTOP22_POS  	 7U

#define BACKTOP_BACKTOP23_ADDR    	 0x31EU
#define BACKTOP_BACKTOP23_DEFAULT 	 0x00U

#define PHY1_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP23_ADDR 	 0x31EU // Low byte of software-override value for ... 
#define PHY1_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP23_MASK 	 0xFFU
#define PHY1_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP23_POS  	 0U

#define BACKTOP_BACKTOP24_ADDR    	 0x31FU
#define BACKTOP_BACKTOP24_DEFAULT 	 0x00U

#define PHY1_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP24_ADDR 	 0x31FU // High nibble of software-override value f... 
#define PHY1_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP24_MASK 	 0x0FU
#define PHY1_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP24_POS  	 0U

#define BPP8DBLY_MODE_BACKTOP_BACKTOP24_ADDR 	 0x31FU // Enable 8-bit write alternate map to RAMs... 
#define BPP8DBLY_MODE_BACKTOP_BACKTOP24_MASK 	 0x20U
#define BPP8DBLY_MODE_BACKTOP_BACKTOP24_POS  	 5U

#define BPP8DBLZ_MODE_BACKTOP_BACKTOP24_ADDR 	 0x31FU // Enable 8-bit write alternate map to RAMs... 
#define BPP8DBLZ_MODE_BACKTOP_BACKTOP24_MASK 	 0x40U
#define BPP8DBLZ_MODE_BACKTOP_BACKTOP24_POS  	 6U

#define BACKTOP_BACKTOP25_ADDR    	 0x320U
#define BACKTOP_BACKTOP25_DEFAULT 	 0x2FU

#define PHY1_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP25_ADDR 	 0x320U // Determines CSI PHY1 output frequency in ... 
#define PHY1_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP25_MASK 	 0x1FU
#define PHY1_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP25_POS  	 0U

#define PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_ADDR 	 0x320U // CSI PHY1 software-override disable for f... 
#define PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_MASK 	 0x20U
#define PHY1_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP25_POS  	 5U

#define OVERRIDE_BPP_VC_DTZ_BACKTOP_BACKTOP25_ADDR 	 0x320U // Software-override enable for BPP, VC, an... 
#define OVERRIDE_BPP_VC_DTZ_BACKTOP_BACKTOP25_MASK 	 0x40U
#define OVERRIDE_BPP_VC_DTZ_BACKTOP_BACKTOP25_POS  	 6U

#define BACKTOP_BACKTOP26_ADDR    	 0x321U
#define BACKTOP_BACKTOP26_DEFAULT 	 0x00U

#define PHY2_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP26_ADDR 	 0x321U // Low byte of software-override value for ... 
#define PHY2_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP26_MASK 	 0xFFU
#define PHY2_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP26_POS  	 0U

#define BACKTOP_BACKTOP27_ADDR    	 0x322U
#define BACKTOP_BACKTOP27_DEFAULT 	 0x00U

#define PHY2_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP27_ADDR 	 0x322U // High nibble of software-override value f... 
#define PHY2_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP27_MASK 	 0x0FU
#define PHY2_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP27_POS  	 0U

#define YUV_8_10_MUX_MODE1_BACKTOP_BACKTOP27_ADDR 	 0x322U // Enable YUV422 8-/10-bit muxed mode suppo... 
#define YUV_8_10_MUX_MODE1_BACKTOP_BACKTOP27_MASK 	 0x10U
#define YUV_8_10_MUX_MODE1_BACKTOP_BACKTOP27_POS  	 4U

#define YUV_8_10_MUX_MODE2_BACKTOP_BACKTOP27_ADDR 	 0x322U // Enable YUV422 8-/10-bit muxed mode suppo... 
#define YUV_8_10_MUX_MODE2_BACKTOP_BACKTOP27_MASK 	 0x20U
#define YUV_8_10_MUX_MODE2_BACKTOP_BACKTOP27_POS  	 5U

#define YUV_8_10_MUX_MODE3_BACKTOP_BACKTOP27_ADDR 	 0x322U // Enable YUV422 8-/10-bit muxed mode suppo... 
#define YUV_8_10_MUX_MODE3_BACKTOP_BACKTOP27_MASK 	 0x40U
#define YUV_8_10_MUX_MODE3_BACKTOP_BACKTOP27_POS  	 6U

#define YUV_8_10_MUX_MODE4_BACKTOP_BACKTOP27_ADDR 	 0x322U // Enable YUV422 8-/10-bit muxed mode suppo... 
#define YUV_8_10_MUX_MODE4_BACKTOP_BACKTOP27_MASK 	 0x80U
#define YUV_8_10_MUX_MODE4_BACKTOP_BACKTOP27_POS  	 7U

#define BACKTOP_BACKTOP28_ADDR    	 0x323U
#define BACKTOP_BACKTOP28_DEFAULT 	 0x2FU

#define PHY2_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP28_ADDR 	 0x323U // Determines CSI PHY2 output frequency in ... 
#define PHY2_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP28_MASK 	 0x1FU
#define PHY2_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP28_POS  	 0U

#define PHY2_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP28_ADDR 	 0x323U // CSI PHY2 software-override disable for f... 
#define PHY2_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP28_MASK 	 0x20U
#define PHY2_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP28_POS  	 5U

#define BACKTOP_BACKTOP29_ADDR    	 0x324U
#define BACKTOP_BACKTOP29_DEFAULT 	 0x00U

#define PHY3_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP29_ADDR 	 0x324U // Low byte of software-override value for ... 
#define PHY3_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP29_MASK 	 0xFFU
#define PHY3_CSI_TX_DPLL_FB_FRACTION_IN_L_BACKTOP_BACKTOP29_POS  	 0U

#define BACKTOP_BACKTOP30_ADDR    	 0x325U
#define BACKTOP_BACKTOP30_DEFAULT 	 0x00U

#define PHY3_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP30_ADDR 	 0x325U // High nibble of software-override value f... 
#define PHY3_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP30_MASK 	 0x0FU
#define PHY3_CSI_TX_DPLL_FB_FRACTION_IN_H_BACKTOP_BACKTOP30_POS  	 0U

#define BACKTOP_W_FRAME_BACKTOP_BACKTOP30_ADDR 	 0x325U // When this register is set, BACKTOP (line... 
#define BACKTOP_W_FRAME_BACKTOP_BACKTOP30_MASK 	 0x80U
#define BACKTOP_W_FRAME_BACKTOP_BACKTOP30_POS  	 7U

#define BACKTOP_BACKTOP31_ADDR    	 0x326U
#define BACKTOP_BACKTOP31_DEFAULT 	 0x2FU

#define PHY3_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP31_ADDR 	 0x326U // Determines CSI PHY3 output frequency in ... 
#define PHY3_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP31_MASK 	 0x1FU
#define PHY3_CSI_TX_DPLL_PREDEF_FREQ_BACKTOP_BACKTOP31_POS  	 0U

#define PHY3_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP31_ADDR 	 0x326U // CSI PHY3 software-override disable for f... 
#define PHY3_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP31_MASK 	 0x20U
#define PHY3_CSI_TX_DPLL_FB_FRACTION_PREDEF_EN_BACKTOP_BACKTOP31_POS  	 5U

#define BACKTOP_BACKTOP32_ADDR    	 0x327U
#define BACKTOP_BACKTOP32_DEFAULT 	 0x00U

#define BPP10DBLY_BACKTOP_BACKTOP32_ADDR 	 0x327U // bpp = 8 processed as 16-bit color enable... 
#define BPP10DBLY_BACKTOP_BACKTOP32_MASK 	 0x02U
#define BPP10DBLY_BACKTOP_BACKTOP32_POS  	 1U

#define BPP10DBLZ_BACKTOP_BACKTOP32_ADDR 	 0x327U // bpp = 8 processed as 16-bit color enable... 
#define BPP10DBLZ_BACKTOP_BACKTOP32_MASK 	 0x04U
#define BPP10DBLZ_BACKTOP_BACKTOP32_POS  	 2U

#define BPP10DBLY_MODE_BACKTOP_BACKTOP32_ADDR 	 0x327U // Enable 8-bit write alternate map to RAMs... 
#define BPP10DBLY_MODE_BACKTOP_BACKTOP32_MASK 	 0x20U
#define BPP10DBLY_MODE_BACKTOP_BACKTOP32_POS  	 5U

#define BPP10DBLZ_MODE_BACKTOP_BACKTOP32_ADDR 	 0x327U // Enable 8-bit write alternate map to RAMs... 
#define BPP10DBLZ_MODE_BACKTOP_BACKTOP32_MASK 	 0x40U
#define BPP10DBLZ_MODE_BACKTOP_BACKTOP32_POS  	 6U

#define BACKTOP_BACKTOP33_ADDR    	 0x328U
#define BACKTOP_BACKTOP33_DEFAULT 	 0x00U

#define BPP12DBLY_BACKTOP_BACKTOP33_ADDR 	 0x328U // bpp = 8 processed as 16-bit color enable... 
#define BPP12DBLY_BACKTOP_BACKTOP33_MASK 	 0x02U
#define BPP12DBLY_BACKTOP_BACKTOP33_POS  	 1U

#define BPP12DBLZ_BACKTOP_BACKTOP33_ADDR 	 0x328U // bpp = 8 processed as 16-bit color enable... 
#define BPP12DBLZ_BACKTOP_BACKTOP33_MASK 	 0x04U
#define BPP12DBLZ_BACKTOP_BACKTOP33_POS  	 2U

#define MIPI_PHY_MIPI_PHY0_ADDR    	 0x330U
#define MIPI_PHY_MIPI_PHY0_DEFAULT 	 0x04U

#define PHY_4X2_MIPI_PHY_MIPI_PHY0_ADDR 	 0x330U // MIPI output configured as 4x2.  
#define PHY_4X2_MIPI_PHY_MIPI_PHY0_MASK 	 0x01U
#define PHY_4X2_MIPI_PHY_MIPI_PHY0_POS  	 0U

#define PHY_2X4_MIPI_PHY_MIPI_PHY0_ADDR 	 0x330U // MIPI output configured as 2x4.  
#define PHY_2X4_MIPI_PHY_MIPI_PHY0_MASK 	 0x04U
#define PHY_2X4_MIPI_PHY_MIPI_PHY0_POS  	 2U

#define PHY_1X4A_22_MIPI_PHY_MIPI_PHY0_ADDR 	 0x330U // MIPI output configured as 1x4 and 2x2.  
#define PHY_1X4A_22_MIPI_PHY_MIPI_PHY0_MASK 	 0x08U
#define PHY_1X4A_22_MIPI_PHY_MIPI_PHY0_POS  	 3U

#define PHY_1X4B_22_MIPI_PHY_MIPI_PHY0_ADDR 	 0x330U // MIPI output configured as 2x2 and 1x4.  
#define PHY_1X4B_22_MIPI_PHY_MIPI_PHY0_MASK 	 0x10U
#define PHY_1X4B_22_MIPI_PHY_MIPI_PHY0_POS  	 4U

#define FORCE_CSI_OUT_EN_MIPI_PHY_MIPI_PHY0_ADDR 	 0x330U // Force CSI output clock for use in MIPI l... 
#define FORCE_CSI_OUT_EN_MIPI_PHY_MIPI_PHY0_MASK 	 0x80U
#define FORCE_CSI_OUT_EN_MIPI_PHY_MIPI_PHY0_POS  	 7U

#define MIPI_PHY_MIPI_PHY1_ADDR    	 0x331U
#define MIPI_PHY_MIPI_PHY1_DEFAULT 	 0x00U

#define T_CLK_PRZERO_MIPI_PHY_MIPI_PHY1_ADDR 	 0x331U // Typical DPHY clock lane HS_prepare + HS_... 
#define T_CLK_PRZERO_MIPI_PHY_MIPI_PHY1_MASK 	 0x03U
#define T_CLK_PRZERO_MIPI_PHY_MIPI_PHY1_POS  	 0U

#define T_HS_PREP_MIPI_PHY_MIPI_PHY1_ADDR 	 0x331U // Typical DPHY data lane HS_prepare timing... 
#define T_HS_PREP_MIPI_PHY_MIPI_PHY1_MASK 	 0x30U
#define T_HS_PREP_MIPI_PHY_MIPI_PHY1_POS  	 4U

#define T_HS_PRZERO_MIPI_PHY_MIPI_PHY1_ADDR 	 0x331U // Typical DPHY data lane HS_prep + HS_zero... 
#define T_HS_PRZERO_MIPI_PHY_MIPI_PHY1_MASK 	 0xC0U
#define T_HS_PRZERO_MIPI_PHY_MIPI_PHY1_POS  	 6U

#define MIPI_PHY_MIPI_PHY2_ADDR    	 0x332U
#define MIPI_PHY_MIPI_PHY2_DEFAULT 	 0xF4U

#define T_HS_TRAIL_MIPI_PHY_MIPI_PHY2_ADDR 	 0x332U // Typical DPHY data lane HS_trail timing  
#define T_HS_TRAIL_MIPI_PHY_MIPI_PHY2_MASK 	 0x03U
#define T_HS_TRAIL_MIPI_PHY_MIPI_PHY2_POS  	 0U

#define T_LPX_MIPI_PHY_MIPI_PHY2_ADDR 	 0x332U // Typical DPHY TLPX timing  
#define T_LPX_MIPI_PHY_MIPI_PHY2_MASK 	 0x0CU
#define T_LPX_MIPI_PHY_MIPI_PHY2_POS  	 2U

#define PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_ADDR 	 0x332U // Put MIPI PHY into standby mode if not us... 
#define PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_MASK 	 0xF0U
#define PHY_STDBY_N_MIPI_PHY_MIPI_PHY2_POS  	 4U

#define MIPI_PHY_MIPI_PHY3_ADDR    	 0x333U
#define MIPI_PHY_MIPI_PHY3_DEFAULT 	 0x4EU

#define PHY0_LANE_MAP_MIPI_PHY_MIPI_PHY3_ADDR 	 0x333U // MIPI PHY0 to Port A {{phyx_lane_map_des}... 
#define PHY0_LANE_MAP_MIPI_PHY_MIPI_PHY3_MASK 	 0x0FU
#define PHY0_LANE_MAP_MIPI_PHY_MIPI_PHY3_POS  	 0U

#define PHY1_LANE_MAP_MIPI_PHY_MIPI_PHY3_ADDR 	 0x333U // MIPI PHY1 to Port A {{phyx_lane_map_des}... 
#define PHY1_LANE_MAP_MIPI_PHY_MIPI_PHY3_MASK 	 0xF0U
#define PHY1_LANE_MAP_MIPI_PHY_MIPI_PHY3_POS  	 4U

#define MIPI_PHY_MIPI_PHY4_ADDR    	 0x334U
#define MIPI_PHY_MIPI_PHY4_DEFAULT 	 0xE4U

#define PHY2_LANE_MAP_MIPI_PHY_MIPI_PHY4_ADDR 	 0x334U // MIPI PHY2 to Port B {{phyx_lane_map_des}... 
#define PHY2_LANE_MAP_MIPI_PHY_MIPI_PHY4_MASK 	 0x0FU
#define PHY2_LANE_MAP_MIPI_PHY_MIPI_PHY4_POS  	 0U

#define PHY3_LANE_MAP_MIPI_PHY_MIPI_PHY4_ADDR 	 0x334U // MIPI PHY3 to Port B {{phyx_lane_map_des}... 
#define PHY3_LANE_MAP_MIPI_PHY_MIPI_PHY4_MASK 	 0xF0U
#define PHY3_LANE_MAP_MIPI_PHY_MIPI_PHY4_POS  	 4U

#define MIPI_PHY_MIPI_PHY5_ADDR    	 0x335U
#define MIPI_PHY_MIPI_PHY5_DEFAULT 	 0x00U

#define PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_ADDR 	 0x335U // MIPI PHY0 lane polarity.  
#define PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_MASK 	 0x07U
#define PHY0_POL_MAP_MIPI_PHY_MIPI_PHY5_POS  	 0U

#define PHY1_POL_MAP_MIPI_PHY_MIPI_PHY5_ADDR 	 0x335U // MIPI PHY1 lane polarity.  
#define PHY1_POL_MAP_MIPI_PHY_MIPI_PHY5_MASK 	 0x38U
#define PHY1_POL_MAP_MIPI_PHY_MIPI_PHY5_POS  	 3U

#define T_CLK_PREP_MIPI_PHY_MIPI_PHY5_ADDR 	 0x335U // Typical DPHY clock lane HS_prepare timin... 
#define T_CLK_PREP_MIPI_PHY_MIPI_PHY5_MASK 	 0xC0U
#define T_CLK_PREP_MIPI_PHY_MIPI_PHY5_POS  	 6U

#define MIPI_PHY_MIPI_PHY6_ADDR    	 0x336U
#define MIPI_PHY_MIPI_PHY6_DEFAULT 	 0x00U

#define PHY2_POL_MAP_MIPI_PHY_MIPI_PHY6_ADDR 	 0x336U // MIPI PHY2 lane polarity.  
#define PHY2_POL_MAP_MIPI_PHY_MIPI_PHY6_MASK 	 0x07U
#define PHY2_POL_MAP_MIPI_PHY_MIPI_PHY6_POS  	 0U

#define PHY3_POL_MAP_MIPI_PHY_MIPI_PHY6_ADDR 	 0x336U // MIPI PHY3 lane polarity.  
#define PHY3_POL_MAP_MIPI_PHY_MIPI_PHY6_MASK 	 0x38U
#define PHY3_POL_MAP_MIPI_PHY_MIPI_PHY6_POS  	 3U

#define PHY_CP0_MIPI_PHY_MIPI_PHY6_ADDR 	 0x336U // MIPI PHY port copy enable 0.  
#define PHY_CP0_MIPI_PHY_MIPI_PHY6_MASK 	 0x40U
#define PHY_CP0_MIPI_PHY_MIPI_PHY6_POS  	 6U

#define PHY_CP1_MIPI_PHY_MIPI_PHY6_ADDR 	 0x336U // MIPI PHY port copy enable 1.  
#define PHY_CP1_MIPI_PHY_MIPI_PHY6_MASK 	 0x80U
#define PHY_CP1_MIPI_PHY_MIPI_PHY6_POS  	 7U

#define MIPI_PHY_MIPI_PHY9_ADDR    	 0x339U
#define MIPI_PHY_MIPI_PHY9_DEFAULT 	 0x00U

#define PHY_CP0_OVERFLOW_MIPI_PHY_MIPI_PHY9_ADDR 	 0x339U // PHY copy 0 FIFO overflow flag (sticky)  
#define PHY_CP0_OVERFLOW_MIPI_PHY_MIPI_PHY9_MASK 	 0x01U
#define PHY_CP0_OVERFLOW_MIPI_PHY_MIPI_PHY9_POS  	 0U

#define PHY_CP0_DST_MIPI_PHY_MIPI_PHY9_ADDR 	 0x339U // MIPI PHY copy 0 destination. See phy_cp0... 
#define PHY_CP0_DST_MIPI_PHY_MIPI_PHY9_MASK 	 0xC0U
#define PHY_CP0_DST_MIPI_PHY_MIPI_PHY9_POS  	 6U

#define MIPI_PHY_MIPI_PHY10_ADDR    	 0x33AU
#define MIPI_PHY_MIPI_PHY10_DEFAULT 	 0x02U

#define PHY_CP0_UNDERFLOW_MIPI_PHY_MIPI_PHY10_ADDR 	 0x33AU // PHY copy 0 FIFO underflow flag (sticky)  
#define PHY_CP0_UNDERFLOW_MIPI_PHY_MIPI_PHY10_MASK 	 0x01U
#define PHY_CP0_UNDERFLOW_MIPI_PHY_MIPI_PHY10_POS  	 0U

#define PHY_CP0_SRC_MIPI_PHY_MIPI_PHY10_ADDR 	 0x33AU // MIPI PHY copy 0 source. See phy_cp0 for ... 
#define PHY_CP0_SRC_MIPI_PHY_MIPI_PHY10_MASK 	 0xC0U
#define PHY_CP0_SRC_MIPI_PHY_MIPI_PHY10_POS  	 6U

#define MIPI_PHY_MIPI_PHY11_ADDR    	 0x33BU
#define MIPI_PHY_MIPI_PHY11_DEFAULT 	 0x00U

#define PHY_CP1_OVERFLOW_MIPI_PHY_MIPI_PHY11_ADDR 	 0x33BU // PHY copy 1 FIFO overflow flag (sticky)  
#define PHY_CP1_OVERFLOW_MIPI_PHY_MIPI_PHY11_MASK 	 0x01U
#define PHY_CP1_OVERFLOW_MIPI_PHY_MIPI_PHY11_POS  	 0U

#define PHY_CP1_DST_MIPI_PHY_MIPI_PHY11_ADDR 	 0x33BU // MIPI PHY copy 1 destination. See phy_cp1... 
#define PHY_CP1_DST_MIPI_PHY_MIPI_PHY11_MASK 	 0xC0U
#define PHY_CP1_DST_MIPI_PHY_MIPI_PHY11_POS  	 6U

#define MIPI_PHY_MIPI_PHY12_ADDR    	 0x33CU
#define MIPI_PHY_MIPI_PHY12_DEFAULT 	 0x02U

#define PHY_CP1_UNDERFLOW_MIPI_PHY_MIPI_PHY12_ADDR 	 0x33CU // PHY copy 1 FIFO underflow flag (sticky)  
#define PHY_CP1_UNDERFLOW_MIPI_PHY_MIPI_PHY12_MASK 	 0x01U
#define PHY_CP1_UNDERFLOW_MIPI_PHY_MIPI_PHY12_POS  	 0U

#define PHY_CP1_SRC_MIPI_PHY_MIPI_PHY12_ADDR 	 0x33CU // MIPI PHY copy 1 source. See phy_cp1 for ... 
#define PHY_CP1_SRC_MIPI_PHY_MIPI_PHY12_MASK 	 0xC0U
#define PHY_CP1_SRC_MIPI_PHY_MIPI_PHY12_POS  	 6U

#define MIPI_PHY_MIPI_PHY13_ADDR    	 0x33DU
#define MIPI_PHY_MIPI_PHY13_DEFAULT 	 0x00U

#define T_T3_PREBEGIN_MIPI_PHY_MIPI_PHY13_ADDR 	 0x33DU // CPHY prebegin phase of the preamble (t3_... 
#define T_T3_PREBEGIN_MIPI_PHY_MIPI_PHY13_MASK 	 0x3FU
#define T_T3_PREBEGIN_MIPI_PHY_MIPI_PHY13_POS  	 0U

#define MIPI_PHY_MIPI_PHY14_ADDR    	 0x33EU
#define MIPI_PHY_MIPI_PHY14_DEFAULT 	 0x11U

#define T_T3_PREP_MIPI_PHY_MIPI_PHY14_ADDR 	 0x33EU // CPHY Ths_prepare timing  
#define T_T3_PREP_MIPI_PHY_MIPI_PHY14_MASK 	 0x03U
#define T_T3_PREP_MIPI_PHY_MIPI_PHY14_POS  	 0U

#define T_T3_POST_MIPI_PHY_MIPI_PHY14_ADDR 	 0x33EU // CPHY post length after HS data = (t3_pos... 
#define T_T3_POST_MIPI_PHY_MIPI_PHY14_MASK 	 0x7CU
#define T_T3_POST_MIPI_PHY_MIPI_PHY14_POS  	 2U

#define MIPI_PHY_MIPI_PHY15_ADDR    	 0x33FU
#define MIPI_PHY_MIPI_PHY15_DEFAULT 	 0x00U

#define RST_MIPITX_LOC_MIPI_PHY_MIPI_PHY15_ADDR 	 0x33FU // Active high reset to MIPI controller  
#define RST_MIPITX_LOC_MIPI_PHY_MIPI_PHY15_MASK 	 0x0FU
#define RST_MIPITX_LOC_MIPI_PHY_MIPI_PHY15_POS  	 0U

#define MIPI_PHY_MIPI_PHY16_ADDR    	 0x340U
#define MIPI_PHY_MIPI_PHY16_DEFAULT 	 0x00U

#define VID_OVERFLOW_OEN_MIPI_PHY_MIPI_PHY16_ADDR 	 0x340U // Enable reporting of video pipe overflow ... 
#define VID_OVERFLOW_OEN_MIPI_PHY_MIPI_PHY16_MASK 	 0x01U
#define VID_OVERFLOW_OEN_MIPI_PHY_MIPI_PHY16_POS  	 0U

#define TUN_ECC_CORR_ERR_OEN_MIPI_PHY_MIPI_PHY16_ADDR 	 0x340U // For tunneling mode, enable reporting at ... 
#define TUN_ECC_CORR_ERR_OEN_MIPI_PHY_MIPI_PHY16_MASK 	 0x08U
#define TUN_ECC_CORR_ERR_OEN_MIPI_PHY_MIPI_PHY16_POS  	 3U

#define TUN_ECC_UNCORR_ERR_OEN_MIPI_PHY_MIPI_PHY16_ADDR 	 0x340U // For tunneling mode, enable reporting at ... 
#define TUN_ECC_UNCORR_ERR_OEN_MIPI_PHY_MIPI_PHY16_MASK 	 0x10U
#define TUN_ECC_UNCORR_ERR_OEN_MIPI_PHY_MIPI_PHY16_POS  	 4U

#define TUN_DATA_CRC_ERR_OEN_MIPI_PHY_MIPI_PHY16_ADDR 	 0x340U // For tunneling mode, enable reporting at ... 
#define TUN_DATA_CRC_ERR_OEN_MIPI_PHY_MIPI_PHY16_MASK 	 0x20U
#define TUN_DATA_CRC_ERR_OEN_MIPI_PHY_MIPI_PHY16_POS  	 5U

#define MIPI_PHY_MIPI_PHY17_ADDR    	 0x341U
#define MIPI_PHY_MIPI_PHY17_DEFAULT 	 0x00U

#define VID_OVERFLOW_FLAG_MIPI_PHY_MIPI_PHY17_ADDR 	 0x341U // Combined error status of all video pipe ... 
#define VID_OVERFLOW_FLAG_MIPI_PHY_MIPI_PHY17_MASK 	 0x01U
#define VID_OVERFLOW_FLAG_MIPI_PHY_MIPI_PHY17_POS  	 0U

#define TUN_ECC_CORR_ERR_MIPI_PHY_MIPI_PHY17_ADDR 	 0x341U // For tunneling mode, correctable errors o... 
#define TUN_ECC_CORR_ERR_MIPI_PHY_MIPI_PHY17_MASK 	 0x08U
#define TUN_ECC_CORR_ERR_MIPI_PHY_MIPI_PHY17_POS  	 3U

#define TUN_ECC_UNCORR_ERR_MIPI_PHY_MIPI_PHY17_ADDR 	 0x341U // For tunneling mode, uncorrectable errors... 
#define TUN_ECC_UNCORR_ERR_MIPI_PHY_MIPI_PHY17_MASK 	 0x10U
#define TUN_ECC_UNCORR_ERR_MIPI_PHY_MIPI_PHY17_POS  	 4U

#define TUN_DATA_CRC_ERR_MIPI_PHY_MIPI_PHY17_ADDR 	 0x341U // For tunneling mode, DPHY/CPHY data CRC e... 
#define TUN_DATA_CRC_ERR_MIPI_PHY_MIPI_PHY17_MASK 	 0x20U
#define TUN_DATA_CRC_ERR_MIPI_PHY_MIPI_PHY17_POS  	 5U

#define MIPI_PHY_MIPI_PHY18_ADDR    	 0x342U
#define MIPI_PHY_MIPI_PHY18_DEFAULT 	 0x00U

#define CSI2_TX1_PKT_CNT_MIPI_PHY_MIPI_PHY18_ADDR 	 0x342U // Packet Count of CSI2 Controller 1.  
#define CSI2_TX1_PKT_CNT_MIPI_PHY_MIPI_PHY18_MASK 	 0x0FU
#define CSI2_TX1_PKT_CNT_MIPI_PHY_MIPI_PHY18_POS  	 0U

#define CSI2_TX2_PKT_CNT_MIPI_PHY_MIPI_PHY18_ADDR 	 0x342U // Packet Count of CSI2 Controller 2.  
#define CSI2_TX2_PKT_CNT_MIPI_PHY_MIPI_PHY18_MASK 	 0xF0U
#define CSI2_TX2_PKT_CNT_MIPI_PHY_MIPI_PHY18_POS  	 4U

#define MIPI_PHY_MIPI_PHY19_ADDR    	 0x343U
#define MIPI_PHY_MIPI_PHY19_DEFAULT 	 0x00U

#define CSI2_DUP1_PKT_CNT_MIPI_PHY_MIPI_PHY19_ADDR 	 0x343U // Packet Count of CSI2 Duplication 1.  
#define CSI2_DUP1_PKT_CNT_MIPI_PHY_MIPI_PHY19_MASK 	 0x0FU
#define CSI2_DUP1_PKT_CNT_MIPI_PHY_MIPI_PHY19_POS  	 0U

#define CSI2_DUP2_PKT_CNT_MIPI_PHY_MIPI_PHY19_ADDR 	 0x343U // Packet Count of CSI2 Duplication 2.  
#define CSI2_DUP2_PKT_CNT_MIPI_PHY_MIPI_PHY19_MASK 	 0xF0U
#define CSI2_DUP2_PKT_CNT_MIPI_PHY_MIPI_PHY19_POS  	 4U

#define MIPI_PHY_MIPI_PHY20_ADDR    	 0x344U
#define MIPI_PHY_MIPI_PHY20_DEFAULT 	 0x00U

#define PHY0_PKT_CNT_MIPI_PHY_MIPI_PHY20_ADDR 	 0x344U // Packet Count of MIPI PHY0.  
#define PHY0_PKT_CNT_MIPI_PHY_MIPI_PHY20_MASK 	 0x0FU
#define PHY0_PKT_CNT_MIPI_PHY_MIPI_PHY20_POS  	 0U

#define PHY1_PKT_CNT_MIPI_PHY_MIPI_PHY20_ADDR 	 0x344U // Packet Count of MIPI PHY1.  
#define PHY1_PKT_CNT_MIPI_PHY_MIPI_PHY20_MASK 	 0xF0U
#define PHY1_PKT_CNT_MIPI_PHY_MIPI_PHY20_POS  	 4U

#define MIPI_PHY_MIPI_PHY21_ADDR    	 0x345U
#define MIPI_PHY_MIPI_PHY21_DEFAULT 	 0x00U

#define PHY2_PKT_CNT_MIPI_PHY_MIPI_PHY21_ADDR 	 0x345U // Packet Count of MIPI PHY2.  
#define PHY2_PKT_CNT_MIPI_PHY_MIPI_PHY21_MASK 	 0x0FU
#define PHY2_PKT_CNT_MIPI_PHY_MIPI_PHY21_POS  	 0U

#define PHY3_PKT_CNT_MIPI_PHY_MIPI_PHY21_ADDR 	 0x345U // Packet Count of MIPI PHY3.  
#define PHY3_PKT_CNT_MIPI_PHY_MIPI_PHY21_MASK 	 0xF0U
#define PHY3_PKT_CNT_MIPI_PHY_MIPI_PHY21_POS  	 4U

#define FSYNC_FSYNC_0_ADDR    	 0x3E0U
#define FSYNC_FSYNC_0_DEFAULT 	 0x0EU

#define FSYNC_METH_FSYNC_FSYNC_0_ADDR 	 0x3E0U // Frame Synchronization Method  
#define FSYNC_METH_FSYNC_FSYNC_0_MASK 	 0x03U
#define FSYNC_METH_FSYNC_FSYNC_0_POS  	 0U

#define FSYNC_MODE_FSYNC_FSYNC_0_ADDR 	 0x3E0U // Frame Synchronization Mode  
#define FSYNC_MODE_FSYNC_FSYNC_0_MASK 	 0x0CU
#define FSYNC_MODE_FSYNC_FSYNC_0_POS  	 2U

#define EN_VS_GEN_FSYNC_FSYNC_0_ADDR 	 0x3E0U // When enabled, VS is generated internally... 
#define EN_VS_GEN_FSYNC_FSYNC_0_MASK 	 0x10U
#define EN_VS_GEN_FSYNC_FSYNC_0_POS  	 4U

#define FSYNC_OUT_PIN_FSYNC_FSYNC_0_ADDR 	 0x3E0U // Select pin to output frame sync signal  
#define FSYNC_OUT_PIN_FSYNC_FSYNC_0_MASK 	 0x20U
#define FSYNC_OUT_PIN_FSYNC_FSYNC_0_POS  	 5U

#define EN_OFLOW_RST_FS_FSYNC_FSYNC_0_ADDR 	 0x3E0U // When enabled, memory overflow resets fra... 
#define EN_OFLOW_RST_FS_FSYNC_FSYNC_0_MASK 	 0x80U
#define EN_OFLOW_RST_FS_FSYNC_FSYNC_0_POS  	 7U

#define FSYNC_FSYNC_1_ADDR    	 0x3E1U
#define FSYNC_FSYNC_1_DEFAULT 	 0x00U

#define FSYNC_PER_DIV_FSYNC_FSYNC_1_ADDR 	 0x3E1U // Frame sync transmission period in terms ... 
#define FSYNC_PER_DIV_FSYNC_FSYNC_1_MASK 	 0x0FU
#define FSYNC_PER_DIV_FSYNC_FSYNC_1_POS  	 0U

#define FSYNC_FSYNC_2_ADDR    	 0x3E2U
#define FSYNC_FSYNC_2_DEFAULT 	 0x81U

#define K_VAL_FSYNC_FSYNC_2_ADDR 	 0x3E2U // Desired frame sync margin (microseconds,... 
#define K_VAL_FSYNC_FSYNC_2_MASK 	 0x0FU
#define K_VAL_FSYNC_FSYNC_2_POS  	 0U

#define K_VAL_SIGN_FSYNC_FSYNC_2_ADDR 	 0x3E2U // Sign bit of K_VAL  
#define K_VAL_SIGN_FSYNC_FSYNC_2_MASK 	 0x10U
#define K_VAL_SIGN_FSYNC_FSYNC_2_POS  	 4U

#define MST_LINK_SEL_FSYNC_FSYNC_2_ADDR 	 0x3E2U // Main link select for frame sync generati... 
#define MST_LINK_SEL_FSYNC_FSYNC_2_MASK 	 0xE0U
#define MST_LINK_SEL_FSYNC_FSYNC_2_POS  	 5U

#define FSYNC_FSYNC_3_ADDR    	 0x3E3U
#define FSYNC_FSYNC_3_DEFAULT 	 0x00U

#define P_VAL_L_FSYNC_FSYNC_3_ADDR 	 0x3E3U // Low byte of desired frame sync margin in... 
#define P_VAL_L_FSYNC_FSYNC_3_MASK 	 0xFFU
#define P_VAL_L_FSYNC_FSYNC_3_POS  	 0U

#define FSYNC_FSYNC_4_ADDR    	 0x3E4U
#define FSYNC_FSYNC_4_DEFAULT 	 0x00U

#define P_VAL_H_FSYNC_FSYNC_4_ADDR 	 0x3E4U // High bits of desired frame sync margin i... 
#define P_VAL_H_FSYNC_FSYNC_4_MASK 	 0x1FU
#define P_VAL_H_FSYNC_FSYNC_4_POS  	 0U

#define P_VAL_SIGN_FSYNC_FSYNC_4_ADDR 	 0x3E4U // Sign bit of P_VAL  
#define P_VAL_SIGN_FSYNC_FSYNC_4_MASK 	 0x20U
#define P_VAL_SIGN_FSYNC_FSYNC_4_POS  	 5U

#define FSYNC_FSYNC_5_ADDR    	 0x3E5U
#define FSYNC_FSYNC_5_DEFAULT 	 0x00U

#define FSYNC_PERIOD_L_FSYNC_FSYNC_5_ADDR 	 0x3E5U // Low byte of frame sync period in terms o... 
#define FSYNC_PERIOD_L_FSYNC_FSYNC_5_MASK 	 0xFFU
#define FSYNC_PERIOD_L_FSYNC_FSYNC_5_POS  	 0U

#define FSYNC_FSYNC_6_ADDR    	 0x3E6U
#define FSYNC_FSYNC_6_DEFAULT 	 0x00U

#define FSYNC_PERIOD_M_FSYNC_FSYNC_6_ADDR 	 0x3E6U // Middle byte of frame sync period in term... 
#define FSYNC_PERIOD_M_FSYNC_FSYNC_6_MASK 	 0xFFU
#define FSYNC_PERIOD_M_FSYNC_FSYNC_6_POS  	 0U

#define FSYNC_FSYNC_7_ADDR    	 0x3E7U
#define FSYNC_FSYNC_7_DEFAULT 	 0x00U

#define FSYNC_PERIOD_H_FSYNC_FSYNC_7_ADDR 	 0x3E7U // High byte of frame sync period in terms ... 
#define FSYNC_PERIOD_H_FSYNC_FSYNC_7_MASK 	 0xFFU
#define FSYNC_PERIOD_H_FSYNC_FSYNC_7_POS  	 0U

#define FSYNC_FSYNC_8_ADDR    	 0x3E8U
#define FSYNC_FSYNC_8_DEFAULT 	 0x00U

#define FRM_DIFF_ERR_THR_L_FSYNC_FSYNC_8_ADDR 	 0x3E8U // Low byte of the error threshold for diff... 
#define FRM_DIFF_ERR_THR_L_FSYNC_FSYNC_8_MASK 	 0xFFU
#define FRM_DIFF_ERR_THR_L_FSYNC_FSYNC_8_POS  	 0U

#define FSYNC_FSYNC_9_ADDR    	 0x3E9U
#define FSYNC_FSYNC_9_DEFAULT 	 0x0FU

#define FRM_DIFF_ERR_THR_H_FSYNC_FSYNC_9_ADDR 	 0x3E9U // High bits of the error threshold for dif... 
#define FRM_DIFF_ERR_THR_H_FSYNC_FSYNC_9_MASK 	 0x1FU
#define FRM_DIFF_ERR_THR_H_FSYNC_FSYNC_9_POS  	 0U

#define FSYNC_FSYNC_10_ADDR    	 0x3EAU
#define FSYNC_FSYNC_10_DEFAULT 	 0x00U

#define OVLP_WINDOW_L_FSYNC_FSYNC_10_ADDR 	 0x3EAU // Low byte of the overlap window value in ... 
#define OVLP_WINDOW_L_FSYNC_FSYNC_10_MASK 	 0xFFU
#define OVLP_WINDOW_L_FSYNC_FSYNC_10_POS  	 0U

#define FSYNC_FSYNC_11_ADDR    	 0x3EBU
#define FSYNC_FSYNC_11_DEFAULT 	 0x00U

#define OVLP_WINDOW_H_FSYNC_FSYNC_11_ADDR 	 0x3EBU // Low byte of the overlap window value in ... 
#define OVLP_WINDOW_H_FSYNC_FSYNC_11_MASK 	 0x1FU
#define OVLP_WINDOW_H_FSYNC_FSYNC_11_POS  	 0U

#define EN_FSIN_LAST_FSYNC_FSYNC_11_ADDR 	 0x3EBU // FSIN position  
#define EN_FSIN_LAST_FSYNC_FSYNC_11_MASK 	 0x80U
#define EN_FSIN_LAST_FSYNC_FSYNC_11_POS  	 7U

#define FSYNC_FSYNC_15_ADDR    	 0x3EFU
#define FSYNC_FSYNC_15_DEFAULT 	 0x96U

#define FS_EN_Y_FSYNC_FSYNC_15_ADDR 	 0x3EFU // Include Video Pipe Y in frame-sync gener... 
#define FS_EN_Y_FSYNC_FSYNC_15_MASK 	 0x02U
#define FS_EN_Y_FSYNC_FSYNC_15_POS  	 1U

#define FS_EN_Z_FSYNC_FSYNC_15_ADDR 	 0x3EFU // Include Video Pipe Z in frame-sync gener... 
#define FS_EN_Z_FSYNC_FSYNC_15_MASK 	 0x04U
#define FS_EN_Z_FSYNC_FSYNC_15_POS  	 2U

#define AUTO_FS_LINKS_FSYNC_FSYNC_15_ADDR 	 0x3EFU // Select how links are selected for frame ... 
#define AUTO_FS_LINKS_FSYNC_FSYNC_15_MASK 	 0x10U
#define AUTO_FS_LINKS_FSYNC_FSYNC_15_POS  	 4U

#define FS_USE_XTAL_FSYNC_FSYNC_15_ADDR 	 0x3EFU // Use crystal oscillator clock for generat... 
#define FS_USE_XTAL_FSYNC_FSYNC_15_MASK 	 0x40U
#define FS_USE_XTAL_FSYNC_FSYNC_15_POS  	 6U

#define FS_GPIO_TYPE_FSYNC_FSYNC_15_ADDR 	 0x3EFU // Select the type of FSYNC signal to outpu... 
#define FS_GPIO_TYPE_FSYNC_FSYNC_15_MASK 	 0x80U
#define FS_GPIO_TYPE_FSYNC_FSYNC_15_POS  	 7U

#define FSYNC_FSYNC_16_ADDR    	 0x3F0U
#define FSYNC_FSYNC_16_DEFAULT 	 0x00U

#define FSYNC_ERR_CNT_FSYNC_FSYNC_16_ADDR 	 0x3F0U // Frame Sync Error Counter  
#define FSYNC_ERR_CNT_FSYNC_FSYNC_16_MASK 	 0xFFU
#define FSYNC_ERR_CNT_FSYNC_FSYNC_16_POS  	 0U

#define FSYNC_FSYNC_17_ADDR    	 0x3F1U
#define FSYNC_FSYNC_17_DEFAULT 	 0xF0U

#define FSYNC_ERR_THR_FSYNC_FSYNC_17_ADDR 	 0x3F1U // Frame Sync Error Threshold Reporting  
#define FSYNC_ERR_THR_FSYNC_FSYNC_17_MASK 	 0x07U
#define FSYNC_ERR_THR_FSYNC_FSYNC_17_POS  	 0U

#define FSYNC_TX_ID_FSYNC_FSYNC_17_ADDR 	 0x3F1U // GPIO ID used for transmitting FSYNC sign... 
#define FSYNC_TX_ID_FSYNC_FSYNC_17_MASK 	 0xF8U
#define FSYNC_TX_ID_FSYNC_FSYNC_17_POS  	 3U

#define FSYNC_FSYNC_18_ADDR    	 0x3F2U
#define FSYNC_FSYNC_18_DEFAULT 	 0x00U

#define CALC_FRM_LEN_L_FSYNC_FSYNC_18_ADDR 	 0x3F2U // Low byte of calculated VS period (number... 
#define CALC_FRM_LEN_L_FSYNC_FSYNC_18_MASK 	 0xFFU
#define CALC_FRM_LEN_L_FSYNC_FSYNC_18_POS  	 0U

#define FSYNC_FSYNC_19_ADDR    	 0x3F3U
#define FSYNC_FSYNC_19_DEFAULT 	 0x00U

#define CALC_FRM_LEN_M_FSYNC_FSYNC_19_ADDR 	 0x3F3U // Middle byte of calculated VS period (num... 
#define CALC_FRM_LEN_M_FSYNC_FSYNC_19_MASK 	 0xFFU
#define CALC_FRM_LEN_M_FSYNC_FSYNC_19_POS  	 0U

#define FSYNC_FSYNC_20_ADDR    	 0x3F4U
#define FSYNC_FSYNC_20_DEFAULT 	 0x00U

#define CALC_FRM_LEN_H_FSYNC_FSYNC_20_ADDR 	 0x3F4U // High byte of calculated VS period (numbe... 
#define CALC_FRM_LEN_H_FSYNC_FSYNC_20_MASK 	 0xFFU
#define CALC_FRM_LEN_H_FSYNC_FSYNC_20_POS  	 0U

#define FSYNC_FSYNC_21_ADDR    	 0x3F5U
#define FSYNC_FSYNC_21_DEFAULT 	 0x00U

#define FRM_DIFF_L_FSYNC_FSYNC_21_ADDR 	 0x3F5U // Low byte of the difference between the f... 
#define FRM_DIFF_L_FSYNC_FSYNC_21_MASK 	 0xFFU
#define FRM_DIFF_L_FSYNC_FSYNC_21_POS  	 0U

#define FSYNC_FSYNC_22_ADDR    	 0x3F6U
#define FSYNC_FSYNC_22_DEFAULT 	 0x00U

#define FRM_DIFF_H_FSYNC_FSYNC_22_ADDR 	 0x3F6U // High bits of the difference between the ... 
#define FRM_DIFF_H_FSYNC_FSYNC_22_MASK 	 0x3FU
#define FRM_DIFF_H_FSYNC_FSYNC_22_POS  	 0U

#define FSYNC_LOCKED_FSYNC_FSYNC_22_ADDR 	 0x3F6U // Frame Synchronization Lock Flag  
#define FSYNC_LOCKED_FSYNC_FSYNC_22_MASK 	 0x40U
#define FSYNC_LOCKED_FSYNC_FSYNC_22_POS  	 6U

#define FSYNC_LOSS_OF_LOCK_FSYNC_FSYNC_22_ADDR 	 0x3F6U // Frame Synchronization Lost Lock Flag  
#define FSYNC_LOSS_OF_LOCK_FSYNC_FSYNC_22_MASK 	 0x80U
#define FSYNC_LOSS_OF_LOCK_FSYNC_FSYNC_22_POS  	 7U

#define FSYNC_FSYNC_23_ADDR    	 0x3F7U
#define FSYNC_FSYNC_23_DEFAULT 	 0x00U

#define FSYNC_OVR_Y_FSYNC_FSYNC_23_ADDR 	 0x3F7U // Override of video_lock_1 during internal... 
#define FSYNC_OVR_Y_FSYNC_FSYNC_23_MASK 	 0x02U
#define FSYNC_OVR_Y_FSYNC_FSYNC_23_POS  	 1U

#define FSYNC_OVR_Z_FSYNC_FSYNC_23_ADDR 	 0x3F7U // Override of video_lock_1 during internal... 
#define FSYNC_OVR_Z_FSYNC_FSYNC_23_MASK 	 0x04U
#define FSYNC_OVR_Z_FSYNC_FSYNC_23_POS  	 2U

#define EN_LINK_RESET_FSYNC_FSYNC_23_ADDR 	 0x3F7U // Internal Frame Sync Reset Link Enable  
#define EN_LINK_RESET_FSYNC_FSYNC_23_MASK 	 0x40U
#define EN_LINK_RESET_FSYNC_FSYNC_23_POS  	 6U

#define EN_SYNC_COMP_FSYNC_FSYNC_23_ADDR 	 0x3F7U // Bypass FSYNC sync complete term in CSI_O... 
#define EN_SYNC_COMP_FSYNC_FSYNC_23_MASK 	 0x80U
#define EN_SYNC_COMP_FSYNC_FSYNC_23_POS  	 7U

#define MIPI_TX_0_MIPI_TX10_ADDR    	 0x40AU
#define MIPI_TX_0_MIPI_TX10_DEFAULT 	 0xD0U

#define CSI2_CPHY_EN_MIPI_TX_0_MIPI_TX10_ADDR 	 0x40AU // Enable CPHY  
#define CSI2_CPHY_EN_MIPI_TX_0_MIPI_TX10_MASK 	 0x20U
#define CSI2_CPHY_EN_MIPI_TX_0_MIPI_TX10_POS  	 5U

#define CSI2_LANE_CNT_MIPI_TX_0_MIPI_TX10_ADDR 	 0x40AU // Set number of MIPI data lanes  
#define CSI2_LANE_CNT_MIPI_TX_0_MIPI_TX10_MASK 	 0xC0U
#define CSI2_LANE_CNT_MIPI_TX_0_MIPI_TX10_POS  	 6U

#define MIPI_TX_1_MIPI_TX1_ADDR    	 0x441U
#define MIPI_TX_1_MIPI_TX1_DEFAULT 	 0x00U

#define MODE_MIPI_TX_1_MIPI_TX1_ADDR 	 0x441U // MIPI Tx Mode  
#define MODE_MIPI_TX_1_MIPI_TX1_MASK 	 0xFFU
#define MODE_MIPI_TX_1_MIPI_TX1_POS  	 0U

#define MIPI_TX_1_MIPI_TX2_ADDR    	 0x442U
#define MIPI_TX_1_MIPI_TX2_DEFAULT 	 0x00U

#define STATUS_MIPI_TX_1_MIPI_TX2_ADDR 	 0x442U // MIPI Tx Status  
#define STATUS_MIPI_TX_1_MIPI_TX2_MASK 	 0xFFU
#define STATUS_MIPI_TX_1_MIPI_TX2_POS  	 0U

#define MIPI_TX_1_MIPI_TX3_ADDR    	 0x443U
#define MIPI_TX_1_MIPI_TX3_DEFAULT 	 0x01U

#define DESKEW_INIT_MIPI_TX_1_MIPI_TX3_ADDR 	 0x443U // DPHY Deskew Initial Calibration Control  
#define DESKEW_INIT_MIPI_TX_1_MIPI_TX3_MASK 	 0xFFU
#define DESKEW_INIT_MIPI_TX_1_MIPI_TX3_POS  	 0U

#define MIPI_TX_1_MIPI_TX4_ADDR    	 0x444U
#define MIPI_TX_1_MIPI_TX4_DEFAULT 	 0x01U

#define DESKEW_PER_MIPI_TX_1_MIPI_TX4_ADDR 	 0x444U // DPHY Periodic Deskew Calibration Control... 
#define DESKEW_PER_MIPI_TX_1_MIPI_TX4_MASK 	 0xFFU
#define DESKEW_PER_MIPI_TX_1_MIPI_TX4_POS  	 0U

#define MIPI_TX_1_MIPI_TX7_ADDR    	 0x447U
#define MIPI_TX_1_MIPI_TX7_DEFAULT 	 0x1CU

#define CSI2_TX_GAP_MIPI_TX_1_MIPI_TX7_ADDR 	 0x447U // Number of MIPI byte clocks to wait after... 
#define CSI2_TX_GAP_MIPI_TX_1_MIPI_TX7_MASK 	 0xFFU
#define CSI2_TX_GAP_MIPI_TX_1_MIPI_TX7_POS  	 0U

#define MIPI_TX_1_MIPI_TX10_ADDR    	 0x44AU
#define MIPI_TX_1_MIPI_TX10_DEFAULT 	 0xD0U

#define CSI_VCX_EN_MIPI_TX_1_MIPI_TX10_ADDR 	 0x44AU // Enable VC Extension  
#define CSI_VCX_EN_MIPI_TX_1_MIPI_TX10_MASK 	 0x08U
#define CSI_VCX_EN_MIPI_TX_1_MIPI_TX10_POS  	 3U

#define CSI2_CPHY_EN_MIPI_TX_1_MIPI_TX10_ADDR 	 0x44AU // Enable CPHY  
#define CSI2_CPHY_EN_MIPI_TX_1_MIPI_TX10_MASK 	 0x20U
#define CSI2_CPHY_EN_MIPI_TX_1_MIPI_TX10_POS  	 5U

#define CSI2_LANE_CNT_MIPI_TX_1_MIPI_TX10_ADDR 	 0x44AU // Set number of MIPI data lanes  
#define CSI2_LANE_CNT_MIPI_TX_1_MIPI_TX10_MASK 	 0xC0U
#define CSI2_LANE_CNT_MIPI_TX_1_MIPI_TX10_POS  	 6U

#define MIPI_TX_1_MIPI_TX11_ADDR    	 0x44BU
#define MIPI_TX_1_MIPI_TX11_DEFAULT 	 0x00U

#define MAP_EN_L_MIPI_TX_1_MIPI_TX11_ADDR 	 0x44BU // Mapping Enable Low Byte [7:0]  
#define MAP_EN_L_MIPI_TX_1_MIPI_TX11_MASK 	 0xFFU
#define MAP_EN_L_MIPI_TX_1_MIPI_TX11_POS  	 0U

#define MIPI_TX_1_MIPI_TX12_ADDR    	 0x44CU
#define MIPI_TX_1_MIPI_TX12_DEFAULT 	 0x00U

#define MAP_EN_H_MIPI_TX_1_MIPI_TX12_ADDR 	 0x44CU // Mapping Enable High Byte [15:8]  
#define MAP_EN_H_MIPI_TX_1_MIPI_TX12_MASK 	 0xFFU
#define MAP_EN_H_MIPI_TX_1_MIPI_TX12_POS  	 0U

#define MIPI_TX_1_MIPI_TX13_ADDR    	 0x44DU
#define MIPI_TX_1_MIPI_TX13_DEFAULT 	 0x00U

#define MAP_SRC_0_MIPI_TX_1_MIPI_TX13_ADDR 	 0x44DU // Mapping Source Register 0  
#define MAP_SRC_0_MIPI_TX_1_MIPI_TX13_MASK 	 0xFFU
#define MAP_SRC_0_MIPI_TX_1_MIPI_TX13_POS  	 0U

#define MIPI_TX_1_MIPI_TX14_ADDR    	 0x44EU
#define MIPI_TX_1_MIPI_TX14_DEFAULT 	 0x00U

#define MAP_DST_0_MIPI_TX_1_MIPI_TX14_ADDR 	 0x44EU // Mapping Destination Register 0  
#define MAP_DST_0_MIPI_TX_1_MIPI_TX14_MASK 	 0xFFU
#define MAP_DST_0_MIPI_TX_1_MIPI_TX14_POS  	 0U

#define MIPI_TX_1_MIPI_TX15_ADDR    	 0x44FU
#define MIPI_TX_1_MIPI_TX15_DEFAULT 	 0x00U

#define MAP_SRC_1_MIPI_TX_1_MIPI_TX15_ADDR 	 0x44FU // Mapping Source Register 1  
#define MAP_SRC_1_MIPI_TX_1_MIPI_TX15_MASK 	 0xFFU
#define MAP_SRC_1_MIPI_TX_1_MIPI_TX15_POS  	 0U

#define MIPI_TX_1_MIPI_TX16_ADDR    	 0x450U
#define MIPI_TX_1_MIPI_TX16_DEFAULT 	 0x00U

#define MAP_DST_1_MIPI_TX_1_MIPI_TX16_ADDR 	 0x450U // Mapping Destination Register 1  
#define MAP_DST_1_MIPI_TX_1_MIPI_TX16_MASK 	 0xFFU
#define MAP_DST_1_MIPI_TX_1_MIPI_TX16_POS  	 0U

#define MIPI_TX_1_MIPI_TX17_ADDR    	 0x451U
#define MIPI_TX_1_MIPI_TX17_DEFAULT 	 0x00U

#define MAP_SRC_2_MIPI_TX_1_MIPI_TX17_ADDR 	 0x451U // Mapping Source Register 2  
#define MAP_SRC_2_MIPI_TX_1_MIPI_TX17_MASK 	 0xFFU
#define MAP_SRC_2_MIPI_TX_1_MIPI_TX17_POS  	 0U

#define MIPI_TX_1_MIPI_TX18_ADDR    	 0x452U
#define MIPI_TX_1_MIPI_TX18_DEFAULT 	 0x00U

#define MAP_DST_2_MIPI_TX_1_MIPI_TX18_ADDR 	 0x452U // Mapping Destination Register 2  
#define MAP_DST_2_MIPI_TX_1_MIPI_TX18_MASK 	 0xFFU
#define MAP_DST_2_MIPI_TX_1_MIPI_TX18_POS  	 0U

#define MIPI_TX_1_MIPI_TX19_ADDR    	 0x453U
#define MIPI_TX_1_MIPI_TX19_DEFAULT 	 0x00U

#define MAP_SRC_3_MIPI_TX_1_MIPI_TX19_ADDR 	 0x453U // Mapping Source Register 3  
#define MAP_SRC_3_MIPI_TX_1_MIPI_TX19_MASK 	 0xFFU
#define MAP_SRC_3_MIPI_TX_1_MIPI_TX19_POS  	 0U

#define MIPI_TX_1_MIPI_TX20_ADDR    	 0x454U
#define MIPI_TX_1_MIPI_TX20_DEFAULT 	 0x00U

#define MAP_DST_3_MIPI_TX_1_MIPI_TX20_ADDR 	 0x454U // Mapping Destination Register 3  
#define MAP_DST_3_MIPI_TX_1_MIPI_TX20_MASK 	 0xFFU
#define MAP_DST_3_MIPI_TX_1_MIPI_TX20_POS  	 0U

#define MIPI_TX_1_MIPI_TX21_ADDR    	 0x455U
#define MIPI_TX_1_MIPI_TX21_DEFAULT 	 0x00U

#define MAP_SRC_4_MIPI_TX_1_MIPI_TX21_ADDR 	 0x455U // Mapping Source Register 4  
#define MAP_SRC_4_MIPI_TX_1_MIPI_TX21_MASK 	 0xFFU
#define MAP_SRC_4_MIPI_TX_1_MIPI_TX21_POS  	 0U

#define MIPI_TX_1_MIPI_TX22_ADDR    	 0x456U
#define MIPI_TX_1_MIPI_TX22_DEFAULT 	 0x00U

#define MAP_DST_4_MIPI_TX_1_MIPI_TX22_ADDR 	 0x456U // Mapping Destination Register 4  
#define MAP_DST_4_MIPI_TX_1_MIPI_TX22_MASK 	 0xFFU
#define MAP_DST_4_MIPI_TX_1_MIPI_TX22_POS  	 0U

#define MIPI_TX_1_MIPI_TX23_ADDR    	 0x457U
#define MIPI_TX_1_MIPI_TX23_DEFAULT 	 0x00U

#define MAP_SRC_5_MIPI_TX_1_MIPI_TX23_ADDR 	 0x457U // Mapping Source Register 5  
#define MAP_SRC_5_MIPI_TX_1_MIPI_TX23_MASK 	 0xFFU
#define MAP_SRC_5_MIPI_TX_1_MIPI_TX23_POS  	 0U

#define MIPI_TX_1_MIPI_TX24_ADDR    	 0x458U
#define MIPI_TX_1_MIPI_TX24_DEFAULT 	 0x00U

#define MAP_DST_5_MIPI_TX_1_MIPI_TX24_ADDR 	 0x458U // Mapping Destination Register 5  
#define MAP_DST_5_MIPI_TX_1_MIPI_TX24_MASK 	 0xFFU
#define MAP_DST_5_MIPI_TX_1_MIPI_TX24_POS  	 0U

#define MIPI_TX_1_MIPI_TX25_ADDR    	 0x459U
#define MIPI_TX_1_MIPI_TX25_DEFAULT 	 0x00U

#define MAP_SRC_6_MIPI_TX_1_MIPI_TX25_ADDR 	 0x459U // Mapping Source Register 6  
#define MAP_SRC_6_MIPI_TX_1_MIPI_TX25_MASK 	 0xFFU
#define MAP_SRC_6_MIPI_TX_1_MIPI_TX25_POS  	 0U

#define MIPI_TX_1_MIPI_TX26_ADDR    	 0x45AU
#define MIPI_TX_1_MIPI_TX26_DEFAULT 	 0x00U

#define MAP_DST_6_MIPI_TX_1_MIPI_TX26_ADDR 	 0x45AU // Mapping Destination Register 6  
#define MAP_DST_6_MIPI_TX_1_MIPI_TX26_MASK 	 0xFFU
#define MAP_DST_6_MIPI_TX_1_MIPI_TX26_POS  	 0U

#define MIPI_TX_1_MIPI_TX27_ADDR    	 0x45BU
#define MIPI_TX_1_MIPI_TX27_DEFAULT 	 0x00U

#define MAP_SRC_7_MIPI_TX_1_MIPI_TX27_ADDR 	 0x45BU // Mapping Source Register 7  
#define MAP_SRC_7_MIPI_TX_1_MIPI_TX27_MASK 	 0xFFU
#define MAP_SRC_7_MIPI_TX_1_MIPI_TX27_POS  	 0U

#define MIPI_TX_1_MIPI_TX28_ADDR    	 0x45CU
#define MIPI_TX_1_MIPI_TX28_DEFAULT 	 0x00U

#define MAP_DST_7_MIPI_TX_1_MIPI_TX28_ADDR 	 0x45CU // Mapping Destination Register 7  
#define MAP_DST_7_MIPI_TX_1_MIPI_TX28_MASK 	 0xFFU
#define MAP_DST_7_MIPI_TX_1_MIPI_TX28_POS  	 0U

#define MIPI_TX_1_MIPI_TX29_ADDR    	 0x45DU
#define MIPI_TX_1_MIPI_TX29_DEFAULT 	 0x00U

#define MAP_SRC_8_MIPI_TX_1_MIPI_TX29_ADDR 	 0x45DU // Mapping Source Register 8  
#define MAP_SRC_8_MIPI_TX_1_MIPI_TX29_MASK 	 0xFFU
#define MAP_SRC_8_MIPI_TX_1_MIPI_TX29_POS  	 0U

#define MIPI_TX_1_MIPI_TX30_ADDR    	 0x45EU
#define MIPI_TX_1_MIPI_TX30_DEFAULT 	 0x00U

#define MAP_DST_8_MIPI_TX_1_MIPI_TX30_ADDR 	 0x45EU // Mapping Destination Register 8  
#define MAP_DST_8_MIPI_TX_1_MIPI_TX30_MASK 	 0xFFU
#define MAP_DST_8_MIPI_TX_1_MIPI_TX30_POS  	 0U

#define MIPI_TX_1_MIPI_TX31_ADDR    	 0x45FU
#define MIPI_TX_1_MIPI_TX31_DEFAULT 	 0x00U

#define MAP_SRC_9_MIPI_TX_1_MIPI_TX31_ADDR 	 0x45FU // Mapping Source Register 9  
#define MAP_SRC_9_MIPI_TX_1_MIPI_TX31_MASK 	 0xFFU
#define MAP_SRC_9_MIPI_TX_1_MIPI_TX31_POS  	 0U

#define MIPI_TX_1_MIPI_TX32_ADDR    	 0x460U
#define MIPI_TX_1_MIPI_TX32_DEFAULT 	 0x00U

#define MAP_DST_9_MIPI_TX_1_MIPI_TX32_ADDR 	 0x460U // Mapping Destination Register 9  
#define MAP_DST_9_MIPI_TX_1_MIPI_TX32_MASK 	 0xFFU
#define MAP_DST_9_MIPI_TX_1_MIPI_TX32_POS  	 0U

#define MIPI_TX_1_MIPI_TX33_ADDR    	 0x461U
#define MIPI_TX_1_MIPI_TX33_DEFAULT 	 0x00U

#define MAP_SRC_10_MIPI_TX_1_MIPI_TX33_ADDR 	 0x461U // Mapping Source Register 10  
#define MAP_SRC_10_MIPI_TX_1_MIPI_TX33_MASK 	 0xFFU
#define MAP_SRC_10_MIPI_TX_1_MIPI_TX33_POS  	 0U

#define MIPI_TX_1_MIPI_TX34_ADDR    	 0x462U
#define MIPI_TX_1_MIPI_TX34_DEFAULT 	 0x00U

#define MAP_DST_10_MIPI_TX_1_MIPI_TX34_ADDR 	 0x462U // Mapping Destination Register 10  
#define MAP_DST_10_MIPI_TX_1_MIPI_TX34_MASK 	 0xFFU
#define MAP_DST_10_MIPI_TX_1_MIPI_TX34_POS  	 0U

#define MIPI_TX_1_MIPI_TX35_ADDR    	 0x463U
#define MIPI_TX_1_MIPI_TX35_DEFAULT 	 0x00U

#define MAP_SRC_11_MIPI_TX_1_MIPI_TX35_ADDR 	 0x463U // Mapping Source Register 11  
#define MAP_SRC_11_MIPI_TX_1_MIPI_TX35_MASK 	 0xFFU
#define MAP_SRC_11_MIPI_TX_1_MIPI_TX35_POS  	 0U

#define MIPI_TX_1_MIPI_TX36_ADDR    	 0x464U
#define MIPI_TX_1_MIPI_TX36_DEFAULT 	 0x00U

#define MAP_DST_11_MIPI_TX_1_MIPI_TX36_ADDR 	 0x464U // Mapping Destination Register 11  
#define MAP_DST_11_MIPI_TX_1_MIPI_TX36_MASK 	 0xFFU
#define MAP_DST_11_MIPI_TX_1_MIPI_TX36_POS  	 0U

#define MIPI_TX_1_MIPI_TX37_ADDR    	 0x465U
#define MIPI_TX_1_MIPI_TX37_DEFAULT 	 0x00U

#define MAP_SRC_12_MIPI_TX_1_MIPI_TX37_ADDR 	 0x465U // Mapping Source Register 12  
#define MAP_SRC_12_MIPI_TX_1_MIPI_TX37_MASK 	 0xFFU
#define MAP_SRC_12_MIPI_TX_1_MIPI_TX37_POS  	 0U

#define MIPI_TX_1_MIPI_TX38_ADDR    	 0x466U
#define MIPI_TX_1_MIPI_TX38_DEFAULT 	 0x00U

#define MAP_DST_12_MIPI_TX_1_MIPI_TX38_ADDR 	 0x466U // Mapping Destination Register 12  
#define MAP_DST_12_MIPI_TX_1_MIPI_TX38_MASK 	 0xFFU
#define MAP_DST_12_MIPI_TX_1_MIPI_TX38_POS  	 0U

#define MIPI_TX_1_MIPI_TX39_ADDR    	 0x467U
#define MIPI_TX_1_MIPI_TX39_DEFAULT 	 0x00U

#define MAP_SRC_13_MIPI_TX_1_MIPI_TX39_ADDR 	 0x467U // Mapping Source Register 13  
#define MAP_SRC_13_MIPI_TX_1_MIPI_TX39_MASK 	 0xFFU
#define MAP_SRC_13_MIPI_TX_1_MIPI_TX39_POS  	 0U

#define MIPI_TX_1_MIPI_TX40_ADDR    	 0x468U
#define MIPI_TX_1_MIPI_TX40_DEFAULT 	 0x00U

#define MAP_DST_13_MIPI_TX_1_MIPI_TX40_ADDR 	 0x468U // Mapping Destination Register 13  
#define MAP_DST_13_MIPI_TX_1_MIPI_TX40_MASK 	 0xFFU
#define MAP_DST_13_MIPI_TX_1_MIPI_TX40_POS  	 0U

#define MIPI_TX_1_MIPI_TX41_ADDR    	 0x469U
#define MIPI_TX_1_MIPI_TX41_DEFAULT 	 0x00U

#define MAP_SRC_14_MIPI_TX_1_MIPI_TX41_ADDR 	 0x469U // Mapping Source Register 14  
#define MAP_SRC_14_MIPI_TX_1_MIPI_TX41_MASK 	 0xFFU
#define MAP_SRC_14_MIPI_TX_1_MIPI_TX41_POS  	 0U

#define MIPI_TX_1_MIPI_TX42_ADDR    	 0x46AU
#define MIPI_TX_1_MIPI_TX42_DEFAULT 	 0x00U

#define MAP_DST_14_MIPI_TX_1_MIPI_TX42_ADDR 	 0x46AU // Mapping Destination Register 14  
#define MAP_DST_14_MIPI_TX_1_MIPI_TX42_MASK 	 0xFFU
#define MAP_DST_14_MIPI_TX_1_MIPI_TX42_POS  	 0U

#define MIPI_TX_1_MIPI_TX43_ADDR    	 0x46BU
#define MIPI_TX_1_MIPI_TX43_DEFAULT 	 0x00U

#define MAP_SRC_15_MIPI_TX_1_MIPI_TX43_ADDR 	 0x46BU // Mapping Source Register 15  
#define MAP_SRC_15_MIPI_TX_1_MIPI_TX43_MASK 	 0xFFU
#define MAP_SRC_15_MIPI_TX_1_MIPI_TX43_POS  	 0U

#define MIPI_TX_1_MIPI_TX44_ADDR    	 0x46CU
#define MIPI_TX_1_MIPI_TX44_DEFAULT 	 0x00U

#define MAP_DST_15_MIPI_TX_1_MIPI_TX44_ADDR 	 0x46CU // Mapping Destination Register 15  
#define MAP_DST_15_MIPI_TX_1_MIPI_TX44_MASK 	 0xFFU
#define MAP_DST_15_MIPI_TX_1_MIPI_TX44_POS  	 0U

#define MIPI_TX_1_MIPI_TX45_ADDR    	 0x46DU
#define MIPI_TX_1_MIPI_TX45_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_0_MIPI_TX_1_MIPI_TX45_ADDR 	 0x46DU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_0_MIPI_TX_1_MIPI_TX45_MASK 	 0x03U
#define MAP_DPHY_DEST_0_MIPI_TX_1_MIPI_TX45_POS  	 0U

#define MAP_DPHY_DEST_1_MIPI_TX_1_MIPI_TX45_ADDR 	 0x46DU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_1_MIPI_TX_1_MIPI_TX45_MASK 	 0x0CU
#define MAP_DPHY_DEST_1_MIPI_TX_1_MIPI_TX45_POS  	 2U

#define MAP_DPHY_DEST_2_MIPI_TX_1_MIPI_TX45_ADDR 	 0x46DU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_2_MIPI_TX_1_MIPI_TX45_MASK 	 0x30U
#define MAP_DPHY_DEST_2_MIPI_TX_1_MIPI_TX45_POS  	 4U

#define MAP_DPHY_DEST_3_MIPI_TX_1_MIPI_TX45_ADDR 	 0x46DU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_3_MIPI_TX_1_MIPI_TX45_MASK 	 0xC0U
#define MAP_DPHY_DEST_3_MIPI_TX_1_MIPI_TX45_POS  	 6U

#define MIPI_TX_1_MIPI_TX46_ADDR    	 0x46EU
#define MIPI_TX_1_MIPI_TX46_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_4_MIPI_TX_1_MIPI_TX46_ADDR 	 0x46EU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_4_MIPI_TX_1_MIPI_TX46_MASK 	 0x03U
#define MAP_DPHY_DEST_4_MIPI_TX_1_MIPI_TX46_POS  	 0U

#define MAP_DPHY_DEST_5_MIPI_TX_1_MIPI_TX46_ADDR 	 0x46EU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_5_MIPI_TX_1_MIPI_TX46_MASK 	 0x0CU
#define MAP_DPHY_DEST_5_MIPI_TX_1_MIPI_TX46_POS  	 2U

#define MAP_DPHY_DEST_6_MIPI_TX_1_MIPI_TX46_ADDR 	 0x46EU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_6_MIPI_TX_1_MIPI_TX46_MASK 	 0x30U
#define MAP_DPHY_DEST_6_MIPI_TX_1_MIPI_TX46_POS  	 4U

#define MAP_DPHY_DEST_7_MIPI_TX_1_MIPI_TX46_ADDR 	 0x46EU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_7_MIPI_TX_1_MIPI_TX46_MASK 	 0xC0U
#define MAP_DPHY_DEST_7_MIPI_TX_1_MIPI_TX46_POS  	 6U

#define MIPI_TX_1_MIPI_TX47_ADDR    	 0x46FU
#define MIPI_TX_1_MIPI_TX47_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_8_MIPI_TX_1_MIPI_TX47_ADDR 	 0x46FU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_8_MIPI_TX_1_MIPI_TX47_MASK 	 0x03U
#define MAP_DPHY_DEST_8_MIPI_TX_1_MIPI_TX47_POS  	 0U

#define MAP_DPHY_DEST_9_MIPI_TX_1_MIPI_TX47_ADDR 	 0x46FU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_9_MIPI_TX_1_MIPI_TX47_MASK 	 0x0CU
#define MAP_DPHY_DEST_9_MIPI_TX_1_MIPI_TX47_POS  	 2U

#define MAP_DPHY_DEST_10_MIPI_TX_1_MIPI_TX47_ADDR 	 0x46FU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_10_MIPI_TX_1_MIPI_TX47_MASK 	 0x30U
#define MAP_DPHY_DEST_10_MIPI_TX_1_MIPI_TX47_POS  	 4U

#define MAP_DPHY_DEST_11_MIPI_TX_1_MIPI_TX47_ADDR 	 0x46FU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_11_MIPI_TX_1_MIPI_TX47_MASK 	 0xC0U
#define MAP_DPHY_DEST_11_MIPI_TX_1_MIPI_TX47_POS  	 6U

#define MIPI_TX_1_MIPI_TX48_ADDR    	 0x470U
#define MIPI_TX_1_MIPI_TX48_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_12_MIPI_TX_1_MIPI_TX48_ADDR 	 0x470U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_12_MIPI_TX_1_MIPI_TX48_MASK 	 0x03U
#define MAP_DPHY_DEST_12_MIPI_TX_1_MIPI_TX48_POS  	 0U

#define MAP_DPHY_DEST_13_MIPI_TX_1_MIPI_TX48_ADDR 	 0x470U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_13_MIPI_TX_1_MIPI_TX48_MASK 	 0x0CU
#define MAP_DPHY_DEST_13_MIPI_TX_1_MIPI_TX48_POS  	 2U

#define MAP_DPHY_DEST_14_MIPI_TX_1_MIPI_TX48_ADDR 	 0x470U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_14_MIPI_TX_1_MIPI_TX48_MASK 	 0x30U
#define MAP_DPHY_DEST_14_MIPI_TX_1_MIPI_TX48_POS  	 4U

#define MAP_DPHY_DEST_15_MIPI_TX_1_MIPI_TX48_ADDR 	 0x470U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_15_MIPI_TX_1_MIPI_TX48_MASK 	 0xC0U
#define MAP_DPHY_DEST_15_MIPI_TX_1_MIPI_TX48_POS  	 6U

#define MIPI_TX_1_MIPI_TX50_ADDR    	 0x472U
#define MIPI_TX_1_MIPI_TX50_DEFAULT 	 0x00U

#define SKEW_PER_SEL_MIPI_TX_1_MIPI_TX50_ADDR 	 0x472U // Periodic Deskew Select Register  
#define SKEW_PER_SEL_MIPI_TX_1_MIPI_TX50_MASK 	 0xFFU
#define SKEW_PER_SEL_MIPI_TX_1_MIPI_TX50_POS  	 0U

#define MIPI_TX_1_MIPI_TX51_ADDR    	 0x473U
#define MIPI_TX_1_MIPI_TX51_DEFAULT 	 0x00U

#define ALT_MEM_MAP12_MIPI_TX_1_MIPI_TX51_ADDR 	 0x473U // Alternative memory map enable for 12-bit... 
#define ALT_MEM_MAP12_MIPI_TX_1_MIPI_TX51_MASK 	 0x01U
#define ALT_MEM_MAP12_MIPI_TX_1_MIPI_TX51_POS  	 0U

#define ALT_MEM_MAP8_MIPI_TX_1_MIPI_TX51_ADDR 	 0x473U // Alternative memory map enable for 8-bit ... 
#define ALT_MEM_MAP8_MIPI_TX_1_MIPI_TX51_MASK 	 0x02U
#define ALT_MEM_MAP8_MIPI_TX_1_MIPI_TX51_POS  	 1U

#define ALT_MEM_MAP10_MIPI_TX_1_MIPI_TX51_ADDR 	 0x473U // Alternative memory map enable for 10-bit... 
#define ALT_MEM_MAP10_MIPI_TX_1_MIPI_TX51_MASK 	 0x04U
#define ALT_MEM_MAP10_MIPI_TX_1_MIPI_TX51_POS  	 2U

#define MODE_DT_MIPI_TX_1_MIPI_TX51_ADDR 	 0x473U // MIPI Tx enable 24-bit packing of 8-bit M... 
#define MODE_DT_MIPI_TX_1_MIPI_TX51_MASK 	 0x08U
#define MODE_DT_MIPI_TX_1_MIPI_TX51_POS  	 3U

#define ALT2_MEM_MAP8_MIPI_TX_1_MIPI_TX51_ADDR 	 0x473U // Alternative memory map enable for 8-bit ... 
#define ALT2_MEM_MAP8_MIPI_TX_1_MIPI_TX51_MASK 	 0x10U
#define ALT2_MEM_MAP8_MIPI_TX_1_MIPI_TX51_POS  	 4U

#define TUN_WAIT_VS_START_MIPI_TX_1_MIPI_TX51_ADDR 	 0x473U // Number of VS frames to wait before sendi... 
#define TUN_WAIT_VS_START_MIPI_TX_1_MIPI_TX51_MASK 	 0xE0U
#define TUN_WAIT_VS_START_MIPI_TX_1_MIPI_TX51_POS  	 5U

#define MIPI_TX_1_MIPI_TX52_ADDR    	 0x474U
#define MIPI_TX_1_MIPI_TX52_DEFAULT 	 0x08U

#define TUN_EN_MIPI_TX_1_MIPI_TX52_ADDR 	 0x474U // Tunneling Enable  
#define TUN_EN_MIPI_TX_1_MIPI_TX52_MASK 	 0x01U
#define TUN_EN_MIPI_TX_1_MIPI_TX52_POS  	 0U

#define TUN_DEST_MIPI_TX_1_MIPI_TX52_ADDR 	 0x474U // Tunneling Pipe Destination  
#define TUN_DEST_MIPI_TX_1_MIPI_TX52_MASK 	 0x02U
#define TUN_DEST_MIPI_TX_1_MIPI_TX52_POS  	 1U

#define DESKEW_TUN_SRC_MIPI_TX_1_MIPI_TX52_ADDR 	 0x474U // Tunneling Deskew Source Select  
#define DESKEW_TUN_SRC_MIPI_TX_1_MIPI_TX52_MASK 	 0x04U
#define DESKEW_TUN_SRC_MIPI_TX_1_MIPI_TX52_POS  	 2U

#define TUN_SER_LANE_NUM_MIPI_TX_1_MIPI_TX52_ADDR 	 0x474U // Number of lanes in the serializer. Appli... 
#define TUN_SER_LANE_NUM_MIPI_TX_1_MIPI_TX52_MASK 	 0x18U
#define TUN_SER_LANE_NUM_MIPI_TX_1_MIPI_TX52_POS  	 3U

#define DESKEW_TUN_MIPI_TX_1_MIPI_TX52_ADDR 	 0x474U // Deskew Mode for CSI2 Tunneling  
#define DESKEW_TUN_MIPI_TX_1_MIPI_TX52_MASK 	 0x60U
#define DESKEW_TUN_MIPI_TX_1_MIPI_TX52_POS  	 5U

#define TUN_NO_CORR_MIPI_TX_1_MIPI_TX52_ADDR 	 0x474U // Do not enable header error correction in... 
#define TUN_NO_CORR_MIPI_TX_1_MIPI_TX52_MASK 	 0x80U
#define TUN_NO_CORR_MIPI_TX_1_MIPI_TX52_POS  	 7U

#define MIPI_TX_1_MIPI_TX53_ADDR    	 0x475U
#define MIPI_TX_1_MIPI_TX53_DEFAULT 	 0x00U

#define DESKEW_TUN_OFFSET_MIPI_TX_1_MIPI_TX53_ADDR 	 0x475U // Tunneling Deskew Width Offset  
#define DESKEW_TUN_OFFSET_MIPI_TX_1_MIPI_TX53_MASK 	 0xFFU
#define DESKEW_TUN_OFFSET_MIPI_TX_1_MIPI_TX53_POS  	 0U

#define MIPI_TX_1_MIPI_TX54_ADDR    	 0x476U
#define MIPI_TX_1_MIPI_TX54_DEFAULT 	 0x00U

#define TUN_PKT_START_ADDR_MIPI_TX_1_MIPI_TX54_ADDR 	 0x476U // Specifies the start address of the long ... 
#define TUN_PKT_START_ADDR_MIPI_TX_1_MIPI_TX54_MASK 	 0xFFU
#define TUN_PKT_START_ADDR_MIPI_TX_1_MIPI_TX54_POS  	 0U

#define MIPI_TX_1_MIPI_TX55_ADDR    	 0x477U
#define MIPI_TX_1_MIPI_TX55_DEFAULT 	 0x00U

#define TUN_NO_CORR_LENGTH_MIPI_TX_1_MIPI_TX55_ADDR 	 0x477U // Do not enable header error packet length... 
#define TUN_NO_CORR_LENGTH_MIPI_TX_1_MIPI_TX55_MASK 	 0x01U
#define TUN_NO_CORR_LENGTH_MIPI_TX_1_MIPI_TX55_POS  	 0U

#define MIPI_TX_2_MIPI_TX1_ADDR    	 0x481U
#define MIPI_TX_2_MIPI_TX1_DEFAULT 	 0x00U

#define MODE_MIPI_TX_2_MIPI_TX1_ADDR 	 0x481U // MIPI Tx Mode  
#define MODE_MIPI_TX_2_MIPI_TX1_MASK 	 0xFFU
#define MODE_MIPI_TX_2_MIPI_TX1_POS  	 0U

#define MIPI_TX_2_MIPI_TX2_ADDR    	 0x482U
#define MIPI_TX_2_MIPI_TX2_DEFAULT 	 0x00U

#define STATUS_MIPI_TX_2_MIPI_TX2_ADDR 	 0x482U // MIPI Tx Status  
#define STATUS_MIPI_TX_2_MIPI_TX2_MASK 	 0xFFU
#define STATUS_MIPI_TX_2_MIPI_TX2_POS  	 0U

#define MIPI_TX_2_MIPI_TX3_ADDR    	 0x483U
#define MIPI_TX_2_MIPI_TX3_DEFAULT 	 0x01U

#define DESKEW_INIT_MIPI_TX_2_MIPI_TX3_ADDR 	 0x483U // DPHY Deskew Initial Calibration Control  
#define DESKEW_INIT_MIPI_TX_2_MIPI_TX3_MASK 	 0xFFU
#define DESKEW_INIT_MIPI_TX_2_MIPI_TX3_POS  	 0U

#define MIPI_TX_2_MIPI_TX4_ADDR    	 0x484U
#define MIPI_TX_2_MIPI_TX4_DEFAULT 	 0x01U

#define DESKEW_PER_MIPI_TX_2_MIPI_TX4_ADDR 	 0x484U // DPHY Periodic Deskew Calibration Control... 
#define DESKEW_PER_MIPI_TX_2_MIPI_TX4_MASK 	 0xFFU
#define DESKEW_PER_MIPI_TX_2_MIPI_TX4_POS  	 0U

#define MIPI_TX_2_MIPI_TX7_ADDR    	 0x487U
#define MIPI_TX_2_MIPI_TX7_DEFAULT 	 0x1CU

#define CSI2_TX_GAP_MIPI_TX_2_MIPI_TX7_ADDR 	 0x487U // Number of MIPI byte clocks to wait after... 
#define CSI2_TX_GAP_MIPI_TX_2_MIPI_TX7_MASK 	 0xFFU
#define CSI2_TX_GAP_MIPI_TX_2_MIPI_TX7_POS  	 0U

#define MIPI_TX_2_MIPI_TX10_ADDR    	 0x48AU
#define MIPI_TX_2_MIPI_TX10_DEFAULT 	 0xD0U

#define CSI_VCX_EN_MIPI_TX_2_MIPI_TX10_ADDR 	 0x48AU // Enable VC Extension  
#define CSI_VCX_EN_MIPI_TX_2_MIPI_TX10_MASK 	 0x08U
#define CSI_VCX_EN_MIPI_TX_2_MIPI_TX10_POS  	 3U

#define CSI2_CPHY_EN_MIPI_TX_2_MIPI_TX10_ADDR 	 0x48AU // Enable CPHY  
#define CSI2_CPHY_EN_MIPI_TX_2_MIPI_TX10_MASK 	 0x20U
#define CSI2_CPHY_EN_MIPI_TX_2_MIPI_TX10_POS  	 5U

#define CSI2_LANE_CNT_MIPI_TX_2_MIPI_TX10_ADDR 	 0x48AU // Set number of MIPI data lanes  
#define CSI2_LANE_CNT_MIPI_TX_2_MIPI_TX10_MASK 	 0xC0U
#define CSI2_LANE_CNT_MIPI_TX_2_MIPI_TX10_POS  	 6U

#define MIPI_TX_2_MIPI_TX11_ADDR    	 0x48BU
#define MIPI_TX_2_MIPI_TX11_DEFAULT 	 0x00U

#define MAP_EN_L_MIPI_TX_2_MIPI_TX11_ADDR 	 0x48BU // Mapping Enable Low Byte [7:0]  
#define MAP_EN_L_MIPI_TX_2_MIPI_TX11_MASK 	 0xFFU
#define MAP_EN_L_MIPI_TX_2_MIPI_TX11_POS  	 0U

#define MIPI_TX_2_MIPI_TX12_ADDR    	 0x48CU
#define MIPI_TX_2_MIPI_TX12_DEFAULT 	 0x00U

#define MAP_EN_H_MIPI_TX_2_MIPI_TX12_ADDR 	 0x48CU // Mapping Enable High Byte [15:8]  
#define MAP_EN_H_MIPI_TX_2_MIPI_TX12_MASK 	 0xFFU
#define MAP_EN_H_MIPI_TX_2_MIPI_TX12_POS  	 0U

#define MIPI_TX_2_MIPI_TX13_ADDR    	 0x48DU
#define MIPI_TX_2_MIPI_TX13_DEFAULT 	 0x00U

#define MAP_SRC_0_MIPI_TX_2_MIPI_TX13_ADDR 	 0x48DU // Mapping Source Register 0  
#define MAP_SRC_0_MIPI_TX_2_MIPI_TX13_MASK 	 0xFFU
#define MAP_SRC_0_MIPI_TX_2_MIPI_TX13_POS  	 0U

#define MIPI_TX_2_MIPI_TX14_ADDR    	 0x48EU
#define MIPI_TX_2_MIPI_TX14_DEFAULT 	 0x00U

#define MAP_DST_0_MIPI_TX_2_MIPI_TX14_ADDR 	 0x48EU // Mapping Destination Register 0  
#define MAP_DST_0_MIPI_TX_2_MIPI_TX14_MASK 	 0xFFU
#define MAP_DST_0_MIPI_TX_2_MIPI_TX14_POS  	 0U

#define MIPI_TX_2_MIPI_TX15_ADDR    	 0x48FU
#define MIPI_TX_2_MIPI_TX15_DEFAULT 	 0x00U

#define MAP_SRC_1_MIPI_TX_2_MIPI_TX15_ADDR 	 0x48FU // Mapping Source Register 1  
#define MAP_SRC_1_MIPI_TX_2_MIPI_TX15_MASK 	 0xFFU
#define MAP_SRC_1_MIPI_TX_2_MIPI_TX15_POS  	 0U

#define MIPI_TX_2_MIPI_TX16_ADDR    	 0x490U
#define MIPI_TX_2_MIPI_TX16_DEFAULT 	 0x00U

#define MAP_DST_1_MIPI_TX_2_MIPI_TX16_ADDR 	 0x490U // Mapping Destination Register 1  
#define MAP_DST_1_MIPI_TX_2_MIPI_TX16_MASK 	 0xFFU
#define MAP_DST_1_MIPI_TX_2_MIPI_TX16_POS  	 0U

#define MIPI_TX_2_MIPI_TX17_ADDR    	 0x491U
#define MIPI_TX_2_MIPI_TX17_DEFAULT 	 0x00U

#define MAP_SRC_2_MIPI_TX_2_MIPI_TX17_ADDR 	 0x491U // Mapping Source Register 2  
#define MAP_SRC_2_MIPI_TX_2_MIPI_TX17_MASK 	 0xFFU
#define MAP_SRC_2_MIPI_TX_2_MIPI_TX17_POS  	 0U

#define MIPI_TX_2_MIPI_TX18_ADDR    	 0x492U
#define MIPI_TX_2_MIPI_TX18_DEFAULT 	 0x00U

#define MAP_DST_2_MIPI_TX_2_MIPI_TX18_ADDR 	 0x492U // Mapping Destination Register 2  
#define MAP_DST_2_MIPI_TX_2_MIPI_TX18_MASK 	 0xFFU
#define MAP_DST_2_MIPI_TX_2_MIPI_TX18_POS  	 0U

#define MIPI_TX_2_MIPI_TX19_ADDR    	 0x493U
#define MIPI_TX_2_MIPI_TX19_DEFAULT 	 0x00U

#define MAP_SRC_3_MIPI_TX_2_MIPI_TX19_ADDR 	 0x493U // Mapping Source Register 3  
#define MAP_SRC_3_MIPI_TX_2_MIPI_TX19_MASK 	 0xFFU
#define MAP_SRC_3_MIPI_TX_2_MIPI_TX19_POS  	 0U

#define MIPI_TX_2_MIPI_TX20_ADDR    	 0x494U
#define MIPI_TX_2_MIPI_TX20_DEFAULT 	 0x00U

#define MAP_DST_3_MIPI_TX_2_MIPI_TX20_ADDR 	 0x494U // Mapping Destination Register 3  
#define MAP_DST_3_MIPI_TX_2_MIPI_TX20_MASK 	 0xFFU
#define MAP_DST_3_MIPI_TX_2_MIPI_TX20_POS  	 0U

#define MIPI_TX_2_MIPI_TX21_ADDR    	 0x495U
#define MIPI_TX_2_MIPI_TX21_DEFAULT 	 0x00U

#define MAP_SRC_4_MIPI_TX_2_MIPI_TX21_ADDR 	 0x495U // Mapping Source Register 4  
#define MAP_SRC_4_MIPI_TX_2_MIPI_TX21_MASK 	 0xFFU
#define MAP_SRC_4_MIPI_TX_2_MIPI_TX21_POS  	 0U

#define MIPI_TX_2_MIPI_TX22_ADDR    	 0x496U
#define MIPI_TX_2_MIPI_TX22_DEFAULT 	 0x00U

#define MAP_DST_4_MIPI_TX_2_MIPI_TX22_ADDR 	 0x496U // Mapping Destination Register 4  
#define MAP_DST_4_MIPI_TX_2_MIPI_TX22_MASK 	 0xFFU
#define MAP_DST_4_MIPI_TX_2_MIPI_TX22_POS  	 0U

#define MIPI_TX_2_MIPI_TX23_ADDR    	 0x497U
#define MIPI_TX_2_MIPI_TX23_DEFAULT 	 0x00U

#define MAP_SRC_5_MIPI_TX_2_MIPI_TX23_ADDR 	 0x497U // Mapping Source Register 5  
#define MAP_SRC_5_MIPI_TX_2_MIPI_TX23_MASK 	 0xFFU
#define MAP_SRC_5_MIPI_TX_2_MIPI_TX23_POS  	 0U

#define MIPI_TX_2_MIPI_TX24_ADDR    	 0x498U
#define MIPI_TX_2_MIPI_TX24_DEFAULT 	 0x00U

#define MAP_DST_5_MIPI_TX_2_MIPI_TX24_ADDR 	 0x498U // Mapping Destination Register 5  
#define MAP_DST_5_MIPI_TX_2_MIPI_TX24_MASK 	 0xFFU
#define MAP_DST_5_MIPI_TX_2_MIPI_TX24_POS  	 0U

#define MIPI_TX_2_MIPI_TX25_ADDR    	 0x499U
#define MIPI_TX_2_MIPI_TX25_DEFAULT 	 0x00U

#define MAP_SRC_6_MIPI_TX_2_MIPI_TX25_ADDR 	 0x499U // Mapping Source Register 6  
#define MAP_SRC_6_MIPI_TX_2_MIPI_TX25_MASK 	 0xFFU
#define MAP_SRC_6_MIPI_TX_2_MIPI_TX25_POS  	 0U

#define MIPI_TX_2_MIPI_TX26_ADDR    	 0x49AU
#define MIPI_TX_2_MIPI_TX26_DEFAULT 	 0x00U

#define MAP_DST_6_MIPI_TX_2_MIPI_TX26_ADDR 	 0x49AU // Mapping Destination Register 6  
#define MAP_DST_6_MIPI_TX_2_MIPI_TX26_MASK 	 0xFFU
#define MAP_DST_6_MIPI_TX_2_MIPI_TX26_POS  	 0U

#define MIPI_TX_2_MIPI_TX27_ADDR    	 0x49BU
#define MIPI_TX_2_MIPI_TX27_DEFAULT 	 0x00U

#define MAP_SRC_7_MIPI_TX_2_MIPI_TX27_ADDR 	 0x49BU // Mapping Source Register 7  
#define MAP_SRC_7_MIPI_TX_2_MIPI_TX27_MASK 	 0xFFU
#define MAP_SRC_7_MIPI_TX_2_MIPI_TX27_POS  	 0U

#define MIPI_TX_2_MIPI_TX28_ADDR    	 0x49CU
#define MIPI_TX_2_MIPI_TX28_DEFAULT 	 0x00U

#define MAP_DST_7_MIPI_TX_2_MIPI_TX28_ADDR 	 0x49CU // Mapping Destination Register 7  
#define MAP_DST_7_MIPI_TX_2_MIPI_TX28_MASK 	 0xFFU
#define MAP_DST_7_MIPI_TX_2_MIPI_TX28_POS  	 0U

#define MIPI_TX_2_MIPI_TX29_ADDR    	 0x49DU
#define MIPI_TX_2_MIPI_TX29_DEFAULT 	 0x00U

#define MAP_SRC_8_MIPI_TX_2_MIPI_TX29_ADDR 	 0x49DU // Mapping Source Register 8  
#define MAP_SRC_8_MIPI_TX_2_MIPI_TX29_MASK 	 0xFFU
#define MAP_SRC_8_MIPI_TX_2_MIPI_TX29_POS  	 0U

#define MIPI_TX_2_MIPI_TX30_ADDR    	 0x49EU
#define MIPI_TX_2_MIPI_TX30_DEFAULT 	 0x00U

#define MAP_DST_8_MIPI_TX_2_MIPI_TX30_ADDR 	 0x49EU // Mapping Destination Register 8  
#define MAP_DST_8_MIPI_TX_2_MIPI_TX30_MASK 	 0xFFU
#define MAP_DST_8_MIPI_TX_2_MIPI_TX30_POS  	 0U

#define MIPI_TX_2_MIPI_TX31_ADDR    	 0x49FU
#define MIPI_TX_2_MIPI_TX31_DEFAULT 	 0x00U

#define MAP_SRC_9_MIPI_TX_2_MIPI_TX31_ADDR 	 0x49FU // Mapping Source Register 9  
#define MAP_SRC_9_MIPI_TX_2_MIPI_TX31_MASK 	 0xFFU
#define MAP_SRC_9_MIPI_TX_2_MIPI_TX31_POS  	 0U

#define MIPI_TX_2_MIPI_TX32_ADDR    	 0x4A0U
#define MIPI_TX_2_MIPI_TX32_DEFAULT 	 0x00U

#define MAP_DST_9_MIPI_TX_2_MIPI_TX32_ADDR 	 0x4A0U // Mapping Destination Register 9  
#define MAP_DST_9_MIPI_TX_2_MIPI_TX32_MASK 	 0xFFU
#define MAP_DST_9_MIPI_TX_2_MIPI_TX32_POS  	 0U

#define MIPI_TX_2_MIPI_TX33_ADDR    	 0x4A1U
#define MIPI_TX_2_MIPI_TX33_DEFAULT 	 0x00U

#define MAP_SRC_10_MIPI_TX_2_MIPI_TX33_ADDR 	 0x4A1U // Mapping Source Register 10  
#define MAP_SRC_10_MIPI_TX_2_MIPI_TX33_MASK 	 0xFFU
#define MAP_SRC_10_MIPI_TX_2_MIPI_TX33_POS  	 0U

#define MIPI_TX_2_MIPI_TX34_ADDR    	 0x4A2U
#define MIPI_TX_2_MIPI_TX34_DEFAULT 	 0x00U

#define MAP_DST_10_MIPI_TX_2_MIPI_TX34_ADDR 	 0x4A2U // Mapping Destination Register 10  
#define MAP_DST_10_MIPI_TX_2_MIPI_TX34_MASK 	 0xFFU
#define MAP_DST_10_MIPI_TX_2_MIPI_TX34_POS  	 0U

#define MIPI_TX_2_MIPI_TX35_ADDR    	 0x4A3U
#define MIPI_TX_2_MIPI_TX35_DEFAULT 	 0x00U

#define MAP_SRC_11_MIPI_TX_2_MIPI_TX35_ADDR 	 0x4A3U // Mapping Source Register 11  
#define MAP_SRC_11_MIPI_TX_2_MIPI_TX35_MASK 	 0xFFU
#define MAP_SRC_11_MIPI_TX_2_MIPI_TX35_POS  	 0U

#define MIPI_TX_2_MIPI_TX36_ADDR    	 0x4A4U
#define MIPI_TX_2_MIPI_TX36_DEFAULT 	 0x00U

#define MAP_DST_11_MIPI_TX_2_MIPI_TX36_ADDR 	 0x4A4U // Mapping Destination Register 11  
#define MAP_DST_11_MIPI_TX_2_MIPI_TX36_MASK 	 0xFFU
#define MAP_DST_11_MIPI_TX_2_MIPI_TX36_POS  	 0U

#define MIPI_TX_2_MIPI_TX37_ADDR    	 0x4A5U
#define MIPI_TX_2_MIPI_TX37_DEFAULT 	 0x00U

#define MAP_SRC_12_MIPI_TX_2_MIPI_TX37_ADDR 	 0x4A5U // Mapping Source Register 12  
#define MAP_SRC_12_MIPI_TX_2_MIPI_TX37_MASK 	 0xFFU
#define MAP_SRC_12_MIPI_TX_2_MIPI_TX37_POS  	 0U

#define MIPI_TX_2_MIPI_TX38_ADDR    	 0x4A6U
#define MIPI_TX_2_MIPI_TX38_DEFAULT 	 0x00U

#define MAP_DST_12_MIPI_TX_2_MIPI_TX38_ADDR 	 0x4A6U // Mapping Destination Register 12  
#define MAP_DST_12_MIPI_TX_2_MIPI_TX38_MASK 	 0xFFU
#define MAP_DST_12_MIPI_TX_2_MIPI_TX38_POS  	 0U

#define MIPI_TX_2_MIPI_TX39_ADDR    	 0x4A7U
#define MIPI_TX_2_MIPI_TX39_DEFAULT 	 0x00U

#define MAP_SRC_13_MIPI_TX_2_MIPI_TX39_ADDR 	 0x4A7U // Mapping Source Register 13  
#define MAP_SRC_13_MIPI_TX_2_MIPI_TX39_MASK 	 0xFFU
#define MAP_SRC_13_MIPI_TX_2_MIPI_TX39_POS  	 0U

#define MIPI_TX_2_MIPI_TX40_ADDR    	 0x4A8U
#define MIPI_TX_2_MIPI_TX40_DEFAULT 	 0x00U

#define MAP_DST_13_MIPI_TX_2_MIPI_TX40_ADDR 	 0x4A8U // Mapping Destination Register 13  
#define MAP_DST_13_MIPI_TX_2_MIPI_TX40_MASK 	 0xFFU
#define MAP_DST_13_MIPI_TX_2_MIPI_TX40_POS  	 0U

#define MIPI_TX_2_MIPI_TX41_ADDR    	 0x4A9U
#define MIPI_TX_2_MIPI_TX41_DEFAULT 	 0x00U

#define MAP_SRC_14_MIPI_TX_2_MIPI_TX41_ADDR 	 0x4A9U // Mapping Source Register 14  
#define MAP_SRC_14_MIPI_TX_2_MIPI_TX41_MASK 	 0xFFU
#define MAP_SRC_14_MIPI_TX_2_MIPI_TX41_POS  	 0U

#define MIPI_TX_2_MIPI_TX42_ADDR    	 0x4AAU
#define MIPI_TX_2_MIPI_TX42_DEFAULT 	 0x00U

#define MAP_DST_14_MIPI_TX_2_MIPI_TX42_ADDR 	 0x4AAU // Mapping Destination Register 14  
#define MAP_DST_14_MIPI_TX_2_MIPI_TX42_MASK 	 0xFFU
#define MAP_DST_14_MIPI_TX_2_MIPI_TX42_POS  	 0U

#define MIPI_TX_2_MIPI_TX43_ADDR    	 0x4ABU
#define MIPI_TX_2_MIPI_TX43_DEFAULT 	 0x00U

#define MAP_SRC_15_MIPI_TX_2_MIPI_TX43_ADDR 	 0x4ABU // Mapping Source Register 15  
#define MAP_SRC_15_MIPI_TX_2_MIPI_TX43_MASK 	 0xFFU
#define MAP_SRC_15_MIPI_TX_2_MIPI_TX43_POS  	 0U

#define MIPI_TX_2_MIPI_TX44_ADDR    	 0x4ACU
#define MIPI_TX_2_MIPI_TX44_DEFAULT 	 0x00U

#define MAP_DST_15_MIPI_TX_2_MIPI_TX44_ADDR 	 0x4ACU // Mapping Destination Register 15  
#define MAP_DST_15_MIPI_TX_2_MIPI_TX44_MASK 	 0xFFU
#define MAP_DST_15_MIPI_TX_2_MIPI_TX44_POS  	 0U

#define MIPI_TX_2_MIPI_TX45_ADDR    	 0x4ADU
#define MIPI_TX_2_MIPI_TX45_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_0_MIPI_TX_2_MIPI_TX45_ADDR 	 0x4ADU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_0_MIPI_TX_2_MIPI_TX45_MASK 	 0x03U
#define MAP_DPHY_DEST_0_MIPI_TX_2_MIPI_TX45_POS  	 0U

#define MAP_DPHY_DEST_1_MIPI_TX_2_MIPI_TX45_ADDR 	 0x4ADU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_1_MIPI_TX_2_MIPI_TX45_MASK 	 0x0CU
#define MAP_DPHY_DEST_1_MIPI_TX_2_MIPI_TX45_POS  	 2U

#define MAP_DPHY_DEST_2_MIPI_TX_2_MIPI_TX45_ADDR 	 0x4ADU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_2_MIPI_TX_2_MIPI_TX45_MASK 	 0x30U
#define MAP_DPHY_DEST_2_MIPI_TX_2_MIPI_TX45_POS  	 4U

#define MAP_DPHY_DEST_3_MIPI_TX_2_MIPI_TX45_ADDR 	 0x4ADU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_3_MIPI_TX_2_MIPI_TX45_MASK 	 0xC0U
#define MAP_DPHY_DEST_3_MIPI_TX_2_MIPI_TX45_POS  	 6U

#define MIPI_TX_2_MIPI_TX46_ADDR    	 0x4AEU
#define MIPI_TX_2_MIPI_TX46_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_4_MIPI_TX_2_MIPI_TX46_ADDR 	 0x4AEU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_4_MIPI_TX_2_MIPI_TX46_MASK 	 0x03U
#define MAP_DPHY_DEST_4_MIPI_TX_2_MIPI_TX46_POS  	 0U

#define MAP_DPHY_DEST_5_MIPI_TX_2_MIPI_TX46_ADDR 	 0x4AEU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_5_MIPI_TX_2_MIPI_TX46_MASK 	 0x0CU
#define MAP_DPHY_DEST_5_MIPI_TX_2_MIPI_TX46_POS  	 2U

#define MAP_DPHY_DEST_6_MIPI_TX_2_MIPI_TX46_ADDR 	 0x4AEU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_6_MIPI_TX_2_MIPI_TX46_MASK 	 0x30U
#define MAP_DPHY_DEST_6_MIPI_TX_2_MIPI_TX46_POS  	 4U

#define MAP_DPHY_DEST_7_MIPI_TX_2_MIPI_TX46_ADDR 	 0x4AEU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_7_MIPI_TX_2_MIPI_TX46_MASK 	 0xC0U
#define MAP_DPHY_DEST_7_MIPI_TX_2_MIPI_TX46_POS  	 6U

#define MIPI_TX_2_MIPI_TX47_ADDR    	 0x4AFU
#define MIPI_TX_2_MIPI_TX47_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_8_MIPI_TX_2_MIPI_TX47_ADDR 	 0x4AFU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_8_MIPI_TX_2_MIPI_TX47_MASK 	 0x03U
#define MAP_DPHY_DEST_8_MIPI_TX_2_MIPI_TX47_POS  	 0U

#define MAP_DPHY_DEST_9_MIPI_TX_2_MIPI_TX47_ADDR 	 0x4AFU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_9_MIPI_TX_2_MIPI_TX47_MASK 	 0x0CU
#define MAP_DPHY_DEST_9_MIPI_TX_2_MIPI_TX47_POS  	 2U

#define MAP_DPHY_DEST_10_MIPI_TX_2_MIPI_TX47_ADDR 	 0x4AFU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_10_MIPI_TX_2_MIPI_TX47_MASK 	 0x30U
#define MAP_DPHY_DEST_10_MIPI_TX_2_MIPI_TX47_POS  	 4U

#define MAP_DPHY_DEST_11_MIPI_TX_2_MIPI_TX47_ADDR 	 0x4AFU // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_11_MIPI_TX_2_MIPI_TX47_MASK 	 0xC0U
#define MAP_DPHY_DEST_11_MIPI_TX_2_MIPI_TX47_POS  	 6U

#define MIPI_TX_2_MIPI_TX48_ADDR    	 0x4B0U
#define MIPI_TX_2_MIPI_TX48_DEFAULT 	 0x00U

#define MAP_DPHY_DEST_12_MIPI_TX_2_MIPI_TX48_ADDR 	 0x4B0U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_12_MIPI_TX_2_MIPI_TX48_MASK 	 0x03U
#define MAP_DPHY_DEST_12_MIPI_TX_2_MIPI_TX48_POS  	 0U

#define MAP_DPHY_DEST_13_MIPI_TX_2_MIPI_TX48_ADDR 	 0x4B0U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_13_MIPI_TX_2_MIPI_TX48_MASK 	 0x0CU
#define MAP_DPHY_DEST_13_MIPI_TX_2_MIPI_TX48_POS  	 2U

#define MAP_DPHY_DEST_14_MIPI_TX_2_MIPI_TX48_ADDR 	 0x4B0U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_14_MIPI_TX_2_MIPI_TX48_MASK 	 0x30U
#define MAP_DPHY_DEST_14_MIPI_TX_2_MIPI_TX48_POS  	 4U

#define MAP_DPHY_DEST_15_MIPI_TX_2_MIPI_TX48_ADDR 	 0x4B0U // CSI2 controller destination for MAP_SRC_... 
#define MAP_DPHY_DEST_15_MIPI_TX_2_MIPI_TX48_MASK 	 0xC0U
#define MAP_DPHY_DEST_15_MIPI_TX_2_MIPI_TX48_POS  	 6U

#define MIPI_TX_2_MIPI_TX50_ADDR    	 0x4B2U
#define MIPI_TX_2_MIPI_TX50_DEFAULT 	 0x00U

#define SKEW_PER_SEL_MIPI_TX_2_MIPI_TX50_ADDR 	 0x4B2U // Periodic Deskew Select Register  
#define SKEW_PER_SEL_MIPI_TX_2_MIPI_TX50_MASK 	 0xFFU
#define SKEW_PER_SEL_MIPI_TX_2_MIPI_TX50_POS  	 0U

#define MIPI_TX_2_MIPI_TX51_ADDR    	 0x4B3U
#define MIPI_TX_2_MIPI_TX51_DEFAULT 	 0x00U

#define ALT_MEM_MAP12_MIPI_TX_2_MIPI_TX51_ADDR 	 0x4B3U // Alternative memory map enable for 12-bit... 
#define ALT_MEM_MAP12_MIPI_TX_2_MIPI_TX51_MASK 	 0x01U
#define ALT_MEM_MAP12_MIPI_TX_2_MIPI_TX51_POS  	 0U

#define ALT_MEM_MAP8_MIPI_TX_2_MIPI_TX51_ADDR 	 0x4B3U // Alternative memory map enable for 8-bit ... 
#define ALT_MEM_MAP8_MIPI_TX_2_MIPI_TX51_MASK 	 0x02U
#define ALT_MEM_MAP8_MIPI_TX_2_MIPI_TX51_POS  	 1U

#define ALT_MEM_MAP10_MIPI_TX_2_MIPI_TX51_ADDR 	 0x4B3U // Alternative memory map enable for 10-bit... 
#define ALT_MEM_MAP10_MIPI_TX_2_MIPI_TX51_MASK 	 0x04U
#define ALT_MEM_MAP10_MIPI_TX_2_MIPI_TX51_POS  	 2U

#define MODE_DT_MIPI_TX_2_MIPI_TX51_ADDR 	 0x4B3U // MIPI Tx enable 24-bit packing of 8-bit M... 
#define MODE_DT_MIPI_TX_2_MIPI_TX51_MASK 	 0x08U
#define MODE_DT_MIPI_TX_2_MIPI_TX51_POS  	 3U

#define ALT2_MEM_MAP8_MIPI_TX_2_MIPI_TX51_ADDR 	 0x4B3U // Alternative memory map enable for 8-bit ... 
#define ALT2_MEM_MAP8_MIPI_TX_2_MIPI_TX51_MASK 	 0x10U
#define ALT2_MEM_MAP8_MIPI_TX_2_MIPI_TX51_POS  	 4U

#define TUN_WAIT_VS_START_MIPI_TX_2_MIPI_TX51_ADDR 	 0x4B3U // Number of VS frames to wait before sendi... 
#define TUN_WAIT_VS_START_MIPI_TX_2_MIPI_TX51_MASK 	 0xE0U
#define TUN_WAIT_VS_START_MIPI_TX_2_MIPI_TX51_POS  	 5U

#define MIPI_TX_2_MIPI_TX52_ADDR    	 0x4B4U
#define MIPI_TX_2_MIPI_TX52_DEFAULT 	 0x0EU

#define TUN_EN_MIPI_TX_2_MIPI_TX52_ADDR 	 0x4B4U // Tunneling Enabled  
#define TUN_EN_MIPI_TX_2_MIPI_TX52_MASK 	 0x01U
#define TUN_EN_MIPI_TX_2_MIPI_TX52_POS  	 0U

#define TUN_DEST_MIPI_TX_2_MIPI_TX52_ADDR 	 0x4B4U // Tunneling Pipe Destination  
#define TUN_DEST_MIPI_TX_2_MIPI_TX52_MASK 	 0x02U
#define TUN_DEST_MIPI_TX_2_MIPI_TX52_POS  	 1U

#define DESKEW_TUN_SRC_MIPI_TX_2_MIPI_TX52_ADDR 	 0x4B4U // Tunneling Deskew Source Select  
#define DESKEW_TUN_SRC_MIPI_TX_2_MIPI_TX52_MASK 	 0x04U
#define DESKEW_TUN_SRC_MIPI_TX_2_MIPI_TX52_POS  	 2U

#define TUN_SER_LANE_NUM_MIPI_TX_2_MIPI_TX52_ADDR 	 0x4B4U // Number of lanes in the serializer. Appli... 
#define TUN_SER_LANE_NUM_MIPI_TX_2_MIPI_TX52_MASK 	 0x18U
#define TUN_SER_LANE_NUM_MIPI_TX_2_MIPI_TX52_POS  	 3U

#define DESKEW_TUN_MIPI_TX_2_MIPI_TX52_ADDR 	 0x4B4U // Deskew Mode for CSI2 Tunneling  
#define DESKEW_TUN_MIPI_TX_2_MIPI_TX52_MASK 	 0x60U
#define DESKEW_TUN_MIPI_TX_2_MIPI_TX52_POS  	 5U

#define TUN_NO_CORR_MIPI_TX_2_MIPI_TX52_ADDR 	 0x4B4U // Do not enable header error correction in... 
#define TUN_NO_CORR_MIPI_TX_2_MIPI_TX52_MASK 	 0x80U
#define TUN_NO_CORR_MIPI_TX_2_MIPI_TX52_POS  	 7U

#define MIPI_TX_2_MIPI_TX53_ADDR    	 0x4B5U
#define MIPI_TX_2_MIPI_TX53_DEFAULT 	 0x00U

#define DESKEW_TUN_OFFSET_MIPI_TX_2_MIPI_TX53_ADDR 	 0x4B5U // Tunneling Deskew Width Offset  
#define DESKEW_TUN_OFFSET_MIPI_TX_2_MIPI_TX53_MASK 	 0xFFU
#define DESKEW_TUN_OFFSET_MIPI_TX_2_MIPI_TX53_POS  	 0U

#define MIPI_TX_2_MIPI_TX54_ADDR    	 0x4B6U
#define MIPI_TX_2_MIPI_TX54_DEFAULT 	 0x00U

#define TUN_PKT_START_ADDR_MIPI_TX_2_MIPI_TX54_ADDR 	 0x4B6U // Specifies the start address of the long ... 
#define TUN_PKT_START_ADDR_MIPI_TX_2_MIPI_TX54_MASK 	 0xFFU
#define TUN_PKT_START_ADDR_MIPI_TX_2_MIPI_TX54_POS  	 0U

#define MIPI_TX_2_MIPI_TX55_ADDR    	 0x4B7U
#define MIPI_TX_2_MIPI_TX55_DEFAULT 	 0x00U

#define TUN_NO_CORR_LENGTH_MIPI_TX_2_MIPI_TX55_ADDR 	 0x4B7U // Do not enable header error packet length... 
#define TUN_NO_CORR_LENGTH_MIPI_TX_2_MIPI_TX55_MASK 	 0x01U
#define TUN_NO_CORR_LENGTH_MIPI_TX_2_MIPI_TX55_POS  	 0U

#define MIPI_TX_3_MIPI_TX10_ADDR    	 0x4CAU
#define MIPI_TX_3_MIPI_TX10_DEFAULT 	 0xD0U

#define CSI2_CPHY_EN_MIPI_TX_3_MIPI_TX10_ADDR 	 0x4CAU // Enable CPHY  
#define CSI2_CPHY_EN_MIPI_TX_3_MIPI_TX10_MASK 	 0x20U
#define CSI2_CPHY_EN_MIPI_TX_3_MIPI_TX10_POS  	 5U

#define CSI2_LANE_CNT_MIPI_TX_3_MIPI_TX10_ADDR 	 0x4CAU // Set number of MIPI data lanes  
#define CSI2_LANE_CNT_MIPI_TX_3_MIPI_TX10_MASK 	 0xC0U
#define CSI2_LANE_CNT_MIPI_TX_3_MIPI_TX10_POS  	 6U

#define MIPI_TX_EXT_1_MIPI_TX_EXT0_ADDR    	 0x510U
#define MIPI_TX_EXT_1_MIPI_TX_EXT0_DEFAULT 	 0x00U

#define MAP_DST_0_H_MIPI_TX_EXT_1_MIPI_TX_EXT0_ADDR 	 0x510U // Mapping register destination VC high 3 b... 
#define MAP_DST_0_H_MIPI_TX_EXT_1_MIPI_TX_EXT0_MASK 	 0x1CU
#define MAP_DST_0_H_MIPI_TX_EXT_1_MIPI_TX_EXT0_POS  	 2U

#define MAP_SRC_0_H_MIPI_TX_EXT_1_MIPI_TX_EXT0_ADDR 	 0x510U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_0_H_MIPI_TX_EXT_1_MIPI_TX_EXT0_MASK 	 0xE0U
#define MAP_SRC_0_H_MIPI_TX_EXT_1_MIPI_TX_EXT0_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT1_ADDR    	 0x511U
#define MIPI_TX_EXT_1_MIPI_TX_EXT1_DEFAULT 	 0x00U

#define MAP_DST_1_H_MIPI_TX_EXT_1_MIPI_TX_EXT1_ADDR 	 0x511U // Mapping register destination VC high 3 b... 
#define MAP_DST_1_H_MIPI_TX_EXT_1_MIPI_TX_EXT1_MASK 	 0x1CU
#define MAP_DST_1_H_MIPI_TX_EXT_1_MIPI_TX_EXT1_POS  	 2U

#define MAP_SRC_1_H_MIPI_TX_EXT_1_MIPI_TX_EXT1_ADDR 	 0x511U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_1_H_MIPI_TX_EXT_1_MIPI_TX_EXT1_MASK 	 0xE0U
#define MAP_SRC_1_H_MIPI_TX_EXT_1_MIPI_TX_EXT1_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT2_ADDR    	 0x512U
#define MIPI_TX_EXT_1_MIPI_TX_EXT2_DEFAULT 	 0x00U

#define MAP_DST_2_H_MIPI_TX_EXT_1_MIPI_TX_EXT2_ADDR 	 0x512U // Mapping register destination VC high 3 b... 
#define MAP_DST_2_H_MIPI_TX_EXT_1_MIPI_TX_EXT2_MASK 	 0x1CU
#define MAP_DST_2_H_MIPI_TX_EXT_1_MIPI_TX_EXT2_POS  	 2U

#define MAP_SRC_2_H_MIPI_TX_EXT_1_MIPI_TX_EXT2_ADDR 	 0x512U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_2_H_MIPI_TX_EXT_1_MIPI_TX_EXT2_MASK 	 0xE0U
#define MAP_SRC_2_H_MIPI_TX_EXT_1_MIPI_TX_EXT2_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT3_ADDR    	 0x513U
#define MIPI_TX_EXT_1_MIPI_TX_EXT3_DEFAULT 	 0x00U

#define MAP_DST_3_H_MIPI_TX_EXT_1_MIPI_TX_EXT3_ADDR 	 0x513U // Mapping register destination VC high 3 b... 
#define MAP_DST_3_H_MIPI_TX_EXT_1_MIPI_TX_EXT3_MASK 	 0x1CU
#define MAP_DST_3_H_MIPI_TX_EXT_1_MIPI_TX_EXT3_POS  	 2U

#define MAP_SRC_3_H_MIPI_TX_EXT_1_MIPI_TX_EXT3_ADDR 	 0x513U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_3_H_MIPI_TX_EXT_1_MIPI_TX_EXT3_MASK 	 0xE0U
#define MAP_SRC_3_H_MIPI_TX_EXT_1_MIPI_TX_EXT3_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT4_ADDR    	 0x514U
#define MIPI_TX_EXT_1_MIPI_TX_EXT4_DEFAULT 	 0x00U

#define MAP_DST_4_H_MIPI_TX_EXT_1_MIPI_TX_EXT4_ADDR 	 0x514U // Mapping register destination VC high 3 b... 
#define MAP_DST_4_H_MIPI_TX_EXT_1_MIPI_TX_EXT4_MASK 	 0x1CU
#define MAP_DST_4_H_MIPI_TX_EXT_1_MIPI_TX_EXT4_POS  	 2U

#define MAP_SRC_4_H_MIPI_TX_EXT_1_MIPI_TX_EXT4_ADDR 	 0x514U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_4_H_MIPI_TX_EXT_1_MIPI_TX_EXT4_MASK 	 0xE0U
#define MAP_SRC_4_H_MIPI_TX_EXT_1_MIPI_TX_EXT4_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT5_ADDR    	 0x515U
#define MIPI_TX_EXT_1_MIPI_TX_EXT5_DEFAULT 	 0x00U

#define MAP_DST_5_H_MIPI_TX_EXT_1_MIPI_TX_EXT5_ADDR 	 0x515U // Mapping register destination VC high 3 b... 
#define MAP_DST_5_H_MIPI_TX_EXT_1_MIPI_TX_EXT5_MASK 	 0x1CU
#define MAP_DST_5_H_MIPI_TX_EXT_1_MIPI_TX_EXT5_POS  	 2U

#define MAP_SRC_5_H_MIPI_TX_EXT_1_MIPI_TX_EXT5_ADDR 	 0x515U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_5_H_MIPI_TX_EXT_1_MIPI_TX_EXT5_MASK 	 0xE0U
#define MAP_SRC_5_H_MIPI_TX_EXT_1_MIPI_TX_EXT5_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT6_ADDR    	 0x516U
#define MIPI_TX_EXT_1_MIPI_TX_EXT6_DEFAULT 	 0x00U

#define MAP_DST_6_H_MIPI_TX_EXT_1_MIPI_TX_EXT6_ADDR 	 0x516U // Mapping register destination VC high 3 b... 
#define MAP_DST_6_H_MIPI_TX_EXT_1_MIPI_TX_EXT6_MASK 	 0x1CU
#define MAP_DST_6_H_MIPI_TX_EXT_1_MIPI_TX_EXT6_POS  	 2U

#define MAP_SRC_6_H_MIPI_TX_EXT_1_MIPI_TX_EXT6_ADDR 	 0x516U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_6_H_MIPI_TX_EXT_1_MIPI_TX_EXT6_MASK 	 0xE0U
#define MAP_SRC_6_H_MIPI_TX_EXT_1_MIPI_TX_EXT6_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT7_ADDR    	 0x517U
#define MIPI_TX_EXT_1_MIPI_TX_EXT7_DEFAULT 	 0x00U

#define MAP_DST_7_H_MIPI_TX_EXT_1_MIPI_TX_EXT7_ADDR 	 0x517U // Mapping register destination VC high 3 b... 
#define MAP_DST_7_H_MIPI_TX_EXT_1_MIPI_TX_EXT7_MASK 	 0x1CU
#define MAP_DST_7_H_MIPI_TX_EXT_1_MIPI_TX_EXT7_POS  	 2U

#define MAP_SRC_7_H_MIPI_TX_EXT_1_MIPI_TX_EXT7_ADDR 	 0x517U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_7_H_MIPI_TX_EXT_1_MIPI_TX_EXT7_MASK 	 0xE0U
#define MAP_SRC_7_H_MIPI_TX_EXT_1_MIPI_TX_EXT7_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT8_ADDR    	 0x518U
#define MIPI_TX_EXT_1_MIPI_TX_EXT8_DEFAULT 	 0x00U

#define MAP_DST_8_H_MIPI_TX_EXT_1_MIPI_TX_EXT8_ADDR 	 0x518U // Mapping register destination VC high 3 b... 
#define MAP_DST_8_H_MIPI_TX_EXT_1_MIPI_TX_EXT8_MASK 	 0x1CU
#define MAP_DST_8_H_MIPI_TX_EXT_1_MIPI_TX_EXT8_POS  	 2U

#define MAP_SRC_8_H_MIPI_TX_EXT_1_MIPI_TX_EXT8_ADDR 	 0x518U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_8_H_MIPI_TX_EXT_1_MIPI_TX_EXT8_MASK 	 0xE0U
#define MAP_SRC_8_H_MIPI_TX_EXT_1_MIPI_TX_EXT8_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT9_ADDR    	 0x519U
#define MIPI_TX_EXT_1_MIPI_TX_EXT9_DEFAULT 	 0x00U

#define MAP_DST_9_H_MIPI_TX_EXT_1_MIPI_TX_EXT9_ADDR 	 0x519U // Mapping register destination VC high 3 b... 
#define MAP_DST_9_H_MIPI_TX_EXT_1_MIPI_TX_EXT9_MASK 	 0x1CU
#define MAP_DST_9_H_MIPI_TX_EXT_1_MIPI_TX_EXT9_POS  	 2U

#define MAP_SRC_9_H_MIPI_TX_EXT_1_MIPI_TX_EXT9_ADDR 	 0x519U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_9_H_MIPI_TX_EXT_1_MIPI_TX_EXT9_MASK 	 0xE0U
#define MAP_SRC_9_H_MIPI_TX_EXT_1_MIPI_TX_EXT9_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT10_ADDR    	 0x51AU
#define MIPI_TX_EXT_1_MIPI_TX_EXT10_DEFAULT 	 0x00U

#define MAP_DST_10_H_MIPI_TX_EXT_1_MIPI_TX_EXT10_ADDR 	 0x51AU // Mapping register destination VC high 3 b... 
#define MAP_DST_10_H_MIPI_TX_EXT_1_MIPI_TX_EXT10_MASK 	 0x1CU
#define MAP_DST_10_H_MIPI_TX_EXT_1_MIPI_TX_EXT10_POS  	 2U

#define MAP_SRC_10_H_MIPI_TX_EXT_1_MIPI_TX_EXT10_ADDR 	 0x51AU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_10_H_MIPI_TX_EXT_1_MIPI_TX_EXT10_MASK 	 0xE0U
#define MAP_SRC_10_H_MIPI_TX_EXT_1_MIPI_TX_EXT10_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT11_ADDR    	 0x51BU
#define MIPI_TX_EXT_1_MIPI_TX_EXT11_DEFAULT 	 0x00U

#define MAP_DST_11_H_MIPI_TX_EXT_1_MIPI_TX_EXT11_ADDR 	 0x51BU // Mapping register destination VC high 3 b... 
#define MAP_DST_11_H_MIPI_TX_EXT_1_MIPI_TX_EXT11_MASK 	 0x1CU
#define MAP_DST_11_H_MIPI_TX_EXT_1_MIPI_TX_EXT11_POS  	 2U

#define MAP_SRC_11_H_MIPI_TX_EXT_1_MIPI_TX_EXT11_ADDR 	 0x51BU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_11_H_MIPI_TX_EXT_1_MIPI_TX_EXT11_MASK 	 0xE0U
#define MAP_SRC_11_H_MIPI_TX_EXT_1_MIPI_TX_EXT11_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT12_ADDR    	 0x51CU
#define MIPI_TX_EXT_1_MIPI_TX_EXT12_DEFAULT 	 0x00U

#define MAP_DST_12_H_MIPI_TX_EXT_1_MIPI_TX_EXT12_ADDR 	 0x51CU // Mapping register destination VC high 3 b... 
#define MAP_DST_12_H_MIPI_TX_EXT_1_MIPI_TX_EXT12_MASK 	 0x1CU
#define MAP_DST_12_H_MIPI_TX_EXT_1_MIPI_TX_EXT12_POS  	 2U

#define MAP_SRC_12_H_MIPI_TX_EXT_1_MIPI_TX_EXT12_ADDR 	 0x51CU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_12_H_MIPI_TX_EXT_1_MIPI_TX_EXT12_MASK 	 0xE0U
#define MAP_SRC_12_H_MIPI_TX_EXT_1_MIPI_TX_EXT12_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT13_ADDR    	 0x51DU
#define MIPI_TX_EXT_1_MIPI_TX_EXT13_DEFAULT 	 0x00U

#define MAP_DST_13_H_MIPI_TX_EXT_1_MIPI_TX_EXT13_ADDR 	 0x51DU // Mapping register destination VC high 3 b... 
#define MAP_DST_13_H_MIPI_TX_EXT_1_MIPI_TX_EXT13_MASK 	 0x1CU
#define MAP_DST_13_H_MIPI_TX_EXT_1_MIPI_TX_EXT13_POS  	 2U

#define MAP_SRC_13_H_MIPI_TX_EXT_1_MIPI_TX_EXT13_ADDR 	 0x51DU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_13_H_MIPI_TX_EXT_1_MIPI_TX_EXT13_MASK 	 0xE0U
#define MAP_SRC_13_H_MIPI_TX_EXT_1_MIPI_TX_EXT13_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT14_ADDR    	 0x51EU
#define MIPI_TX_EXT_1_MIPI_TX_EXT14_DEFAULT 	 0x00U

#define MAP_DST_14_H_MIPI_TX_EXT_1_MIPI_TX_EXT14_ADDR 	 0x51EU // Mapping register destination VC high 3 b... 
#define MAP_DST_14_H_MIPI_TX_EXT_1_MIPI_TX_EXT14_MASK 	 0x1CU
#define MAP_DST_14_H_MIPI_TX_EXT_1_MIPI_TX_EXT14_POS  	 2U

#define MAP_SRC_14_H_MIPI_TX_EXT_1_MIPI_TX_EXT14_ADDR 	 0x51EU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_14_H_MIPI_TX_EXT_1_MIPI_TX_EXT14_MASK 	 0xE0U
#define MAP_SRC_14_H_MIPI_TX_EXT_1_MIPI_TX_EXT14_POS  	 5U

#define MIPI_TX_EXT_1_MIPI_TX_EXT15_ADDR    	 0x51FU
#define MIPI_TX_EXT_1_MIPI_TX_EXT15_DEFAULT 	 0x00U

#define MAP_DST_15_H_MIPI_TX_EXT_1_MIPI_TX_EXT15_ADDR 	 0x51FU // Mapping register destination VC high 3 b... 
#define MAP_DST_15_H_MIPI_TX_EXT_1_MIPI_TX_EXT15_MASK 	 0x1CU
#define MAP_DST_15_H_MIPI_TX_EXT_1_MIPI_TX_EXT15_POS  	 2U

#define MAP_SRC_15_H_MIPI_TX_EXT_1_MIPI_TX_EXT15_ADDR 	 0x51FU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_15_H_MIPI_TX_EXT_1_MIPI_TX_EXT15_MASK 	 0xE0U
#define MAP_SRC_15_H_MIPI_TX_EXT_1_MIPI_TX_EXT15_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT0_ADDR    	 0x520U
#define MIPI_TX_EXT_2_MIPI_TX_EXT0_DEFAULT 	 0x00U

#define MAP_DST_0_H_MIPI_TX_EXT_2_MIPI_TX_EXT0_ADDR 	 0x520U // Mapping register destination VC high 3 b... 
#define MAP_DST_0_H_MIPI_TX_EXT_2_MIPI_TX_EXT0_MASK 	 0x1CU
#define MAP_DST_0_H_MIPI_TX_EXT_2_MIPI_TX_EXT0_POS  	 2U

#define MAP_SRC_0_H_MIPI_TX_EXT_2_MIPI_TX_EXT0_ADDR 	 0x520U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_0_H_MIPI_TX_EXT_2_MIPI_TX_EXT0_MASK 	 0xE0U
#define MAP_SRC_0_H_MIPI_TX_EXT_2_MIPI_TX_EXT0_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT1_ADDR    	 0x521U
#define MIPI_TX_EXT_2_MIPI_TX_EXT1_DEFAULT 	 0x00U

#define MAP_DST_1_H_MIPI_TX_EXT_2_MIPI_TX_EXT1_ADDR 	 0x521U // Mapping register destination VC high 3 b... 
#define MAP_DST_1_H_MIPI_TX_EXT_2_MIPI_TX_EXT1_MASK 	 0x1CU
#define MAP_DST_1_H_MIPI_TX_EXT_2_MIPI_TX_EXT1_POS  	 2U

#define MAP_SRC_1_H_MIPI_TX_EXT_2_MIPI_TX_EXT1_ADDR 	 0x521U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_1_H_MIPI_TX_EXT_2_MIPI_TX_EXT1_MASK 	 0xE0U
#define MAP_SRC_1_H_MIPI_TX_EXT_2_MIPI_TX_EXT1_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT2_ADDR    	 0x522U
#define MIPI_TX_EXT_2_MIPI_TX_EXT2_DEFAULT 	 0x00U

#define MAP_DST_2_H_MIPI_TX_EXT_2_MIPI_TX_EXT2_ADDR 	 0x522U // Mapping register destination VC high 3 b... 
#define MAP_DST_2_H_MIPI_TX_EXT_2_MIPI_TX_EXT2_MASK 	 0x1CU
#define MAP_DST_2_H_MIPI_TX_EXT_2_MIPI_TX_EXT2_POS  	 2U

#define MAP_SRC_2_H_MIPI_TX_EXT_2_MIPI_TX_EXT2_ADDR 	 0x522U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_2_H_MIPI_TX_EXT_2_MIPI_TX_EXT2_MASK 	 0xE0U
#define MAP_SRC_2_H_MIPI_TX_EXT_2_MIPI_TX_EXT2_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT3_ADDR    	 0x523U
#define MIPI_TX_EXT_2_MIPI_TX_EXT3_DEFAULT 	 0x00U

#define MAP_DST_3_H_MIPI_TX_EXT_2_MIPI_TX_EXT3_ADDR 	 0x523U // Mapping register destination VC high 3 b... 
#define MAP_DST_3_H_MIPI_TX_EXT_2_MIPI_TX_EXT3_MASK 	 0x1CU
#define MAP_DST_3_H_MIPI_TX_EXT_2_MIPI_TX_EXT3_POS  	 2U

#define MAP_SRC_3_H_MIPI_TX_EXT_2_MIPI_TX_EXT3_ADDR 	 0x523U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_3_H_MIPI_TX_EXT_2_MIPI_TX_EXT3_MASK 	 0xE0U
#define MAP_SRC_3_H_MIPI_TX_EXT_2_MIPI_TX_EXT3_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT4_ADDR    	 0x524U
#define MIPI_TX_EXT_2_MIPI_TX_EXT4_DEFAULT 	 0x00U

#define MAP_DST_4_H_MIPI_TX_EXT_2_MIPI_TX_EXT4_ADDR 	 0x524U // Mapping register destination VC high 3 b... 
#define MAP_DST_4_H_MIPI_TX_EXT_2_MIPI_TX_EXT4_MASK 	 0x1CU
#define MAP_DST_4_H_MIPI_TX_EXT_2_MIPI_TX_EXT4_POS  	 2U

#define MAP_SRC_4_H_MIPI_TX_EXT_2_MIPI_TX_EXT4_ADDR 	 0x524U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_4_H_MIPI_TX_EXT_2_MIPI_TX_EXT4_MASK 	 0xE0U
#define MAP_SRC_4_H_MIPI_TX_EXT_2_MIPI_TX_EXT4_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT5_ADDR    	 0x525U
#define MIPI_TX_EXT_2_MIPI_TX_EXT5_DEFAULT 	 0x00U

#define MAP_DST_5_H_MIPI_TX_EXT_2_MIPI_TX_EXT5_ADDR 	 0x525U // Mapping register destination VC high 3 b... 
#define MAP_DST_5_H_MIPI_TX_EXT_2_MIPI_TX_EXT5_MASK 	 0x1CU
#define MAP_DST_5_H_MIPI_TX_EXT_2_MIPI_TX_EXT5_POS  	 2U

#define MAP_SRC_5_H_MIPI_TX_EXT_2_MIPI_TX_EXT5_ADDR 	 0x525U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_5_H_MIPI_TX_EXT_2_MIPI_TX_EXT5_MASK 	 0xE0U
#define MAP_SRC_5_H_MIPI_TX_EXT_2_MIPI_TX_EXT5_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT6_ADDR    	 0x526U
#define MIPI_TX_EXT_2_MIPI_TX_EXT6_DEFAULT 	 0x00U

#define MAP_DST_6_H_MIPI_TX_EXT_2_MIPI_TX_EXT6_ADDR 	 0x526U // Mapping register destination VC high 3 b... 
#define MAP_DST_6_H_MIPI_TX_EXT_2_MIPI_TX_EXT6_MASK 	 0x1CU
#define MAP_DST_6_H_MIPI_TX_EXT_2_MIPI_TX_EXT6_POS  	 2U

#define MAP_SRC_6_H_MIPI_TX_EXT_2_MIPI_TX_EXT6_ADDR 	 0x526U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_6_H_MIPI_TX_EXT_2_MIPI_TX_EXT6_MASK 	 0xE0U
#define MAP_SRC_6_H_MIPI_TX_EXT_2_MIPI_TX_EXT6_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT7_ADDR    	 0x527U
#define MIPI_TX_EXT_2_MIPI_TX_EXT7_DEFAULT 	 0x00U

#define MAP_DST_7_H_MIPI_TX_EXT_2_MIPI_TX_EXT7_ADDR 	 0x527U // Mapping register destination VC high 3 b... 
#define MAP_DST_7_H_MIPI_TX_EXT_2_MIPI_TX_EXT7_MASK 	 0x1CU
#define MAP_DST_7_H_MIPI_TX_EXT_2_MIPI_TX_EXT7_POS  	 2U

#define MAP_SRC_7_H_MIPI_TX_EXT_2_MIPI_TX_EXT7_ADDR 	 0x527U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_7_H_MIPI_TX_EXT_2_MIPI_TX_EXT7_MASK 	 0xE0U
#define MAP_SRC_7_H_MIPI_TX_EXT_2_MIPI_TX_EXT7_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT8_ADDR    	 0x528U
#define MIPI_TX_EXT_2_MIPI_TX_EXT8_DEFAULT 	 0x00U

#define MAP_DST_8_H_MIPI_TX_EXT_2_MIPI_TX_EXT8_ADDR 	 0x528U // Mapping register destination VC high 3 b... 
#define MAP_DST_8_H_MIPI_TX_EXT_2_MIPI_TX_EXT8_MASK 	 0x1CU
#define MAP_DST_8_H_MIPI_TX_EXT_2_MIPI_TX_EXT8_POS  	 2U

#define MAP_SRC_8_H_MIPI_TX_EXT_2_MIPI_TX_EXT8_ADDR 	 0x528U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_8_H_MIPI_TX_EXT_2_MIPI_TX_EXT8_MASK 	 0xE0U
#define MAP_SRC_8_H_MIPI_TX_EXT_2_MIPI_TX_EXT8_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT9_ADDR    	 0x529U
#define MIPI_TX_EXT_2_MIPI_TX_EXT9_DEFAULT 	 0x00U

#define MAP_DST_9_H_MIPI_TX_EXT_2_MIPI_TX_EXT9_ADDR 	 0x529U // Mapping register destination VC high 3 b... 
#define MAP_DST_9_H_MIPI_TX_EXT_2_MIPI_TX_EXT9_MASK 	 0x1CU
#define MAP_DST_9_H_MIPI_TX_EXT_2_MIPI_TX_EXT9_POS  	 2U

#define MAP_SRC_9_H_MIPI_TX_EXT_2_MIPI_TX_EXT9_ADDR 	 0x529U // Mapping register source VC high 3 bits f... 
#define MAP_SRC_9_H_MIPI_TX_EXT_2_MIPI_TX_EXT9_MASK 	 0xE0U
#define MAP_SRC_9_H_MIPI_TX_EXT_2_MIPI_TX_EXT9_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT10_ADDR    	 0x52AU
#define MIPI_TX_EXT_2_MIPI_TX_EXT10_DEFAULT 	 0x00U

#define MAP_DST_10_H_MIPI_TX_EXT_2_MIPI_TX_EXT10_ADDR 	 0x52AU // Mapping register destination VC high 3 b... 
#define MAP_DST_10_H_MIPI_TX_EXT_2_MIPI_TX_EXT10_MASK 	 0x1CU
#define MAP_DST_10_H_MIPI_TX_EXT_2_MIPI_TX_EXT10_POS  	 2U

#define MAP_SRC_10_H_MIPI_TX_EXT_2_MIPI_TX_EXT10_ADDR 	 0x52AU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_10_H_MIPI_TX_EXT_2_MIPI_TX_EXT10_MASK 	 0xE0U
#define MAP_SRC_10_H_MIPI_TX_EXT_2_MIPI_TX_EXT10_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT11_ADDR    	 0x52BU
#define MIPI_TX_EXT_2_MIPI_TX_EXT11_DEFAULT 	 0x00U

#define MAP_DST_11_H_MIPI_TX_EXT_2_MIPI_TX_EXT11_ADDR 	 0x52BU // Mapping register destination VC high 3 b... 
#define MAP_DST_11_H_MIPI_TX_EXT_2_MIPI_TX_EXT11_MASK 	 0x1CU
#define MAP_DST_11_H_MIPI_TX_EXT_2_MIPI_TX_EXT11_POS  	 2U

#define MAP_SRC_11_H_MIPI_TX_EXT_2_MIPI_TX_EXT11_ADDR 	 0x52BU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_11_H_MIPI_TX_EXT_2_MIPI_TX_EXT11_MASK 	 0xE0U
#define MAP_SRC_11_H_MIPI_TX_EXT_2_MIPI_TX_EXT11_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT12_ADDR    	 0x52CU
#define MIPI_TX_EXT_2_MIPI_TX_EXT12_DEFAULT 	 0x00U

#define MAP_DST_12_H_MIPI_TX_EXT_2_MIPI_TX_EXT12_ADDR 	 0x52CU // Mapping register destination VC high 3 b... 
#define MAP_DST_12_H_MIPI_TX_EXT_2_MIPI_TX_EXT12_MASK 	 0x1CU
#define MAP_DST_12_H_MIPI_TX_EXT_2_MIPI_TX_EXT12_POS  	 2U

#define MAP_SRC_12_H_MIPI_TX_EXT_2_MIPI_TX_EXT12_ADDR 	 0x52CU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_12_H_MIPI_TX_EXT_2_MIPI_TX_EXT12_MASK 	 0xE0U
#define MAP_SRC_12_H_MIPI_TX_EXT_2_MIPI_TX_EXT12_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT13_ADDR    	 0x52DU
#define MIPI_TX_EXT_2_MIPI_TX_EXT13_DEFAULT 	 0x00U

#define MAP_DST_13_H_MIPI_TX_EXT_2_MIPI_TX_EXT13_ADDR 	 0x52DU // Mapping register destination VC high 3 b... 
#define MAP_DST_13_H_MIPI_TX_EXT_2_MIPI_TX_EXT13_MASK 	 0x1CU
#define MAP_DST_13_H_MIPI_TX_EXT_2_MIPI_TX_EXT13_POS  	 2U

#define MAP_SRC_13_H_MIPI_TX_EXT_2_MIPI_TX_EXT13_ADDR 	 0x52DU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_13_H_MIPI_TX_EXT_2_MIPI_TX_EXT13_MASK 	 0xE0U
#define MAP_SRC_13_H_MIPI_TX_EXT_2_MIPI_TX_EXT13_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT14_ADDR    	 0x52EU
#define MIPI_TX_EXT_2_MIPI_TX_EXT14_DEFAULT 	 0x00U

#define MAP_DST_14_H_MIPI_TX_EXT_2_MIPI_TX_EXT14_ADDR 	 0x52EU // Mapping register destination VC high 3 b... 
#define MAP_DST_14_H_MIPI_TX_EXT_2_MIPI_TX_EXT14_MASK 	 0x1CU
#define MAP_DST_14_H_MIPI_TX_EXT_2_MIPI_TX_EXT14_POS  	 2U

#define MAP_SRC_14_H_MIPI_TX_EXT_2_MIPI_TX_EXT14_ADDR 	 0x52EU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_14_H_MIPI_TX_EXT_2_MIPI_TX_EXT14_MASK 	 0xE0U
#define MAP_SRC_14_H_MIPI_TX_EXT_2_MIPI_TX_EXT14_POS  	 5U

#define MIPI_TX_EXT_2_MIPI_TX_EXT15_ADDR    	 0x52FU
#define MIPI_TX_EXT_2_MIPI_TX_EXT15_DEFAULT 	 0x00U

#define MAP_DST_15_H_MIPI_TX_EXT_2_MIPI_TX_EXT15_ADDR 	 0x52FU // Mapping register destination VC high 3 b... 
#define MAP_DST_15_H_MIPI_TX_EXT_2_MIPI_TX_EXT15_MASK 	 0x1CU
#define MAP_DST_15_H_MIPI_TX_EXT_2_MIPI_TX_EXT15_POS  	 2U

#define MAP_SRC_15_H_MIPI_TX_EXT_2_MIPI_TX_EXT15_ADDR 	 0x52FU // Mapping register source VC high 3 bits f... 
#define MAP_SRC_15_H_MIPI_TX_EXT_2_MIPI_TX_EXT15_MASK 	 0xE0U
#define MAP_SRC_15_H_MIPI_TX_EXT_2_MIPI_TX_EXT15_POS  	 5U

#define MISC_CFG_0_ADDR    	 0x540U
#define MISC_CFG_0_DEFAULT 	 0x00U

#define VS_OUT1_MISC_CFG_0_ADDR 	 0x540U // Output VSYNC from VS1 pin  
#define VS_OUT1_MISC_CFG_0_MASK 	 0xE0U
#define VS_OUT1_MISC_CFG_0_POS  	 5U

#define MISC_CFG_1_ADDR    	 0x541U
#define MISC_CFG_1_DEFAULT 	 0x00U

#define VS_OUT2_MISC_CFG_1_ADDR 	 0x541U // Output VSYNC from VS2 pin  
#define VS_OUT2_MISC_CFG_1_MASK 	 0xE0U
#define VS_OUT2_MISC_CFG_1_POS  	 5U

#define MISC_CFG_2_ADDR    	 0x542U
#define MISC_CFG_2_DEFAULT 	 0x00U

#define HS_OUT1_MISC_CFG_2_ADDR 	 0x542U // Output DE/DV from HS1 pin  
#define HS_OUT1_MISC_CFG_2_MASK 	 0xE0U
#define HS_OUT1_MISC_CFG_2_POS  	 5U

#define MISC_UART_PT_0_ADDR    	 0x548U
#define MISC_UART_PT_0_DEFAULT 	 0x96U

#define BITLEN_PT_1_L_MISC_UART_PT_0_ADDR 	 0x548U // Custom UART bit length for pass-through ... 
#define BITLEN_PT_1_L_MISC_UART_PT_0_MASK 	 0xFFU
#define BITLEN_PT_1_L_MISC_UART_PT_0_POS  	 0U

#define MISC_UART_PT_1_ADDR    	 0x549U
#define MISC_UART_PT_1_DEFAULT 	 0x00U

#define BITLEN_PT_1_H_MISC_UART_PT_1_ADDR 	 0x549U // High byte of custom UART bit length for ... 
#define BITLEN_PT_1_H_MISC_UART_PT_1_MASK 	 0x3FU
#define BITLEN_PT_1_H_MISC_UART_PT_1_POS  	 0U

#define MISC_UART_PT_2_ADDR    	 0x54AU
#define MISC_UART_PT_2_DEFAULT 	 0x96U

#define BITLEN_PT_2_L_MISC_UART_PT_2_ADDR 	 0x54AU // Low byte of custom UART bit length for p... 
#define BITLEN_PT_2_L_MISC_UART_PT_2_MASK 	 0xFFU
#define BITLEN_PT_2_L_MISC_UART_PT_2_POS  	 0U

#define MISC_UART_PT_3_ADDR    	 0x54BU
#define MISC_UART_PT_3_DEFAULT 	 0x00U

#define BITLEN_PT_2_H_MISC_UART_PT_3_ADDR 	 0x54BU // High byte of custom UART bit length for ... 
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

#define MISC_CNT4_ADDR    	 0x55CU
#define MISC_CNT4_DEFAULT 	 0x00U

#define VID_PXL_CRC_ERR0_MISC_CNT4_ADDR 	 0x55CU // Total number of video pixel CRC errors d... 
#define VID_PXL_CRC_ERR0_MISC_CNT4_MASK 	 0xFFU
#define VID_PXL_CRC_ERR0_MISC_CNT4_POS  	 0U

#define MISC_CNT5_ADDR    	 0x55DU
#define MISC_CNT5_DEFAULT 	 0x00U

#define VID_PXL_CRC_ERR1_MISC_CNT5_ADDR 	 0x55DU // Total number of video pixel CRC errors d... 
#define VID_PXL_CRC_ERR1_MISC_CNT5_MASK 	 0xFFU
#define VID_PXL_CRC_ERR1_MISC_CNT5_POS  	 0U

#define MISC_CNT6_ADDR    	 0x55EU
#define MISC_CNT6_DEFAULT 	 0x00U

#define VID_PXL_CRC_ERR2_MISC_CNT6_ADDR 	 0x55EU // Total number of video pixel CRC errors d... 
#define VID_PXL_CRC_ERR2_MISC_CNT6_MASK 	 0xFFU
#define VID_PXL_CRC_ERR2_MISC_CNT6_POS  	 0U

#define MISC_CNT7_ADDR    	 0x55FU
#define MISC_CNT7_DEFAULT 	 0x00U

#define VID_PXL_CRC_ERR3_MISC_CNT7_ADDR 	 0x55FU // Total number of video pixel CRC errors d... 
#define VID_PXL_CRC_ERR3_MISC_CNT7_MASK 	 0xFFU
#define VID_PXL_CRC_ERR3_MISC_CNT7_POS  	 0U

#define MISC_PORT_TUN_ONLY_ADDR    	 0x568U
#define MISC_PORT_TUN_ONLY_DEFAULT 	 0x06U

#define TUN_ONLY_CC_MISC_PORT_TUN_ONLY_ADDR 	 0x568U // Disable control channel access from RX/S... 
#define TUN_ONLY_CC_MISC_PORT_TUN_ONLY_MASK 	 0x01U
#define TUN_ONLY_CC_MISC_PORT_TUN_ONLY_POS  	 0U

#define TUN_ONLY_1_MISC_PORT_TUN_ONLY_ADDR 	 0x568U // Disable control channel access from RX1/... 
#define TUN_ONLY_1_MISC_PORT_TUN_ONLY_MASK 	 0x02U
#define TUN_ONLY_1_MISC_PORT_TUN_ONLY_POS  	 1U

#define TUN_ONLY_2_MISC_PORT_TUN_ONLY_ADDR 	 0x568U // Disable control channel access from RX2/... 
#define TUN_ONLY_2_MISC_PORT_TUN_ONLY_MASK 	 0x04U
#define TUN_ONLY_2_MISC_PORT_TUN_ONLY_POS  	 2U

#define MISC_UNLOCK_KEY_ADDR    	 0x569U
#define MISC_UNLOCK_KEY_DEFAULT 	 0xAAU

#define UNLOCK_KEY_MISC_UNLOCK_KEY_ADDR 	 0x569U // Register must be at unlock value to enab... 
#define UNLOCK_KEY_MISC_UNLOCK_KEY_MASK 	 0xFFU
#define UNLOCK_KEY_MISC_UNLOCK_KEY_POS  	 0U

#define MISC_PIO_SLEW_0_ADDR    	 0x570U
#define MISC_PIO_SLEW_0_DEFAULT 	 0xFEU

#define PIO00_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x570U // Rise and fall time speed setting on pad ... 
#define PIO00_SLEW_MISC_PIO_SLEW_0_MASK 	 0x03U
#define PIO00_SLEW_MISC_PIO_SLEW_0_POS  	 0U

#define PIO01_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x570U // Rise and fall time speed setting on pad ... 
#define PIO01_SLEW_MISC_PIO_SLEW_0_MASK 	 0x0CU
#define PIO01_SLEW_MISC_PIO_SLEW_0_POS  	 2U

#define PIO02_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x570U // Rise and fall time speed setting on pad ... 
#define PIO02_SLEW_MISC_PIO_SLEW_0_MASK 	 0x30U
#define PIO02_SLEW_MISC_PIO_SLEW_0_POS  	 4U

#define PIO03_SLEW_MISC_PIO_SLEW_0_ADDR 	 0x570U // Rise and fall time speed setting on pad ... 
#define PIO03_SLEW_MISC_PIO_SLEW_0_MASK 	 0xC0U
#define PIO03_SLEW_MISC_PIO_SLEW_0_POS  	 6U

#define MISC_PIO_SLEW_1_ADDR    	 0x571U
#define MISC_PIO_SLEW_1_DEFAULT 	 0x83U

#define PIO04_SLEW_MISC_PIO_SLEW_1_ADDR 	 0x571U // Rise and fall time speed setting on pad ... 
#define PIO04_SLEW_MISC_PIO_SLEW_1_MASK 	 0x03U
#define PIO04_SLEW_MISC_PIO_SLEW_1_POS  	 0U

#define PIO07_SLEW_MISC_PIO_SLEW_1_ADDR 	 0x571U // Rise and fall time speed setting on pad ... 
#define PIO07_SLEW_MISC_PIO_SLEW_1_MASK 	 0xC0U
#define PIO07_SLEW_MISC_PIO_SLEW_1_POS  	 6U

#define MISC_PIO_SLEW_2_ADDR    	 0x572U
#define MISC_PIO_SLEW_2_DEFAULT 	 0x02U

#define PIO08_SLEW_MISC_PIO_SLEW_2_ADDR 	 0x572U // Rise and fall time speed setting on pad ... 
#define PIO08_SLEW_MISC_PIO_SLEW_2_MASK 	 0x03U
#define PIO08_SLEW_MISC_PIO_SLEW_2_POS  	 0U

#define MISC_HS_VS_ACT_Y_ADDR    	 0x575U
#define MISC_HS_VS_ACT_Y_DEFAULT 	 0x00U

#define HS_POL_Y_MISC_HS_VS_ACT_Y_ADDR 	 0x575U // Detected HS polarity  
#define HS_POL_Y_MISC_HS_VS_ACT_Y_MASK 	 0x01U
#define HS_POL_Y_MISC_HS_VS_ACT_Y_POS  	 0U

#define VS_POL_Y_MISC_HS_VS_ACT_Y_ADDR 	 0x575U // Detected VS polarity  
#define VS_POL_Y_MISC_HS_VS_ACT_Y_MASK 	 0x02U
#define VS_POL_Y_MISC_HS_VS_ACT_Y_POS  	 1U

#define HS_DET_Y_MISC_HS_VS_ACT_Y_ADDR 	 0x575U // HS activity is detected  
#define HS_DET_Y_MISC_HS_VS_ACT_Y_MASK 	 0x10U
#define HS_DET_Y_MISC_HS_VS_ACT_Y_POS  	 4U

#define VS_DET_Y_MISC_HS_VS_ACT_Y_ADDR 	 0x575U // VS activity is detected  
#define VS_DET_Y_MISC_HS_VS_ACT_Y_MASK 	 0x20U
#define VS_DET_Y_MISC_HS_VS_ACT_Y_POS  	 5U

#define DE_DET_Y_MISC_HS_VS_ACT_Y_ADDR 	 0x575U // DE activity is detected  
#define DE_DET_Y_MISC_HS_VS_ACT_Y_MASK 	 0x40U
#define DE_DET_Y_MISC_HS_VS_ACT_Y_POS  	 6U

#define MISC_HS_VS_ACT_Z_ADDR    	 0x576U
#define MISC_HS_VS_ACT_Z_DEFAULT 	 0x00U

#define HS_POL_Z_MISC_HS_VS_ACT_Z_ADDR 	 0x576U // Detected HS polarity  
#define HS_POL_Z_MISC_HS_VS_ACT_Z_MASK 	 0x01U
#define HS_POL_Z_MISC_HS_VS_ACT_Z_POS  	 0U

#define VS_POL_Z_MISC_HS_VS_ACT_Z_ADDR 	 0x576U // Detected VS polarity  
#define VS_POL_Z_MISC_HS_VS_ACT_Z_MASK 	 0x02U
#define VS_POL_Z_MISC_HS_VS_ACT_Z_POS  	 1U

#define HS_DET_Z_MISC_HS_VS_ACT_Z_ADDR 	 0x576U // HS activity is detected  
#define HS_DET_Z_MISC_HS_VS_ACT_Z_MASK 	 0x10U
#define HS_DET_Z_MISC_HS_VS_ACT_Z_POS  	 4U

#define VS_DET_Z_MISC_HS_VS_ACT_Z_ADDR 	 0x576U // VS activity is detected  
#define VS_DET_Z_MISC_HS_VS_ACT_Z_MASK 	 0x20U
#define VS_DET_Z_MISC_HS_VS_ACT_Z_POS  	 5U

#define DE_DET_Z_MISC_HS_VS_ACT_Z_ADDR 	 0x576U // DE activity is detected  
#define DE_DET_Z_MISC_HS_VS_ACT_Z_MASK 	 0x40U
#define DE_DET_Z_MISC_HS_VS_ACT_Z_POS  	 6U

#define MISC_DP_ORSTB_CTL_ADDR    	 0x577U
#define MISC_DP_ORSTB_CTL_DEFAULT 	 0x60U

#define DP_RST_VP_CHKB_MISC_DP_ORSTB_CTL_ADDR 	 0x577U // Select reset mode to VIDEO_RX, VRX block... 
#define DP_RST_VP_CHKB_MISC_DP_ORSTB_CTL_MASK 	 0x04U
#define DP_RST_VP_CHKB_MISC_DP_ORSTB_CTL_POS  	 2U

#define DP_RST_MIPI_CHKB_MISC_DP_ORSTB_CTL_ADDR 	 0x577U // Select reset mode to MIPI controllers.  
#define DP_RST_MIPI_CHKB_MISC_DP_ORSTB_CTL_MASK 	 0x08U
#define DP_RST_MIPI_CHKB_MISC_DP_ORSTB_CTL_POS  	 3U

#define DP_RST_MIPI2_CHKB_MISC_DP_ORSTB_CTL_ADDR 	 0x577U // Select reset mode to MIPI controllers. S... 
#define DP_RST_MIPI2_CHKB_MISC_DP_ORSTB_CTL_MASK 	 0x10U
#define DP_RST_MIPI2_CHKB_MISC_DP_ORSTB_CTL_POS  	 4U

#define DP_RST_STABLE_CHKB_MISC_DP_ORSTB_CTL_ADDR 	 0x577U // Select reset mode when changing register... 
#define DP_RST_STABLE_CHKB_MISC_DP_ORSTB_CTL_MASK 	 0x20U
#define DP_RST_STABLE_CHKB_MISC_DP_ORSTB_CTL_POS  	 5U

#define DP_RST_MIPI3_CHKB_MISC_DP_ORSTB_CTL_ADDR 	 0x577U // Select reset mode to MIPI controllers. S... 
#define DP_RST_MIPI3_CHKB_MISC_DP_ORSTB_CTL_MASK 	 0x40U
#define DP_RST_MIPI3_CHKB_MISC_DP_ORSTB_CTL_POS  	 6U

#define MISC_PM_OV_STAT2_ADDR    	 0x578U
#define MISC_PM_OV_STAT2_DEFAULT 	 0x15U

#define VREG_OV_LEVEL_MISC_PM_OV_STAT2_ADDR 	 0x578U // VREG (LDO regulated VDD) overvoltage com... 
#define VREG_OV_LEVEL_MISC_PM_OV_STAT2_MASK 	 0x03U
#define VREG_OV_LEVEL_MISC_PM_OV_STAT2_POS  	 0U

#define VTERM_OV_LEVEL_MISC_PM_OV_STAT2_ADDR 	 0x578U // VTERM overvoltage comparator trip level  
#define VTERM_OV_LEVEL_MISC_PM_OV_STAT2_MASK 	 0x30U
#define VTERM_OV_LEVEL_MISC_PM_OV_STAT2_POS  	 4U

#define VREG_OV_OEN_MISC_PM_OV_STAT2_ADDR 	 0x578U // Enable VREG (LDO regulated VDD) overvolt... 
#define VREG_OV_OEN_MISC_PM_OV_STAT2_MASK 	 0x40U
#define VREG_OV_OEN_MISC_PM_OV_STAT2_POS  	 6U

#define VTERM_OV_OEN_MISC_PM_OV_STAT2_ADDR 	 0x578U // Enable VTERM overvoltage status on ERRB  
#define VTERM_OV_OEN_MISC_PM_OV_STAT2_MASK 	 0x80U
#define VTERM_OV_OEN_MISC_PM_OV_STAT2_POS  	 7U

#define MISC_PM_OV_STAT3_ADDR    	 0x579U
#define MISC_PM_OV_STAT3_DEFAULT 	 0x00U

#define VREG_OV_FLAG_MISC_PM_OV_STAT3_ADDR 	 0x579U // Sticky status value for VREG (LDO regula... 
#define VREG_OV_FLAG_MISC_PM_OV_STAT3_MASK 	 0x40U
#define VREG_OV_FLAG_MISC_PM_OV_STAT3_POS  	 6U

#define VTERM_OV_FLAG_MISC_PM_OV_STAT3_ADDR 	 0x579U // Sticky status value for VTERM overvoltag... 
#define VTERM_OV_FLAG_MISC_PM_OV_STAT3_MASK 	 0x80U
#define VTERM_OV_FLAG_MISC_PM_OV_STAT3_POS  	 7U

#define CC_EXT_UART_0_ADDR    	 0x808U
#define CC_EXT_UART_0_DEFAULT 	 0x02U

#define BYPASS_EN_1_CC_EXT_UART_0_ADDR 	 0x808U // Enable UART soft-bypass mode.  
#define BYPASS_EN_1_CC_EXT_UART_0_MASK 	 0x01U
#define BYPASS_EN_1_CC_EXT_UART_0_POS  	 0U

#define BYPASS_TO_1_CC_EXT_UART_0_ADDR 	 0x808U // UART soft-bypass timeout duration.  
#define BYPASS_TO_1_CC_EXT_UART_0_MASK 	 0x06U
#define BYPASS_TO_1_CC_EXT_UART_0_POS  	 1U

#define LOC_MS_EN_1_CC_EXT_UART_0_ADDR 	 0x808U // Enable UART Bypass mode control by local... 
#define LOC_MS_EN_1_CC_EXT_UART_0_MASK 	 0x10U
#define LOC_MS_EN_1_CC_EXT_UART_0_POS  	 4U

#define REM_MS_EN_1_CC_EXT_UART_0_ADDR 	 0x808U // Enable UART Bypass mode control by remot... 
#define REM_MS_EN_1_CC_EXT_UART_0_MASK 	 0x20U
#define REM_MS_EN_1_CC_EXT_UART_0_POS  	 5U

#define CC_EXT_UART_1_ADDR    	 0x809U
#define CC_EXT_UART_1_DEFAULT 	 0x02U

#define BYPASS_EN_2_CC_EXT_UART_1_ADDR 	 0x809U // Enable UART Soft-bypass mode.  
#define BYPASS_EN_2_CC_EXT_UART_1_MASK 	 0x01U
#define BYPASS_EN_2_CC_EXT_UART_1_POS  	 0U

#define BYPASS_TO_2_CC_EXT_UART_1_ADDR 	 0x809U // UART soft-bypass timeout duration.  
#define BYPASS_TO_2_CC_EXT_UART_1_MASK 	 0x06U
#define BYPASS_TO_2_CC_EXT_UART_1_POS  	 1U

#define LOC_MS_EN_2_CC_EXT_UART_1_ADDR 	 0x809U // Enable UART Bypass mode control by local... 
#define LOC_MS_EN_2_CC_EXT_UART_1_MASK 	 0x10U
#define LOC_MS_EN_2_CC_EXT_UART_1_POS  	 4U

#define REM_MS_EN_2_CC_EXT_UART_1_ADDR 	 0x809U // Enable UART Bypass mode control by remot... 
#define REM_MS_EN_2_CC_EXT_UART_1_MASK 	 0x20U
#define REM_MS_EN_2_CC_EXT_UART_1_POS  	 5U

#define CC_EXT_I2C_PT_0_ADDR    	 0x80EU
#define CC_EXT_I2C_PT_0_DEFAULT 	 0x06U

#define I2C_INTREG_SLV_TO_CC_EXT_I2C_PT_0_ADDR 	 0x80EU // I2C-to-Internal Register Subordinate 0 T... 
#define I2C_INTREG_SLV_TO_CC_EXT_I2C_PT_0_MASK 	 0x07U
#define I2C_INTREG_SLV_TO_CC_EXT_I2C_PT_0_POS  	 0U

#define I2C_REGSLV_0_TIMED_OUT_CC_EXT_I2C_PT_0_ADDR 	 0x80EU // Internal I2C-to-Register subordinate for... 
#define I2C_REGSLV_0_TIMED_OUT_CC_EXT_I2C_PT_0_MASK 	 0x40U
#define I2C_REGSLV_0_TIMED_OUT_CC_EXT_I2C_PT_0_POS  	 6U

#define CC_EXT_I2C_PT_1_ADDR    	 0x80FU
#define CC_EXT_I2C_PT_1_DEFAULT 	 0x36U

#define I2C_INTREG_SLV_1_TO_CC_EXT_I2C_PT_1_ADDR 	 0x80FU // I2C-to-Internal Register Subordinate 1 T... 
#define I2C_INTREG_SLV_1_TO_CC_EXT_I2C_PT_1_MASK 	 0x07U
#define I2C_INTREG_SLV_1_TO_CC_EXT_I2C_PT_1_POS  	 0U

#define I2C_INTREG_SLV_2_TO_CC_EXT_I2C_PT_1_ADDR 	 0x80FU // I2C-to-Internal Register Subordinate 2 t... 
#define I2C_INTREG_SLV_2_TO_CC_EXT_I2C_PT_1_MASK 	 0x38U
#define I2C_INTREG_SLV_2_TO_CC_EXT_I2C_PT_1_POS  	 3U

#define I2C_REGSLV_1_TIMED_OUT_CC_EXT_I2C_PT_1_ADDR 	 0x80FU // Internal I2C-to-Register subordinate for... 
#define I2C_REGSLV_1_TIMED_OUT_CC_EXT_I2C_PT_1_MASK 	 0x40U
#define I2C_REGSLV_1_TIMED_OUT_CC_EXT_I2C_PT_1_POS  	 6U

#define I2C_REGSLV_2_TIMED_OUT_CC_EXT_I2C_PT_1_ADDR 	 0x80FU // Internal I2C-to-Register subordinate for... 
#define I2C_REGSLV_2_TIMED_OUT_CC_EXT_I2C_PT_1_MASK 	 0x80U
#define I2C_REGSLV_2_TIMED_OUT_CC_EXT_I2C_PT_1_POS  	 7U

#define GMSL1_COMMON_GMSL1_EN_ADDR    	 0xF00U
#define GMSL1_COMMON_GMSL1_EN_DEFAULT 	 0x03U

#define LINK_EN_A_GMSL1_COMMON_GMSL1_EN_ADDR 	 0xF00U // Enable link A  
#define LINK_EN_A_GMSL1_COMMON_GMSL1_EN_MASK 	 0x01U
#define LINK_EN_A_GMSL1_COMMON_GMSL1_EN_POS  	 0U

#define LINK_EN_B_GMSL1_COMMON_GMSL1_EN_ADDR 	 0xF00U // Enable link B  
#define LINK_EN_B_GMSL1_COMMON_GMSL1_EN_MASK 	 0x02U
#define LINK_EN_B_GMSL1_COMMON_GMSL1_EN_POS  	 1U

#define SPI_CC_WR_SPI_CC_WR__ADDR    	 0x1300U
#define SPI_CC_WR_SPI_CC_WR__DEFAULT 	 0x00U

#define SPI_CC_RD_SPI_CC_RD__ADDR    	 0x1380U
#define SPI_CC_RD_SPI_CC_RD__DEFAULT 	 0x00U

#define RLMS_A_RLMS3_ADDR    	 0x1403U
#define RLMS_A_RLMS3_DEFAULT 	 0x0AU

#define ADAPTEN_RLMS_A_RLMS3_ADDR 	 0x1403U // Adapt process enable  
#define ADAPTEN_RLMS_A_RLMS3_MASK 	 0x80U
#define ADAPTEN_RLMS_A_RLMS3_POS  	 7U

#define RLMS_A_RLMS4_ADDR    	 0x1404U
#define RLMS_A_RLMS4_DEFAULT 	 0x4BU

#define EOM_EN_RLMS_A_RLMS4_ADDR 	 0x1404U // Eye-Opening Monitor Enable  
#define EOM_EN_RLMS_A_RLMS4_MASK 	 0x01U
#define EOM_EN_RLMS_A_RLMS4_POS  	 0U

#define EOM_PER_MODE_RLMS_A_RLMS4_ADDR 	 0x1404U // Eye-Opening Monitor Periodic Mode Enable... 
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

#define EOM_MIN_THR_RLMS_A_RLMS5_ADDR 	 0x1405U // The eye-opening monitor minimum threshol... 
#define EOM_MIN_THR_RLMS_A_RLMS5_MASK 	 0x7FU
#define EOM_MIN_THR_RLMS_A_RLMS5_POS  	 0U

#define EOM_MAN_TRG_REQ_RLMS_A_RLMS5_ADDR 	 0x1405U // Eye-Opening Monitor Manual Trigger  
#define EOM_MAN_TRG_REQ_RLMS_A_RLMS5_MASK 	 0x80U
#define EOM_MAN_TRG_REQ_RLMS_A_RLMS5_POS  	 7U

#define RLMS_A_RLMS6_ADDR    	 0x1406U
#define RLMS_A_RLMS6_DEFAULT 	 0x80U

#define EOM_RST_THR_RLMS_A_RLMS6_ADDR 	 0x1406U // The eye-opening monitor refresh threshol... 
#define EOM_RST_THR_RLMS_A_RLMS6_MASK 	 0x7FU
#define EOM_RST_THR_RLMS_A_RLMS6_POS  	 0U

#define EOM_PV_MODE_RLMS_A_RLMS6_ADDR 	 0x1406U // Eye-opening is measured vertically or ho... 
#define EOM_PV_MODE_RLMS_A_RLMS6_MASK 	 0x80U
#define EOM_PV_MODE_RLMS_A_RLMS6_POS  	 7U

#define RLMS_A_RLMS7_ADDR    	 0x1407U
#define RLMS_A_RLMS7_DEFAULT 	 0x00U

#define EOM_RLMS_A_RLMS7_ADDR 	 0x1407U // Last completed EOM observation  
#define EOM_RLMS_A_RLMS7_MASK 	 0x7FU
#define EOM_RLMS_A_RLMS7_POS  	 0U

#define EOM_DONE_RLMS_A_RLMS7_ADDR 	 0x1407U // Eye-Opening Monitor Measurement Done  
#define EOM_DONE_RLMS_A_RLMS7_MASK 	 0x80U
#define EOM_DONE_RLMS_A_RLMS7_POS  	 7U

#define RLMS_A_RLMSA_ADDR    	 0x140AU
#define RLMS_A_RLMSA_DEFAULT 	 0x08U

#define DFEADPDLY_RLMS_A_RLMSA_ADDR 	 0x140AU // DFE adapt enable delay (milliseconds)  
#define DFEADPDLY_RLMS_A_RLMSA_MASK 	 0x0FU
#define DFEADPDLY_RLMS_A_RLMSA_POS  	 0U

#define RLMS_A_RLMSB_ADDR    	 0x140BU
#define RLMS_A_RLMSB_DEFAULT 	 0x44U

#define AGCACQDLY_RLMS_A_RLMSB_ADDR 	 0x140BU // AGC Acquisition Delay : (milliseconds)  
#define AGCACQDLY_RLMS_A_RLMSB_MASK 	 0xF0U
#define AGCACQDLY_RLMS_A_RLMSB_POS  	 4U

#define RLMS_A_RLMS18_ADDR    	 0x1418U
#define RLMS_A_RLMS18_DEFAULT 	 0x0FU

#define VGAHIGAIN_RLMS_A_RLMS18_ADDR 	 0x1418U // 55nm FR VGA has an addition gain stage i... 
#define VGAHIGAIN_RLMS_A_RLMS18_MASK 	 0x04U
#define VGAHIGAIN_RLMS_A_RLMS18_POS  	 2U

#define RLMS_A_RLMS1F_ADDR    	 0x141FU
#define RLMS_A_RLMS1F_DEFAULT 	 0xA7U

#define AGCINITG2_RLMS_A_RLMS1F_ADDR 	 0x141FU // AGC initial value for G2 mode  
#define AGCINITG2_RLMS_A_RLMS1F_MASK 	 0xFFU
#define AGCINITG2_RLMS_A_RLMS1F_POS  	 0U

#define RLMS_A_RLMS21_ADDR    	 0x1421U
#define RLMS_A_RLMS21_DEFAULT 	 0x04U

#define BSTMUH_RLMS_A_RLMS21_ADDR 	 0x1421U // BST adapt gain MSB  
#define BSTMUH_RLMS_A_RLMS21_MASK 	 0x3FU
#define BSTMUH_RLMS_A_RLMS21_POS  	 0U

#define RLMS_A_RLMS23_ADDR    	 0x1423U
#define RLMS_A_RLMS23_DEFAULT 	 0x45U

#define BSTINIT_RLMS_A_RLMS23_ADDR 	 0x1423U // BST initial value  
#define BSTINIT_RLMS_A_RLMS23_MASK 	 0x3FU
#define BSTINIT_RLMS_A_RLMS23_POS  	 0U

#define RLMS_A_RLMS31_ADDR    	 0x1431U
#define RLMS_A_RLMS31_DEFAULT 	 0x18U

#define OSNMUH_RLMS_A_RLMS31_ADDR 	 0x1431U // OSN adapt gain MSB  
#define OSNMUH_RLMS_A_RLMS31_MASK 	 0x3FU
#define OSNMUH_RLMS_A_RLMS31_POS  	 0U

#define RLMS_A_RLMS3E_ADDR    	 0x143EU
#define RLMS_A_RLMS3E_DEFAULT 	 0x94U

#define ERRCHPHSECFR6G_RLMS_A_RLMS3E_ADDR 	 0x143EU // Error channel sampling point phase adjus... 
#define ERRCHPHSECFR6G_RLMS_A_RLMS3E_MASK 	 0x7FU
#define ERRCHPHSECFR6G_RLMS_A_RLMS3E_POS  	 0U

#define ERRCHPHSECTAFR6G_RLMS_A_RLMS3E_ADDR 	 0x143EU // Error channel phase secondary timing adj... 
#define ERRCHPHSECTAFR6G_RLMS_A_RLMS3E_MASK 	 0x80U
#define ERRCHPHSECTAFR6G_RLMS_A_RLMS3E_POS  	 7U

#define RLMS_A_RLMS3F_ADDR    	 0x143FU
#define RLMS_A_RLMS3F_DEFAULT 	 0x54U

#define ERRCHPHPRIFR6G_RLMS_A_RLMS3F_ADDR 	 0x143FU // Error channel sampling point phase adjus... 
#define ERRCHPHPRIFR6G_RLMS_A_RLMS3F_MASK 	 0x7FU
#define ERRCHPHPRIFR6G_RLMS_A_RLMS3F_POS  	 0U

#define ERRCHPHPRITAFR6G_RLMS_A_RLMS3F_ADDR 	 0x143FU // Error channel phase primary timing adjus... 
#define ERRCHPHPRITAFR6G_RLMS_A_RLMS3F_MASK 	 0x80U
#define ERRCHPHPRITAFR6G_RLMS_A_RLMS3F_POS  	 7U

#define RLMS_A_RLMS45_ADDR    	 0x1445U
#define RLMS_A_RLMS45_DEFAULT 	 0xC8U

#define CRUSSCSELSREN_RLMS_A_RLMS45_ADDR 	 0x1445U // Override enable for CRU SSC SEL for Slow... 
#define CRUSSCSELSREN_RLMS_A_RLMS45_MASK 	 0x40U
#define CRUSSCSELSREN_RLMS_A_RLMS45_POS  	 6U

#define CRULPCTRLSREN_RLMS_A_RLMS45_ADDR 	 0x1445U // Override enable for CRU Loop control. Wh... 
#define CRULPCTRLSREN_RLMS_A_RLMS45_MASK 	 0x80U
#define CRULPCTRLSREN_RLMS_A_RLMS45_POS  	 7U

#define RLMS_A_RLMS46_ADDR    	 0x1446U
#define RLMS_A_RLMS46_DEFAULT 	 0xB3U

#define CRULPCTRL_RLMS_A_RLMS46_ADDR 	 0x1446U // CRU loop control for Fast Receiver and S... 
#define CRULPCTRL_RLMS_A_RLMS46_MASK 	 0x07U
#define CRULPCTRL_RLMS_A_RLMS46_POS  	 0U

#define RLMS_A_RLMS47_ADDR    	 0x1447U
#define RLMS_A_RLMS47_DEFAULT 	 0x03U

#define CRUSSCSEL_RLMS_A_RLMS47_ADDR 	 0x1447U // CRU spread spectrum adjust select  
#define CRUSSCSEL_RLMS_A_RLMS47_MASK 	 0x06U
#define CRUSSCSEL_RLMS_A_RLMS47_POS  	 1U

#define RLMS_A_RLMS49_ADDR    	 0x1449U
#define RLMS_A_RLMS49_DEFAULT 	 0xF5U

#define ERRCHPWRUP_RLMS_A_RLMS49_ADDR 	 0x1449U // Error channel power down  
#define ERRCHPWRUP_RLMS_A_RLMS49_MASK 	 0x04U
#define ERRCHPWRUP_RLMS_A_RLMS49_POS  	 2U

#define RLMS_A_RLMS64_ADDR    	 0x1464U
#define RLMS_A_RLMS64_DEFAULT 	 0x90U

#define TXSSCMODE_RLMS_A_RLMS64_ADDR 	 0x1464U // Tx Spread Spectrum Mode  
#define TXSSCMODE_RLMS_A_RLMS64_MASK 	 0x03U
#define TXSSCMODE_RLMS_A_RLMS64_POS  	 0U

#define RLMS_A_RLMS70_ADDR    	 0x1470U
#define RLMS_A_RLMS70_DEFAULT 	 0x01U

#define TXSSCFRQCTRL_RLMS_A_RLMS70_ADDR 	 0x1470U // Tx SSC modulation amplitude (frequency d... 
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

#define RLMS_A_RLMS8C_ADDR    	 0x148CU
#define RLMS_A_RLMS8C_DEFAULT 	 0x00U

#define CAP_PRE_OUT_RLMS_RLMS_A_RLMS8C_ADDR 	 0x148CU // cap_preout value during RLMS if overridd... 
#define CAP_PRE_OUT_RLMS_RLMS_A_RLMS8C_MASK 	 0x7FU
#define CAP_PRE_OUT_RLMS_RLMS_A_RLMS8C_POS  	 0U

#define RLMS_A_RLMS95_ADDR    	 0x1495U
#define RLMS_A_RLMS95_DEFAULT 	 0x69U

#define TXAMPLMAN_RLMS_A_RLMS95_ADDR 	 0x1495U // TX amplitude  
#define TXAMPLMAN_RLMS_A_RLMS95_MASK 	 0x3FU
#define TXAMPLMAN_RLMS_A_RLMS95_POS  	 0U

#define TXAMPLMANEN_RLMS_A_RLMS95_ADDR 	 0x1495U // TX amplitude manual override  
#define TXAMPLMANEN_RLMS_A_RLMS95_MASK 	 0x80U
#define TXAMPLMANEN_RLMS_A_RLMS95_POS  	 7U

#define RLMS_A_RLMS98_ADDR    	 0x1498U
#define RLMS_A_RLMS98_DEFAULT 	 0x40U

#define CAL_CAP_PRE_OUT_EN_RLMS_A_RLMS98_ADDR 	 0x1498U // Enable manual override for cap_pre_out d... 
#define CAL_CAP_PRE_OUT_EN_RLMS_A_RLMS98_MASK 	 0x80U
#define CAL_CAP_PRE_OUT_EN_RLMS_A_RLMS98_POS  	 7U

#define RLMS_A_RLMSA4_ADDR    	 0x14A4U
#define RLMS_A_RLMSA4_DEFAULT 	 0xBDU

#define AEQ_PER_RLMS_A_RLMSA4_ADDR 	 0x14A4U // Adaptive EQ period  
#define AEQ_PER_RLMS_A_RLMSA4_MASK 	 0x3FU
#define AEQ_PER_RLMS_A_RLMSA4_POS  	 0U

#define AEQ_PER_MULT_RLMS_A_RLMSA4_ADDR 	 0x14A4U // Adaptive EQ period multiplier  
#define AEQ_PER_MULT_RLMS_A_RLMSA4_MASK 	 0xC0U
#define AEQ_PER_MULT_RLMS_A_RLMSA4_POS  	 6U

#define RLMS_A_RLMSA5_ADDR    	 0x14A5U
#define RLMS_A_RLMSA5_DEFAULT 	 0x50U

#define PHYC_WBLOCK_DLY_RLMS_A_RLMSA5_ADDR 	 0x14A5U // PHY controller word boundary lock start ... 
#define PHYC_WBLOCK_DLY_RLMS_A_RLMSA5_MASK 	 0x30U
#define PHYC_WBLOCK_DLY_RLMS_A_RLMSA5_POS  	 4U

#define RLMS_A_RLMSA7_ADDR    	 0x14A7U
#define RLMS_A_RLMSA7_DEFAULT 	 0x01U

#define MAN_CTRL_EN_RLMS_A_RLMSA7_ADDR 	 0x14A7U // PHY controller manual mode enable  
#define MAN_CTRL_EN_RLMS_A_RLMSA7_MASK 	 0x80U
#define MAN_CTRL_EN_RLMS_A_RLMSA7_POS  	 7U

#define RLMS_A_RLMSA8_ADDR    	 0x14A8U
#define RLMS_A_RLMSA8_DEFAULT 	 0x00U

#define FW_PHY_RSTB_RLMS_A_RLMSA8_ADDR 	 0x14A8U // Override PHY controller output  
#define FW_PHY_RSTB_RLMS_A_RLMSA8_MASK 	 0x20U
#define FW_PHY_RSTB_RLMS_A_RLMSA8_POS  	 5U

#define FW_PHY_PU_TX_RLMS_A_RLMSA8_ADDR 	 0x14A8U // Override PHY controller output  
#define FW_PHY_PU_TX_RLMS_A_RLMSA8_MASK 	 0x40U
#define FW_PHY_PU_TX_RLMS_A_RLMSA8_POS  	 6U

#define FW_PHY_CTRL_RLMS_A_RLMSA8_ADDR 	 0x14A8U // PHY controller firmware mode enable  
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

#define RLMS_A_RLMSAC_ADDR    	 0x14ACU
#define RLMS_A_RLMSAC_DEFAULT 	 0xA0U

#define ERRCHPHSECFR3G_RLMS_A_RLMSAC_ADDR 	 0x14ACU // Error channel phase secondary (odd)  
#define ERRCHPHSECFR3G_RLMS_A_RLMSAC_MASK 	 0x7FU
#define ERRCHPHSECFR3G_RLMS_A_RLMSAC_POS  	 0U

#define RLMS_A_RLMSAD_ADDR    	 0x14ADU
#define RLMS_A_RLMSAD_DEFAULT 	 0x60U

#define ERRCHPHPRIFR3G_RLMS_A_RLMSAD_ADDR 	 0x14ADU // Error channel phase primary (even)  
#define ERRCHPHPRIFR3G_RLMS_A_RLMSAD_MASK 	 0x7FU
#define ERRCHPHPRIFR3G_RLMS_A_RLMSAD_POS  	 0U

#define RLMS_B_RLMS3_ADDR    	 0x1503U
#define RLMS_B_RLMS3_DEFAULT 	 0x0AU

#define ADAPTEN_RLMS_B_RLMS3_ADDR 	 0x1503U // Adapt process enable  
#define ADAPTEN_RLMS_B_RLMS3_MASK 	 0x80U
#define ADAPTEN_RLMS_B_RLMS3_POS  	 7U

#define RLMS_B_RLMS4_ADDR    	 0x1504U
#define RLMS_B_RLMS4_DEFAULT 	 0x4BU

#define EOM_EN_RLMS_B_RLMS4_ADDR 	 0x1504U // Eye-Opening Monitor Enable  
#define EOM_EN_RLMS_B_RLMS4_MASK 	 0x01U
#define EOM_EN_RLMS_B_RLMS4_POS  	 0U

#define EOM_PER_MODE_RLMS_B_RLMS4_ADDR 	 0x1504U // Eye-Opening Monitor Periodic Mode Enable... 
#define EOM_PER_MODE_RLMS_B_RLMS4_MASK 	 0x02U
#define EOM_PER_MODE_RLMS_B_RLMS4_POS  	 1U

#define EOM_CHK_THR_RLMS_B_RLMS4_ADDR 	 0x1504U // Eye-opening monitor number of error bits... 
#define EOM_CHK_THR_RLMS_B_RLMS4_MASK 	 0x0CU
#define EOM_CHK_THR_RLMS_B_RLMS4_POS  	 2U

#define EOM_CHK_AMOUNT_RLMS_B_RLMS4_ADDR 	 0x1504U // A factor (N) used to select the order of... 
#define EOM_CHK_AMOUNT_RLMS_B_RLMS4_MASK 	 0xF0U
#define EOM_CHK_AMOUNT_RLMS_B_RLMS4_POS  	 4U

#define RLMS_B_RLMS5_ADDR    	 0x1505U
#define RLMS_B_RLMS5_DEFAULT 	 0x10U

#define EOM_MIN_THR_RLMS_B_RLMS5_ADDR 	 0x1505U // The eye-opening monitor minimum threshol... 
#define EOM_MIN_THR_RLMS_B_RLMS5_MASK 	 0x7FU
#define EOM_MIN_THR_RLMS_B_RLMS5_POS  	 0U

#define EOM_MAN_TRG_REQ_RLMS_B_RLMS5_ADDR 	 0x1505U // Eye-Opening Monitor Manual Trigger  
#define EOM_MAN_TRG_REQ_RLMS_B_RLMS5_MASK 	 0x80U
#define EOM_MAN_TRG_REQ_RLMS_B_RLMS5_POS  	 7U

#define RLMS_B_RLMS6_ADDR    	 0x1506U
#define RLMS_B_RLMS6_DEFAULT 	 0x80U

#define EOM_RST_THR_RLMS_B_RLMS6_ADDR 	 0x1506U // The eye-opening monitor refresh threshol... 
#define EOM_RST_THR_RLMS_B_RLMS6_MASK 	 0x7FU
#define EOM_RST_THR_RLMS_B_RLMS6_POS  	 0U

#define EOM_PV_MODE_RLMS_B_RLMS6_ADDR 	 0x1506U // Eye-opening is measured vertically or ho... 
#define EOM_PV_MODE_RLMS_B_RLMS6_MASK 	 0x80U
#define EOM_PV_MODE_RLMS_B_RLMS6_POS  	 7U

#define RLMS_B_RLMS7_ADDR    	 0x1507U
#define RLMS_B_RLMS7_DEFAULT 	 0x00U

#define EOM_RLMS_B_RLMS7_ADDR 	 0x1507U // Last completed EOM observation  
#define EOM_RLMS_B_RLMS7_MASK 	 0x7FU
#define EOM_RLMS_B_RLMS7_POS  	 0U

#define EOM_DONE_RLMS_B_RLMS7_ADDR 	 0x1507U // Eye-Opening Monitor Measurement Done  
#define EOM_DONE_RLMS_B_RLMS7_MASK 	 0x80U
#define EOM_DONE_RLMS_B_RLMS7_POS  	 7U

#define RLMS_B_RLMSA_ADDR    	 0x150AU
#define RLMS_B_RLMSA_DEFAULT 	 0x08U

#define DFEADPDLY_RLMS_B_RLMSA_ADDR 	 0x150AU // DFE adapt enable delay (milliseconds)  
#define DFEADPDLY_RLMS_B_RLMSA_MASK 	 0x0FU
#define DFEADPDLY_RLMS_B_RLMSA_POS  	 0U

#define RLMS_B_RLMSB_ADDR    	 0x150BU
#define RLMS_B_RLMSB_DEFAULT 	 0x44U

#define AGCACQDLY_RLMS_B_RLMSB_ADDR 	 0x150BU // AGC Acquisition Delay : (milliseconds)  
#define AGCACQDLY_RLMS_B_RLMSB_MASK 	 0xF0U
#define AGCACQDLY_RLMS_B_RLMSB_POS  	 4U

#define RLMS_B_RLMS18_ADDR    	 0x1518U
#define RLMS_B_RLMS18_DEFAULT 	 0x0FU

#define VGAHIGAIN_RLMS_B_RLMS18_ADDR 	 0x1518U // 55nm FR VGA has an addition gain stage i... 
#define VGAHIGAIN_RLMS_B_RLMS18_MASK 	 0x04U
#define VGAHIGAIN_RLMS_B_RLMS18_POS  	 2U

#define RLMS_B_RLMS1F_ADDR    	 0x151FU
#define RLMS_B_RLMS1F_DEFAULT 	 0xA7U

#define AGCINITG2_RLMS_B_RLMS1F_ADDR 	 0x151FU // AGC initial value for G2 mode  
#define AGCINITG2_RLMS_B_RLMS1F_MASK 	 0xFFU
#define AGCINITG2_RLMS_B_RLMS1F_POS  	 0U

#define RLMS_B_RLMS21_ADDR    	 0x1521U
#define RLMS_B_RLMS21_DEFAULT 	 0x04U

#define BSTMUH_RLMS_B_RLMS21_ADDR 	 0x1521U // BST adapt gain MSB  
#define BSTMUH_RLMS_B_RLMS21_MASK 	 0x3FU
#define BSTMUH_RLMS_B_RLMS21_POS  	 0U

#define RLMS_B_RLMS23_ADDR    	 0x1523U
#define RLMS_B_RLMS23_DEFAULT 	 0x45U

#define BSTINIT_RLMS_B_RLMS23_ADDR 	 0x1523U // BST initial value  
#define BSTINIT_RLMS_B_RLMS23_MASK 	 0x3FU
#define BSTINIT_RLMS_B_RLMS23_POS  	 0U

#define RLMS_B_RLMS31_ADDR    	 0x1531U
#define RLMS_B_RLMS31_DEFAULT 	 0x18U

#define OSNMUH_RLMS_B_RLMS31_ADDR 	 0x1531U // OSN adapt gain MSB  
#define OSNMUH_RLMS_B_RLMS31_MASK 	 0x3FU
#define OSNMUH_RLMS_B_RLMS31_POS  	 0U

#define RLMS_B_RLMS3E_ADDR    	 0x153EU
#define RLMS_B_RLMS3E_DEFAULT 	 0x94U

#define ERRCHPHSECFR6G_RLMS_B_RLMS3E_ADDR 	 0x153EU // Error channel sampling point phase adjus... 
#define ERRCHPHSECFR6G_RLMS_B_RLMS3E_MASK 	 0x7FU
#define ERRCHPHSECFR6G_RLMS_B_RLMS3E_POS  	 0U

#define ERRCHPHSECTAFR6G_RLMS_B_RLMS3E_ADDR 	 0x153EU // Error channel phase secondary timing adj... 
#define ERRCHPHSECTAFR6G_RLMS_B_RLMS3E_MASK 	 0x80U
#define ERRCHPHSECTAFR6G_RLMS_B_RLMS3E_POS  	 7U

#define RLMS_B_RLMS3F_ADDR    	 0x153FU
#define RLMS_B_RLMS3F_DEFAULT 	 0x54U

#define ERRCHPHPRIFR6G_RLMS_B_RLMS3F_ADDR 	 0x153FU // Error channel sampling point phase adjus... 
#define ERRCHPHPRIFR6G_RLMS_B_RLMS3F_MASK 	 0x7FU
#define ERRCHPHPRIFR6G_RLMS_B_RLMS3F_POS  	 0U

#define ERRCHPHPRITAFR6G_RLMS_B_RLMS3F_ADDR 	 0x153FU // Error channel phase primary timing adjus... 
#define ERRCHPHPRITAFR6G_RLMS_B_RLMS3F_MASK 	 0x80U
#define ERRCHPHPRITAFR6G_RLMS_B_RLMS3F_POS  	 7U

#define RLMS_B_RLMS45_ADDR    	 0x1545U
#define RLMS_B_RLMS45_DEFAULT 	 0xC8U

#define CRUSSCSELSREN_RLMS_B_RLMS45_ADDR 	 0x1545U // Override enable for CRU SSC SEL for Slow... 
#define CRUSSCSELSREN_RLMS_B_RLMS45_MASK 	 0x40U
#define CRUSSCSELSREN_RLMS_B_RLMS45_POS  	 6U

#define CRULPCTRLSREN_RLMS_B_RLMS45_ADDR 	 0x1545U // Override enable for CRU Loop control. Wh... 
#define CRULPCTRLSREN_RLMS_B_RLMS45_MASK 	 0x80U
#define CRULPCTRLSREN_RLMS_B_RLMS45_POS  	 7U

#define RLMS_B_RLMS46_ADDR    	 0x1546U
#define RLMS_B_RLMS46_DEFAULT 	 0xB3U

#define CRULPCTRL_RLMS_B_RLMS46_ADDR 	 0x1546U // CRU loop control for Fast Receiver and S... 
#define CRULPCTRL_RLMS_B_RLMS46_MASK 	 0x07U
#define CRULPCTRL_RLMS_B_RLMS46_POS  	 0U

#define RLMS_B_RLMS47_ADDR    	 0x1547U
#define RLMS_B_RLMS47_DEFAULT 	 0x03U

#define CRUSSCSEL_RLMS_B_RLMS47_ADDR 	 0x1547U // CRU spread spectrum adjust select  
#define CRUSSCSEL_RLMS_B_RLMS47_MASK 	 0x06U
#define CRUSSCSEL_RLMS_B_RLMS47_POS  	 1U

#define RLMS_B_RLMS49_ADDR    	 0x1549U
#define RLMS_B_RLMS49_DEFAULT 	 0xF5U

#define ERRCHPWRUP_RLMS_B_RLMS49_ADDR 	 0x1549U // Error channel power down  
#define ERRCHPWRUP_RLMS_B_RLMS49_MASK 	 0x04U
#define ERRCHPWRUP_RLMS_B_RLMS49_POS  	 2U

#define RLMS_B_RLMS64_ADDR    	 0x1564U
#define RLMS_B_RLMS64_DEFAULT 	 0x90U

#define TXSSCMODE_RLMS_B_RLMS64_ADDR 	 0x1564U // Tx Spread Spectrum Mode  
#define TXSSCMODE_RLMS_B_RLMS64_MASK 	 0x03U
#define TXSSCMODE_RLMS_B_RLMS64_POS  	 0U

#define RLMS_B_RLMS70_ADDR    	 0x1570U
#define RLMS_B_RLMS70_DEFAULT 	 0x01U

#define TXSSCFRQCTRL_RLMS_B_RLMS70_ADDR 	 0x1570U // Tx SSC modulation amplitude (frequency d... 
#define TXSSCFRQCTRL_RLMS_B_RLMS70_MASK 	 0x7FU
#define TXSSCFRQCTRL_RLMS_B_RLMS70_POS  	 0U

#define RLMS_B_RLMS71_ADDR    	 0x1571U
#define RLMS_B_RLMS71_DEFAULT 	 0x02U

#define TXSSCEN_RLMS_B_RLMS71_ADDR 	 0x1571U // Tx spread spectrum enable  
#define TXSSCEN_RLMS_B_RLMS71_MASK 	 0x01U
#define TXSSCEN_RLMS_B_RLMS71_POS  	 0U

#define TXSSCCENSPRST_RLMS_B_RLMS71_ADDR 	 0x1571U // Tx SSC center spread starting phase  
#define TXSSCCENSPRST_RLMS_B_RLMS71_MASK 	 0x7EU
#define TXSSCCENSPRST_RLMS_B_RLMS71_POS  	 1U

#define RLMS_B_RLMS72_ADDR    	 0x1572U
#define RLMS_B_RLMS72_DEFAULT 	 0xCFU

#define TXSSCPRESCLL_RLMS_B_RLMS72_ADDR 	 0x1572U // Tx SSC frequency prescaler bits 7:0. Dec... 
#define TXSSCPRESCLL_RLMS_B_RLMS72_MASK 	 0xFFU
#define TXSSCPRESCLL_RLMS_B_RLMS72_POS  	 0U

#define RLMS_B_RLMS73_ADDR    	 0x1573U
#define RLMS_B_RLMS73_DEFAULT 	 0x00U

#define TXSSCPRESCLH_RLMS_B_RLMS73_ADDR 	 0x1573U // Tx SSC frequency prescaler bits 10:8. De... 
#define TXSSCPRESCLH_RLMS_B_RLMS73_MASK 	 0x07U
#define TXSSCPRESCLH_RLMS_B_RLMS73_POS  	 0U

#define RLMS_B_RLMS74_ADDR    	 0x1574U
#define RLMS_B_RLMS74_DEFAULT 	 0x00U

#define TXSSCPHL_RLMS_B_RLMS74_ADDR 	 0x1574U // Tx SSC phase accumulator increment bits ... 
#define TXSSCPHL_RLMS_B_RLMS74_MASK 	 0xFFU
#define TXSSCPHL_RLMS_B_RLMS74_POS  	 0U

#define RLMS_B_RLMS75_ADDR    	 0x1575U
#define RLMS_B_RLMS75_DEFAULT 	 0x00U

#define TXSSCPHH_RLMS_B_RLMS75_ADDR 	 0x1575U // Tx SSC phase accumulator increment bits ... 
#define TXSSCPHH_RLMS_B_RLMS75_MASK 	 0x7FU
#define TXSSCPHH_RLMS_B_RLMS75_POS  	 0U

#define RLMS_B_RLMS8C_ADDR    	 0x158CU
#define RLMS_B_RLMS8C_DEFAULT 	 0x00U

#define CAP_PRE_OUT_RLMS_RLMS_B_RLMS8C_ADDR 	 0x158CU // cap_preout value during RLMS if overridd... 
#define CAP_PRE_OUT_RLMS_RLMS_B_RLMS8C_MASK 	 0x7FU
#define CAP_PRE_OUT_RLMS_RLMS_B_RLMS8C_POS  	 0U

#define RLMS_B_RLMS95_ADDR    	 0x1595U
#define RLMS_B_RLMS95_DEFAULT 	 0x69U

#define TXAMPLMAN_RLMS_B_RLMS95_ADDR 	 0x1595U // TX amplitude  
#define TXAMPLMAN_RLMS_B_RLMS95_MASK 	 0x3FU
#define TXAMPLMAN_RLMS_B_RLMS95_POS  	 0U

#define TXAMPLMANEN_RLMS_B_RLMS95_ADDR 	 0x1595U // TX amplitude manual override  
#define TXAMPLMANEN_RLMS_B_RLMS95_MASK 	 0x80U
#define TXAMPLMANEN_RLMS_B_RLMS95_POS  	 7U

#define RLMS_B_RLMS98_ADDR    	 0x1598U
#define RLMS_B_RLMS98_DEFAULT 	 0x40U

#define CAL_CAP_PRE_OUT_EN_RLMS_B_RLMS98_ADDR 	 0x1598U // Enable manual override for cap_pre_out d... 
#define CAL_CAP_PRE_OUT_EN_RLMS_B_RLMS98_MASK 	 0x80U
#define CAL_CAP_PRE_OUT_EN_RLMS_B_RLMS98_POS  	 7U

#define RLMS_B_RLMSA4_ADDR    	 0x15A4U
#define RLMS_B_RLMSA4_DEFAULT 	 0xBDU

#define AEQ_PER_RLMS_B_RLMSA4_ADDR 	 0x15A4U // Adaptive EQ period  
#define AEQ_PER_RLMS_B_RLMSA4_MASK 	 0x3FU
#define AEQ_PER_RLMS_B_RLMSA4_POS  	 0U

#define AEQ_PER_MULT_RLMS_B_RLMSA4_ADDR 	 0x15A4U // Adaptive EQ period multiplier  
#define AEQ_PER_MULT_RLMS_B_RLMSA4_MASK 	 0xC0U
#define AEQ_PER_MULT_RLMS_B_RLMSA4_POS  	 6U

#define RLMS_B_RLMSA5_ADDR    	 0x15A5U
#define RLMS_B_RLMSA5_DEFAULT 	 0x50U

#define PHYC_WBLOCK_DLY_RLMS_B_RLMSA5_ADDR 	 0x15A5U // PHY controller word boundary lock start ... 
#define PHYC_WBLOCK_DLY_RLMS_B_RLMSA5_MASK 	 0x30U
#define PHYC_WBLOCK_DLY_RLMS_B_RLMSA5_POS  	 4U

#define RLMS_B_RLMSA7_ADDR    	 0x15A7U
#define RLMS_B_RLMSA7_DEFAULT 	 0x01U

#define MAN_CTRL_EN_RLMS_B_RLMSA7_ADDR 	 0x15A7U // PHY controller manual mode enable  
#define MAN_CTRL_EN_RLMS_B_RLMSA7_MASK 	 0x80U
#define MAN_CTRL_EN_RLMS_B_RLMSA7_POS  	 7U

#define RLMS_B_RLMSA8_ADDR    	 0x15A8U
#define RLMS_B_RLMSA8_DEFAULT 	 0x00U

#define FW_PHY_RSTB_RLMS_B_RLMSA8_ADDR 	 0x15A8U // Override PHY controller output  
#define FW_PHY_RSTB_RLMS_B_RLMSA8_MASK 	 0x20U
#define FW_PHY_RSTB_RLMS_B_RLMSA8_POS  	 5U

#define FW_PHY_PU_TX_RLMS_B_RLMSA8_ADDR 	 0x15A8U // Override PHY controller output  
#define FW_PHY_PU_TX_RLMS_B_RLMSA8_MASK 	 0x40U
#define FW_PHY_PU_TX_RLMS_B_RLMSA8_POS  	 6U

#define FW_PHY_CTRL_RLMS_B_RLMSA8_ADDR 	 0x15A8U // PHY controller firmware mode enable  
#define FW_PHY_CTRL_RLMS_B_RLMSA8_MASK 	 0x80U
#define FW_PHY_CTRL_RLMS_B_RLMSA8_POS  	 7U

#define RLMS_B_RLMSA9_ADDR    	 0x15A9U
#define RLMS_B_RLMSA9_DEFAULT 	 0x00U

#define FW_RXD_EN_RLMS_B_RLMSA9_ADDR 	 0x15A9U // Override PHY controller output  
#define FW_RXD_EN_RLMS_B_RLMSA9_MASK 	 0x08U
#define FW_RXD_EN_RLMS_B_RLMSA9_POS  	 3U

#define FW_TXD_EN_RLMS_B_RLMSA9_ADDR 	 0x15A9U // Override PHY controller output  
#define FW_TXD_EN_RLMS_B_RLMSA9_MASK 	 0x10U
#define FW_TXD_EN_RLMS_B_RLMSA9_POS  	 4U

#define FW_TXD_SQUELCH_RLMS_B_RLMSA9_ADDR 	 0x15A9U // Override PHY controller output  
#define FW_TXD_SQUELCH_RLMS_B_RLMSA9_MASK 	 0x20U
#define FW_TXD_SQUELCH_RLMS_B_RLMSA9_POS  	 5U

#define FW_REPCAL_RSTB_RLMS_B_RLMSA9_ADDR 	 0x15A9U // Override PHY controller output  
#define FW_REPCAL_RSTB_RLMS_B_RLMSA9_MASK 	 0x80U
#define FW_REPCAL_RSTB_RLMS_B_RLMSA9_POS  	 7U

#define RLMS_B_RLMSAC_ADDR    	 0x15ACU
#define RLMS_B_RLMSAC_DEFAULT 	 0xA0U

#define ERRCHPHSECFR3G_RLMS_B_RLMSAC_ADDR 	 0x15ACU // Error channel phase secondary (odd)  
#define ERRCHPHSECFR3G_RLMS_B_RLMSAC_MASK 	 0x7FU
#define ERRCHPHSECFR3G_RLMS_B_RLMSAC_POS  	 0U

#define RLMS_B_RLMSAD_ADDR    	 0x15ADU
#define RLMS_B_RLMSAD_DEFAULT 	 0x60U

#define ERRCHPHPRIFR3G_RLMS_B_RLMSAD_ADDR 	 0x15ADU // Error channel phase primary (even)  
#define ERRCHPHPRIFR3G_RLMS_B_RLMSAD_MASK 	 0x7FU
#define ERRCHPHPRIFR3G_RLMS_B_RLMSAD_POS  	 0U

#define DPLL_CSI1_DPLL_0_ADDR    	 0x1C00U
#define DPLL_CSI1_DPLL_0_DEFAULT 	 0xF5U

#define CONFIG_SOFT_RST_N_DPLL_CSI1_DPLL_0_ADDR 	 0x1C00U // Setting this to 0 resets the PLL functio... 
#define CONFIG_SOFT_RST_N_DPLL_CSI1_DPLL_0_MASK 	 0x01U
#define CONFIG_SOFT_RST_N_DPLL_CSI1_DPLL_0_POS  	 0U

#define DPLL_CSI1_DPLL_3_ADDR    	 0x1C03U
#define DPLL_CSI1_DPLL_3_DEFAULT 	 0x82U

#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI1_DPLL_3_ADDR 	 0x1C03U // Forces all divider values to come from i... 
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI1_DPLL_3_MASK 	 0x10U
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI1_DPLL_3_POS  	 4U

#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI1_DPLL_3_ADDR 	 0x1C03U // Bypasses internal pll_mode controls and ... 
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI1_DPLL_3_MASK 	 0x20U
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI1_DPLL_3_POS  	 5U

#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI1_DPLL_3_ADDR 	 0x1C03U // Forces div_out_exp to 7, which disables ... 
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI1_DPLL_3_MASK 	 0x40U
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI1_DPLL_3_POS  	 6U

#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI1_DPLL_3_ADDR 	 0x1C03U // When 1, i_sel_clock_out is used to selec... 
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI1_DPLL_3_MASK 	 0x80U
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI1_DPLL_3_POS  	 7U

#define DPLL_CSI1_DPLL_7_ADDR    	 0x1C07U
#define DPLL_CSI1_DPLL_7_DEFAULT 	 0x04U

#define CONFIG_DIV_IN_DPLL_CSI1_DPLL_7_ADDR 	 0x1C07U // Sets the divide value of the input divid... 
#define CONFIG_DIV_IN_DPLL_CSI1_DPLL_7_MASK 	 0x7CU
#define CONFIG_DIV_IN_DPLL_CSI1_DPLL_7_POS  	 2U

#define CONFIG_DIV_FB_L_DPLL_CSI1_DPLL_7_ADDR 	 0x1C07U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_L_DPLL_CSI1_DPLL_7_MASK 	 0x80U
#define CONFIG_DIV_FB_L_DPLL_CSI1_DPLL_7_POS  	 7U

#define DPLL_CSI1_DPLL_8_ADDR    	 0x1C08U
#define DPLL_CSI1_DPLL_8_DEFAULT 	 0x14U

#define CONFIG_DIV_FB_H_DPLL_CSI1_DPLL_8_ADDR 	 0x1C08U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_H_DPLL_CSI1_DPLL_8_MASK 	 0xFFU
#define CONFIG_DIV_FB_H_DPLL_CSI1_DPLL_8_POS  	 0U

#define DPLL_CSI1_DPLL_10_ADDR    	 0x1C0AU
#define DPLL_CSI1_DPLL_10_DEFAULT 	 0x81U

#define CONFIG_DIV_OUT_EXP_DPLL_CSI1_DPLL_10_ADDR 	 0x1C0AU // Sets the output exponential divider valu... 
#define CONFIG_DIV_OUT_EXP_DPLL_CSI1_DPLL_10_MASK 	 0x70U
#define CONFIG_DIV_OUT_EXP_DPLL_CSI1_DPLL_10_POS  	 4U

#define DPLL_CSI2_DPLL_0_ADDR    	 0x1D00U
#define DPLL_CSI2_DPLL_0_DEFAULT 	 0xF5U

#define CONFIG_SOFT_RST_N_DPLL_CSI2_DPLL_0_ADDR 	 0x1D00U // Setting this to 0 resets the PLL functio... 
#define CONFIG_SOFT_RST_N_DPLL_CSI2_DPLL_0_MASK 	 0x01U
#define CONFIG_SOFT_RST_N_DPLL_CSI2_DPLL_0_POS  	 0U

#define DPLL_CSI2_DPLL_3_ADDR    	 0x1D03U
#define DPLL_CSI2_DPLL_3_DEFAULT 	 0x82U

#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI2_DPLL_3_ADDR 	 0x1D03U // Forces all divider values to come from i... 
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI2_DPLL_3_MASK 	 0x10U
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI2_DPLL_3_POS  	 4U

#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI2_DPLL_3_ADDR 	 0x1D03U // Bypasses internal pll_mode controls and ... 
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI2_DPLL_3_MASK 	 0x20U
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI2_DPLL_3_POS  	 5U

#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI2_DPLL_3_ADDR 	 0x1D03U // Forces div_out_exp to 7, which disables ... 
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI2_DPLL_3_MASK 	 0x40U
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI2_DPLL_3_POS  	 6U

#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI2_DPLL_3_ADDR 	 0x1D03U // When 1, i_sel_clock_out is used to selec... 
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI2_DPLL_3_MASK 	 0x80U
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI2_DPLL_3_POS  	 7U

#define DPLL_CSI2_DPLL_7_ADDR    	 0x1D07U
#define DPLL_CSI2_DPLL_7_DEFAULT 	 0x04U

#define CONFIG_DIV_IN_DPLL_CSI2_DPLL_7_ADDR 	 0x1D07U // Sets the divide value of the input divid... 
#define CONFIG_DIV_IN_DPLL_CSI2_DPLL_7_MASK 	 0x7CU
#define CONFIG_DIV_IN_DPLL_CSI2_DPLL_7_POS  	 2U

#define CONFIG_DIV_FB_L_DPLL_CSI2_DPLL_7_ADDR 	 0x1D07U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_L_DPLL_CSI2_DPLL_7_MASK 	 0x80U
#define CONFIG_DIV_FB_L_DPLL_CSI2_DPLL_7_POS  	 7U

#define DPLL_CSI2_DPLL_8_ADDR    	 0x1D08U
#define DPLL_CSI2_DPLL_8_DEFAULT 	 0x14U

#define CONFIG_DIV_FB_H_DPLL_CSI2_DPLL_8_ADDR 	 0x1D08U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_H_DPLL_CSI2_DPLL_8_MASK 	 0xFFU
#define CONFIG_DIV_FB_H_DPLL_CSI2_DPLL_8_POS  	 0U

#define DPLL_CSI2_DPLL_10_ADDR    	 0x1D0AU
#define DPLL_CSI2_DPLL_10_DEFAULT 	 0x81U

#define CONFIG_DIV_OUT_EXP_DPLL_CSI2_DPLL_10_ADDR 	 0x1D0AU // Sets the output exponential divider valu... 
#define CONFIG_DIV_OUT_EXP_DPLL_CSI2_DPLL_10_MASK 	 0x70U
#define CONFIG_DIV_OUT_EXP_DPLL_CSI2_DPLL_10_POS  	 4U

#define DPLL_CSI3_DPLL_0_ADDR    	 0x1E00U
#define DPLL_CSI3_DPLL_0_DEFAULT 	 0xF5U

#define CONFIG_SOFT_RST_N_DPLL_CSI3_DPLL_0_ADDR 	 0x1E00U // Setting this to 0 resets the PLL functio... 
#define CONFIG_SOFT_RST_N_DPLL_CSI3_DPLL_0_MASK 	 0x01U
#define CONFIG_SOFT_RST_N_DPLL_CSI3_DPLL_0_POS  	 0U

#define DPLL_CSI3_DPLL_3_ADDR    	 0x1E03U
#define DPLL_CSI3_DPLL_3_DEFAULT 	 0x82U

#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI3_DPLL_3_ADDR 	 0x1E03U // Forces all divider values to come from i... 
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI3_DPLL_3_MASK 	 0x10U
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI3_DPLL_3_POS  	 4U

#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI3_DPLL_3_ADDR 	 0x1E03U // Bypasses internal pll_mode controls and ... 
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI3_DPLL_3_MASK 	 0x20U
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI3_DPLL_3_POS  	 5U

#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI3_DPLL_3_ADDR 	 0x1E03U // Forces div_out_exp to 7, which disables ... 
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI3_DPLL_3_MASK 	 0x40U
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI3_DPLL_3_POS  	 6U

#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI3_DPLL_3_ADDR 	 0x1E03U // When 1, i_sel_clock_out is used to selec... 
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI3_DPLL_3_MASK 	 0x80U
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI3_DPLL_3_POS  	 7U

#define DPLL_CSI3_DPLL_7_ADDR    	 0x1E07U
#define DPLL_CSI3_DPLL_7_DEFAULT 	 0x04U

#define CONFIG_DIV_IN_DPLL_CSI3_DPLL_7_ADDR 	 0x1E07U // Sets the divide value of the input divid... 
#define CONFIG_DIV_IN_DPLL_CSI3_DPLL_7_MASK 	 0x7CU
#define CONFIG_DIV_IN_DPLL_CSI3_DPLL_7_POS  	 2U

#define CONFIG_DIV_FB_L_DPLL_CSI3_DPLL_7_ADDR 	 0x1E07U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_L_DPLL_CSI3_DPLL_7_MASK 	 0x80U
#define CONFIG_DIV_FB_L_DPLL_CSI3_DPLL_7_POS  	 7U

#define DPLL_CSI3_DPLL_8_ADDR    	 0x1E08U
#define DPLL_CSI3_DPLL_8_DEFAULT 	 0x14U

#define CONFIG_DIV_FB_H_DPLL_CSI3_DPLL_8_ADDR 	 0x1E08U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_H_DPLL_CSI3_DPLL_8_MASK 	 0xFFU
#define CONFIG_DIV_FB_H_DPLL_CSI3_DPLL_8_POS  	 0U

#define DPLL_CSI3_DPLL_10_ADDR    	 0x1E0AU
#define DPLL_CSI3_DPLL_10_DEFAULT 	 0x81U

#define CONFIG_DIV_OUT_EXP_DPLL_CSI3_DPLL_10_ADDR 	 0x1E0AU // Sets the output exponential divider valu... 
#define CONFIG_DIV_OUT_EXP_DPLL_CSI3_DPLL_10_MASK 	 0x70U
#define CONFIG_DIV_OUT_EXP_DPLL_CSI3_DPLL_10_POS  	 4U

#define DPLL_CSI4_DPLL_0_ADDR    	 0x1F00U
#define DPLL_CSI4_DPLL_0_DEFAULT 	 0xF5U

#define CONFIG_SOFT_RST_N_DPLL_CSI4_DPLL_0_ADDR 	 0x1F00U // Setting this to 0 resets the PLL functio... 
#define CONFIG_SOFT_RST_N_DPLL_CSI4_DPLL_0_MASK 	 0x01U
#define CONFIG_SOFT_RST_N_DPLL_CSI4_DPLL_0_POS  	 0U

#define DPLL_CSI4_DPLL_3_ADDR    	 0x1F03U
#define DPLL_CSI4_DPLL_3_DEFAULT 	 0x82U

#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI4_DPLL_3_ADDR 	 0x1F03U // Forces all divider values to come from i... 
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI4_DPLL_3_MASK 	 0x10U
#define CONFIG_USE_INTERNAL_DIVIDER_VALUES_DPLL_CSI4_DPLL_3_POS  	 4U

#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI4_DPLL_3_ADDR 	 0x1F03U // Bypasses internal pll_mode controls and ... 
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI4_DPLL_3_MASK 	 0x20U
#define CONFIG_USE_INTERNAL_PLL_MODE_VALUES_DPLL_CSI4_DPLL_3_POS  	 5U

#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI4_DPLL_3_ADDR 	 0x1F03U // Forces div_out_exp to 7, which disables ... 
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI4_DPLL_3_MASK 	 0x40U
#define CONFIG_DISABLE_DIV_OUT_EXP_DPLL_CSI4_DPLL_3_POS  	 6U

#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI4_DPLL_3_ADDR 	 0x1F03U // When 1, i_sel_clock_out is used to selec... 
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI4_DPLL_3_MASK 	 0x80U
#define CONFIG_SEL_CLOCK_OUT_USE_EXTERNAL_DPLL_CSI4_DPLL_3_POS  	 7U

#define DPLL_CSI4_DPLL_7_ADDR    	 0x1F07U
#define DPLL_CSI4_DPLL_7_DEFAULT 	 0x04U

#define CONFIG_DIV_IN_DPLL_CSI4_DPLL_7_ADDR 	 0x1F07U // Sets the divide value of the input divid... 
#define CONFIG_DIV_IN_DPLL_CSI4_DPLL_7_MASK 	 0x7CU
#define CONFIG_DIV_IN_DPLL_CSI4_DPLL_7_POS  	 2U

#define CONFIG_DIV_FB_L_DPLL_CSI4_DPLL_7_ADDR 	 0x1F07U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_L_DPLL_CSI4_DPLL_7_MASK 	 0x80U
#define CONFIG_DIV_FB_L_DPLL_CSI4_DPLL_7_POS  	 7U

#define DPLL_CSI4_DPLL_8_ADDR    	 0x1F08U
#define DPLL_CSI4_DPLL_8_DEFAULT 	 0x14U

#define CONFIG_DIV_FB_H_DPLL_CSI4_DPLL_8_ADDR 	 0x1F08U // Sets the feedback divider value when i_d... 
#define CONFIG_DIV_FB_H_DPLL_CSI4_DPLL_8_MASK 	 0xFFU
#define CONFIG_DIV_FB_H_DPLL_CSI4_DPLL_8_POS  	 0U

#define DPLL_CSI4_DPLL_10_ADDR    	 0x1F0AU
#define DPLL_CSI4_DPLL_10_DEFAULT 	 0x81U

#define CONFIG_DIV_OUT_EXP_DPLL_CSI4_DPLL_10_ADDR 	 0x1F0AU // Sets the output exponential divider valu... 
#define CONFIG_DIV_OUT_EXP_DPLL_CSI4_DPLL_10_MASK 	 0x70U
#define CONFIG_DIV_OUT_EXP_DPLL_CSI4_DPLL_10_POS  	 4U

#define FEC_CLEAR_STATS_ADDR    	 0x2000U
#define FEC_CLEAR_STATS_DEFAULT 	 0x00U

#define CLEAR_ALL_STATS_FEC_CLEAR_STATS_ADDR 	 0x2000U // Clear all FEC stats and counters  
#define CLEAR_ALL_STATS_FEC_CLEAR_STATS_MASK 	 0x01U
#define CLEAR_ALL_STATS_FEC_CLEAR_STATS_POS  	 0U

#define CLEAR_BLOCKS_PROCESSED_FEC_CLEAR_STATS_ADDR 	 0x2000U // Clear counter for number of FEC blocks p... 
#define CLEAR_BLOCKS_PROCESSED_FEC_CLEAR_STATS_MASK 	 0x02U
#define CLEAR_BLOCKS_PROCESSED_FEC_CLEAR_STATS_POS  	 1U

#define CLEAR_BLOCKS_UNCORRECTABLE_FEC_CLEAR_STATS_ADDR 	 0x2000U // Clear counter for number of uncorrectabl... 
#define CLEAR_BLOCKS_UNCORRECTABLE_FEC_CLEAR_STATS_MASK 	 0x04U
#define CLEAR_BLOCKS_UNCORRECTABLE_FEC_CLEAR_STATS_POS  	 2U

#define CLEAR_BITS_CORRECTED_FEC_CLEAR_STATS_ADDR 	 0x2000U // Clear counter for number of bits correct... 
#define CLEAR_BITS_CORRECTED_FEC_CLEAR_STATS_MASK 	 0x08U
#define CLEAR_BITS_CORRECTED_FEC_CLEAR_STATS_POS  	 3U

#define FEC_STATS_CONTROL_ADDR    	 0x2001U
#define FEC_STATS_CONTROL_DEFAULT 	 0x00U

#define STATS_ENABLE_FEC_STATS_CONTROL_ADDR 	 0x2001U // Enable FEC stats collection for Link A  
#define STATS_ENABLE_FEC_STATS_CONTROL_MASK 	 0x01U
#define STATS_ENABLE_FEC_STATS_CONTROL_POS  	 0U

#define FEC_CORRECTED_THRESHOLD_0_ADDR    	 0x2008U
#define FEC_CORRECTED_THRESHOLD_0_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_0_FEC_CORRECTED_THRESHOLD_0_ADDR 	 0x2008U // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_0_FEC_CORRECTED_THRESHOLD_0_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_0_FEC_CORRECTED_THRESHOLD_0_POS  	 0U

#define FEC_CORRECTED_THRESHOLD_1_ADDR    	 0x2009U
#define FEC_CORRECTED_THRESHOLD_1_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_1_FEC_CORRECTED_THRESHOLD_1_ADDR 	 0x2009U // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_1_FEC_CORRECTED_THRESHOLD_1_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_1_FEC_CORRECTED_THRESHOLD_1_POS  	 0U

#define FEC_CORRECTED_THRESHOLD_2_ADDR    	 0x200AU
#define FEC_CORRECTED_THRESHOLD_2_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_2_FEC_CORRECTED_THRESHOLD_2_ADDR 	 0x200AU // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_2_FEC_CORRECTED_THRESHOLD_2_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_2_FEC_CORRECTED_THRESHOLD_2_POS  	 0U

#define FEC_CORRECTED_THRESHOLD_3_ADDR    	 0x200BU
#define FEC_CORRECTED_THRESHOLD_3_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_3_FEC_CORRECTED_THRESHOLD_3_ADDR 	 0x200BU // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_3_FEC_CORRECTED_THRESHOLD_3_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_3_FEC_CORRECTED_THRESHOLD_3_POS  	 0U

#define FEC_ERROR_THRESHOLD_0_ADDR    	 0x200CU
#define FEC_ERROR_THRESHOLD_0_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_0_FEC_ERROR_THRESHOLD_0_ADDR 	 0x200CU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_0_FEC_ERROR_THRESHOLD_0_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_0_FEC_ERROR_THRESHOLD_0_POS  	 0U

#define FEC_ERROR_THRESHOLD_1_ADDR    	 0x200DU
#define FEC_ERROR_THRESHOLD_1_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_1_FEC_ERROR_THRESHOLD_1_ADDR 	 0x200DU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_1_FEC_ERROR_THRESHOLD_1_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_1_FEC_ERROR_THRESHOLD_1_POS  	 0U

#define FEC_ERROR_THRESHOLD_2_ADDR    	 0x200EU
#define FEC_ERROR_THRESHOLD_2_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_2_FEC_ERROR_THRESHOLD_2_ADDR 	 0x200EU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_2_FEC_ERROR_THRESHOLD_2_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_2_FEC_ERROR_THRESHOLD_2_POS  	 0U

#define FEC_ERROR_THRESHOLD_3_ADDR    	 0x200FU
#define FEC_ERROR_THRESHOLD_3_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_3_FEC_ERROR_THRESHOLD_3_ADDR 	 0x200FU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_3_FEC_ERROR_THRESHOLD_3_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_3_FEC_ERROR_THRESHOLD_3_POS  	 0U

#define FEC_BLOCKS_UNCORRECTABLE_0_ADDR    	 0x2020U
#define FEC_BLOCKS_UNCORRECTABLE_0_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_0_FEC_BLOCKS_UNCORRECTABLE_0_ADDR 	 0x2020U // Number of uncorrectable blocks. Bits 7:0... 
#define UNCORRECTABLE_BLOCKS_0_FEC_BLOCKS_UNCORRECTABLE_0_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_0_FEC_BLOCKS_UNCORRECTABLE_0_POS  	 0U

#define FEC_BLOCKS_UNCORRECTABLE_1_ADDR    	 0x2021U
#define FEC_BLOCKS_UNCORRECTABLE_1_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_1_FEC_BLOCKS_UNCORRECTABLE_1_ADDR 	 0x2021U // Number of uncorrectable blocks. Bits 15:... 
#define UNCORRECTABLE_BLOCKS_1_FEC_BLOCKS_UNCORRECTABLE_1_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_1_FEC_BLOCKS_UNCORRECTABLE_1_POS  	 0U

#define FEC_BLOCKS_UNCORRECTABLE_2_ADDR    	 0x2022U
#define FEC_BLOCKS_UNCORRECTABLE_2_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_2_FEC_BLOCKS_UNCORRECTABLE_2_ADDR 	 0x2022U // Number of uncorrectable blocks. Bits 23:... 
#define UNCORRECTABLE_BLOCKS_2_FEC_BLOCKS_UNCORRECTABLE_2_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_2_FEC_BLOCKS_UNCORRECTABLE_2_POS  	 0U

#define FEC_BLOCKS_UNCORRECTABLE_3_ADDR    	 0x2023U
#define FEC_BLOCKS_UNCORRECTABLE_3_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_3_FEC_BLOCKS_UNCORRECTABLE_3_ADDR 	 0x2023U // Number of uncorrectable blocks. Bits 31:... 
#define UNCORRECTABLE_BLOCKS_3_FEC_BLOCKS_UNCORRECTABLE_3_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_3_FEC_BLOCKS_UNCORRECTABLE_3_POS  	 0U

#define FEC_BITS_CORRECTED_0_ADDR    	 0x2024U
#define FEC_BITS_CORRECTED_0_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_0_FEC_BITS_CORRECTED_0_ADDR 	 0x2024U // Number of bit errors corrected. Bits 7:0... 
#define BIT_ERRS_CORRECTED_0_FEC_BITS_CORRECTED_0_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_0_FEC_BITS_CORRECTED_0_POS  	 0U

#define FEC_BITS_CORRECTED_1_ADDR    	 0x2025U
#define FEC_BITS_CORRECTED_1_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_1_FEC_BITS_CORRECTED_1_ADDR 	 0x2025U // Number of bit errors corrected. Bits 15:... 
#define BIT_ERRS_CORRECTED_1_FEC_BITS_CORRECTED_1_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_1_FEC_BITS_CORRECTED_1_POS  	 0U

#define FEC_BITS_CORRECTED_2_ADDR    	 0x2026U
#define FEC_BITS_CORRECTED_2_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_2_FEC_BITS_CORRECTED_2_ADDR 	 0x2026U // Number of bit errors corrected. Bits 23:... 
#define BIT_ERRS_CORRECTED_2_FEC_BITS_CORRECTED_2_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_2_FEC_BITS_CORRECTED_2_POS  	 0U

#define FEC_BITS_CORRECTED_3_ADDR    	 0x2027U
#define FEC_BITS_CORRECTED_3_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_3_FEC_BITS_CORRECTED_3_ADDR 	 0x2027U // Number of bit errors corrected. Bits 31:... 
#define BIT_ERRS_CORRECTED_3_FEC_BITS_CORRECTED_3_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_3_FEC_BITS_CORRECTED_3_POS  	 0U

#define FEC_BLOCKS_PROCESSED_0_ADDR    	 0x2028U
#define FEC_BLOCKS_PROCESSED_0_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_0_FEC_BLOCKS_PROCESSED_0_ADDR 	 0x2028U // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_0_FEC_BLOCKS_PROCESSED_0_MASK 	 0xFFU
#define BLOCKS_PROCESSED_0_FEC_BLOCKS_PROCESSED_0_POS  	 0U

#define FEC_BLOCKS_PROCESSED_1_ADDR    	 0x2029U
#define FEC_BLOCKS_PROCESSED_1_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_1_FEC_BLOCKS_PROCESSED_1_ADDR 	 0x2029U // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_1_FEC_BLOCKS_PROCESSED_1_MASK 	 0xFFU
#define BLOCKS_PROCESSED_1_FEC_BLOCKS_PROCESSED_1_POS  	 0U

#define FEC_BLOCKS_PROCESSED_2_ADDR    	 0x202AU
#define FEC_BLOCKS_PROCESSED_2_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_2_FEC_BLOCKS_PROCESSED_2_ADDR 	 0x202AU // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_2_FEC_BLOCKS_PROCESSED_2_MASK 	 0xFFU
#define BLOCKS_PROCESSED_2_FEC_BLOCKS_PROCESSED_2_POS  	 0U

#define FEC_BLOCKS_PROCESSED_3_ADDR    	 0x202BU
#define FEC_BLOCKS_PROCESSED_3_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_3_FEC_BLOCKS_PROCESSED_3_ADDR 	 0x202BU // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_3_FEC_BLOCKS_PROCESSED_3_MASK 	 0xFFU
#define BLOCKS_PROCESSED_3_FEC_BLOCKS_PROCESSED_3_POS  	 0U

#define FEC_B_CLEAR_STATS_ADDR    	 0x2100U
#define FEC_B_CLEAR_STATS_DEFAULT 	 0x00U

#define CLEAR_ALL_STATS_B_FEC_B_CLEAR_STATS_ADDR 	 0x2100U // Clear all FEC stats and counters  
#define CLEAR_ALL_STATS_B_FEC_B_CLEAR_STATS_MASK 	 0x01U
#define CLEAR_ALL_STATS_B_FEC_B_CLEAR_STATS_POS  	 0U

#define CLEAR_BLOCKS_PROCESSED_B_FEC_B_CLEAR_STATS_ADDR 	 0x2100U // Clear counter for number of FEC blocks p... 
#define CLEAR_BLOCKS_PROCESSED_B_FEC_B_CLEAR_STATS_MASK 	 0x02U
#define CLEAR_BLOCKS_PROCESSED_B_FEC_B_CLEAR_STATS_POS  	 1U

#define CLEAR_BLOCKS_UNCORRECTABLE_B_FEC_B_CLEAR_STATS_ADDR 	 0x2100U // Clear counter for number of uncorrectabl... 
#define CLEAR_BLOCKS_UNCORRECTABLE_B_FEC_B_CLEAR_STATS_MASK 	 0x04U
#define CLEAR_BLOCKS_UNCORRECTABLE_B_FEC_B_CLEAR_STATS_POS  	 2U

#define CLEAR_BITS_CORRECTED_B_FEC_B_CLEAR_STATS_ADDR 	 0x2100U // Clear counter for number of bits correct... 
#define CLEAR_BITS_CORRECTED_B_FEC_B_CLEAR_STATS_MASK 	 0x08U
#define CLEAR_BITS_CORRECTED_B_FEC_B_CLEAR_STATS_POS  	 3U

#define FEC_B_STATS_CONTROL_ADDR    	 0x2101U
#define FEC_B_STATS_CONTROL_DEFAULT 	 0x00U

#define STATS_ENABLE_B_FEC_B_STATS_CONTROL_ADDR 	 0x2101U // Enable FEC stats collection for Link B  
#define STATS_ENABLE_B_FEC_B_STATS_CONTROL_MASK 	 0x01U
#define STATS_ENABLE_B_FEC_B_STATS_CONTROL_POS  	 0U

#define FEC_B_CORRECTED_THRESHOLD_0_ADDR    	 0x2108U
#define FEC_B_CORRECTED_THRESHOLD_0_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_0_B_FEC_B_CORRECTED_THRESHOLD_0_ADDR 	 0x2108U // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_0_B_FEC_B_CORRECTED_THRESHOLD_0_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_0_B_FEC_B_CORRECTED_THRESHOLD_0_POS  	 0U

#define FEC_B_CORRECTED_THRESHOLD_1_ADDR    	 0x2109U
#define FEC_B_CORRECTED_THRESHOLD_1_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_1_B_FEC_B_CORRECTED_THRESHOLD_1_ADDR 	 0x2109U // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_1_B_FEC_B_CORRECTED_THRESHOLD_1_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_1_B_FEC_B_CORRECTED_THRESHOLD_1_POS  	 0U

#define FEC_B_CORRECTED_THRESHOLD_2_ADDR    	 0x210AU
#define FEC_B_CORRECTED_THRESHOLD_2_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_2_B_FEC_B_CORRECTED_THRESHOLD_2_ADDR 	 0x210AU // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_2_B_FEC_B_CORRECTED_THRESHOLD_2_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_2_B_FEC_B_CORRECTED_THRESHOLD_2_POS  	 0U

#define FEC_B_CORRECTED_THRESHOLD_3_ADDR    	 0x210BU
#define FEC_B_CORRECTED_THRESHOLD_3_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_THRESHOLD_3_B_FEC_B_CORRECTED_THRESHOLD_3_ADDR 	 0x210BU // Threshold (decimal value) for number of ... 
#define BIT_ERRS_CORRECTED_THRESHOLD_3_B_FEC_B_CORRECTED_THRESHOLD_3_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_THRESHOLD_3_B_FEC_B_CORRECTED_THRESHOLD_3_POS  	 0U

#define FEC_B_ERROR_THRESHOLD_0_ADDR    	 0x210CU
#define FEC_B_ERROR_THRESHOLD_0_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_0_B_FEC_B_ERROR_THRESHOLD_0_ADDR 	 0x210CU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_0_B_FEC_B_ERROR_THRESHOLD_0_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_0_B_FEC_B_ERROR_THRESHOLD_0_POS  	 0U

#define FEC_B_ERROR_THRESHOLD_1_ADDR    	 0x210DU
#define FEC_B_ERROR_THRESHOLD_1_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_1_B_FEC_B_ERROR_THRESHOLD_1_ADDR 	 0x210DU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_1_B_FEC_B_ERROR_THRESHOLD_1_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_1_B_FEC_B_ERROR_THRESHOLD_1_POS  	 0U

#define FEC_B_ERROR_THRESHOLD_2_ADDR    	 0x210EU
#define FEC_B_ERROR_THRESHOLD_2_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_2_B_FEC_B_ERROR_THRESHOLD_2_ADDR 	 0x210EU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_2_B_FEC_B_ERROR_THRESHOLD_2_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_2_B_FEC_B_ERROR_THRESHOLD_2_POS  	 0U

#define FEC_B_ERROR_THRESHOLD_3_ADDR    	 0x210FU
#define FEC_B_ERROR_THRESHOLD_3_DEFAULT 	 0x00U

#define UNCORRECTED_ERROR_THRESHOLD_3_B_FEC_B_ERROR_THRESHOLD_3_ADDR 	 0x210FU // Threshold (decimal value) for number of ... 
#define UNCORRECTED_ERROR_THRESHOLD_3_B_FEC_B_ERROR_THRESHOLD_3_MASK 	 0xFFU
#define UNCORRECTED_ERROR_THRESHOLD_3_B_FEC_B_ERROR_THRESHOLD_3_POS  	 0U

#define FEC_B_BLOCKS_UNCORRECTABLE_0_ADDR    	 0x2120U
#define FEC_B_BLOCKS_UNCORRECTABLE_0_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_0_B_FEC_B_BLOCKS_UNCORRECTABLE_0_ADDR 	 0x2120U // Number of uncorrectable blocks. Bits 7:0... 
#define UNCORRECTABLE_BLOCKS_0_B_FEC_B_BLOCKS_UNCORRECTABLE_0_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_0_B_FEC_B_BLOCKS_UNCORRECTABLE_0_POS  	 0U

#define FEC_B_BLOCKS_UNCORRECTABLE_1_ADDR    	 0x2121U
#define FEC_B_BLOCKS_UNCORRECTABLE_1_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_1_B_FEC_B_BLOCKS_UNCORRECTABLE_1_ADDR 	 0x2121U // Number of uncorrectable blocks. Bits 15:... 
#define UNCORRECTABLE_BLOCKS_1_B_FEC_B_BLOCKS_UNCORRECTABLE_1_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_1_B_FEC_B_BLOCKS_UNCORRECTABLE_1_POS  	 0U

#define FEC_B_BLOCKS_UNCORRECTABLE_2_ADDR    	 0x2122U
#define FEC_B_BLOCKS_UNCORRECTABLE_2_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_2_B_FEC_B_BLOCKS_UNCORRECTABLE_2_ADDR 	 0x2122U // Number of uncorrectable blocks. Bits 23:... 
#define UNCORRECTABLE_BLOCKS_2_B_FEC_B_BLOCKS_UNCORRECTABLE_2_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_2_B_FEC_B_BLOCKS_UNCORRECTABLE_2_POS  	 0U

#define FEC_B_BLOCKS_UNCORRECTABLE_3_ADDR    	 0x2123U
#define FEC_B_BLOCKS_UNCORRECTABLE_3_DEFAULT 	 0x00U

#define UNCORRECTABLE_BLOCKS_3_B_FEC_B_BLOCKS_UNCORRECTABLE_3_ADDR 	 0x2123U // Number of uncorrectable blocks. Bits 31:... 
#define UNCORRECTABLE_BLOCKS_3_B_FEC_B_BLOCKS_UNCORRECTABLE_3_MASK 	 0xFFU
#define UNCORRECTABLE_BLOCKS_3_B_FEC_B_BLOCKS_UNCORRECTABLE_3_POS  	 0U

#define FEC_B_BITS_CORRECTED_0_ADDR    	 0x2124U
#define FEC_B_BITS_CORRECTED_0_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_0_B_FEC_B_BITS_CORRECTED_0_ADDR 	 0x2124U // Number of bit errors corrected. Bits 7:0... 
#define BIT_ERRS_CORRECTED_0_B_FEC_B_BITS_CORRECTED_0_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_0_B_FEC_B_BITS_CORRECTED_0_POS  	 0U

#define FEC_B_BITS_CORRECTED_1_ADDR    	 0x2125U
#define FEC_B_BITS_CORRECTED_1_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_1_B_FEC_B_BITS_CORRECTED_1_ADDR 	 0x2125U // Number of bit errors corrected. Bits 15:... 
#define BIT_ERRS_CORRECTED_1_B_FEC_B_BITS_CORRECTED_1_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_1_B_FEC_B_BITS_CORRECTED_1_POS  	 0U

#define FEC_B_BITS_CORRECTED_2_ADDR    	 0x2126U
#define FEC_B_BITS_CORRECTED_2_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_2_B_FEC_B_BITS_CORRECTED_2_ADDR 	 0x2126U // Number of bit errors corrected. Bits 23:... 
#define BIT_ERRS_CORRECTED_2_B_FEC_B_BITS_CORRECTED_2_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_2_B_FEC_B_BITS_CORRECTED_2_POS  	 0U

#define FEC_B_BITS_CORRECTED_3_ADDR    	 0x2127U
#define FEC_B_BITS_CORRECTED_3_DEFAULT 	 0x00U

#define BIT_ERRS_CORRECTED_3_B_FEC_B_BITS_CORRECTED_3_ADDR 	 0x2127U // Number of bit errors corrected. Bits 31:... 
#define BIT_ERRS_CORRECTED_3_B_FEC_B_BITS_CORRECTED_3_MASK 	 0xFFU
#define BIT_ERRS_CORRECTED_3_B_FEC_B_BITS_CORRECTED_3_POS  	 0U

#define FEC_B_BLOCKS_PROCESSED_0_ADDR    	 0x2128U
#define FEC_B_BLOCKS_PROCESSED_0_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_0_B_FEC_B_BLOCKS_PROCESSED_0_ADDR 	 0x2128U // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_0_B_FEC_B_BLOCKS_PROCESSED_0_MASK 	 0xFFU
#define BLOCKS_PROCESSED_0_B_FEC_B_BLOCKS_PROCESSED_0_POS  	 0U

#define FEC_B_BLOCKS_PROCESSED_1_ADDR    	 0x2129U
#define FEC_B_BLOCKS_PROCESSED_1_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_1_B_FEC_B_BLOCKS_PROCESSED_1_ADDR 	 0x2129U // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_1_B_FEC_B_BLOCKS_PROCESSED_1_MASK 	 0xFFU
#define BLOCKS_PROCESSED_1_B_FEC_B_BLOCKS_PROCESSED_1_POS  	 0U

#define FEC_B_BLOCKS_PROCESSED_2_ADDR    	 0x212AU
#define FEC_B_BLOCKS_PROCESSED_2_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_2_B_FEC_B_BLOCKS_PROCESSED_2_ADDR 	 0x212AU // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_2_B_FEC_B_BLOCKS_PROCESSED_2_MASK 	 0xFFU
#define BLOCKS_PROCESSED_2_B_FEC_B_BLOCKS_PROCESSED_2_POS  	 0U

#define FEC_B_BLOCKS_PROCESSED_3_ADDR    	 0x212BU
#define FEC_B_BLOCKS_PROCESSED_3_DEFAULT 	 0x00U

#define BLOCKS_PROCESSED_3_B_FEC_B_BLOCKS_PROCESSED_3_ADDR 	 0x212BU // Number of 120-bit blocks processed. Bits... 
#define BLOCKS_PROCESSED_3_B_FEC_B_BLOCKS_PROCESSED_3_MASK 	 0xFFU
#define BLOCKS_PROCESSED_3_B_FEC_B_BLOCKS_PROCESSED_3_POS  	 0U

#define FUNC_SAFE_REGCRC0_ADDR    	 0x3000U
#define FUNC_SAFE_REGCRC0_DEFAULT 	 0x00U

#define RESET_CRC_FUNC_SAFE_REGCRC0_ADDR 	 0x3000U // Reset CRC value to 16'FFFF.  
#define RESET_CRC_FUNC_SAFE_REGCRC0_MASK 	 0x01U
#define RESET_CRC_FUNC_SAFE_REGCRC0_POS  	 0U

#define CHECK_CRC_FUNC_SAFE_REGCRC0_ADDR 	 0x3000U // Upon calculation of CRC, compare with pr... 
#define CHECK_CRC_FUNC_SAFE_REGCRC0_MASK 	 0x02U
#define CHECK_CRC_FUNC_SAFE_REGCRC0_POS  	 1U

#define PERIODIC_COMPUTE_FUNC_SAFE_REGCRC0_ADDR 	 0x3000U // Perform CRC check on a periodic basis, b... 
#define PERIODIC_COMPUTE_FUNC_SAFE_REGCRC0_MASK 	 0x04U
#define PERIODIC_COMPUTE_FUNC_SAFE_REGCRC0_POS  	 2U

#define I2C_WR_COMPUTE_FUNC_SAFE_REGCRC0_ADDR 	 0x3000U // Execute CRC computation after every I2C ... 
#define I2C_WR_COMPUTE_FUNC_SAFE_REGCRC0_MASK 	 0x08U
#define I2C_WR_COMPUTE_FUNC_SAFE_REGCRC0_POS  	 3U

#define GEN_ROLLING_CRC_FUNC_SAFE_REGCRC0_ADDR 	 0x3000U // Calculate CRC using additional 2-bit cou... 
#define GEN_ROLLING_CRC_FUNC_SAFE_REGCRC0_MASK 	 0x10U
#define GEN_ROLLING_CRC_FUNC_SAFE_REGCRC0_POS  	 4U

#define FUNC_SAFE_REGCRC1_ADDR    	 0x3001U
#define FUNC_SAFE_REGCRC1_DEFAULT 	 0x00U

#define CRC_PERIOD_FUNC_SAFE_REGCRC1_ADDR 	 0x3001U // Period for CRC recomputation.  
#define CRC_PERIOD_FUNC_SAFE_REGCRC1_MASK 	 0xFFU
#define CRC_PERIOD_FUNC_SAFE_REGCRC1_POS  	 0U

#define FUNC_SAFE_REGCRC2_ADDR    	 0x3002U
#define FUNC_SAFE_REGCRC2_DEFAULT 	 0x00U

#define REGCRC_LSB_FUNC_SAFE_REGCRC2_ADDR 	 0x3002U // CRC result LSB  
#define REGCRC_LSB_FUNC_SAFE_REGCRC2_MASK 	 0xFFU
#define REGCRC_LSB_FUNC_SAFE_REGCRC2_POS  	 0U

#define FUNC_SAFE_REGCRC3_ADDR    	 0x3003U
#define FUNC_SAFE_REGCRC3_DEFAULT 	 0x00U

#define REGCRC_MSB_FUNC_SAFE_REGCRC3_ADDR 	 0x3003U // CRC result MSB  
#define REGCRC_MSB_FUNC_SAFE_REGCRC3_MASK 	 0xFFU
#define REGCRC_MSB_FUNC_SAFE_REGCRC3_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC0_ADDR    	 0x3008U
#define FUNC_SAFE_I2C_UART_CRC0_DEFAULT 	 0x00U

#define RESET_MSGCNTR_FUNC_SAFE_I2C_UART_CRC0_ADDR 	 0x3008U // Reset Message Counter Value to 0  
#define RESET_MSGCNTR_FUNC_SAFE_I2C_UART_CRC0_MASK 	 0x01U
#define RESET_MSGCNTR_FUNC_SAFE_I2C_UART_CRC0_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC1_ADDR    	 0x3009U
#define FUNC_SAFE_I2C_UART_CRC1_DEFAULT 	 0x00U

#define RESET_CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_ADDR 	 0x3009U // Reset CRC Error Count to 0  
#define RESET_CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_MASK 	 0x01U
#define RESET_CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_POS  	 0U

#define RESET_MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_ADDR 	 0x3009U // Reset Message Counter Error Count to 0  
#define RESET_MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_MASK 	 0x02U
#define RESET_MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC1_POS  	 1U

#define CRC_ERR_THR_FUNC_SAFE_I2C_UART_CRC1_ADDR 	 0x3009U // I2C/UART CRC error reporting threshold  
#define CRC_ERR_THR_FUNC_SAFE_I2C_UART_CRC1_MASK 	 0x1CU
#define CRC_ERR_THR_FUNC_SAFE_I2C_UART_CRC1_POS  	 2U

#define MSGCNTR_ERR_THR_FUNC_SAFE_I2C_UART_CRC1_ADDR 	 0x3009U // I2C/UART CRC error reporting threshold  
#define MSGCNTR_ERR_THR_FUNC_SAFE_I2C_UART_CRC1_MASK 	 0xE0U
#define MSGCNTR_ERR_THR_FUNC_SAFE_I2C_UART_CRC1_POS  	 5U

#define FUNC_SAFE_I2C_UART_CRC2_ADDR    	 0x300AU
#define FUNC_SAFE_I2C_UART_CRC2_DEFAULT 	 0x00U

#define CRC_VAL_FUNC_SAFE_I2C_UART_CRC2_ADDR 	 0x300AU // Calculated CRC value for the last write ... 
#define CRC_VAL_FUNC_SAFE_I2C_UART_CRC2_MASK 	 0xFFU
#define CRC_VAL_FUNC_SAFE_I2C_UART_CRC2_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC3_ADDR    	 0x300BU
#define FUNC_SAFE_I2C_UART_CRC3_DEFAULT 	 0x00U

#define MSGCNTR_LSB_FUNC_SAFE_I2C_UART_CRC3_ADDR 	 0x300BU // Bits 7:0 of current Message Counter valu... 
#define MSGCNTR_LSB_FUNC_SAFE_I2C_UART_CRC3_MASK 	 0xFFU
#define MSGCNTR_LSB_FUNC_SAFE_I2C_UART_CRC3_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC4_ADDR    	 0x300CU
#define FUNC_SAFE_I2C_UART_CRC4_DEFAULT 	 0x00U

#define MSGCNTR_MSB_FUNC_SAFE_I2C_UART_CRC4_ADDR 	 0x300CU // Bits 7:0 of current Message Counter valu... 
#define MSGCNTR_MSB_FUNC_SAFE_I2C_UART_CRC4_MASK 	 0xFFU
#define MSGCNTR_MSB_FUNC_SAFE_I2C_UART_CRC4_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC5_ADDR    	 0x300DU
#define FUNC_SAFE_I2C_UART_CRC5_DEFAULT 	 0x00U

#define CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC5_ADDR 	 0x300DU // Number of CRC errors observed  
#define CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC5_MASK 	 0xFFU
#define CRC_ERR_CNT_FUNC_SAFE_I2C_UART_CRC5_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC6_ADDR    	 0x300EU
#define FUNC_SAFE_I2C_UART_CRC6_DEFAULT 	 0x00U

#define MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC6_ADDR 	 0x300EU // Number of Message Counter errors observe... 
#define MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC6_MASK 	 0xFFU
#define MSGCNTR_ERR_CNT_FUNC_SAFE_I2C_UART_CRC6_POS  	 0U

#define FUNC_SAFE_I2C_UART_CRC7_ADDR    	 0x300FU
#define FUNC_SAFE_I2C_UART_CRC7_DEFAULT 	 0x06U

#define CC_CRC_MSGCNTR_OVR_FUNC_SAFE_I2C_UART_CRC7_ADDR 	 0x300FU // Enable I2C/UART CRC or Message Counter o... 
#define CC_CRC_MSGCNTR_OVR_FUNC_SAFE_I2C_UART_CRC7_MASK 	 0x01U
#define CC_CRC_MSGCNTR_OVR_FUNC_SAFE_I2C_UART_CRC7_POS  	 0U

#define CC_CRC_EN_FUNC_SAFE_I2C_UART_CRC7_ADDR 	 0x300FU // Enable I2C/UART CRC override when set to... 
#define CC_CRC_EN_FUNC_SAFE_I2C_UART_CRC7_MASK 	 0x02U
#define CC_CRC_EN_FUNC_SAFE_I2C_UART_CRC7_POS  	 1U

#define CC_MSGCNTR_EN_FUNC_SAFE_I2C_UART_CRC7_ADDR 	 0x300FU // Enable I2C/UART Message Counter override... 
#define CC_MSGCNTR_EN_FUNC_SAFE_I2C_UART_CRC7_MASK 	 0x04U
#define CC_MSGCNTR_EN_FUNC_SAFE_I2C_UART_CRC7_POS  	 2U

#define MSGCNTR_PORT_SEL_FUNC_SAFE_I2C_UART_CRC7_ADDR 	 0x300FU // Selects the current message/CRC counter  
#define MSGCNTR_PORT_SEL_FUNC_SAFE_I2C_UART_CRC7_MASK 	 0x18U
#define MSGCNTR_PORT_SEL_FUNC_SAFE_I2C_UART_CRC7_POS  	 3U

#define FUNC_SAFE_FS_INTR0_ADDR    	 0x3010U
#define FUNC_SAFE_FS_INTR0_DEFAULT 	 0xE2U

#define REG_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x3010U // Enable reporting register CRC at ERRB pi... 
#define REG_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x01U
#define REG_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 0U

#define EFUSE_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x3010U // Enable reporting eFuse CRC at ERRB pin  
#define EFUSE_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x02U
#define EFUSE_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 1U

#define MEM_ECC_ERR1_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x3010U // Enable reporting of memory ECC 1-bit cor... 
#define MEM_ECC_ERR1_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x10U
#define MEM_ECC_ERR1_OEN_FUNC_SAFE_FS_INTR0_POS  	 4U

#define MEM_ECC_ERR2_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x3010U // Enable reporting of memory ECC 2-bit unc... 
#define MEM_ECC_ERR2_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x20U
#define MEM_ECC_ERR2_OEN_FUNC_SAFE_FS_INTR0_POS  	 5U

#define I2C_UART_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x3010U // Enable reporting of I2C/UART CRC errors ... 
#define I2C_UART_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x40U
#define I2C_UART_CRC_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 6U

#define I2C_UART_MSGCNTR_ERR_OEN_FUNC_SAFE_FS_INTR0_ADDR 	 0x3010U // Enable reporting of I2C/UART Message Cou... 
#define I2C_UART_MSGCNTR_ERR_OEN_FUNC_SAFE_FS_INTR0_MASK 	 0x80U
#define I2C_UART_MSGCNTR_ERR_OEN_FUNC_SAFE_FS_INTR0_POS  	 7U

#define FUNC_SAFE_FS_INTR1_ADDR    	 0x3011U
#define FUNC_SAFE_FS_INTR1_DEFAULT 	 0x00U

#define REG_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_ADDR 	 0x3011U // An error occurred on the register CRC ca... 
#define REG_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_MASK 	 0x01U
#define REG_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_POS  	 0U

#define EFUSE_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_ADDR 	 0x3011U // An error occurred on the eFuse CRC calcu... 
#define EFUSE_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_MASK 	 0x02U
#define EFUSE_CRC_ERR_FLAG_FUNC_SAFE_FS_INTR1_POS  	 1U

#define MEM_ECC_ERR1_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x3011U // Decoding error flag for 1-bit correctabl... 
#define MEM_ECC_ERR1_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x10U
#define MEM_ECC_ERR1_INT_FUNC_SAFE_FS_INTR1_POS  	 4U

#define MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x3011U // Decoding error flag for 2-bit uncorrecta... 
#define MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x20U
#define MEM_ECC_ERR2_INT_FUNC_SAFE_FS_INTR1_POS  	 5U

#define I2C_UART_CRC_ERR_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x3011U // I2C/UART CRC error, asserted when CRC_ER... 
#define I2C_UART_CRC_ERR_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x40U
#define I2C_UART_CRC_ERR_INT_FUNC_SAFE_FS_INTR1_POS  	 6U

#define I2C_UART_MSGCNTR_ERR_INT_FUNC_SAFE_FS_INTR1_ADDR 	 0x3011U // I2C/UART Message Counter error, asserted... 
#define I2C_UART_MSGCNTR_ERR_INT_FUNC_SAFE_FS_INTR1_MASK 	 0x80U
#define I2C_UART_MSGCNTR_ERR_INT_FUNC_SAFE_FS_INTR1_POS  	 7U

#define FUNC_SAFE_MEM_ECC0_ADDR    	 0x3016U
#define FUNC_SAFE_MEM_ECC0_DEFAULT 	 0x00U

#define RESET_MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC0_ADDR 	 0x3016U // Reset memory ECC 1-bit error count to 0  
#define RESET_MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC0_MASK 	 0x01U
#define RESET_MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC0_POS  	 0U

#define RESET_MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC0_ADDR 	 0x3016U // Reset memory ECC 2-bit error count to 0  
#define RESET_MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC0_MASK 	 0x02U
#define RESET_MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC0_POS  	 1U

#define MEM_ECC_ERR1_THR_FUNC_SAFE_MEM_ECC0_ADDR 	 0x3016U // Decoding and error reporting threshold  
#define MEM_ECC_ERR1_THR_FUNC_SAFE_MEM_ECC0_MASK 	 0x1CU
#define MEM_ECC_ERR1_THR_FUNC_SAFE_MEM_ECC0_POS  	 2U

#define MEM_ECC_ERR2_THR_FUNC_SAFE_MEM_ECC0_ADDR 	 0x3016U // Decoding and error reporting threshold.  
#define MEM_ECC_ERR2_THR_FUNC_SAFE_MEM_ECC0_MASK 	 0xE0U
#define MEM_ECC_ERR2_THR_FUNC_SAFE_MEM_ECC0_POS  	 5U

#define FUNC_SAFE_MEM_ECC1_ADDR    	 0x3017U
#define FUNC_SAFE_MEM_ECC1_DEFAULT 	 0x00U

#define MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC1_ADDR 	 0x3017U // Number of 1-bit correctable memory ECC e... 
#define MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC1_MASK 	 0xFFU
#define MEM_ECC_ERR1_CNT_FUNC_SAFE_MEM_ECC1_POS  	 0U

#define FUNC_SAFE_MEM_ECC2_ADDR    	 0x3018U
#define FUNC_SAFE_MEM_ECC2_DEFAULT 	 0x00U

#define MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC2_ADDR 	 0x3018U // Number of 2-bit uncorrectable memory ECC... 
#define MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC2_MASK 	 0xFFU
#define MEM_ECC_ERR2_CNT_FUNC_SAFE_MEM_ECC2_POS  	 0U

#define FUNC_SAFE_REG_POST0_ADDR    	 0x3020U
#define FUNC_SAFE_REG_POST0_DEFAULT 	 0x00U

#define POST_RUN_LBIST_FUNC_SAFE_REG_POST0_ADDR 	 0x3020U // Indicates if this device is enabled to r... 
#define POST_RUN_LBIST_FUNC_SAFE_REG_POST0_MASK 	 0x01U
#define POST_RUN_LBIST_FUNC_SAFE_REG_POST0_POS  	 0U

#define POST_RUN_MBIST_FUNC_SAFE_REG_POST0_ADDR 	 0x3020U // Indicates if this device is enabled to r... 
#define POST_RUN_MBIST_FUNC_SAFE_REG_POST0_MASK 	 0x02U
#define POST_RUN_MBIST_FUNC_SAFE_REG_POST0_POS  	 1U

#define POST_LBIST_PASSED_FUNC_SAFE_REG_POST0_ADDR 	 0x3020U // LBIST passed during POST. Valid when POS... 
#define POST_LBIST_PASSED_FUNC_SAFE_REG_POST0_MASK 	 0x20U
#define POST_LBIST_PASSED_FUNC_SAFE_REG_POST0_POS  	 5U

#define POST_MBIST_PASSED_FUNC_SAFE_REG_POST0_ADDR 	 0x3020U // MBIST passed during POST. Valid when POS... 
#define POST_MBIST_PASSED_FUNC_SAFE_REG_POST0_MASK 	 0x40U
#define POST_MBIST_PASSED_FUNC_SAFE_REG_POST0_POS  	 6U

#define POST_DONE_FUNC_SAFE_REG_POST0_ADDR 	 0x3020U // Power-on-self-test (POST) (LBIST and/or ... 
#define POST_DONE_FUNC_SAFE_REG_POST0_MASK 	 0x80U
#define POST_DONE_FUNC_SAFE_REG_POST0_POS  	 7U

#define FUNC_SAFE_REGCRC8_ADDR    	 0x3030U
#define FUNC_SAFE_REGCRC8_DEFAULT 	 0xFFU

#define SKIP0_LSB_FUNC_SAFE_REGCRC8_ADDR 	 0x3030U // Address 0 to skip (LSB).  
#define SKIP0_LSB_FUNC_SAFE_REGCRC8_MASK 	 0xFFU
#define SKIP0_LSB_FUNC_SAFE_REGCRC8_POS  	 0U

#define FUNC_SAFE_REGCRC9_ADDR    	 0x3031U
#define FUNC_SAFE_REGCRC9_DEFAULT 	 0xFFU

#define SKIP0_MSB_FUNC_SAFE_REGCRC9_ADDR 	 0x3031U // Address 0 to skip (MSB).  
#define SKIP0_MSB_FUNC_SAFE_REGCRC9_MASK 	 0xFFU
#define SKIP0_MSB_FUNC_SAFE_REGCRC9_POS  	 0U

#define FUNC_SAFE_REGCRC10_ADDR    	 0x3032U
#define FUNC_SAFE_REGCRC10_DEFAULT 	 0xFFU

#define SKIP1_LSB_FUNC_SAFE_REGCRC10_ADDR 	 0x3032U // Address 1 to skip (LSB).  
#define SKIP1_LSB_FUNC_SAFE_REGCRC10_MASK 	 0xFFU
#define SKIP1_LSB_FUNC_SAFE_REGCRC10_POS  	 0U

#define FUNC_SAFE_REGCRC11_ADDR    	 0x3033U
#define FUNC_SAFE_REGCRC11_DEFAULT 	 0xFFU

#define SKIP1_MSB_FUNC_SAFE_REGCRC11_ADDR 	 0x3033U // Address 1 to skip (MSB).  
#define SKIP1_MSB_FUNC_SAFE_REGCRC11_MASK 	 0xFFU
#define SKIP1_MSB_FUNC_SAFE_REGCRC11_POS  	 0U

#define FUNC_SAFE_REGCRC12_ADDR    	 0x3034U
#define FUNC_SAFE_REGCRC12_DEFAULT 	 0xFFU

#define SKIP2_LSB_FUNC_SAFE_REGCRC12_ADDR 	 0x3034U // Address 2 to skip (LSB).  
#define SKIP2_LSB_FUNC_SAFE_REGCRC12_MASK 	 0xFFU
#define SKIP2_LSB_FUNC_SAFE_REGCRC12_POS  	 0U

#define FUNC_SAFE_REGCRC13_ADDR    	 0x3035U
#define FUNC_SAFE_REGCRC13_DEFAULT 	 0xFFU

#define SKIP2_MSB_FUNC_SAFE_REGCRC13_ADDR 	 0x3035U // Address 2 to skip (MSB).  
#define SKIP2_MSB_FUNC_SAFE_REGCRC13_MASK 	 0xFFU
#define SKIP2_MSB_FUNC_SAFE_REGCRC13_POS  	 0U

#define FUNC_SAFE_REGCRC14_ADDR    	 0x3036U
#define FUNC_SAFE_REGCRC14_DEFAULT 	 0xFFU

#define SKIP3_LSB_FUNC_SAFE_REGCRC14_ADDR 	 0x3036U // Address 3 to skip (LSB).  
#define SKIP3_LSB_FUNC_SAFE_REGCRC14_MASK 	 0xFFU
#define SKIP3_LSB_FUNC_SAFE_REGCRC14_POS  	 0U

#define FUNC_SAFE_REGCRC15_ADDR    	 0x3037U
#define FUNC_SAFE_REGCRC15_DEFAULT 	 0xFFU

#define SKIP3_MSB_FUNC_SAFE_REGCRC15_ADDR 	 0x3037U // Address 3 to skip (MSB).  
#define SKIP3_MSB_FUNC_SAFE_REGCRC15_MASK 	 0xFFU
#define SKIP3_MSB_FUNC_SAFE_REGCRC15_POS  	 0U

#define FUNC_SAFE_REGCRC16_ADDR    	 0x3038U
#define FUNC_SAFE_REGCRC16_DEFAULT 	 0xFFU

#define SKIP4_LSB_FUNC_SAFE_REGCRC16_ADDR 	 0x3038U // Address 4 to skip (LSB).  
#define SKIP4_LSB_FUNC_SAFE_REGCRC16_MASK 	 0xFFU
#define SKIP4_LSB_FUNC_SAFE_REGCRC16_POS  	 0U

#define FUNC_SAFE_REGCRC17_ADDR    	 0x3039U
#define FUNC_SAFE_REGCRC17_DEFAULT 	 0xFFU

#define SKIP4_MSB_FUNC_SAFE_REGCRC17_ADDR 	 0x3039U // Address 4 to skip (MSB).  
#define SKIP4_MSB_FUNC_SAFE_REGCRC17_MASK 	 0xFFU
#define SKIP4_MSB_FUNC_SAFE_REGCRC17_POS  	 0U

#define FUNC_SAFE_REGCRC18_ADDR    	 0x303AU
#define FUNC_SAFE_REGCRC18_DEFAULT 	 0xFFU

#define SKIP5_LSB_FUNC_SAFE_REGCRC18_ADDR 	 0x303AU // Address 5 to skip (LSB).  
#define SKIP5_LSB_FUNC_SAFE_REGCRC18_MASK 	 0xFFU
#define SKIP5_LSB_FUNC_SAFE_REGCRC18_POS  	 0U

#define FUNC_SAFE_REGCRC19_ADDR    	 0x303BU
#define FUNC_SAFE_REGCRC19_DEFAULT 	 0xFFU

#define SKIP5_MSB_FUNC_SAFE_REGCRC19_ADDR 	 0x303BU // Address 5 to skip (MSB).  
#define SKIP5_MSB_FUNC_SAFE_REGCRC19_MASK 	 0xFFU
#define SKIP5_MSB_FUNC_SAFE_REGCRC19_POS  	 0U

#define FUNC_SAFE_REGCRC20_ADDR    	 0x303CU
#define FUNC_SAFE_REGCRC20_DEFAULT 	 0xFFU

#define SKIP6_LSB_FUNC_SAFE_REGCRC20_ADDR 	 0x303CU // Address 6 to skip (LSB).  
#define SKIP6_LSB_FUNC_SAFE_REGCRC20_MASK 	 0xFFU
#define SKIP6_LSB_FUNC_SAFE_REGCRC20_POS  	 0U

#define FUNC_SAFE_REGCRC21_ADDR    	 0x303DU
#define FUNC_SAFE_REGCRC21_DEFAULT 	 0xFFU

#define SKIP6_MSB_FUNC_SAFE_REGCRC21_ADDR 	 0x303DU // Address 6 to skip (MSB).  
#define SKIP6_MSB_FUNC_SAFE_REGCRC21_MASK 	 0xFFU
#define SKIP6_MSB_FUNC_SAFE_REGCRC21_POS  	 0U

#define FUNC_SAFE_REGCRC22_ADDR    	 0x303EU
#define FUNC_SAFE_REGCRC22_DEFAULT 	 0xFFU

#define SKIP7_LSB_FUNC_SAFE_REGCRC22_ADDR 	 0x303EU // Address 7 to skip (LSB).  
#define SKIP7_LSB_FUNC_SAFE_REGCRC22_MASK 	 0xFFU
#define SKIP7_LSB_FUNC_SAFE_REGCRC22_POS  	 0U

#define FUNC_SAFE_REGCRC23_ADDR    	 0x303FU
#define FUNC_SAFE_REGCRC23_DEFAULT 	 0xFFU

#define SKIP7_MSB_FUNC_SAFE_REGCRC23_ADDR 	 0x303FU // Address 7 to skip (MSB).  
#define SKIP7_MSB_FUNC_SAFE_REGCRC23_MASK 	 0xFFU
#define SKIP7_MSB_FUNC_SAFE_REGCRC23_POS  	 0U

#define FUNC_SAFE_CC_RTTN_ERR_ADDR    	 0x304FU
#define FUNC_SAFE_CC_RTTN_ERR_DEFAULT 	 0x00U

#define INJECT_RTTN_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_ADDR 	 0x304FU // Set this bit before going into sleep mod... 
#define INJECT_RTTN_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_MASK 	 0x01U
#define INJECT_RTTN_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_POS  	 0U

#define INJECT_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_ADDR 	 0x304FU // Set this bit before reading eFuse values... 
#define INJECT_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_MASK 	 0x02U
#define INJECT_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_POS  	 1U

#define RESET_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_ADDR 	 0x304FU // Reset eFuse CRC error status to 0  
#define RESET_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_MASK 	 0x04U
#define RESET_EFUSE_CRC_ERR_FUNC_SAFE_CC_RTTN_ERR_POS  	 2U

#define TCTRL_EXT_CTRL9_ADDR    	 0x5009U
#define TCTRL_EXT_CTRL9_DEFAULT 	 0x00U

#define LOCKED_B_TCTRL_EXT_CTRL9_ADDR 	 0x5009U // GMSL link locked (bidirectional). For Li... 
#define LOCKED_B_TCTRL_EXT_CTRL9_MASK 	 0x08U
#define LOCKED_B_TCTRL_EXT_CTRL9_POS  	 3U

#define TCTRL_EXT_INTR10_ADDR    	 0x5010U
#define TCTRL_EXT_INTR10_DEFAULT 	 0x88U

#define VDD_OV_OEN_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable VDD overvoltage status on ERRB  
#define VDD_OV_OEN_TCTRL_EXT_INTR10_MASK 	 0x01U
#define VDD_OV_OEN_TCTRL_EXT_INTR10_POS  	 0U

#define PKT_CNT_OEN_B_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable reporting of Link B packet count ... 
#define PKT_CNT_OEN_B_TCTRL_EXT_INTR10_MASK 	 0x02U
#define PKT_CNT_OEN_B_TCTRL_EXT_INTR10_POS  	 1U

#define RT_CNT_OEN_B_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable reporting of Link B combined ARQ ... 
#define RT_CNT_OEN_B_TCTRL_EXT_INTR10_MASK 	 0x04U
#define RT_CNT_OEN_B_TCTRL_EXT_INTR10_POS  	 2U

#define MAX_RT_OEN_B_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable reporting of Link B combined ARQ ... 
#define MAX_RT_OEN_B_TCTRL_EXT_INTR10_MASK 	 0x08U
#define MAX_RT_OEN_B_TCTRL_EXT_INTR10_POS  	 3U

#define VDD18_OV_OEN_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable VDD18 overvoltage status on ERRB  
#define VDD18_OV_OEN_TCTRL_EXT_INTR10_MASK 	 0x10U
#define VDD18_OV_OEN_TCTRL_EXT_INTR10_POS  	 4U

#define FEC_RX_ERR_OEN_B_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable reporting of Link B FEC receive e... 
#define FEC_RX_ERR_OEN_B_TCTRL_EXT_INTR10_MASK 	 0x20U
#define FEC_RX_ERR_OEN_B_TCTRL_EXT_INTR10_POS  	 5U

#define IDLE_ERR_OEN_B_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Enable reporting of Link B idle-word err... 
#define IDLE_ERR_OEN_B_TCTRL_EXT_INTR10_MASK 	 0x40U
#define IDLE_ERR_OEN_B_TCTRL_EXT_INTR10_POS  	 6U

#define RTTN_CRC_ERR_OEN_TCTRL_EXT_INTR10_ADDR 	 0x5010U // Retention Memory Restore CRC Error Outpu... 
#define RTTN_CRC_ERR_OEN_TCTRL_EXT_INTR10_MASK 	 0x80U
#define RTTN_CRC_ERR_OEN_TCTRL_EXT_INTR10_POS  	 7U

#define TCTRL_EXT_INTR11_ADDR    	 0x5011U
#define TCTRL_EXT_INTR11_DEFAULT 	 0x00U

#define VDD_OV_FLAG_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Sticky status value for VDD overvoltage ... 
#define VDD_OV_FLAG_TCTRL_EXT_INTR11_MASK 	 0x01U
#define VDD_OV_FLAG_TCTRL_EXT_INTR11_POS  	 0U

#define PKT_CNT_FLAG_B_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Packet Count Flag for Link B  
#define PKT_CNT_FLAG_B_TCTRL_EXT_INTR11_MASK 	 0x02U
#define PKT_CNT_FLAG_B_TCTRL_EXT_INTR11_POS  	 1U

#define RT_CNT_FLAG_B_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Combined ARQ retransmission event flag f... 
#define RT_CNT_FLAG_B_TCTRL_EXT_INTR11_MASK 	 0x04U
#define RT_CNT_FLAG_B_TCTRL_EXT_INTR11_POS  	 2U

#define MAX_RT_FLAG_B_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Combined ARQ maximum retransmission limi... 
#define MAX_RT_FLAG_B_TCTRL_EXT_INTR11_MASK 	 0x08U
#define MAX_RT_FLAG_B_TCTRL_EXT_INTR11_POS  	 3U

#define VDD18_OV_FLAG_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Sticky status value for VDD overvoltage  
#define VDD18_OV_FLAG_TCTRL_EXT_INTR11_MASK 	 0x10U
#define VDD18_OV_FLAG_TCTRL_EXT_INTR11_POS  	 4U

#define FEC_RX_ERR_FLAG_B_TCTRL_EXT_INTR11_ADDR 	 0x5011U // FEC Receive Errors Flag for Link B  
#define FEC_RX_ERR_FLAG_B_TCTRL_EXT_INTR11_MASK 	 0x20U
#define FEC_RX_ERR_FLAG_B_TCTRL_EXT_INTR11_POS  	 5U

#define IDLE_ERR_FLAG_B_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Idle-Word Error Flag for Link B  
#define IDLE_ERR_FLAG_B_TCTRL_EXT_INTR11_MASK 	 0x40U
#define IDLE_ERR_FLAG_B_TCTRL_EXT_INTR11_POS  	 6U

#define RTTN_CRC_INT_TCTRL_EXT_INTR11_ADDR 	 0x5011U // Retention Memory Restore CRC Error Inter... 
#define RTTN_CRC_INT_TCTRL_EXT_INTR11_MASK 	 0x80U
#define RTTN_CRC_INT_TCTRL_EXT_INTR11_POS  	 7U

#define TCTRL_EXT_INTR13_ADDR    	 0x5012U
#define TCTRL_EXT_INTR13_DEFAULT 	 0x00U

#define LOSS_OF_LOCK_OEN_TCTRL_EXT_INTR13_ADDR 	 0x5012U // Enable reporting loss-of-lock detection ... 
#define LOSS_OF_LOCK_OEN_TCTRL_EXT_INTR13_MASK 	 0x01U
#define LOSS_OF_LOCK_OEN_TCTRL_EXT_INTR13_POS  	 0U

#define VIDEO_MEM_OVERFLOW_OEN_TCTRL_EXT_INTR13_ADDR 	 0x5012U // Enable reporting of BACKTOP video memory... 
#define VIDEO_MEM_OVERFLOW_OEN_TCTRL_EXT_INTR13_MASK 	 0x02U
#define VIDEO_MEM_OVERFLOW_OEN_TCTRL_EXT_INTR13_POS  	 1U

#define FEC_A_INACTIVE_OEN_TCTRL_EXT_INTR13_ADDR 	 0x5012U // Enable reporting of link A FEC handshake... 
#define FEC_A_INACTIVE_OEN_TCTRL_EXT_INTR13_MASK 	 0x40U
#define FEC_A_INACTIVE_OEN_TCTRL_EXT_INTR13_POS  	 6U

#define FEC_B_INACTIVE_OEN_TCTRL_EXT_INTR13_ADDR 	 0x5012U // Enable reporting of link B FEC handshake... 
#define FEC_B_INACTIVE_OEN_TCTRL_EXT_INTR13_MASK 	 0x80U
#define FEC_B_INACTIVE_OEN_TCTRL_EXT_INTR13_POS  	 7U

#define TCTRL_EXT_INTR14_ADDR    	 0x5013U
#define TCTRL_EXT_INTR14_DEFAULT 	 0x00U

#define LOSS_OF_LOCK_FLAG_TCTRL_EXT_INTR14_ADDR 	 0x5013U // Loss of lock detection flag (sticky)  
#define LOSS_OF_LOCK_FLAG_TCTRL_EXT_INTR14_MASK 	 0x01U
#define LOSS_OF_LOCK_FLAG_TCTRL_EXT_INTR14_POS  	 0U

#define VIDEO_MEM_OVERFLOW_TCTRL_EXT_INTR14_ADDR 	 0x5013U // Flag indicating that one or more of the ... 
#define VIDEO_MEM_OVERFLOW_TCTRL_EXT_INTR14_MASK 	 0x02U
#define VIDEO_MEM_OVERFLOW_TCTRL_EXT_INTR14_POS  	 1U

#define FEC_A_INACTIVE_TCTRL_EXT_INTR14_ADDR 	 0x5013U // Error flag indicating that when the FEC ... 
#define FEC_A_INACTIVE_TCTRL_EXT_INTR14_MASK 	 0x40U
#define FEC_A_INACTIVE_TCTRL_EXT_INTR14_POS  	 6U

#define FEC_B_INACTIVE_TCTRL_EXT_INTR14_ADDR 	 0x5013U // Error flag indicating that when the FEC ... 
#define FEC_B_INACTIVE_TCTRL_EXT_INTR14_MASK 	 0x80U
#define FEC_B_INACTIVE_TCTRL_EXT_INTR14_POS  	 7U

#define TCTRL_EXT_INTR12_ADDR    	 0x5018U
#define TCTRL_EXT_INTR12_DEFAULT 	 0x1FU

#define ERR_RX_ID_B_TCTRL_EXT_INTR12_ADDR 	 0x5018U // GPIO ID used for receiving ERR_RX for Li... 
#define ERR_RX_ID_B_TCTRL_EXT_INTR12_MASK 	 0x1FU
#define ERR_RX_ID_B_TCTRL_EXT_INTR12_POS  	 0U

#define TCTRL_EXT_CNT2_ADDR    	 0x5024U
#define TCTRL_EXT_CNT2_DEFAULT 	 0x00U

#define IDLE_ERR_B_TCTRL_EXT_CNT2_ADDR 	 0x5024U // Number of idle-word errors detected for ... 
#define IDLE_ERR_B_TCTRL_EXT_CNT2_MASK 	 0xFFU
#define IDLE_ERR_B_TCTRL_EXT_CNT2_POS  	 0U

#define TCTRL_EXT_CNT3_ADDR    	 0x5025U
#define TCTRL_EXT_CNT3_DEFAULT 	 0x00U

#define PKT_CNT_B_TCTRL_EXT_CNT3_ADDR 	 0x5025U // Number of received packets of a selected... 
#define PKT_CNT_B_TCTRL_EXT_CNT3_MASK 	 0xFFU
#define PKT_CNT_B_TCTRL_EXT_CNT3_POS  	 0U

#define VID_RX_EXT_Y_VIDEO_RX13_ADDR    	 0x501AU
#define VID_RX_EXT_Y_VIDEO_RX13_DEFAULT 	 0x00U

#define LOSS_OF_VIDEO_LOCK_OEN_VID_RX_EXT_Y_VIDEO_RX13_ADDR 	 0x501AU // Enable reporting loss of pipe Y video lo... 
#define LOSS_OF_VIDEO_LOCK_OEN_VID_RX_EXT_Y_VIDEO_RX13_MASK 	 0x01U
#define LOSS_OF_VIDEO_LOCK_OEN_VID_RX_EXT_Y_VIDEO_RX13_POS  	 0U

#define VID_RX_EXT_Y_VIDEO_RX14_ADDR    	 0x501BU
#define VID_RX_EXT_Y_VIDEO_RX14_DEFAULT 	 0x00U

#define LOSS_OF_VIDEO_LOCK_VID_RX_EXT_Y_VIDEO_RX14_ADDR 	 0x501BU // Loss of pipe Y video lock detection flag... 
#define LOSS_OF_VIDEO_LOCK_VID_RX_EXT_Y_VIDEO_RX14_MASK 	 0x01U
#define LOSS_OF_VIDEO_LOCK_VID_RX_EXT_Y_VIDEO_RX14_POS  	 0U

#define VID_RX_EXT_Z_VIDEO_RX13_ADDR    	 0x5020U
#define VID_RX_EXT_Z_VIDEO_RX13_DEFAULT 	 0x00U

#define LOSS_OF_VIDEO_LOCK_OEN_VID_RX_EXT_Z_VIDEO_RX13_ADDR 	 0x5020U // Enable reporting loss of pipe Z video lo... 
#define LOSS_OF_VIDEO_LOCK_OEN_VID_RX_EXT_Z_VIDEO_RX13_MASK 	 0x01U
#define LOSS_OF_VIDEO_LOCK_OEN_VID_RX_EXT_Z_VIDEO_RX13_POS  	 0U

#define VID_RX_EXT_Z_VIDEO_RX14_ADDR    	 0x5021U
#define VID_RX_EXT_Z_VIDEO_RX14_DEFAULT 	 0x00U

#define LOSS_OF_VIDEO_LOCK_VID_RX_EXT_Z_VIDEO_RX14_ADDR 	 0x5021U // Loss of pipe Z video lock detection flag... 
#define LOSS_OF_VIDEO_LOCK_VID_RX_EXT_Z_VIDEO_RX14_MASK 	 0x01U
#define LOSS_OF_VIDEO_LOCK_VID_RX_EXT_Z_VIDEO_RX14_POS  	 0U

#define GMSL_B_TX0_ADDR    	 0x5028U
#define GMSL_B_TX0_DEFAULT 	 0x60U

#define RX_FEC_EN_GMSL_B_TX0_ADDR 	 0x5028U // Enable forward error correction (FEC) on... 
#define RX_FEC_EN_GMSL_B_TX0_MASK 	 0x02U
#define RX_FEC_EN_GMSL_B_TX0_POS  	 1U

#define GMSL_B_TX1_ADDR    	 0x5029U
#define GMSL_B_TX1_DEFAULT 	 0x08U

#define ERRG_EN_B_GMSL_B_TX1_ADDR 	 0x5029U // Error generator enable for Link B (rever... 
#define ERRG_EN_B_GMSL_B_TX1_MASK 	 0x10U
#define ERRG_EN_B_GMSL_B_TX1_POS  	 4U

#define LINK_PRBS_GEN_GMSL_B_TX1_ADDR 	 0x5029U // Enable link PRBS-7 generator  
#define LINK_PRBS_GEN_GMSL_B_TX1_MASK 	 0x80U
#define LINK_PRBS_GEN_GMSL_B_TX1_POS  	 7U

#define GMSL_B_TX2_ADDR    	 0x502AU
#define GMSL_B_TX2_DEFAULT 	 0x20U

#define ERRG_PER_GMSL_B_TX2_ADDR 	 0x502AU // Error-generator error-distribution selec... 
#define ERRG_PER_GMSL_B_TX2_MASK 	 0x01U
#define ERRG_PER_GMSL_B_TX2_POS  	 0U

#define ERRG_BURST_GMSL_B_TX2_ADDR 	 0x502AU // Error-generator burst-error length  
#define ERRG_BURST_GMSL_B_TX2_MASK 	 0x0EU
#define ERRG_BURST_GMSL_B_TX2_POS  	 1U

#define ERRG_RATE_GMSL_B_TX2_ADDR 	 0x502AU // Error-generator average-bit error rate  
#define ERRG_RATE_GMSL_B_TX2_MASK 	 0x30U
#define ERRG_RATE_GMSL_B_TX2_POS  	 4U

#define ERRG_CNT_GMSL_B_TX2_ADDR 	 0x502AU // Number of errors to be generated  
#define ERRG_CNT_GMSL_B_TX2_MASK 	 0xC0U
#define ERRG_CNT_GMSL_B_TX2_POS  	 6U

#define GMSL_B_TX3_ADDR    	 0x502BU
#define GMSL_B_TX3_DEFAULT 	 0x44U

#define RX_FEC_ACTIVE_GMSL_B_TX3_ADDR 	 0x502BU // FEC is active  
#define RX_FEC_ACTIVE_GMSL_B_TX3_MASK 	 0x20U
#define RX_FEC_ACTIVE_GMSL_B_TX3_POS  	 5U

#define GMSL_B_RX0_ADDR    	 0x502CU
#define GMSL_B_RX0_DEFAULT 	 0x00U

#define PKT_CNT_SEL_GMSL_B_RX0_ADDR 	 0x502CU // Select the type of received packets to c... 
#define PKT_CNT_SEL_GMSL_B_RX0_MASK 	 0x0FU
#define PKT_CNT_SEL_GMSL_B_RX0_POS  	 0U

#define PKT_CNT_LBW_GMSL_B_RX0_ADDR 	 0x502CU // Select the subtype of low-bandwidth (LBW... 
#define PKT_CNT_LBW_GMSL_B_RX0_MASK 	 0xC0U
#define PKT_CNT_LBW_GMSL_B_RX0_POS  	 6U

#define GMSL_B_GPIOA_ADDR    	 0x5030U
#define GMSL_B_GPIOA_DEFAULT 	 0x41U

#define GPIO_FWD_CDLY_GMSL_B_GPIOA_ADDR 	 0x5030U // Compensation delay multiplier for the fo... 
#define GPIO_FWD_CDLY_GMSL_B_GPIOA_MASK 	 0x3FU
#define GPIO_FWD_CDLY_GMSL_B_GPIOA_POS  	 0U

#define GMSL_B_GPIOB_ADDR    	 0x5031U
#define GMSL_B_GPIOB_DEFAULT 	 0x88U

#define GPIO_REV_CDLY_GMSL_B_GPIOB_ADDR 	 0x5031U // Compensation delay multiplier for the re... 
#define GPIO_REV_CDLY_GMSL_B_GPIOB_MASK 	 0x3FU
#define GPIO_REV_CDLY_GMSL_B_GPIOB_POS  	 0U

#define GPIO_TX_WNDW_GMSL_B_GPIOB_ADDR 	 0x5031U // Wait time after a GPIO transition to cre... 
#define GPIO_TX_WNDW_GMSL_B_GPIOB_MASK 	 0xC0U
#define GPIO_TX_WNDW_GMSL_B_GPIOB_POS  	 6U

#define CFGH_B_VIDEO_X_RX0_ADDR    	 0x5050U
#define CFGH_B_VIDEO_X_RX0_DEFAULT 	 0x00U

#define STR_SEL_B_CFGH_B_VIDEO_X_RX0_ADDR 	 0x5050U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_B_CFGH_B_VIDEO_X_RX0_MASK 	 0x03U
#define STR_SEL_B_CFGH_B_VIDEO_X_RX0_POS  	 0U

#define RX_CRC_EN_B_CFGH_B_VIDEO_X_RX0_ADDR 	 0x5050U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGH_B_VIDEO_X_RX0_MASK 	 0x80U
#define RX_CRC_EN_B_CFGH_B_VIDEO_X_RX0_POS  	 7U

#define CFGH_B_VIDEO_Y_RX0_ADDR    	 0x5051U
#define CFGH_B_VIDEO_Y_RX0_DEFAULT 	 0x01U

#define STR_SEL_B_CFGH_B_VIDEO_Y_RX0_ADDR 	 0x5051U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_B_CFGH_B_VIDEO_Y_RX0_MASK 	 0x03U
#define STR_SEL_B_CFGH_B_VIDEO_Y_RX0_POS  	 0U

#define RX_CRC_EN_B_CFGH_B_VIDEO_Y_RX0_ADDR 	 0x5051U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGH_B_VIDEO_Y_RX0_MASK 	 0x80U
#define RX_CRC_EN_B_CFGH_B_VIDEO_Y_RX0_POS  	 7U

#define CFGH_B_VIDEO_Z_RX0_ADDR    	 0x5052U
#define CFGH_B_VIDEO_Z_RX0_DEFAULT 	 0x02U

#define STR_SEL_B_CFGH_B_VIDEO_Z_RX0_ADDR 	 0x5052U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_B_CFGH_B_VIDEO_Z_RX0_MASK 	 0x03U
#define STR_SEL_B_CFGH_B_VIDEO_Z_RX0_POS  	 0U

#define RX_CRC_EN_B_CFGH_B_VIDEO_Z_RX0_ADDR 	 0x5052U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGH_B_VIDEO_Z_RX0_MASK 	 0x80U
#define RX_CRC_EN_B_CFGH_B_VIDEO_Z_RX0_POS  	 7U

#define CFGH_B_VIDEO_U_RX0_ADDR    	 0x5053U
#define CFGH_B_VIDEO_U_RX0_DEFAULT 	 0x03U

#define STR_SEL_B_CFGH_B_VIDEO_U_RX0_ADDR 	 0x5053U // Reserved. Do not use (legacy). Use regis... 
#define STR_SEL_B_CFGH_B_VIDEO_U_RX0_MASK 	 0x03U
#define STR_SEL_B_CFGH_B_VIDEO_U_RX0_POS  	 0U

#define RX_CRC_EN_B_CFGH_B_VIDEO_U_RX0_ADDR 	 0x5053U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGH_B_VIDEO_U_RX0_MASK 	 0x80U
#define RX_CRC_EN_B_CFGH_B_VIDEO_U_RX0_POS  	 7U

#define CFGI_B_INFOFR_TR0_ADDR    	 0x5060U
#define CFGI_B_INFOFR_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_B_CFGI_B_INFOFR_TR0_ADDR 	 0x5060U // Adjust the priority used for this channe... 
#define PRIO_CFG_B_CFGI_B_INFOFR_TR0_MASK 	 0x03U
#define PRIO_CFG_B_CFGI_B_INFOFR_TR0_POS  	 0U

#define PRIO_VAL_B_CFGI_B_INFOFR_TR0_ADDR 	 0x5060U // Sets the priority for this channel's pac... 
#define PRIO_VAL_B_CFGI_B_INFOFR_TR0_MASK 	 0x0CU
#define PRIO_VAL_B_CFGI_B_INFOFR_TR0_POS  	 2U

#define RX_CRC_EN_B_CFGI_B_INFOFR_TR0_ADDR 	 0x5060U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGI_B_INFOFR_TR0_MASK 	 0x40U
#define RX_CRC_EN_B_CFGI_B_INFOFR_TR0_POS  	 6U

#define TX_CRC_EN_B_CFGI_B_INFOFR_TR0_ADDR 	 0x5060U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_B_CFGI_B_INFOFR_TR0_MASK 	 0x80U
#define TX_CRC_EN_B_CFGI_B_INFOFR_TR0_POS  	 7U

#define CFGI_B_INFOFR_TR1_ADDR    	 0x5061U
#define CFGI_B_INFOFR_TR1_DEFAULT 	 0xB0U

#define BW_VAL_B_CFGI_B_INFOFR_TR1_ADDR 	 0x5061U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_B_CFGI_B_INFOFR_TR1_MASK 	 0x3FU
#define BW_VAL_B_CFGI_B_INFOFR_TR1_POS  	 0U

#define BW_MULT_B_CFGI_B_INFOFR_TR1_ADDR 	 0x5061U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_B_CFGI_B_INFOFR_TR1_MASK 	 0xC0U
#define BW_MULT_B_CFGI_B_INFOFR_TR1_POS  	 6U

#define CFGI_B_INFOFR_TR3_ADDR    	 0x5063U
#define CFGI_B_INFOFR_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_B_CFGI_B_INFOFR_TR3_ADDR 	 0x5063U // Source identifier used in packets transm... 
#define TX_SRC_ID_B_CFGI_B_INFOFR_TR3_MASK 	 0x07U
#define TX_SRC_ID_B_CFGI_B_INFOFR_TR3_POS  	 0U

#define CFGI_B_INFOFR_TR4_ADDR    	 0x5064U
#define CFGI_B_INFOFR_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_B_CFGI_B_INFOFR_TR4_ADDR 	 0x5064U // Receive packets from selected sources.  
#define RX_SRC_SEL_B_CFGI_B_INFOFR_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_B_CFGI_B_INFOFR_TR4_POS  	 0U

#define CFGC_B_CC_TR0_ADDR    	 0x5070U
#define CFGC_B_CC_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_B_CFGC_B_CC_TR0_ADDR 	 0x5070U // Adjust the priority used for this channe... 
#define PRIO_CFG_B_CFGC_B_CC_TR0_MASK 	 0x03U
#define PRIO_CFG_B_CFGC_B_CC_TR0_POS  	 0U

#define PRIO_VAL_B_CFGC_B_CC_TR0_ADDR 	 0x5070U // Sets the priority for this channel's pac... 
#define PRIO_VAL_B_CFGC_B_CC_TR0_MASK 	 0x0CU
#define PRIO_VAL_B_CFGC_B_CC_TR0_POS  	 2U

#define RX_CRC_EN_B_CFGC_B_CC_TR0_ADDR 	 0x5070U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGC_B_CC_TR0_MASK 	 0x40U
#define RX_CRC_EN_B_CFGC_B_CC_TR0_POS  	 6U

#define TX_CRC_EN_B_CFGC_B_CC_TR0_ADDR 	 0x5070U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_B_CFGC_B_CC_TR0_MASK 	 0x80U
#define TX_CRC_EN_B_CFGC_B_CC_TR0_POS  	 7U

#define CFGC_B_CC_TR1_ADDR    	 0x5071U
#define CFGC_B_CC_TR1_DEFAULT 	 0xB0U

#define BW_VAL_B_CFGC_B_CC_TR1_ADDR 	 0x5071U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_B_CFGC_B_CC_TR1_MASK 	 0x3FU
#define BW_VAL_B_CFGC_B_CC_TR1_POS  	 0U

#define BW_MULT_B_CFGC_B_CC_TR1_ADDR 	 0x5071U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_B_CFGC_B_CC_TR1_MASK 	 0xC0U
#define BW_MULT_B_CFGC_B_CC_TR1_POS  	 6U

#define CFGC_B_CC_TR3_ADDR    	 0x5073U
#define CFGC_B_CC_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_B_CFGC_B_CC_TR3_ADDR 	 0x5073U // Source identifier used in packets transm... 
#define TX_SRC_ID_B_CFGC_B_CC_TR3_MASK 	 0x07U
#define TX_SRC_ID_B_CFGC_B_CC_TR3_POS  	 0U

#define CFGC_B_CC_TR4_ADDR    	 0x5074U
#define CFGC_B_CC_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_B_CFGC_B_CC_TR4_ADDR 	 0x5074U // Receive packets from selected sources.  
#define RX_SRC_SEL_B_CFGC_B_CC_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_B_CFGC_B_CC_TR4_POS  	 0U

#define CFGC_B_CC_ARQ0_ADDR    	 0x5075U
#define CFGC_B_CC_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_B_CFGC_B_CC_ARQ0_ADDR 	 0x5075U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_B_CFGC_B_CC_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_B_CFGC_B_CC_ARQ0_POS  	 2U

#define EN_B_CFGC_B_CC_ARQ0_ADDR 	 0x5075U // Enable ARQ  
#define EN_B_CFGC_B_CC_ARQ0_MASK 	 0x08U
#define EN_B_CFGC_B_CC_ARQ0_POS  	 3U

#define CFGC_B_CC_ARQ1_ADDR    	 0x5076U
#define CFGC_B_CC_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_B_CFGC_B_CC_ARQ1_ADDR 	 0x5076U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_B_CFGC_B_CC_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_B_CFGC_B_CC_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_B_CFGC_B_CC_ARQ1_ADDR 	 0x5076U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_B_CFGC_B_CC_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_B_CFGC_B_CC_ARQ1_POS  	 1U

#define CFGC_B_CC_ARQ2_ADDR    	 0x5077U
#define CFGC_B_CC_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_B_CFGC_B_CC_ARQ2_ADDR 	 0x5077U // Total retransmission count in this chann... 
#define RT_CNT_B_CFGC_B_CC_ARQ2_MASK 	 0x7FU
#define RT_CNT_B_CFGC_B_CC_ARQ2_POS  	 0U

#define MAX_RT_ERR_B_CFGC_B_CC_ARQ2_ADDR 	 0x5077U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_B_CFGC_B_CC_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_B_CFGC_B_CC_ARQ2_POS  	 7U

#define CFGL_B_GPIO_TR0_ADDR    	 0x5078U
#define CFGL_B_GPIO_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_B_CFGL_B_GPIO_TR0_ADDR 	 0x5078U // Adjust the priority used for this channe... 
#define PRIO_CFG_B_CFGL_B_GPIO_TR0_MASK 	 0x03U
#define PRIO_CFG_B_CFGL_B_GPIO_TR0_POS  	 0U

#define PRIO_VAL_B_CFGL_B_GPIO_TR0_ADDR 	 0x5078U // Sets the priority for this channel's pac... 
#define PRIO_VAL_B_CFGL_B_GPIO_TR0_MASK 	 0x0CU
#define PRIO_VAL_B_CFGL_B_GPIO_TR0_POS  	 2U

#define RX_CRC_EN_B_CFGL_B_GPIO_TR0_ADDR 	 0x5078U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGL_B_GPIO_TR0_MASK 	 0x40U
#define RX_CRC_EN_B_CFGL_B_GPIO_TR0_POS  	 6U

#define TX_CRC_EN_B_CFGL_B_GPIO_TR0_ADDR 	 0x5078U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_B_CFGL_B_GPIO_TR0_MASK 	 0x80U
#define TX_CRC_EN_B_CFGL_B_GPIO_TR0_POS  	 7U

#define CFGL_B_GPIO_TR1_ADDR    	 0x5079U
#define CFGL_B_GPIO_TR1_DEFAULT 	 0xB0U

#define BW_VAL_B_CFGL_B_GPIO_TR1_ADDR 	 0x5079U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_B_CFGL_B_GPIO_TR1_MASK 	 0x3FU
#define BW_VAL_B_CFGL_B_GPIO_TR1_POS  	 0U

#define BW_MULT_B_CFGL_B_GPIO_TR1_ADDR 	 0x5079U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_B_CFGL_B_GPIO_TR1_MASK 	 0xC0U
#define BW_MULT_B_CFGL_B_GPIO_TR1_POS  	 6U

#define CFGL_B_GPIO_TR3_ADDR    	 0x507BU
#define CFGL_B_GPIO_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_B_CFGL_B_GPIO_TR3_ADDR 	 0x507BU // Source identifier used in packets transm... 
#define TX_SRC_ID_B_CFGL_B_GPIO_TR3_MASK 	 0x07U
#define TX_SRC_ID_B_CFGL_B_GPIO_TR3_POS  	 0U

#define CFGL_B_GPIO_TR4_ADDR    	 0x507CU
#define CFGL_B_GPIO_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_B_CFGL_B_GPIO_TR4_ADDR 	 0x507CU // Receive packets from selected sources.  
#define RX_SRC_SEL_B_CFGL_B_GPIO_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_B_CFGL_B_GPIO_TR4_POS  	 0U

#define CFGL_B_GPIO_ARQ0_ADDR    	 0x507DU
#define CFGL_B_GPIO_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_B_CFGL_B_GPIO_ARQ0_ADDR 	 0x507DU // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_B_CFGL_B_GPIO_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_B_CFGL_B_GPIO_ARQ0_POS  	 2U

#define EN_B_CFGL_B_GPIO_ARQ0_ADDR 	 0x507DU // Enable ARQ  
#define EN_B_CFGL_B_GPIO_ARQ0_MASK 	 0x08U
#define EN_B_CFGL_B_GPIO_ARQ0_POS  	 3U

#define CFGL_B_GPIO_ARQ1_ADDR    	 0x507EU
#define CFGL_B_GPIO_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_B_CFGL_B_GPIO_ARQ1_ADDR 	 0x507EU // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_B_CFGL_B_GPIO_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_B_CFGL_B_GPIO_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_B_CFGL_B_GPIO_ARQ1_ADDR 	 0x507EU // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_B_CFGL_B_GPIO_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_B_CFGL_B_GPIO_ARQ1_POS  	 1U

#define CFGL_B_GPIO_ARQ2_ADDR    	 0x507FU
#define CFGL_B_GPIO_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_B_CFGL_B_GPIO_ARQ2_ADDR 	 0x507FU // Total retransmission count in this chann... 
#define RT_CNT_B_CFGL_B_GPIO_ARQ2_MASK 	 0x7FU
#define RT_CNT_B_CFGL_B_GPIO_ARQ2_POS  	 0U

#define MAX_RT_ERR_B_CFGL_B_GPIO_ARQ2_ADDR 	 0x507FU // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_B_CFGL_B_GPIO_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_B_CFGL_B_GPIO_ARQ2_POS  	 7U

#define CFGC_B_IIC_X_TR0_ADDR    	 0x5080U
#define CFGC_B_IIC_X_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_B_CFGC_B_IIC_X_TR0_ADDR 	 0x5080U // Adjust the priority used for this channe... 
#define PRIO_CFG_B_CFGC_B_IIC_X_TR0_MASK 	 0x03U
#define PRIO_CFG_B_CFGC_B_IIC_X_TR0_POS  	 0U

#define PRIO_VAL_B_CFGC_B_IIC_X_TR0_ADDR 	 0x5080U // Sets the priority for this channel's pac... 
#define PRIO_VAL_B_CFGC_B_IIC_X_TR0_MASK 	 0x0CU
#define PRIO_VAL_B_CFGC_B_IIC_X_TR0_POS  	 2U

#define RX_CRC_EN_B_CFGC_B_IIC_X_TR0_ADDR 	 0x5080U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGC_B_IIC_X_TR0_MASK 	 0x40U
#define RX_CRC_EN_B_CFGC_B_IIC_X_TR0_POS  	 6U

#define TX_CRC_EN_B_CFGC_B_IIC_X_TR0_ADDR 	 0x5080U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_B_CFGC_B_IIC_X_TR0_MASK 	 0x80U
#define TX_CRC_EN_B_CFGC_B_IIC_X_TR0_POS  	 7U

#define CFGC_B_IIC_X_TR1_ADDR    	 0x5081U
#define CFGC_B_IIC_X_TR1_DEFAULT 	 0xB0U

#define BW_VAL_B_CFGC_B_IIC_X_TR1_ADDR 	 0x5081U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_B_CFGC_B_IIC_X_TR1_MASK 	 0x3FU
#define BW_VAL_B_CFGC_B_IIC_X_TR1_POS  	 0U

#define BW_MULT_B_CFGC_B_IIC_X_TR1_ADDR 	 0x5081U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_B_CFGC_B_IIC_X_TR1_MASK 	 0xC0U
#define BW_MULT_B_CFGC_B_IIC_X_TR1_POS  	 6U

#define CFGC_B_IIC_X_TR3_ADDR    	 0x5083U
#define CFGC_B_IIC_X_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_B_CFGC_B_IIC_X_TR3_ADDR 	 0x5083U // Source identifier used in packets transm... 
#define TX_SRC_ID_B_CFGC_B_IIC_X_TR3_MASK 	 0x07U
#define TX_SRC_ID_B_CFGC_B_IIC_X_TR3_POS  	 0U

#define CFGC_B_IIC_X_TR4_ADDR    	 0x5084U
#define CFGC_B_IIC_X_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_B_CFGC_B_IIC_X_TR4_ADDR 	 0x5084U // Receive packets from selected sources.  
#define RX_SRC_SEL_B_CFGC_B_IIC_X_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_B_CFGC_B_IIC_X_TR4_POS  	 0U

#define CFGC_B_IIC_X_ARQ0_ADDR    	 0x5085U
#define CFGC_B_IIC_X_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_B_CFGC_B_IIC_X_ARQ0_ADDR 	 0x5085U // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_B_CFGC_B_IIC_X_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_B_CFGC_B_IIC_X_ARQ0_POS  	 2U

#define EN_B_CFGC_B_IIC_X_ARQ0_ADDR 	 0x5085U // Enable ARQ  
#define EN_B_CFGC_B_IIC_X_ARQ0_MASK 	 0x08U
#define EN_B_CFGC_B_IIC_X_ARQ0_POS  	 3U

#define CFGC_B_IIC_X_ARQ1_ADDR    	 0x5086U
#define CFGC_B_IIC_X_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_B_CFGC_B_IIC_X_ARQ1_ADDR 	 0x5086U // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_B_CFGC_B_IIC_X_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_B_CFGC_B_IIC_X_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_B_CFGC_B_IIC_X_ARQ1_ADDR 	 0x5086U // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_B_CFGC_B_IIC_X_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_B_CFGC_B_IIC_X_ARQ1_POS  	 1U

#define CFGC_B_IIC_X_ARQ2_ADDR    	 0x5087U
#define CFGC_B_IIC_X_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_B_CFGC_B_IIC_X_ARQ2_ADDR 	 0x5087U // Total retransmission count in this chann... 
#define RT_CNT_B_CFGC_B_IIC_X_ARQ2_MASK 	 0x7FU
#define RT_CNT_B_CFGC_B_IIC_X_ARQ2_POS  	 0U

#define MAX_RT_ERR_B_CFGC_B_IIC_X_ARQ2_ADDR 	 0x5087U // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_B_CFGC_B_IIC_X_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_B_CFGC_B_IIC_X_ARQ2_POS  	 7U

#define CFGC_B_IIC_Y_TR0_ADDR    	 0x5088U
#define CFGC_B_IIC_Y_TR0_DEFAULT 	 0xF0U

#define PRIO_CFG_B_CFGC_B_IIC_Y_TR0_ADDR 	 0x5088U // Adjust the priority used for this channe... 
#define PRIO_CFG_B_CFGC_B_IIC_Y_TR0_MASK 	 0x03U
#define PRIO_CFG_B_CFGC_B_IIC_Y_TR0_POS  	 0U

#define PRIO_VAL_B_CFGC_B_IIC_Y_TR0_ADDR 	 0x5088U // Sets the priority for this channel's pac... 
#define PRIO_VAL_B_CFGC_B_IIC_Y_TR0_MASK 	 0x0CU
#define PRIO_VAL_B_CFGC_B_IIC_Y_TR0_POS  	 2U

#define RX_CRC_EN_B_CFGC_B_IIC_Y_TR0_ADDR 	 0x5088U // When set, indicates that packets receive... 
#define RX_CRC_EN_B_CFGC_B_IIC_Y_TR0_MASK 	 0x40U
#define RX_CRC_EN_B_CFGC_B_IIC_Y_TR0_POS  	 6U

#define TX_CRC_EN_B_CFGC_B_IIC_Y_TR0_ADDR 	 0x5088U // When set, calculate and append CRC to ea... 
#define TX_CRC_EN_B_CFGC_B_IIC_Y_TR0_MASK 	 0x80U
#define TX_CRC_EN_B_CFGC_B_IIC_Y_TR0_POS  	 7U

#define CFGC_B_IIC_Y_TR1_ADDR    	 0x5089U
#define CFGC_B_IIC_Y_TR1_DEFAULT 	 0xB0U

#define BW_VAL_B_CFGC_B_IIC_Y_TR1_ADDR 	 0x5089U // Channel bandwidth-allocation base. Used ... 
#define BW_VAL_B_CFGC_B_IIC_Y_TR1_MASK 	 0x3FU
#define BW_VAL_B_CFGC_B_IIC_Y_TR1_POS  	 0U

#define BW_MULT_B_CFGC_B_IIC_Y_TR1_ADDR 	 0x5089U // Channel bandwidth-allocation multiplicat... 
#define BW_MULT_B_CFGC_B_IIC_Y_TR1_MASK 	 0xC0U
#define BW_MULT_B_CFGC_B_IIC_Y_TR1_POS  	 6U

#define CFGC_B_IIC_Y_TR3_ADDR    	 0x508BU
#define CFGC_B_IIC_Y_TR3_DEFAULT 	 0x00U

#define TX_SRC_ID_B_CFGC_B_IIC_Y_TR3_ADDR 	 0x508BU // Source identifier used in packets transm... 
#define TX_SRC_ID_B_CFGC_B_IIC_Y_TR3_MASK 	 0x07U
#define TX_SRC_ID_B_CFGC_B_IIC_Y_TR3_POS  	 0U

#define CFGC_B_IIC_Y_TR4_ADDR    	 0x508CU
#define CFGC_B_IIC_Y_TR4_DEFAULT 	 0xFFU

#define RX_SRC_SEL_B_CFGC_B_IIC_Y_TR4_ADDR 	 0x508CU // Receive packets from selected sources.  
#define RX_SRC_SEL_B_CFGC_B_IIC_Y_TR4_MASK 	 0xFFU
#define RX_SRC_SEL_B_CFGC_B_IIC_Y_TR4_POS  	 0U

#define CFGC_B_IIC_Y_ARQ0_ADDR    	 0x508DU
#define CFGC_B_IIC_Y_ARQ0_DEFAULT 	 0x98U

#define DIS_DBL_ACK_RETX_B_CFGC_B_IIC_Y_ARQ0_ADDR 	 0x508DU // Disable retransmission due to receiving ... 
#define DIS_DBL_ACK_RETX_B_CFGC_B_IIC_Y_ARQ0_MASK 	 0x04U
#define DIS_DBL_ACK_RETX_B_CFGC_B_IIC_Y_ARQ0_POS  	 2U

#define EN_B_CFGC_B_IIC_Y_ARQ0_ADDR 	 0x508DU // Enable ARQ  
#define EN_B_CFGC_B_IIC_Y_ARQ0_MASK 	 0x08U
#define EN_B_CFGC_B_IIC_Y_ARQ0_POS  	 3U

#define CFGC_B_IIC_Y_ARQ1_ADDR    	 0x508EU
#define CFGC_B_IIC_Y_ARQ1_DEFAULT 	 0x72U

#define RT_CNT_OEN_B_CFGC_B_IIC_Y_ARQ1_ADDR 	 0x508EU // Enable reporting of ARQ retransmission e... 
#define RT_CNT_OEN_B_CFGC_B_IIC_Y_ARQ1_MASK 	 0x01U
#define RT_CNT_OEN_B_CFGC_B_IIC_Y_ARQ1_POS  	 0U

#define MAX_RT_ERR_OEN_B_CFGC_B_IIC_Y_ARQ1_ADDR 	 0x508EU // Enable reporting of ARQ maximum retransm... 
#define MAX_RT_ERR_OEN_B_CFGC_B_IIC_Y_ARQ1_MASK 	 0x02U
#define MAX_RT_ERR_OEN_B_CFGC_B_IIC_Y_ARQ1_POS  	 1U

#define CFGC_B_IIC_Y_ARQ2_ADDR    	 0x508FU
#define CFGC_B_IIC_Y_ARQ2_DEFAULT 	 0x00U

#define RT_CNT_B_CFGC_B_IIC_Y_ARQ2_ADDR 	 0x508FU // Total retransmission count in this chann... 
#define RT_CNT_B_CFGC_B_IIC_Y_ARQ2_MASK 	 0x7FU
#define RT_CNT_B_CFGC_B_IIC_Y_ARQ2_POS  	 0U

#define MAX_RT_ERR_B_CFGC_B_IIC_Y_ARQ2_ADDR 	 0x508FU // Reached maximum retransmission limit (MA... 
#define MAX_RT_ERR_B_CFGC_B_IIC_Y_ARQ2_MASK 	 0x80U
#define MAX_RT_ERR_B_CFGC_B_IIC_Y_ARQ2_POS  	 7U

#define GPIO0_B_0_GPIO_A_ADDR    	 0x52B0U
#define GPIO0_B_0_GPIO_A_DEFAULT 	 0x02U

#define GPIO_TX_EN_B_GPIO0_B_0_GPIO_A_ADDR 	 0x52B0U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_0_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_0_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_0_GPIO_A_ADDR 	 0x52B0U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_0_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_0_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_0_GPIO_A_ADDR 	 0x52B0U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_0_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_0_GPIO_A_POS  	 5U

#define GPIO0_B_0_GPIO_B_ADDR    	 0x52B1U
#define GPIO0_B_0_GPIO_B_DEFAULT 	 0x00U

#define GPIO_TX_ID_B_GPIO0_B_0_GPIO_B_ADDR 	 0x52B1U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_0_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_0_GPIO_B_POS  	 0U

#define GPIO0_B_0_GPIO_C_ADDR    	 0x52B2U
#define GPIO0_B_0_GPIO_C_DEFAULT 	 0x40U

#define GPIO_RX_ID_B_GPIO0_B_0_GPIO_C_ADDR 	 0x52B2U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_0_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_0_GPIO_C_POS  	 0U

#define GPIO_RECVED_B_GPIO0_B_0_GPIO_C_ADDR 	 0x52B2U // Received GPIO value from across the GMSL... 
#define GPIO_RECVED_B_GPIO0_B_0_GPIO_C_MASK 	 0x40U
#define GPIO_RECVED_B_GPIO0_B_0_GPIO_C_POS  	 6U

#define GPIO0_B_1_GPIO_A_ADDR    	 0x52B3U
#define GPIO0_B_1_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_1_GPIO_A_ADDR 	 0x52B3U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_1_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_1_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_1_GPIO_A_ADDR 	 0x52B3U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_1_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_1_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_1_GPIO_A_ADDR 	 0x52B3U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_1_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_1_GPIO_A_POS  	 5U

#define GPIO0_B_1_GPIO_B_ADDR    	 0x52B4U
#define GPIO0_B_1_GPIO_B_DEFAULT 	 0x01U

#define GPIO_TX_ID_B_GPIO0_B_1_GPIO_B_ADDR 	 0x52B4U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_1_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_1_GPIO_B_POS  	 0U

#define GPIO0_B_1_GPIO_C_ADDR    	 0x52B5U
#define GPIO0_B_1_GPIO_C_DEFAULT 	 0x41U

#define GPIO_RX_ID_B_GPIO0_B_1_GPIO_C_ADDR 	 0x52B5U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_1_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_1_GPIO_C_POS  	 0U

#define GPIO0_B_2_GPIO_A_ADDR    	 0x52B6U
#define GPIO0_B_2_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_2_GPIO_A_ADDR 	 0x52B6U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_2_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_2_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_2_GPIO_A_ADDR 	 0x52B6U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_2_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_2_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_2_GPIO_A_ADDR 	 0x52B6U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_2_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_2_GPIO_A_POS  	 5U

#define GPIO0_B_2_GPIO_B_ADDR    	 0x52B7U
#define GPIO0_B_2_GPIO_B_DEFAULT 	 0x02U

#define GPIO_TX_ID_B_GPIO0_B_2_GPIO_B_ADDR 	 0x52B7U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_2_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_2_GPIO_B_POS  	 0U

#define GPIO0_B_2_GPIO_C_ADDR    	 0x52B8U
#define GPIO0_B_2_GPIO_C_DEFAULT 	 0x42U

#define GPIO_RX_ID_B_GPIO0_B_2_GPIO_C_ADDR 	 0x52B8U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_2_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_2_GPIO_C_POS  	 0U

#define GPIO0_B_3_GPIO_A_ADDR    	 0x52B9U
#define GPIO0_B_3_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_3_GPIO_A_ADDR 	 0x52B9U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_3_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_3_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_3_GPIO_A_ADDR 	 0x52B9U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_3_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_3_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_3_GPIO_A_ADDR 	 0x52B9U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_3_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_3_GPIO_A_POS  	 5U

#define GPIO0_B_3_GPIO_B_ADDR    	 0x52BAU
#define GPIO0_B_3_GPIO_B_DEFAULT 	 0x03U

#define GPIO_TX_ID_B_GPIO0_B_3_GPIO_B_ADDR 	 0x52BAU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_3_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_3_GPIO_B_POS  	 0U

#define GPIO0_B_3_GPIO_C_ADDR    	 0x52BBU
#define GPIO0_B_3_GPIO_C_DEFAULT 	 0x43U

#define GPIO_RX_ID_B_GPIO0_B_3_GPIO_C_ADDR 	 0x52BBU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_3_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_3_GPIO_C_POS  	 0U

#define GPIO0_B_4_GPIO_A_ADDR    	 0x52BCU
#define GPIO0_B_4_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_4_GPIO_A_ADDR 	 0x52BCU // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_4_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_4_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_4_GPIO_A_ADDR 	 0x52BCU // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_4_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_4_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_4_GPIO_A_ADDR 	 0x52BCU // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_4_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_4_GPIO_A_POS  	 5U

#define GPIO0_B_4_GPIO_B_ADDR    	 0x52BDU
#define GPIO0_B_4_GPIO_B_DEFAULT 	 0x04U

#define GPIO_TX_ID_B_GPIO0_B_4_GPIO_B_ADDR 	 0x52BDU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_4_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_4_GPIO_B_POS  	 0U

#define GPIO0_B_4_GPIO_C_ADDR    	 0x52BEU
#define GPIO0_B_4_GPIO_C_DEFAULT 	 0x44U

#define GPIO_RX_ID_B_GPIO0_B_4_GPIO_C_ADDR 	 0x52BEU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_4_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_4_GPIO_C_POS  	 0U

#define GPIO0_B_5_GPIO_A_ADDR    	 0x52BFU
#define GPIO0_B_5_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_5_GPIO_A_ADDR 	 0x52BFU // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_5_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_5_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_5_GPIO_A_ADDR 	 0x52BFU // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_5_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_5_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_5_GPIO_A_ADDR 	 0x52BFU // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_5_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_5_GPIO_A_POS  	 5U

#define GPIO0_B_5_GPIO_B_ADDR    	 0x52C0U
#define GPIO0_B_5_GPIO_B_DEFAULT 	 0x05U

#define GPIO_TX_ID_B_GPIO0_B_5_GPIO_B_ADDR 	 0x52C0U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_5_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_5_GPIO_B_POS  	 0U

#define GPIO0_B_5_GPIO_C_ADDR    	 0x52C1U
#define GPIO0_B_5_GPIO_C_DEFAULT 	 0x45U

#define GPIO_RX_ID_B_GPIO0_B_5_GPIO_C_ADDR 	 0x52C1U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_5_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_5_GPIO_C_POS  	 0U

#define GPIO0_B_6_GPIO_A_ADDR    	 0x52C2U
#define GPIO0_B_6_GPIO_A_DEFAULT 	 0x02U

#define GPIO_TX_EN_B_GPIO0_B_6_GPIO_A_ADDR 	 0x52C2U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_6_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_6_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_6_GPIO_A_ADDR 	 0x52C2U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_6_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_6_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_6_GPIO_A_ADDR 	 0x52C2U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_6_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_6_GPIO_A_POS  	 5U

#define GPIO0_B_6_GPIO_B_ADDR    	 0x52C3U
#define GPIO0_B_6_GPIO_B_DEFAULT 	 0x06U

#define GPIO_TX_ID_B_GPIO0_B_6_GPIO_B_ADDR 	 0x52C3U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_6_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_6_GPIO_B_POS  	 0U

#define GPIO0_B_6_GPIO_C_ADDR    	 0x52C4U
#define GPIO0_B_6_GPIO_C_DEFAULT 	 0x46U

#define GPIO_RX_ID_B_GPIO0_B_6_GPIO_C_ADDR 	 0x52C4U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_6_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_6_GPIO_C_POS  	 0U

#define GPIO0_B_7_GPIO_A_ADDR    	 0x52C5U
#define GPIO0_B_7_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_7_GPIO_A_ADDR 	 0x52C5U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_7_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_7_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_7_GPIO_A_ADDR 	 0x52C5U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_7_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_7_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_7_GPIO_A_ADDR 	 0x52C5U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_7_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_7_GPIO_A_POS  	 5U

#define GPIO0_B_7_GPIO_B_ADDR    	 0x52C6U
#define GPIO0_B_7_GPIO_B_DEFAULT 	 0x07U

#define GPIO_TX_ID_B_GPIO0_B_7_GPIO_B_ADDR 	 0x52C6U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_7_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_7_GPIO_B_POS  	 0U

#define GPIO0_B_7_GPIO_C_ADDR    	 0x52C7U
#define GPIO0_B_7_GPIO_C_DEFAULT 	 0x47U

#define GPIO_RX_ID_B_GPIO0_B_7_GPIO_C_ADDR 	 0x52C7U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_7_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_7_GPIO_C_POS  	 0U

#define GPIO0_B_8_GPIO_A_ADDR    	 0x52C8U
#define GPIO0_B_8_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_8_GPIO_A_ADDR 	 0x52C8U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_8_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_8_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_8_GPIO_A_ADDR 	 0x52C8U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_8_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_8_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_8_GPIO_A_ADDR 	 0x52C8U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_8_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_8_GPIO_A_POS  	 5U

#define GPIO0_B_8_GPIO_B_ADDR    	 0x52C9U
#define GPIO0_B_8_GPIO_B_DEFAULT 	 0x08U

#define GPIO_TX_ID_B_GPIO0_B_8_GPIO_B_ADDR 	 0x52C9U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_8_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_8_GPIO_B_POS  	 0U

#define GPIO0_B_8_GPIO_C_ADDR    	 0x52CAU
#define GPIO0_B_8_GPIO_C_DEFAULT 	 0x48U

#define GPIO_RX_ID_B_GPIO0_B_8_GPIO_C_ADDR 	 0x52CAU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_8_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_8_GPIO_C_POS  	 0U

#define GPIO0_B_9_GPIO_A_ADDR    	 0x52CBU
#define GPIO0_B_9_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_9_GPIO_A_ADDR 	 0x52CBU // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_9_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_9_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_9_GPIO_A_ADDR 	 0x52CBU // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_9_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_9_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_9_GPIO_A_ADDR 	 0x52CBU // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_9_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_9_GPIO_A_POS  	 5U

#define GPIO0_B_9_GPIO_B_ADDR    	 0x52CCU
#define GPIO0_B_9_GPIO_B_DEFAULT 	 0x09U

#define GPIO_TX_ID_B_GPIO0_B_9_GPIO_B_ADDR 	 0x52CCU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_9_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_9_GPIO_B_POS  	 0U

#define GPIO0_B_9_GPIO_C_ADDR    	 0x52CDU
#define GPIO0_B_9_GPIO_C_DEFAULT 	 0x49U

#define GPIO_RX_ID_B_GPIO0_B_9_GPIO_C_ADDR 	 0x52CDU // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_9_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_9_GPIO_C_POS  	 0U

#define GPIO0_B_10_GPIO_A_ADDR    	 0x52CEU
#define GPIO0_B_10_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_10_GPIO_A_ADDR 	 0x52CEU // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_10_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_10_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_10_GPIO_A_ADDR 	 0x52CEU // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_10_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_10_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_10_GPIO_A_ADDR 	 0x52CEU // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_10_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_10_GPIO_A_POS  	 5U

#define GPIO0_B_10_GPIO_B_ADDR    	 0x52CFU
#define GPIO0_B_10_GPIO_B_DEFAULT 	 0x0AU

#define GPIO_TX_ID_B_GPIO0_B_10_GPIO_B_ADDR 	 0x52CFU // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_10_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_10_GPIO_B_POS  	 0U

#define GPIO0_B_10_GPIO_C_ADDR    	 0x52D0U
#define GPIO0_B_10_GPIO_C_DEFAULT 	 0x4AU

#define GPIO_RX_ID_B_GPIO0_B_10_GPIO_C_ADDR 	 0x52D0U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_10_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_10_GPIO_C_POS  	 0U

#define GPIO0_B_11_GPIO_A_ADDR    	 0x52D1U
#define GPIO0_B_11_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_11_GPIO_A_ADDR 	 0x52D1U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_11_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_11_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_11_GPIO_A_ADDR 	 0x52D1U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_11_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_11_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_11_GPIO_A_ADDR 	 0x52D1U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_11_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_11_GPIO_A_POS  	 5U

#define GPIO0_B_11_GPIO_B_ADDR    	 0x52D2U
#define GPIO0_B_11_GPIO_B_DEFAULT 	 0x0BU

#define GPIO_TX_ID_B_GPIO0_B_11_GPIO_B_ADDR 	 0x52D2U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_11_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_11_GPIO_B_POS  	 0U

#define GPIO0_B_11_GPIO_C_ADDR    	 0x52D3U
#define GPIO0_B_11_GPIO_C_DEFAULT 	 0x4BU

#define GPIO_RX_ID_B_GPIO0_B_11_GPIO_C_ADDR 	 0x52D3U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_11_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_11_GPIO_C_POS  	 0U

#define GPIO0_B_12_GPIO_A_ADDR    	 0x52D4U
#define GPIO0_B_12_GPIO_A_DEFAULT 	 0x00U

#define GPIO_TX_EN_B_GPIO0_B_12_GPIO_A_ADDR 	 0x52D4U // GPIO Tx source control.  
#define GPIO_TX_EN_B_GPIO0_B_12_GPIO_A_MASK 	 0x02U
#define GPIO_TX_EN_B_GPIO0_B_12_GPIO_A_POS  	 1U

#define GPIO_RX_EN_B_GPIO0_B_12_GPIO_A_ADDR 	 0x52D4U // GPIO out source control.  
#define GPIO_RX_EN_B_GPIO0_B_12_GPIO_A_MASK 	 0x04U
#define GPIO_RX_EN_B_GPIO0_B_12_GPIO_A_POS  	 2U

#define TX_COMP_EN_B_GPIO0_B_12_GPIO_A_ADDR 	 0x52D4U // Enables jitter minimization compensation... 
#define TX_COMP_EN_B_GPIO0_B_12_GPIO_A_MASK 	 0x20U
#define TX_COMP_EN_B_GPIO0_B_12_GPIO_A_POS  	 5U

#define GPIO0_B_12_GPIO_B_ADDR    	 0x52D5U
#define GPIO0_B_12_GPIO_B_DEFAULT 	 0x0CU

#define GPIO_TX_ID_B_GPIO0_B_12_GPIO_B_ADDR 	 0x52D5U // GPIO ID for pin while transmitting  
#define GPIO_TX_ID_B_GPIO0_B_12_GPIO_B_MASK 	 0x1FU
#define GPIO_TX_ID_B_GPIO0_B_12_GPIO_B_POS  	 0U

#define GPIO0_B_12_GPIO_C_ADDR    	 0x52D6U
#define GPIO0_B_12_GPIO_C_DEFAULT 	 0x4CU

#define GPIO_RX_ID_B_GPIO0_B_12_GPIO_C_ADDR 	 0x52D6U // GPIO ID for pin while receiving  
#define GPIO_RX_ID_B_GPIO0_B_12_GPIO_C_MASK 	 0x1FU
#define GPIO_RX_ID_B_GPIO0_B_12_GPIO_C_POS  	 0U

#define MAX96792_MASK_TO_RW_ALL_MASK         (0xFFU)

#define MAX96792_READ_ALT_MEM_MAP_MASK         (0x17U)


#endif