/**
* \file
* \brief Contains ARM private features related function implementation defined in
*        adrv9001_arm.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
/* "adi_adrv9001_user.h" contains the #define that other header file use */
#include "adi_adrv9001_user.h"

/* Header file corresponding to the C file */
#include "adi_adrv9001_arm.h"

/* ADI specific header files */
#include "adi_adrv9001_types.h"
#include "adi_common_error.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_spi.h"
#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_arm_error_mapping.h"
#include "adrv9001_crc32.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf.h"

/* Header files related to libraries */

/* System header files */

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

//#define ADI_INIT_DMA_SKIP 1
#ifdef ADI_INIT_DMA_SKIP
#define ADRV9001_DMA_SKIP() \
{ \
 if (device->devStateInfo.swTest) \
 { \
     return (int32_t)recoveryAction; \
 } \
}
#else
#define ADRV9001_DMA_SKIP()
#endif

#define ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT    2
#define ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT   10
#define ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT   18
#define ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT   26
#define ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK 0x3

#define ADRV9001_PROFILE_CHUNK_MAX              256u

const char* const adrv9001_error_table_ArmBootStatus[] =
{
    "ARM is powering up",
    "ARM booted with no failure",
    "ARM Firmware checksum error",
    "Efuse data error",
    "Stream image checksum error",
    "Device profile checksum error",
    "Bootup clkgen setup error",
    "Bootup SCCG init setup error",
    "Device profile init setup error",
    "JTAG build status ready indication",
    "Bootup clock LOGEN error",
    "Error initializing RxQEC hardware",
    "Failed to create Health monitor timers",
    "ADC RCAL error",
    "ADC CCAL error",
    "Stream Run error"
};

const char* const adrv9001_error_table_CmdCtrlMboxCmdError[] =
{
    "No error",
    "Nested Mailbox command, second command ignored",
    "Command not supported",
    "Invalid state",
    "Reserved 1",
    "Reserved 2",
    "Reserved 3",
    "Command error"
};

const char* const adrv9001_error_table_CmdError[] = 
{ 
    "Error occurred during an Init Calibration. Check that no signal is being applied to the Rx ports. Check that "
        "correct external LOs are applied, and synchronized,  where appropriate",
    "Error occurred during a Tracking Calibration. Disable tracking calibrations, reset and program. If enabled "
        "tracking calibrations require external loopback, check loopback connections. If issue persists, contact "
        "Analog Devices for support",
    "Error occurred during a Firmware Mailbox SET or GET transaction.Check the connection between ADRV9001 Evaluation "
        "card and FPGA Platform",
    "Error occurred during a Firmware Mailbox GET transaction.Check the connection between ADRV9001 Evaluation card "
        "and FPGA Platform",
    "Error occurred during a Firmware Mailbox CONFIG transaction.Check the connection between ADRV9001 Evaluation card"
        " and FPGA Platform",
    "Error occurred in Firmware Driver code",
    "Error occurred in Firmware System code"
};

/****************************************************** Local Constants ********************************************************/

static const uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
static const uint32_t RX_CHANNELS[] = {
    ADI_ADRV9001_RX1,
    ADI_ADRV9001_RX2,
    ADI_ADRV9001_ORX1,
    ADI_ADRV9001_ORX2,
    ADI_ADRV9001_ILB1,
    ADI_ADRV9001_ILB2,
    ADI_ADRV9001_ELB1,
    ADI_ADRV9001_ELB2
};

/****************************************************** Static Functions *******************************************************/

static int32_t adrv9001_DmaMemWriteByte(adi_adrv9001_Device_t *device,
    uint32_t address,
    const uint8_t data[],
    uint32_t byteCount)
{
    uint32_t i = 0;
    uint8_t regWrite = 0;
    uint8_t autoInc = ADI_ADRV9001_ARM_MEM_AUTO_INCR;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, data, byteCount);

    ADRV9001_DMAINFO("ARM_MEM_WRITE", address, byteCount);

    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regWrite &= ~ADRV9001_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (address >= ADRV9001_ADDR_ARM_START_DATA)
    {
        regWrite |= ADRV9001_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regWrite |= ADRV9001_BF_ENCODE(0, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != 0)
    {
        regWrite |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, regWrite);

    /* AHB address for write */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", ADRV9001_ADDR_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    for (i = 0; i < byteCount; i++)
    {
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_0+", ADRV9001_ADDR_ARM_DMA_DATA0 + ((address & 0x00000003)), data[i]);
        if (address & 0x00000003)
        {
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_0", ADRV9001_ADDR_ARM_DMA_DATA0, data[i]); /* inc address */
        }

        address++;
        if (autoInc == 0)
        {
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", ADRV9001_ADDR_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    /* setting up the DMA control register for a read to avoid accidental writing */
    regWrite |= ADRV9001_DMA_CTL_RD_WRB;
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, regWrite);

    ADI_API_RETURN(device);
}

static int32_t adrv9001_DmaMemReadByte(adi_adrv9001_Device_t *device,
    uint32_t address,
    uint8_t returnData[],
    uint32_t byteCount,
    uint8_t autoIncrement)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t regRead = 0;
    uint8_t dataRead0 = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    ADRV9001_DMAINFO("ARM_MEM_READ", address, byteCount);

    /*
    Use SPI DMA for Read
    */
    /* Read */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regRead |= ADRV9001_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (!((address >= ADRV9001_ADDR_ARM_START_PROG) && (address <= ADRV9001_ADDR_ARM_END_PROG)))
    {
        regRead |= ADRV9001_DMA_CTL_SYS_CODEB;
    }

    regRead |= ADRV9001_DMA_CTL_LEGACY_MODE;
    
    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regRead |= ADRV9001_BF_ENCODE(0, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoIncrement != 0)
    {
        regRead |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, regRead);

    if (autoIncrement == 0)
    {
        /* AHB address for read */
        /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
        /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", ADRV9001_ADDR_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));


        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i++)
        {
            ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0+", ADRV9001_ADDR_ARM_DMA_DATA0 + (address % 4), &dataRead0);
            returnData[i] = dataRead0;

            if (address & 0x00000003)
            {
                ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0", ADRV9001_ADDR_ARM_DMA_DATA0, &dataRead0); /* inc address */
            }

            address++;

            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", ADRV9001_ADDR_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }
    else
    {
        /* AHB address for read */
        /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
        /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
        ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));


        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i += 4)
        {
            ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0", ADRV9001_ADDR_ARM_DMA_DATA0, &dataRead0);
            returnData[i] = dataRead0;
            ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_1", ADRV9001_ADDR_ARM_DMA_DATA1, &dataRead0);
            returnData[i + 1] = dataRead0;
            ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_2", ADRV9001_ADDR_ARM_DMA_DATA2, &dataRead0);
            returnData[i + 2] = dataRead0;
            
            /* 'single_instruction' has to be cleared before reading DMA_DATA3 and set back after */
            ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x0);
            ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_3", ADRV9001_ADDR_ARM_DMA_DATA3, &dataRead0);
            returnData[i + 3] = dataRead0;
            ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x1);
        }
    }

    ADI_API_RETURN(device);
}

static int32_t adrv9001_FlexStreamProcessorMemWriteByte(adi_adrv9001_Device_t *device,
                                                        uint32_t address,
                                                        const uint8_t data[],
                                                        uint32_t byteCount,
                                                        uint8_t flexSpNumber)
{
    uint32_t i = 0;
    uint8_t regWrite = 0;
    uint8_t autoInc = ADI_ADRV9001_ARM_MEM_AUTO_INCR;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, data, byteCount);

    ADRV9001_DMAINFO("FLEX_SP_ARM_MEM_WRITE", address, byteCount);

    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regWrite &= ~ADRV9001_DMA_CTL_RD_WRB;

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regWrite |= ADRV9001_BF_ENCODE(0, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != 0)
    {
        regWrite |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the flex SP DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, regWrite);

    /* Access the flex SP needed; 0x1 - flex SP0, 0x2 - flex SP1, 0x4 - flex SP2, 0x8 - flex SP3 */
    ADRV9001_SPIWRITEBYTEDMA(device, "FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_SELECT, (0x1 << (flexSpNumber - 1)));

    /* flex SP AHB address for write */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    for (i = 0; i < byteCount; i++)
    {
        ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_0+", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0 + ((address & 0x00000003)), data[i]);
        if (address & 0x00000003)
        {
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0, data[i]); /* inc address */
        }

        address++;
        if (autoInc == 0)
        {
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    /* setting up the DMA control register for a read to avoid accidental writing */
    regWrite |= ADRV9001_DMA_CTL_RD_WRB;
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, regWrite);

    ADI_API_RETURN(device);
}

static int32_t adrv9001_FlexStreamProcessorMemReadByte(adi_adrv9001_Device_t *device,
                                                       uint32_t address,
                                                       uint8_t returnData[],
                                                       uint32_t byteCount,
                                                       uint8_t autoIncrement,
                                                       uint8_t flexSpNumber)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t regRead = 0;
    uint8_t dataRead0 = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    ADRV9001_DMAINFO("ADRV9001_ADDR_FLEX_SP_ARM_MEM_READ", address, byteCount);

    /*
    Use SPI DMA for Read
    */
    /* Read */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regRead |= ADRV9001_DMA_CTL_RD_WRB;

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regRead |= ADRV9001_BF_ENCODE(0, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoIncrement != 0)
    {
        regRead |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, regRead);

    /* Access the flex SP needed; 0x1 - flex SP0, 0x2 - flex SP1, 0x4 - flex SP2, 0x8 - flex SP3 */
    ADRV9001_SPIWRITEBYTEDMA(device, "FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_SELECT, (0x1 << (flexSpNumber - 1)));

    /* flex SP AHB address for read */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    /* starting write at zero address offset */
    for (i = 0; i < byteCount; i++)
    {
        ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_0+", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0 + (address % 4), &dataRead0);
        returnData[i] = dataRead0;

        if (address & 0x00000003)
        {
            ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0, &dataRead0); /* inc address */
        }

        address++;
        if (autoIncrement == 0)
        {
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV9001_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    ADI_API_RETURN(device);
}

static void adrv9001_cfgDataSet(uint8_t cfgData[], uint8_t value, int count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        cfgData[i] = value;
    }
}

/*
 * The 'data' passed in to this function is written at cfgData[] array at 'offset' location and
 * increments 'offset' by 2
 **/
void adrv9001_LoadTwoBytes(uint32_t *offset, uint8_t cfgData[], const uint16_t data)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(data & 0x00FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 8) & 0x00FF);

    *offset = tempOffset;
}

/*
 * The 'data' passed in to this function is written at cfgData[] array at 'offset' location and
 * increments 'offset' by 4
 **/
void adrv9001_LoadFourBytes(uint32_t *offset, uint8_t cfgData[], const uint32_t data)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(data & 0x000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 8) & 0x000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 16) & 0x000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 24) & 0x000000FF);

    *offset = tempOffset;
}

/*
 * The 'data' passed in to this function is written at cfgData[] array at 'offset' location and
 * increments 'offset' by 8
 **/
void adrv9001_LoadEightBytes(uint32_t *offset, uint8_t cfgData[], const uint64_t data)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(data & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 8) & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 16) & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 24) & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 32) & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 40) & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 48) & 0x00000000000000FF);
    cfgData[tempOffset++] = (uint8_t)((data >> 56) & 0x00000000000000FF);

    *offset = tempOffset;
}

void adrv9001_ParseTwoBytes(uint32_t *offset, uint8_t cfgData[], uint16_t *value)
{
    uint32_t tempOffset = *offset;

    *value = 0;
    *value |= (uint16_t)(cfgData[tempOffset++]) << 0;
    *value |= (uint16_t)(cfgData[tempOffset++]) << 8;

    *offset = tempOffset;
}

void adrv9001_ParseFourBytes(uint32_t *offset, uint8_t cfgData[], uint32_t *value)
{
    uint32_t tempOffset = *offset;

    *value = 0;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 0;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 8;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 16;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 24;

    *offset = tempOffset;
}

void adrv9001_ParseEightBytes(uint32_t *offset, uint8_t cfgData[], uint64_t *value)
{
    uint32_t tempOffset = *offset;

    *value = 0;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 0;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 8;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 16;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 24;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 32;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 40;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 48;
    *value |= (uint64_t)(cfgData[tempOffset++]) << 56;

    *offset = tempOffset;
}

/* This function handles only the standard data type in TxConfig_t struct */
/*
Transmitter Structure: TxConfig_t
Size : 44 Bytes

uint32_t            primaryBw_Hz;			//!< TX primary Bandwidth
uint32_t            txInputRate;			//!< TX Input data rate
uint32_t            txInterfaceSampleRate;	//!< TX sample rate at serial interface
int32_t             offsetLo_Hz;            //!< Frequency of LO relative to carrier, positive means LO higher
uint32_t            validDataDelay;			//!< Valid data delay relative to TX Enable by 184MHz clock counter
uint32_t            butterFilterBw;			//!< TX butter filter BW for TxBBF
signalType_e        outputSignalingSel;		//!< Output to Analog signal type
uint8_t             pdBiasCurrent;			//!< pre-distorter programmable bias current
uint8_t             pdGainEn;				//!< TX Pre-distortion gain enable
uint8_t             txBbfPowerMode;         //!< TXBBF  filter power mode
uint32_t            prePDRealPole;			//!< TX Pre-distortion filter Fc
uint32_t            postPDRealPole;			//!< Post-distorter (i.e. interstage) filter Fc
uint8_t             extLoopBackType;		//!< 0: No external loopback connect,  1: loopback before PA, 2: loopback after PA.
uint8_t             extLoopBackForInitCal;	//!< 0: ext loop back should not be used for init cal
int16_t             peakLoopBackPower;      //!< target RMS signal power at input to receiver plus its peak-to-average ratio, -40 to +5 dBm
uint32_t			freqDev;                //!< freq dev value in Hz for both FM_IQ and FM_DM
*/
static void adrv9001_LoadTxByte(adi_adrv9001_Device_t *device,
                                    uint32_t *offset,
                                    uint8_t cfgData[],
                                    const adi_adrv9001_TxProfile_t *txProfile)
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txProfile->primarySigBandwidth_Hz);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txProfile->txInputRate_Hz);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txProfile->txInterfaceSampleRate_Hz);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txProfile->txOffsetLo_kHz);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txProfile->validDataDelay);

    /* txBbf3dBCorner_kHz is referred as butterFilterBw in FW code */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(txProfile->txBbf3dBCorner_kHz));

    cfgData[tempOffset++] = (uint8_t)(txProfile->outputSignaling & 0xFF);

    /* 'txPdBiasCurrent' is referred as 'pdBiasCurrent' in FW */
    cfgData[tempOffset++] = txProfile->txPdBiasCurrent;

    /* 'txPdGainEnable' is referred as 'pdGainEn' in FW */
    cfgData[tempOffset++] = txProfile->txPdGainEnable;

    cfgData[tempOffset++] = (uint8_t)txProfile->txBbfPower;

    /* 'txPrePdRealPole' is referred as 'prePDRealPole' in FW */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(txProfile->txPrePdRealPole_kHz));

    /* 'txPostPdRealPole' is referred as 'postPDRealPole' in FW */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(txProfile->txPostPdRealPole_kHz));

    /* 'txExtLoopBackType' is referred as 'extLoopBackType' in FW */
    cfgData[tempOffset++] = txProfile->txExtLoopBackType;

    /* 'txExtLoopBackForInitCal' is referred as 'extLoopBackForInitCal' in FW */
    cfgData[tempOffset++] = txProfile->txExtLoopBackForInitCal;

    adrv9001_LoadTwoBytes(&tempOffset, cfgData, txProfile->txPeakLoopBackPower);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txProfile->frequencyDeviation_Hz);

    *offset = tempOffset;
}

/* This function handles only txPreProc_t struct in txDpProfile_t struct */
/*
Transmitter Structure: txPreProc_t
Size : 20 Bytes

// tx preprocessor symbol map
uint32_t    symMapSymbol0;   //!< STATIC, not configurable on the fly, BBIC to determine based on user case
uint32_t    symMapSymbol1;   //!< STATIC, not configurable on the fly, BBIC to determine based on user case
uint32_t    symMapSymbol2;   //!< STATIC, not configurable on the fly, BBIC to determine based on user case
uint32_t    symMapSymbol3;   //!< STATIC, not configurable on the fly, BBIC to determine based on user case
uint8_t     symMapDivFactor; //!< STATIC, not configurable on the fly, BBIC to determine based on user case

// Tx pre-processor config parameters that support four modes of operation
txDpPreProcMode_e preProcMode; //!< STATIC, not configurable on the fly, BBIC to determine based on user case
pfirBank_e txPreProcWbNbPfirI; //!< Block #3, TPFIR_I, Dynamic, configurable during dynamic profile switching
pfirBank_e txPreProcWbNbPfirQ; //!< Block #5, TPFIR_Q, Dynamic, configurable during dynamic profile switching

*/
static void adrv9001_LoadTxPreProcBlockConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxPreProc_t *txPreProc)
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txPreProc->txPreProcSymbol0);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txPreProc->txPreProcSymbol1);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txPreProc->txPreProcSymbol2);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txPreProc->txPreProcSymbol3);

    cfgData[tempOffset++] = txPreProc->txPreProcSymMapDivFactor;

    cfgData[tempOffset++] = (uint8_t)(txPreProc->txPreProcMode & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(txPreProc->txPreProcWbNbPfirIBankSel & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(txPreProc->txPreProcWbNbPfirQBankSel & 0xFF);

    *offset = tempOffset;
}

/* This function handles only txWbIntTop_t struct in txDpProfile_t struct */
/*
Transmitter Structure: txWbIntTop_t
Size : 8 Bytes

uint8_t interpBy2Blk30En;    //!< Setting for block #30. INT2_5
uint8_t interpBy2Blk28En;    //!< Setting for block #28. INT2_4
uint8_t interpBy2Blk26En;    //!< Setting for block #26. INT2_3
uint8_t interpBy2Blk24En;    //!< Setting for block #24. INT2_2
uint8_t interpBy2Blk22En;    //!< Setting for block #22. INT2_1
uint8_t wbLpfBlk22p1En;      //!< Setting for block #22.1 WBLPF
uint8_t padding[2u];
*/
static void adrv9001_LoadTxWbBlockInterpConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxWbIntTop_t *txWbIntTop)
{
    uint32_t tempOffset = *offset;

    /* 'txInterpBy2Blk30En' is referred as 'interpBy2Blk30En' in FW */
    cfgData[tempOffset++] = txWbIntTop->txInterpBy2Blk30En;

    /* 'txInterpBy2Blk28En' is referred as 'interpBy2Blk28En' in FW */
    cfgData[tempOffset++] = txWbIntTop->txInterpBy2Blk28En;

    /* 'txInterpBy2Blk26En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txWbIntTop->txInterpBy2Blk26En;

    /* 'txInterpBy2Blk24En' is referred as 'interpBy2Blk24En' in FW */
    cfgData[tempOffset++] = txWbIntTop->txInterpBy2Blk24En;

    /* 'txInterpBy2Blk22En' is referred as 'interpBy2Blk22En' in FW */
    cfgData[tempOffset++] = txWbIntTop->txInterpBy2Blk22En;

    /* 'txWbLpfBlk22p1En' is referred as 'wbLpfBlk22p1En' in FW */
    cfgData[tempOffset++] = txWbIntTop->txWbLpfBlk22p1En;

    /* 2 bytes padding is needed for alignment */
    tempOffset += 2;

    *offset = tempOffset;
}

/* This function handles only txNbIntTop_t struct in txDpProfile_t struct */
/*
Transmitter Structure: txNbIntTop_t
Size : 12 Bytes

uint8_t interpBy2Blk20En	//!< Setting for block #20. INT2_6
uint8_t interpBy2Blk18En;   //!< Setting for block #18. INT2_5
uint8_t interpBy2Blk16En;	//!< Setting for block #16. INT2_4
uint8_t interpBy2Blk14En;	//!< Setting for block #14. INT2_3
uint8_t interpBy2Blk12En;	//!< Setting for block #12. INT2_2
uint8_t interpBy3Blk10En;	//!< Setting for block #10. INT3
uint8_t	interpBy2Blk8En;	//!< Setting for block #8. INT2_1

uint8_t scicBlk32En;		//!< Setting for block #32. TSCIC
uint8_t scicBlk32DivFactor; //!< STATIC or DYNAMIC, BBIC to determine based on user case
uint8_t padding[3u];
*/
static void adrv9001_LoadTxNbBlockInterpConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxNbIntTop_t *txNbIntTop)
{
    uint32_t tempOffset = *offset;

    /* 'txInterpBy2Blk20En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy2Blk20En;

    /* 'txInterpBy2Blk18En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy2Blk18En;

    /* 'txInterpBy2Blk16En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy2Blk16En;

    /* offset = 91; 'txInterpBy2Blk14En' is written at cfgData[91] and 'offset' is incremented by 1
     * 'txInterpBy2Blk14En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy2Blk14En;

    /* 'txInterpBy2Blk12En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy2Blk12En;

    /* offset = 93; 'txInterpBy3Blk10En' is written at cfgData[93] and 'offset' is incremented by 1
     * 'txInterpBy3Blk10En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy3Blk10En;

    /* 'txInterpBy2Blk8En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txInterpBy2Blk8En;

    /* 'txScicBlk32En' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txScicBlk32En;

    /* 'txScicBlk32DivFactor' is referred as 'interpBy2Blk26En' in FW */
    cfgData[tempOffset++] = txNbIntTop->txScicBlk32DivFactor;

    /* 3 bytes padding is needed for alignment */
    tempOffset += 3;

    *offset = tempOffset;
}

/* This function handles only txIntTop_t struct in txDpProfile_t struct */
/*
Transmitter Structure: txIntTop_t
Size : 8 Bytes

uint8_t interpBy3Blk44p1En; //!< Setting for block #44.1. INT3_50
uint8_t sinc3Blk44En;       //!< Setting for block #44. SINC3TX
uint8_t sinc2Blk42En;       //!< Setting for block #42. SINC2
uint8_t interpBy3Blk40En;   //!< Setting for block #40. INT3
uint8_t interpBy2Blk38En;   //!< Setting for block #38. INT2_2
uint8_t interpBy2Blk36En;   //!< Setting for block #36. INT2_1
uint8_t padding[2u];
*/
static void adrv9001_LoadTxTopBlockInterpConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxIntTop_t *txIntTop)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = txIntTop->interpBy3Blk44p1En;

    cfgData[tempOffset++] = txIntTop->sinc3Blk44En;

    cfgData[tempOffset++] = txIntTop->sinc2Blk42En;

    cfgData[tempOffset++] = txIntTop->interpBy3Blk40En;

    cfgData[tempOffset++] = txIntTop->interpBy2Blk38En;

    cfgData[tempOffset++] = txIntTop->interpBy2Blk36En;

    /* 2 bytes padding is needed for alignment */
    tempOffset += 2;

    *offset = tempOffset;
}

/* This function handles only txIntTopFreqDevMap_t struct in txDpProfile_t struct */
/*
Transmitter Structure: txIntTopFreqDevMap_t
Size : 16 Bytes

//!< Frequency dev mapper
uint32_t rrc2Frac;		//!< Frequency Deviation
uint32_t mpll;			//!< Frequency fraction multiplier
uint32_t nchLsw;		//!< Frequency offset. Contains lest significant word, i.e Bit[0:31]
uint8_t  nchMsb;		//!< Frequency offset. Contains most significant 3 bits, i.e Bit[32:34]
uint8_t  freqDevMapEn;	//!< Enable/disable block #47 Freq_Dev Mapper
uint8_t  txRoundEn;		//!< Enable/disable tx round block #46
uint8_t  padding[1u];
*/
static void adrv9001_LoadTxTopBlockFreqDevMapConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxIntTopFreqDevMap_t *intTopFreqDevMap)
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, intTopFreqDevMap->rrc2Frac);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, intTopFreqDevMap->mpll);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, intTopFreqDevMap->nchLsw);

    cfgData[tempOffset++] = (intTopFreqDevMap->nchMsb & 0x07);

    cfgData[tempOffset++] = intTopFreqDevMap->freqDevMapEn;

    cfgData[tempOffset++] = intTopFreqDevMap->txRoundEn;

    /* 1 byte padding is needed for alignment */
    tempOffset += 1;

    *offset = tempOffset;
}

/* This function handles only ncoDpConfig_t struct */
/*
Transmitter Structure: ncoDpConfig_t
Size : 12 Bytes

int32_t        freq;       //!~ NCO output frequency
uint32_t       samplefreq; //! Sample rate frequency NCO connects to
uint16_t       phase;      //! phase offset
uint8_t        realOut;    //! real out enable
uint8_t        padding[1u];
*/
static void adrv9001_LoadNcoDpConfig(const adi_adrv9001_NcoDpConfig_t *iqdmNco, uint32_t *offset, uint8_t cfgData[])
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, iqdmNco->freq);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, iqdmNco->sampleFreq);

    adrv9001_LoadTwoBytes(&tempOffset, cfgData, iqdmNco->phase);

    cfgData[tempOffset++] = iqdmNco->realOut;

    /* 1 byte padding is needed for alignment */
    tempOffset += 1;

    *offset = tempOffset;
}

/* This function handles only txIqdmDuc_t struct in txDpProfile_t struct */
/*
Transmitter Structure: txIqdmDuc_t
Size : 32 Bytes

// Tx IQ DM DUC config parameters that support three modes of operation
txDpIqDmDucMode_e iqdmDucMode;

uint8_t  padding[3u];

// Parameters for TX_DP_IQDMDUC_MODE1 IqDm mode
uint32_t iqdmDev;

uint32_t iqdmDevOffset;
uint32_t iqdmScaler;
uint32_t iqdmThreshold;

// Parameters for IQDM DUC NCO
ncoDpConfig_t iqdmNco;
*/
static void adrv9001_LoadTxIqdmDucBlockConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxIqdmDuc_t *iqdmDuc)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(iqdmDuc->iqdmDucMode & 0xFF);

    /* 3 bytes padding is needed for alignment */
    tempOffset += 3;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, iqdmDuc->iqdmDev);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, iqdmDuc->iqdmDevOffset);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, iqdmDuc->iqdmScalar);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, iqdmDuc->iqdmThreshold);

    adrv9001_LoadNcoDpConfig(&(iqdmDuc->iqdmNco), &tempOffset, cfgData);

    *offset = tempOffset;
}

/* This function handles only txDpProfile_t struct in TxConfig_t struct */
/*
Transmitter Structure: txDpProfile_t
Size : 96 Bytes

txPreProc_t           preProc;				//! pre_proc block settings
txWbIntTop_t          wbIntTop;				//! wb_int_top block settings
txNbIntTop_t          nbIntTop;				//! nb_int_top block settings
txIntTop_t			  intTop;				//! int_top block settings
txIntTopFreqDevMap_t  intTopFreqDevMap;
txIqdmDuc_t           iqdmDuc;				//! iqdm_duc block settings
*/
static void adrv9001_LoadTxDataPathProfileConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxDpProfile_t *txDpProfile)
{
    uint32_t tempOffset = *offset;

    /* This function handles only txPreProc_t struct in txDpProfile_t struct */
    adrv9001_LoadTxPreProcBlockConfig(device, &tempOffset, cfgData, &(txDpProfile->txPreProc));

    /* This function handles only txWbIntTop_t struct in txDpProfile_t struct */
    adrv9001_LoadTxWbBlockInterpConfig(&tempOffset, cfgData, &(txDpProfile->txWbIntTop));

    /* This function handles only txNbIntTop_t struct in txDpProfile_t struct */
    adrv9001_LoadTxNbBlockInterpConfig(&tempOffset, cfgData, &(txDpProfile->txNbIntTop));

    /* This function handles only txIntTop_t struct in txDpProfile_t struct */
    adrv9001_LoadTxTopBlockInterpConfig(&tempOffset, cfgData, &(txDpProfile->txIntTop));

    /* This function handles only txIntTopFreqDevMap_t struct in txDpProfile_t struct */
    adrv9001_LoadTxTopBlockFreqDevMapConfig(&tempOffset, cfgData, &(txDpProfile->txIntTopFreqDevMap));

    /* This function handles only txIqdmDuc_t struct in txDpProfile_t struct */
    adrv9001_LoadTxIqdmDucBlockConfig(device, &tempOffset, cfgData, &(txDpProfile->txIqdmDuc));

    *offset = tempOffset;
}

/* This function handles only ssiConfig_t struct in TxConfig_t struct */
/*
Transmitter Structure: ssiConfig_t
Size : 20 Bytes

ssiType_e       ssiType;                //!< SSI type
ssiDataFormat_e ssiDataFormatSel;       //!< SSI data format
ssiNumLane_e    numLaneSel;             //!< SSI number of lanes
ssiStrobeType_e strobeType;             //!< SSI strobe type
uint8_t         lsbFirst;               //!< SSI LSB first
uint8_t         qFirst;                 //!< SSI Q data first
uint8_t         txRefClockPin;          //!< TX reference clock pin control
uint8_t         lvdsBitInversion;       //!< LVDS SSI bit inversion
uint8_t         lvdsUseLsbIn12bitMode;  //!< LVDS use LSB in 12 bit mode
uint8_t         lvdsTxFullRefClkEn;     //!< LVDS Tx full refclk enable
uint8_t         lvdsRxClkInversionEn;   //!< LVDS Rx clock inversion enable
uint8_t         cmosTxDdrNegStrobeEn;   //!< CMOS Tx DDR negative strobe enable
uint8_t         cmosDdrPosClkEn;        //!< CMOS DDR positive clock enable
uint8_t         cmosDdrClkInversionEn;  //!< CMOS DDR clock inversion enable
uint8_t         cmosDdrEn;              //!< CMOS DDR enable
uint8_t         rxMaskStrobeEn;         //!< Rx enable for strobe mask
uint8_t         padding[4u];
*/
static void adrv9001_LoadSsiConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_SsiConfig_t *ssiConfig)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(ssiConfig->ssiType & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(ssiConfig->ssiDataFormatSel & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(ssiConfig->numLaneSel & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(ssiConfig->strobeType & 0xFF);

    cfgData[tempOffset++] = ssiConfig->lsbFirst;

    cfgData[tempOffset++] = ssiConfig->qFirst;

    cfgData[tempOffset++] = (uint8_t)(ssiConfig->txRefClockPin & 0xFF);

    cfgData[tempOffset++] = ssiConfig->lvdsBitInversion;

    cfgData[tempOffset++] = ssiConfig->lvdsUseLsbIn12bitMode;

    cfgData[tempOffset++] = (uint8_t)ssiConfig->lvdsTxFullRefClkEn;

    cfgData[tempOffset++] = (uint8_t)ssiConfig->lvdsRxClkInversionEn;

    cfgData[tempOffset++] = (uint8_t)ssiConfig->cmosTxDdrNegStrobeEn;

    cfgData[tempOffset++] = (uint8_t)ssiConfig->cmosDdrPosClkEn;

    cfgData[tempOffset++] = (uint8_t)ssiConfig->cmosDdrClkInversionEn;

    cfgData[tempOffset++] = (uint8_t)ssiConfig->cmosDdrEn;
    
    cfgData[tempOffset++] = (uint8_t)ssiConfig->rxMaskStrobeEn;

    /* 4 bytes of padding is needed for alignment */
    tempOffset += 4;

    *offset = tempOffset;
}

/* This function handles only ChannelEnableDelay_t struct for Tx/Rx */
/*
Transmitter Structure: ChannelEnableDelay_t
Size : 28 Bytes

uint32_t            channelEnableRiseToOnDelay;
uint32_t            channelEnableRiseToAnalogOnDelay;
uint32_t            channelEnableFallToOffDelay;
uint32_t            channelEnableGuardDelay;
uint32_t            channelEnableHoldDelay;
uint32_t            reserved[2u];
*/
static void adrv9001_LoadChannelEnableDelayConfig(adi_adrv9001_Device_t *device,
                                                  uint32_t *offset,
                                                  uint8_t cfgData[])
{
    /* The entire struct is now reserved */
    *offset += 28;
}
/*********************************************************************************************
* Refer TxConfig_t structure below or in device_profile_t.h file in Navassa ARM firmware
* for the order of transferring the Tx profile config info from API to ARM
**********************************************************************************************
Size : 188 Bytes

typedef struct
{
    uint32_t            primaryBw_Hz;			     //!< TX primary Bandwidth
    uint32_t            txInputRate;			     //!< TX Input data rate
    uint32_t            txInterfaceSampleRate;	     //!< TX sample rate at serial interface
    int32_t             offsetLo_Hz;                 //!< Frequency of LO relative to carrier, positive means LO higher
    uint32_t            validDataDelay;			     //!< Valid data delay relative to TX Enable by 184MHz clock counter
    uint32_t            butterFilterBw;			     //!< TX butter filter BW for TxBBF
    signalType_e        outputSignalingSel;		     //!< Output to Analog signal type
    uint8_t             pdBiasCurrent;			     //!< pre-distorter programmable bias current
    uint8_t             pdGainEn;				     //!< TX Pre-distortion gain enable
    uint8_t             txBbfPowerMode;              //!< TXBBF  filter power mode
    uint32_t            prePDRealPole;			     //!< TX Pre-distortion filter Fc
    uint32_t            postPDRealPole;			     //!< Post-distorter (i.e. interstage) filter Fc
    uint8_t             extLoopBackType;		     //!< 0: No external loopback connect,  1: loopback before PA, 2: loopback after PA.
    uint8_t             extLoopBackForInitCal;	     //!< 0: ext loop back should not be used for init cal
    int16_t             peakLoopBackPower;           //!< target RMS signal power at input to receiver plus its peak-to-average ratio, -40 to +5 dBm
    uint32_t			freqDev;                     //!< freq dev value in Hz for both FM_IQ and FM_DM
    txDpProfile_t       txDpProfile;			     //!< TX digital data path config
    ssiConfig_t         txSsiConfig;			     //!< TX Serial data interface config
    enableDelay_t       txEnableDelay;               //!< TX enable delay timing
    uint32_t            reserved[2u];                // Reserved for future feature
} TxConfig_t;
*/
static void adrv9001_TxProfileConfigWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_TxProfile_t *txProfile, uint8_t cfgData[], uint32_t *offset)
{
    uint32_t tempOffset = *offset;

    /* All standard data type in TxConfig_t struct are packed in adrv9001_LoadTxByte() and stored in cfgData[]
     * i.e. structures such as txDpProfile_t, and ssiConfig_t within TxConfig_t are handled in separate functions below */
    adrv9001_LoadTxByte(device, &tempOffset, cfgData, txProfile);

    /* Handles txDpProfile_t within TxConfig_t in adrv9001_LoadTxDataPathProfileConfig() */
    adrv9001_LoadTxDataPathProfileConfig(device, &tempOffset, cfgData, &(txProfile->txDpProfile));

    /* Handles ssiConfig_t within TxConfig_t in adrv9001_LoadSsiConfig() */
    adrv9001_LoadSsiConfig(device, &tempOffset, cfgData, &(txProfile->txSsiConfig));

    adrv9001_LoadChannelEnableDelayConfig(device, &tempOffset, cfgData);

    *offset = tempOffset;
}

/* This function handles only the standard data type in RxConfig_t struct */
/*
Transmitter Structure: RxConfig_t
Size : 44 Bytes

uint32_t            primaryBw_Hz;           //!< RX primary Bandwidth
uint32_t            rxOutputRate;			//!< RX Output data rate
uint32_t            rxInterfaceSampleRate;	//!< RX sample rate at serial interface
int32_t             offsetLo_Hz;			//!< Offset in Hz. 0: On LO
uint8_t             signalOnLo;				//!< !0: Signal on LO  0: no signal on LO
signalType_e        outputSignalingSel;		//!< Output to BBIC signal type
uint8_t             filterOrder;			//!< 1st or 2nd order ABBF filter
uint8_t             filterOrderLp;			//!< 1st or 2nd order ABBF filter Low Power ADC
uint32_t            hpAdcCorner;			//!< High Power ADC corner freq
uint32_t            lpAdcCorner;            //!< Low Power ADC 3dB corner frequency in Hz
uint32_t            adcClk_Hz;				//!< ADC clock 2.2G/1.47G/1.1G
uint32_t            corner3dB;				//!< TIA bandwidth
uint32_t            corner3dBLp;			//!< TIA bandwidth for Low Power ADC
uint8_t             tiaPower;				//!< RX TIA Power setting
uint8_t             tiaPowerLp;				//!< RX TIA Power setting Low Power ADC
adcType_e           adcType;				//!< ADC type: low/high power ADC
uint8_t             padding[1u];
*/
static void adrv9001_LoadRxByte(adi_adrv9001_Device_t *device,
                                uint32_t *offset,
                                uint8_t cfgData[],
                                const adi_adrv9001_RxProfile_t *rxProfile)
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxProfile->primarySigBandwidth_Hz);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxProfile->rxOutputRate_Hz);

    /* rxInterfaceSampleRate_Hz is referred as rxInterfaceSampleRate in FW code */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxProfile->rxInterfaceSampleRate_Hz);

    /* 'rxOffsetLo_kHz' is referred as 'offsetLo_Hz' in FW */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(rxProfile->rxOffsetLo_kHz));

    /* 'rxSignalOnLo' is referred as 'signalOnLo' in FW */
    cfgData[tempOffset++] = rxProfile->rxSignalOnLo;

    cfgData[tempOffset++] = (uint8_t)(rxProfile->outputSignaling & 0xFF);

    cfgData[tempOffset++] = rxProfile->filterOrder;

    cfgData[tempOffset++] = rxProfile->filterOrderLp;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxProfile->hpAdcCorner);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxProfile->lpAdcCorner);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(rxProfile->adcClk_kHz));

    /* 'rxCorner3dB_kHz' is referred as 'corner3dB' in FW */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(rxProfile->rxCorner3dB_kHz));

    /* 'rxCorner3dBLp_kHz' is referred as 'corner3dBLp_kHz' in FW */
    adrv9001_LoadFourBytes(&tempOffset, cfgData, KILO_TO_BASE_UNIT(rxProfile->rxCorner3dBLp_kHz));

    cfgData[tempOffset++] = (uint8_t)rxProfile->tiaPower;
    cfgData[tempOffset++] = (uint8_t)rxProfile->tiaPowerLp;

    cfgData[tempOffset++] = (uint8_t)(rxProfile->adcType & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(rxProfile->lpAdcCalMode & 0xFF);

    *offset = tempOffset;
}

/* This function handles only rxNbDecTop_t struct in rxDpProfile_t struct */
/*
Transmitter Structure: rxNbDecTop_t
Size : 12 Bytes

uint8_t scicBlk23En;			//!< Setting for block #23. RSCIC
uint8_t scicBlk23DivFactor;		//!< RSCIC division factor
uint8_t scicBlk23LowRippleEn;	//!< Enable low ripple mode
uint8_t decBy2Blk35En;			//!< Setting for decimator block #35, decimation factor by 2
uint8_t decBy2Blk37En;			//!< Setting for decimator block #37, decimation factor by 2
uint8_t decBy2Blk39En;			//!< Setting for decimator block #39, decimation factor by 2
uint8_t decBy2Blk41En;			//!< Setting for decimator block #41, decimation factor by 2
uint8_t decBy2Blk43En;			//!< Setting for decimator block #43, decimation factor by 2
uint8_t decBy3Blk45En;			//!< Setting for decimator block #45, decimation factor by 3
uint8_t decBy2Blk47En;			//!< Setting for decimator block #47, decimation factor by 2
uint8_t padding[2u];
*/
static void adrv9001_LoadRxNbDecTopConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxNbDecTop_t *rxNbDecTop)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = rxNbDecTop->scicBlk23En;
    cfgData[tempOffset++] = rxNbDecTop->scicBlk23DivFactor;
    cfgData[tempOffset++] = rxNbDecTop->scicBlk23LowRippleEn;
    cfgData[tempOffset++] = rxNbDecTop->decBy2Blk35En;
    cfgData[tempOffset++] = rxNbDecTop->decBy2Blk37En;
    cfgData[tempOffset++] = rxNbDecTop->decBy2Blk39En;
    cfgData[tempOffset++] = rxNbDecTop->decBy2Blk41En;
    cfgData[tempOffset++] = rxNbDecTop->decBy2Blk43En;
    cfgData[tempOffset++] = rxNbDecTop->decBy3Blk45En;
    cfgData[tempOffset++] = rxNbDecTop->decBy2Blk47En;

    /* 2 bytes padding is needed for alignment */
    tempOffset += 2;

    *offset = tempOffset;
}

/* This function handles only rxWbDecTop_t struct in rxDpProfile_t struct */
/*
Transmitter Structure: rxWbDecTop_t
Size : 8 Bytes

uint8_t decBy2Blk25En; //!< Setting for decimator block #25. decimation factor by 2
uint8_t decBy2Blk27En; //!< Setting for decimator block #27. decimation factor by 2
uint8_t decBy2Blk29En; //!< Setting for decimator block #29. decimation factor by 2
uint8_t decBy2Blk31En; //!< Setting for decimator block #31. decimation factor by 2
uint8_t decBy2Blk33En; //!< Setting for decimator block #33. decimation factor by 2
uint8_t wbLpfBlk33p1En;//!< Setting for decimator block #33.1. WBLPF decimation factor by 2
uint8_t padding[2u];
*/
static void adrv9001_LoadRxWbDecTopConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxWbDecTop_t *rxWbDecTop)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = rxWbDecTop->decBy2Blk25En;
    cfgData[tempOffset++] = rxWbDecTop->decBy2Blk27En;
    cfgData[tempOffset++] = rxWbDecTop->decBy2Blk29En;
    cfgData[tempOffset++] = rxWbDecTop->decBy2Blk31En;
    cfgData[tempOffset++] = rxWbDecTop->decBy2Blk33En;
    cfgData[tempOffset++] = rxWbDecTop->wbLpfBlk33p1En;

    /* 2 bytes padding is needed for alignment */
    tempOffset += 2;

    *offset = tempOffset;
}

/* This function handles only rxDecTop_t struct in rxDpProfile_t struct */
/*
Transmitter Structure: rxDecTop_t
Size : 8 Bytes

uint8_t decBy3Blk15En;      //!< Setting for block #15.   DEC2
uint8_t decBy2Hb3Blk17p1En; //!< Setting for block #17.1. HB3
uint8_t decBy2Hb4Blk17p2En; //!< Setting for block #17.2. HB4
uint8_t decBy2Hb5Blk19p1En; //!< Setting for block #19.1. HB5
uint8_t decBy2Hb6Blk19p2En; //!< Setting for block #19.2. HB6
uint8_t padding[3u];
*/
static void adrv9001_LoadRxDecTopConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxDecTop_t *rxDecTop)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = rxDecTop->decBy3Blk15En;
    cfgData[tempOffset++] = rxDecTop->decBy2Hb3Blk17p1En;
    cfgData[tempOffset++] = rxDecTop->decBy2Hb4Blk17p2En;
    cfgData[tempOffset++] = rxDecTop->decBy2Hb5Blk19p1En;
    cfgData[tempOffset++] = rxDecTop->decBy2Hb6Blk19p2En;

    /* 3 bytes padding is needed for alignment */
    tempOffset += 3;

    *offset = tempOffset;
}

/* This function handles only rxSincHbTop_t struct in rxDpProfile_t struct */
/*
Transmitter Structure: rxSincHbTop_t
Size : 28 Bytes

rxSincGainMuxOutput_e sincGainMux;							//!< Sinc gain setting
rxSincMux5Output_e    sincMux;								//!< Sinc MUX selection
rxHBMuxOutput_e       hbMux;								//!< HB MUX selection
uint8_t               isGainCompEnabled;					//!< Gain compensation
int16_t               gainComp9GainI[NUM_GAIN_COMP9_VALUES];
int16_t               gainComp9GainQ[NUM_GAIN_COMP9_VALUES];
*/
static void adrv9001_LoadRxSincHbTopConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxSincHbTop_t *rxSincHBTop)
{
    uint32_t tempOffset = *offset;
    uint32_t count = 0;

    cfgData[tempOffset++] = (uint8_t)(rxSincHBTop->sincGainMux & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(rxSincHBTop->sincMux & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(rxSincHBTop->hbMux & 0xFF);

    cfgData[tempOffset++] = rxSincHBTop->isGainCompEnabled;

    /* gainComp9GainI[] and gainComp9GainQ[] are arrays and each array values have to be
       written in contiguous memory location. So need a separate 'for' loop */
    for (count = 0; count < ADI_ADRV9001_NUM_GAIN_COMP9; count++)
    {
        adrv9001_LoadTwoBytes(&tempOffset, cfgData, rxSincHBTop->gainComp9GainI[count]);
    }

    for (count = 0; count < ADI_ADRV9001_NUM_GAIN_COMP9; count++)
    {
        adrv9001_LoadTwoBytes(&tempOffset, cfgData, rxSincHBTop->gainComp9GainQ[count]);
    }

    *offset = tempOffset;
}

/* This function handles only dpInFifoConfig_t struct in rxnbDemConfig_t */
/*
Transmitter Structure: dpInFifoConfig_t
Size : 4 Bytes

dpInFifoInputSel_e dpInFifoTestDataSel;
uint8_t            dpInFifoEn;          //!< dpinfifo enable
dpInFifoMode_e     dpInFifoMode;
uint8_t            padding[1u];
*/
static void adrv9001_LoadRxDpInFifoConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxDpInFifoConfig_t *dpInFifo)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(dpInFifo->dpInFifoTestDataSel & 0xFF);

    cfgData[tempOffset++] = dpInFifo->dpInFifoEn;

    cfgData[tempOffset++] = (uint8_t)(dpInFifo->dpInFifoMode & 0xFF);

    /* 1 byte padding is needed for alignment */
    tempOffset += 1;

    *offset = tempOffset;
}

/* This function handles only rxNbNcoConfig_t struct in rxnbDemConfig_t */
/*
Transmitter Structure: rxNbNcoConfig_t
Size : 16 Bytes

uint8_t            rxNbNcoEn;		//!< rxnb nco enable
uint8_t            padding[3u];
ncoDpConfig_t      rxNbNcoConfig;
*/
static void adrv9001_LoadRxNbNcoConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxNbNcoConfig_t *rxNbNco)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = rxNbNco->rxNbNcoEn;

    /* 3 bytes padding is needed for alignment */
    tempOffset += 3;

    /* This function handles only ncoDpConfig_t struct in rxNbNcoConfig_t struct */
    adrv9001_LoadNcoDpConfig(&(rxNbNco->rxNbNcoConfig), &tempOffset, cfgData);

    *offset = tempOffset;
}

/* This function handles only rxWbNbCompPFir_t struct in rxnbDemConfig_t */
/*
Transmitter Structure: rxWbNbCompPFir_t
Size : 4 Bytes

rpFirInMuxSel_e    rxWbNbCompPFirInMuxSel;
uint8_t            rxWbNbCompPFirEn;            //!< rx WbNb compensation PFir enable
pfirBank_e 		   rxWbNbCompPFirBankSel;
uint8_t            padding[1u];
*/
static void adrv9001_LoadRxWbNbCompPfirConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxWbNbCompPFir_t *rxWbNbCompPFir)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = (uint8_t)(rxWbNbCompPFir->rxWbNbCompPFirInMuxSel & 0xFF);

    cfgData[tempOffset++] = rxWbNbCompPFir->rxWbNbCompPFirEn;

    cfgData[tempOffset++] = (uint8_t)(rxWbNbCompPFir->bankSel & 0xFF);

    /* 1 byte padding is needed for alignment */
    tempOffset += 1;

    *offset = tempOffset;
}

/* This function handles only rxResampConfig_t struct in rxDpProfile_t struct */
/*
Transmitter Structure: rxResampConfig_t
Size : 12 Bytes

uint8_t            rxReSampEn;   //!< resampler enable
uint8_t            padding[3u];
uint32_t           reSampPhaseI; //!< I channel resampler phase
uint32_t           reSampPhaseQ; //!< Q channel resampler phase
*/
static void adrv9001_RxResampConfig(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxResampConfig_t *rxsamp)
{
    uint32_t tempOffset = *offset;

    cfgData[tempOffset++] = rxsamp->rxResampEn;

    /* 3 bytes padding is needed for alignment */
    tempOffset += 3;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxsamp->resampPhaseI);

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxsamp->resampPhaseQ);

    *offset = tempOffset;
}

/* This function handles only rxnbDemConfig_t struct in rxDpProfile_t struct */
/*
Transmitter Structure: rxnbDemConfig_t
Size : 40 Bytes

dpInFifoConfig_t   dpInFifo;
rxNbNcoConfig_t    rxNbNco;
rxWbNbCompPFir_t   rxWbNbCompPFir;
rxReSampConfig_t   reSamp;
gsOutMuxSel_e      gsOutMuxSel;		//!< fic algorithm enable
rxOutSel_e         rxOutSel;
rxRoundMode_e      rxRoundMode;
dpArmSel_e         dpArmSel;
*/
static void adrv9001_LoadRxNbDemConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxNbDemConfig_t *rxNbDem)
{
    uint32_t tempOffset = *offset;

    /* This function handles only dpInFifoConfig_t struct in rxnbDemConfig_t struct */
    adrv9001_LoadRxDpInFifoConfig(device, &tempOffset, cfgData, &(rxNbDem->dpInFifo));

    /* This function handles only rxNbNcoConfig_t struct in rxnbDemConfig_t struct */
    adrv9001_LoadRxNbNcoConfig(device, &tempOffset, cfgData, &(rxNbDem->rxNbNco));

    /* This function handles only rxWbNbCompPFir_t struct in rxnbDemConfig_t struct */
    adrv9001_LoadRxWbNbCompPfirConfig(device, &tempOffset, cfgData, &(rxNbDem->rxWbNbCompPFir));

    /* This function handles only rxResampConfig_t struct in rxnbDemConfig_t struct */
    adrv9001_RxResampConfig(&tempOffset, cfgData, &(rxNbDem->resamp));

    cfgData[tempOffset++] = (uint8_t)(rxNbDem->gsOutMuxSel & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(rxNbDem->rxOutSel & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(rxNbDem->rxRoundMode & 0xFF);

    cfgData[tempOffset++] = (uint8_t)(rxNbDem->dpArmSel & 0xFF);

    *offset = tempOffset;
}

/* This function handles only rxDpProfile_t struct in RxConfig_t struct */
/*
Transmitter Structure: rxDpProfile_t
Size : 96 Bytes

rxNbDecTop_t    nbDecTop;	//!< rxnb_dec_top block
rxWbDecTop_t    wbDecTop;	//!< rxwb_dec_top block
rxDecTop_t      decTop;		//!< rx_dec_top block
rxSincHbTop_t   sincHbTop;	//!< rx sinc hb block
rxNbDemConfig_t rxNbDem;	//!< rx nb dem block
*/
static void adrv9001_LoadRxDataPathProfileConfig(adi_adrv9001_Device_t *device, uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxDpProfile_t *rxDpProfile)
{
    uint32_t tempOffset = *offset;

    /* This function handles only rxNbDecTop_t struct in rxDpProfile_t struct */
    adrv9001_LoadRxNbDecTopConfig(&tempOffset, cfgData, &(rxDpProfile->rxNbDecTop));

    /* This function handles only rxWbDecTop_t struct in rxDpProfile_t struct */
    adrv9001_LoadRxWbDecTopConfig(&tempOffset, cfgData, &(rxDpProfile->rxWbDecTop));

    /* This function handles only rxDecTop_t struct in rxDpProfile_t struct */
    adrv9001_LoadRxDecTopConfig(&tempOffset, cfgData, &(rxDpProfile->rxDecTop));

    /* This function handles only rxSincHbTop_t struct in rxDpProfile_t struct */
    adrv9001_LoadRxSincHbTopConfig(device, &tempOffset, cfgData, &(rxDpProfile->rxSincHBTop));

    /* This function handles only rxNbDemConfig_t struct in rxDpProfile_t struct */
    adrv9001_LoadRxNbDemConfig(device, &tempOffset, cfgData, &(rxDpProfile->rxNbDem));

    *offset = tempOffset;
}

/*********************************************************************************************
* Refer RxConfig_t structure below or in device_profile_t.h file in Navassa ARM firmware
* for the order of transferring the Rx, ORx, ILB and ELB profile config info from API to ARM
**********************************************************************************************
Size : 188 Bytes

typedef struct
{
    uint32_t            primaryBw_Hz;           //!< RX primary Bandwidth
    uint32_t            rxOutputRate;			//!< RX Output data rate
    uint32_t            rxInterfaceSampleRate;	//!< RX sample rate at serial interface
    int32_t             offsetLo_Hz;			//!< Offset in Hz. 0: On LO
    uint8_t             signalOnLo;				//!< !0: Signal on LO  0: no signal on LO
    rxSignalType_e      outputSignalingSel;		//!< Output to BBIC signal type
    uint8_t             filterOrder;			//!< 1st or 2nd order ABBF filter
    uint8_t             filterOrderLp;			//!< 1st or 2nd order ABBF filter Low Power ADC
    uint32_t            hpAdcCorner;			//!< High Power ADC corner freq
    uint32_t            lpAdcCorner;            //!< Low Power ADC 3dB corner frequency in Hz
    uint32_t            adcClk_Hz;				//!< ADC clock 2.2G/1.47G/1.1G
    uint32_t            corner3dB;				//!< TIA bandwidth
    uint32_t            corner3dBLp;			//!< TIA bandwidth for Low Power ADC
    uint8_t             tiaPower;				//!< RX TIA Power setting
    uint8_t             tiaPowerLp;				//!< RX TIA Power setting Low Power ADC
    adcType_e           adcType;				//!< ADC type: low/high power ADC
    lpAdcCalMode_e      lpAdcCalMode;           //!< Select periodic or continuous Low Power ADC calibration
    uint8_t             padding[1u];
    rxDpProfile_t       rxDpProfile;			//!< RX digital data path config
    ssiConfig_t         rxSsiConfig;			//!< RX Serial data interface config
    enableDelay_t       txEnableDelay;               //!< TX enable delay timing
    uint32_t            reserved[2u];                // Reserved for future feature
} rxConfig_t;
*/
static void adrv9001_RxProfileConfigWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_RxChannelCfg_t *rxChannelCfg, uint8_t cfgData[], uint32_t *offset)
{
    uint32_t tempOffset = *offset;

    /* All standard data type in RxConfig_t struct are packed in adrv9001_LoadRxByte() and stored in cfgData[]
     * i.e. structures such as rxDpProfile_t, and ssiConfig_t within RxConfig_t are handled in separate functions below */
    adrv9001_LoadRxByte(device, &tempOffset, cfgData, &(rxChannelCfg->profile));

    /* Handles rxDpProfile_t within RxConfig_t in adrv9001_LoadRxDataPathProfileConfig() */
    adrv9001_LoadRxDataPathProfileConfig(device, &tempOffset, cfgData, &(rxChannelCfg->profile.rxDpProfile));

    /* Handles ssiConfig_t within RxConfig_t in adrv9001_LoadSsiConfig() */
    adrv9001_LoadSsiConfig(device, &tempOffset, cfgData, &(rxChannelCfg->profile.rxSsiConfig));

    adrv9001_LoadChannelEnableDelayConfig(device, &tempOffset, cfgData);

    *offset = tempOffset;
}

/*********************************************************************************************
* Refer DeviceSysConfig_t structure below or in device_profile_t.h file in Navassa ARM firmware
* for the order of transferring sysConfig profile config info from API to ARM
**********************************************************************************************
Size : 44 Bytes

typedef struct
{
    duplexMode_e    duplexMode;
    uint8_t         fhModeOn;
    uint8_t         reserved1[1u];       //!< Reserved for future feature
    uint8_t         numDynamicProfile;   //!< Number of dynamic Profile
    uint8_t         extMcsOn;            //!< External MCS On flag. 0 means off
    adcType_e       adcTypeMonitor;      //!< ADC type used in Monitor Mode
    uint16_t        pllLockTime_us;      //!< Required lock time in microseconds for PLLs, based on ref_clk and loop bandwidth
    uint8_t         padding[2u];
    pllModulus_t    pllModuli;           //!< PLL moduli
    uint32_t        reserved[2u];        //<! Reserved for future feature
} deviceSysConfig_t;
*/
static void adrv9001_DeviceSysConfigWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_DeviceSysConfig_t *sysConfig, uint8_t cfgData[], uint32_t *offset)
{
    uint32_t tempOffset = *offset;
    uint32_t i = 0;

    /* Duplex mode */
    /* 0 = ADI_ADRV9001_TDD_MODE ; 1 = ADI_ADRV9001_FDD_MODE */
    cfgData[tempOffset++] = (uint8_t)(sysConfig->duplexMode & 0xFF);

    cfgData[tempOffset++] = sysConfig->fhModeOn;

    /* 1 byte padding */
    tempOffset += 1;

    cfgData[tempOffset++] = sysConfig->numDynamicProfile;

    cfgData[tempOffset++] = sysConfig->mcsMode;

    /* ADC type */
    /* 0 = ADI_ADRV9001_ADC_LP ; 1 = ADI_ADRV9001_ADC_HP */
    cfgData[tempOffset++] = (uint8_t)(sysConfig->adcTypeMonitor & 0xFF);

    adrv9001_LoadTwoBytes(&tempOffset, cfgData, sysConfig->pllLockTime_us);

    /* modulus[] and dmModulus[] are arrays and each array values have to be
       written in contiguous memory location. So need a separate 'for' loop */
    for (i = 0; i < ADI_ADRV9001_MAX_NUM_PLL; i++)
    {
        adrv9001_LoadFourBytes(&tempOffset, cfgData, sysConfig->pllModulus.modulus[i]);
    }

    for (i = 0; i < ADI_ADRV9001_NUM_RF_PLL; i++)
    {
        adrv9001_LoadFourBytes(&tempOffset, cfgData, sysConfig->pllModulus.dmModulus[i]);
    }

    /* 8 bytes padding; Reserved for future use */
    tempOffset += 8;

    *offset = tempOffset;
}

/* Write PFIR coefficients */
static int32_t adrv9001_PfirCoeff_Write(adi_adrv9001_Device_t *device,
                                        const int32_t *coefficients,
                                        uint32_t pfirSize,
                                        uint8_t cfgData[],
                                        uint32_t *profileAddr,
                                        uint32_t offset,
                                        uint32_t *checksum)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t tempOffset = offset;
    uint32_t tempProfileAddr = *profileAddr;
    uint32_t tempChecksum = *checksum;
    uint32_t i = 0;

    for (i = 0; i < pfirSize; i++)
    {
        /* Copy pfirBufferAddr[i] in 'cfgData' */
        adrv9001_LoadFourBytes(&tempOffset, &cfgData[0], coefficients[i]);

        if (tempOffset == ADRV9001_PROFILE_CHUNK_MAX)
        {
            tempChecksum = adrv9001_Crc32ForChunk(&cfgData[0], tempOffset, tempChecksum, 0);
            recoveryAction = adi_adrv9001_arm_Memory_Write(device, tempProfileAddr, &cfgData[0], tempOffset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
            ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
            ADI_ERROR_RETURN(device->common.error.newAction);

            tempProfileAddr += tempOffset;

            /* Clear cfgData[] array */
            adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);
            tempOffset = 0;
        }
    }

    if (tempOffset != 0)
    {
        tempChecksum = adrv9001_Crc32ForChunk(&cfgData[0], tempOffset, tempChecksum, 0);
        recoveryAction = adi_adrv9001_arm_Memory_Write(device, tempProfileAddr, &cfgData[0], tempOffset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
        ADI_ERROR_RETURN(device->common.error.newAction);

        tempProfileAddr += tempOffset;
        tempOffset = 0;
    }

    *profileAddr = tempProfileAddr;
    *checksum = tempChecksum;

    ADI_API_RETURN(device);
}

/*
 * Each WBNB PFIR filter bank contains 128 coefficients of type int32_t
 * The filter bank is written in cfgData[].
typedef struct {
    uint8_t  		numCoeff;     //!< number of coefficients
    pfirSymmetric_e symmetricSel; //!< symmetric selection
    pfirNumTaps_e   tapsSel;      //!< taps selection
    pfirGain_e      gainSel;      //!< gain selection
    int32_t 		coefficients[PFIR_WBNB_SIZE]; //!< coefficients
} pfirWbNbBuffer_t;
 **/
static int32_t adrv9001_PfirWbNbFilterBank_Write(adi_adrv9001_Device_t *device,
                                                 const adi_adrv9001_PfirWbNbBuffer_t *pfirBufferAddr,
                                                 uint32_t pfirSize,
                                                 uint8_t cfgData[],
                                                 uint32_t *profileAddr,
                                                 uint32_t *checksum)
{
    uint32_t offset = 0;

    cfgData[offset++] = pfirBufferAddr->numCoeff;

    ADI_RANGE_CHECK(device,
                    pfirBufferAddr->symmetricSel,
                    ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
                    ADI_ADRV9001_PFIR_COEF_SYMMETRIC);

    ADI_RANGE_CHECK(device,
                    pfirBufferAddr->tapsSel,
                    ADI_ADRV9001_PFIR_32_TAPS,
                    ADI_ADRV9001_PFIR_TAPS_MAX_ID);

    ADI_RANGE_CHECK(device,
                    pfirBufferAddr->gainSel,
                    ADI_ADRV9001_PFIR_GAIN_NEG_12_DB,
                    ADI_ADRV9001_PFIR_GAIN_MAX);

    cfgData[offset++] = (uint8_t)(pfirBufferAddr->symmetricSel & 0xFF);

    cfgData[offset++] = (uint8_t)(pfirBufferAddr->tapsSel & 0xFF);

    cfgData[offset++] = (uint8_t)(pfirBufferAddr->gainSel & 0xFF);

    ADI_EXPECT(adrv9001_PfirCoeff_Write,
               device,
               pfirBufferAddr->coefficients,
               pfirSize,
               &cfgData[0],
               profileAddr,
               offset,
               checksum);

    ADI_API_RETURN(device);
}

/*
 * Each Rx NB Pulse shape PFIR filter bank contains 128 coefficients of type int32_t
 * The filter bank is written in cfgData[].
typedef struct {
    uint8_t  		numCoeff;			//!< number of coefficients
    pfirSymmetric_e symmetricSel;       //!< symmetric selection
    uint8_t         taps;               //!< taps in number
    pfirGain_e      gainSel;            //!< gain selection
    int32_t 		coefficients[PFIR_PULSE_SIZE]; //!< coefficients
} pfirPulseBuffer_t;
 **/
static int32_t adrv9001_PfirRxNbPulShpFilterBank_Write(adi_adrv9001_Device_t *device,
                                                       const adi_adrv9001_PfirPulseBuffer_t *pfirBufferAddr,
                                                       uint32_t pfirSize,
                                                       uint8_t cfgData[],
                                                       uint32_t *profileAddr,
                                                       uint32_t *checksum)
{
    uint32_t offset = 0;

    cfgData[offset++] = pfirBufferAddr->numCoeff;

    ADI_RANGE_CHECK(device,
                    pfirBufferAddr->symmetricSel,
                    ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
                    ADI_ADRV9001_PFIR_COEF_SYMMETRIC);

    ADI_RANGE_CHECK(device,
                    pfirBufferAddr->gainSel,
                    ADI_ADRV9001_PFIR_GAIN_NEG_12_DB,
                    ADI_ADRV9001_PFIR_GAIN_MAX);

    cfgData[offset++] = (uint8_t)(pfirBufferAddr->symmetricSel & 0xFF);

    cfgData[offset++] = pfirBufferAddr->taps;

    cfgData[offset++] = (uint8_t)(pfirBufferAddr->gainSel & 0xFF);

    ADI_EXPECT(adrv9001_PfirCoeff_Write,
               device,
               pfirBufferAddr->coefficients,
               pfirSize,
               &cfgData[0],
               profileAddr,
               offset,
               checksum);

    ADI_API_RETURN(device);
}

static int32_t adrv9001_mag21CompPfir_Write(adi_adrv9001_Device_t *device,
                                            const adi_adrv9001_PfirMag21Buffer_t **pfirMag21BufferStructAddr,
                                            uint32_t totalFilters,
                                            uint32_t pfirSize,
                                            uint8_t cfgData[],
                                            uint32_t *profileAddr,
                                            uint32_t *checksum)
{
    uint32_t offset = 0;
    uint32_t i = 0;
    const adi_adrv9001_PfirMag21Buffer_t *pfirBufferAddr = NULL;

    for (i = 0; i < totalFilters; i++)
    {
        pfirBufferAddr = pfirMag21BufferStructAddr[i];
        cfgData[offset++] = pfirBufferAddr[i].numCoeff;

        /* 3 bytes padding is needed for alignment */
        offset += 3;

        ADI_EXPECT(adrv9001_PfirCoeff_Write,
                   device,
                   pfirBufferAddr->coefficients,
                   pfirSize,
                   &cfgData[0],
                   profileAddr,
                   offset,
                   checksum);
        offset = 0;
    }

    ADI_API_RETURN(device);
}

static int32_t adrv9001_mag13CompPfir_Write(adi_adrv9001_Device_t *device,
                                            const adi_adrv9001_PfirMag13Buffer_t **pfirMag13BufferStructAddr,
                                            uint32_t totalFilters,
                                            uint32_t pfirSize,
                                            uint8_t cfgData[],
                                            uint32_t *profileAddr,
                                            uint32_t *checksum)
{
    uint32_t offset = 0;
    uint32_t i = 0;
    const adi_adrv9001_PfirMag13Buffer_t *pfirBufferAddr = NULL;

    for (i = 0; i < totalFilters; i++)
    {
        pfirBufferAddr = pfirMag13BufferStructAddr[i];
        cfgData[offset++] = pfirBufferAddr[i].numCoeff;

        /* 3 bytes padding is needed for alignment */
        offset += 3;

        ADI_EXPECT(adrv9001_PfirCoeff_Write,
                   device,
                   pfirBufferAddr->coefficients,
                   pfirSize,
                   &cfgData[0],
                   profileAddr,
                   offset,
                   checksum);
        offset = 0;
    }

    ADI_API_RETURN(device);
}

/* Refer PFIR buffer Structure: pfirBuffer_t in ARM firmware REPO for details */
static int32_t adrv9001_PfirFilterCoeffWrite(adi_adrv9001_Device_t *device,
                                             const adi_adrv9001_PfirBuffer_t *pfirBuffer,
                                             uint32_t *profileAddr,
                                             uint32_t *checksum)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t offset = 0;
    uint32_t tempProfileAddr = *profileAddr;
    uint32_t tempChecksum = *checksum;
    uint32_t i = 0;
    uint32_t arraySize = 0;

    uint8_t cfgData[ADRV9001_PROFILE_CHUNK_MAX + 10] = { 0 };

    /* Number of Tx/Rx magnitude compensation PFIR filters */
    static const uint32_t ADRV9001_MAG21_PFIR = 14;

    /* Number of Tx/Rx NB magnitude compensation PFIR filters */
    static const uint32_t ADRV9001_MAG13_PFIR = 4;

    const adi_adrv9001_PfirWbNbBuffer_t *PfirWbNbFilterBankInfo[] =
    {
        &pfirBuffer->pfirRxWbNbChFilterCoeff_A, &pfirBuffer->pfirRxWbNbChFilterCoeff_B,
        &pfirBuffer->pfirRxWbNbChFilterCoeff_C, &pfirBuffer->pfirRxWbNbChFilterCoeff_D,
        &pfirBuffer->pfirTxWbNbPulShpCoeff_A,   &pfirBuffer->pfirTxWbNbPulShpCoeff_B,
        &pfirBuffer->pfirTxWbNbPulShpCoeff_C,   &pfirBuffer->pfirTxWbNbPulShpCoeff_D
    };

    const adi_adrv9001_PfirPulseBuffer_t *PfirRxNbPulShpFilterBankInfo[] =
    {
        &pfirBuffer->pfirRxNbPulShp[0], &pfirBuffer->pfirRxNbPulShp[1]
    };

    const adi_adrv9001_PfirMag21Buffer_t *PfirMag21FilterBankInfo[] =
    {
        &pfirBuffer->pfirRxMagLowTiaLowSRHp[0],   &pfirBuffer->pfirRxMagLowTiaLowSRHp[1],
        &pfirBuffer->pfirRxMagLowTiaHighSRHp[0],  &pfirBuffer->pfirRxMagLowTiaHighSRHp[1],
        &pfirBuffer->pfirRxMagHighTiaHighSRHp[0], &pfirBuffer->pfirRxMagHighTiaHighSRHp[1],
        &pfirBuffer->pfirRxMagLowTiaLowSRLp[0],   &pfirBuffer->pfirRxMagLowTiaLowSRLp[1],
        &pfirBuffer->pfirRxMagLowTiaHighSRLp[0],  &pfirBuffer->pfirRxMagLowTiaHighSRLp[1],
        &pfirBuffer->pfirRxMagHighTiaHighSRLp[0], &pfirBuffer->pfirRxMagHighTiaHighSRLp[1],
        &pfirBuffer->pfirTxMagComp1,              &pfirBuffer->pfirTxMagComp2
    };

    const adi_adrv9001_PfirMag13Buffer_t *PfirMag13FilterBankInfo[] =
    {
        &pfirBuffer->pfirTxMagCompNb[0], &pfirBuffer->pfirTxMagCompNb[1],
        &pfirBuffer->pfirRxMagCompNb[0], &pfirBuffer->pfirRxMagCompNb[1]
    };

    ADI_ENTRY_PTR_EXPECT(device, pfirBuffer);

    arraySize = sizeof(PfirWbNbFilterBankInfo) / sizeof(PfirWbNbFilterBankInfo[0]);
    for (i = 0; i < arraySize; i++)
    {
        /* Writes pfirRxWbNbChFilterCoeff_A[128] to pfirRxWbNbChFilterCoeff_D[128],
         * pfirTxWbNbPulShpCoeff_A[128] to pfirTxWbNbPulShpCoeff_D[128],
         * in cfgData[], which is then written in ARM memory */
        recoveryAction = adrv9001_PfirWbNbFilterBank_Write(device,
                                                           PfirWbNbFilterBankInfo[i],
                                                           ADI_ADRV9001_WB_NB_PFIR_COEFS_MAX_SIZE,
                                                           &cfgData[0],
                                                           &tempProfileAddr,
                                                           &tempChecksum);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, PfirWbNbFilterBankInfo[i], "Error from adrv9001_PfirWbNbFilterBank_Write()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Clear cfgData[] array */
    adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);

    arraySize = sizeof(PfirRxNbPulShpFilterBankInfo) / sizeof(PfirRxNbPulShpFilterBankInfo[0]);
    for (i = 0; i < arraySize; i++)
    {
        /* Writes pfirRxNbPulShp1[128] and pfirRxNbPulShp2[128] in cfgData[], which is then written in ARM memory */
        recoveryAction = adrv9001_PfirRxNbPulShpFilterBank_Write(device,
                                                                 PfirRxNbPulShpFilterBankInfo[i],
                                                                 ADI_ADRV9001_WB_NB_PFIR_COEFS_MAX_SIZE,
                                                                 &cfgData[0],
                                                                 &tempProfileAddr,
                                                                 &tempChecksum);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, PfirRxNbPulShpFilterBankInfo[i], "Error from adrv9001_PfirRxNbPulShpFilterBank_Write()");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Clear cfgData[] array */
    adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);
    offset = 0;

    /* Write Channel 1/2 Low/High TIA Bandwidth HP/LP ADC between Mux200 and 201 filter coefficients and
       TX Magnitude Compensation PFIR 21 taps at Mux410B in ARM memory */
    ADI_EXPECT(adrv9001_mag21CompPfir_Write,
               device,
               PfirMag21FilterBankInfo,
               ADRV9001_MAG21_PFIR,
               ADI_ADRV9001_MAG_COMP_PFIR_COEFS_MAX_SIZE,
               &cfgData[0],
               &tempProfileAddr,
               &tempChecksum);

    /* Write TX/RX Magnitude Compensation PFIR for NB in ARM memory */
    ADI_EXPECT(adrv9001_mag13CompPfir_Write,
               device,
               PfirMag13FilterBankInfo,
               ADRV9001_MAG13_PFIR,
               ADI_ADRV9001_MAG_COMP_NB_PFIR_COEFS_MAX_SIZE,
               &cfgData[0],
               &tempProfileAddr,
               &tempChecksum);

    adrv9001_LoadFourBytes(&offset, &cfgData[0], tempChecksum); /* Copy final Checksum in 'cfgData'*/
    recoveryAction = adi_adrv9001_arm_Memory_Write(device, tempProfileAddr, &cfgData[0], offset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    tempProfileAddr += offset; /* Add the size of checksum */
    *profileAddr = tempProfileAddr;
    *checksum = tempChecksum;

    return recoveryAction;
}

/****************************************************** Private APIs *******************************************************/
int32_t adrv9001_DmaMemWrite(adi_adrv9001_Device_t *device, uint32_t address, const uint8_t data[], uint32_t byteCount, adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{
    uint32_t i = 0;
    uint8_t regWrite = 0;
    uint8_t autoInc = ADI_ADRV9001_ARM_MEM_AUTO_INCR;
    uint32_t addrIndex = 0;
    uint32_t dataIndex = 0;
    uint32_t spiBufferSize = ((HAL_SPIWRITEARRAY_BUFFERSIZE / 3) - 1);
    uint16_t addrArray[(HAL_SPIWRITEARRAY_BUFFERSIZE / 3)] = { 0 };
    uint8_t  dataArray[(HAL_SPIWRITEARRAY_BUFFERSIZE / 3)] = { 0 };
    uint32_t ADDR_ARM_DMA_DATA[4] = { ADRV9001_ADDR_ARM_DMA_DATA3, ADRV9001_ADDR_ARM_DMA_DATA2, ADRV9001_ADDR_ARM_DMA_DATA1, ADRV9001_ADDR_ARM_DMA_DATA0 };
    uint32_t index = 0;
    uint32_t armMemAddress = address;
    uint8_t regVal = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t singleInstruction = 0;
    uint8_t spiMode = 0;
    uint8_t spiConfig_A = 0;
    
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, data, byteCount);

    ADRV9001_DMAINFO("ARM_MEM_WRITE", armMemAddress, byteCount);

    spiMode = device->spiSettings.enSpiStreaming;
    ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Get, device, &singleInstruction);
    ADRV9001_SPIREADBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, &spiConfig_A);

    ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, &regVal);

    if (ADRV9001_BF_EQUAL(regVal, ADRV9001_DMA_CTL_BUS_WAITING))
    {
        ADRV9001_DEBUG_INFO_NUM("ARM_MEM_WRITE BUS ERROR ADRV9001_ADDR_ARM_DMA_CTL", regVal);
    }

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (((armMemAddress & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        if ((ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252 == spiWriteMode) || 
            (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode))
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARMCMD,
                spiWriteMode,
                recoveryAction,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                "SPI_WRITE_MODE_STANDARD_BYTES_252 and ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 are not supported if byteCount is not a multiple of 4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        return adrv9001_DmaMemWriteByte(device, armMemAddress, data, byteCount);
    }

    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regWrite &= ~ADRV9001_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (armMemAddress >= ADRV9001_ADDR_ARM_START_DATA)
    {
        regWrite |= ADRV9001_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regWrite |= ADRV9001_BF_ENCODE(2, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != 0)
    {
        regWrite |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, regWrite);
    
    /* Enable single instruction and disable SPI streaming mode by default.
     * If ADRV9001 SPI streming mode is selected, then single instruction and single instruction are disbled */
    ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x1);
    device->spiSettings.enSpiStreaming = 0;

    if (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode)
    {
        ADRV9001_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, 0x18);
        ADI_EXPECT(adrv9001_NvsRegmapCore_AddrAscension2_Set, device, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x0);
        device->spiSettings.enSpiStreaming = 1;
    }

    /* AHB address for write */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    if (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252 == spiWriteMode)
    {
        /* Cache Enable and Auto Inc */
        for (i = 0; i < byteCount; i++)
        {
            /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
            /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
            index = armMemAddress % 4;

            /* Writing byte order: 3,2,1,0 */
            if (index == 0)
            {
                dataIndex = i + 3;
            }
            else
            {
                dataIndex--;
            }

            dataArray[addrIndex] = data[dataIndex];
            addrArray[addrIndex] = ADDR_ARM_DMA_DATA[index];

            addrIndex++;
            armMemAddress++;

            if (addrIndex >= spiBufferSize)
            {
                ADRV9001_SPIWRITEBYTESDMA(device,
                    "DMA_MEM_WRITE_CACHE",
                    armMemAddress - addrIndex,
                    &addrArray[0],
                    &dataArray[0],
                    addrIndex);
                addrIndex = 0;
            }
        }

        if (addrIndex > 0)
        {
            ADRV9001_SPIWRITEBYTESDMA(device,
                "DMA_MEM_WRITE_CACHE_LAST",
                armMemAddress - addrIndex,
                &addrArray[0],
                &dataArray[0],
                addrIndex);
        }
    }
    else
    {
        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i += 4)
        {
            if (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode)
            {
                ADRV9001_SPIWRITEBYTESTREAMDMA(device, "ARM_DMA_DATA_3", ADRV9001_ADDR_ARM_DMA_DATA3, (data + i), 4);
            }
            else
            {
                /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
                /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_3", ADRV9001_ADDR_ARM_DMA_DATA3, data[i + 3]);
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_2", ADRV9001_ADDR_ARM_DMA_DATA2, data[i + 2]);
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_1", ADRV9001_ADDR_ARM_DMA_DATA1, data[i + 1]);
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_0", ADRV9001_ADDR_ARM_DMA_DATA0, data[i]);
            }

            if (autoInc == 0)
            {
                /* AHB address for write */
                /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
                armMemAddress += 4;
                /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
                ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((armMemAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
            }
        }
    }

    /* setting up the DMA control register for a read to avoid accidental writing */
    regWrite |= ADRV9001_DMA_CTL_RD_WRB;
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, regWrite);

    ADRV9001_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, spiConfig_A);
    ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, singleInstruction);
    device->spiSettings.enSpiStreaming = spiMode;

    ADI_API_RETURN(device);
}

int32_t adrv9001_DmaMemRead(adi_adrv9001_Device_t *device, uint32_t address, uint8_t returnData[], uint32_t byteCount, uint8_t autoIncrement)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t regRead = 0;
    uint8_t dataRead = 0;
    uint8_t regVal = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    ADRV9001_DMAINFO("ARM_MEM_READ", address, byteCount);

    ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, &regVal);

    if (ADRV9001_BF_EQUAL(regVal, ADRV9001_DMA_CTL_BUS_WAITING))
    {
        ADRV9001_DEBUG_INFO_NUM("ARM_MEM_READ BUS ERROR ADRV9001_ADDR_ARM_DMA_CTL", regVal);
    }

    ADRV9001_DMA_SKIP();

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (((address & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        return adrv9001_DmaMemReadByte(device, address, returnData, byteCount, autoIncrement);
    }

    /*
    Use SPI DMA for Read
    */
    /* Read */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regRead |= ADRV9001_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (!((address >= ADRV9001_ADDR_ARM_START_PROG) && (address <= ADRV9001_ADDR_ARM_END_PROG)))
    {
        regRead |= ADRV9001_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regRead |= ADRV9001_BF_ENCODE(2, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoIncrement != 0)
    {
        regRead |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV9001_ADDR_ARM_DMA_CTL, regRead);

    /* AHB address for read */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    /* starting write at zero address offset */
    for (i = 0; i < byteCount; i += 4)
    {
        /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
        /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */

        ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_3", ADRV9001_ADDR_ARM_DMA_DATA3, &dataRead);
        returnData[i + 3] = dataRead;
        ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_2", ADRV9001_ADDR_ARM_DMA_DATA2, &dataRead);
        returnData[i + 2] = dataRead;
        ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_1", ADRV9001_ADDR_ARM_DMA_DATA1, &dataRead);
        returnData[i + 1] = dataRead;
        
        /* 'single_instruction' has to be cleared before reading DMA_DATA3 and set back after */
        ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x0);
        ADRV9001_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0", ADRV9001_ADDR_ARM_DMA_DATA0, &dataRead);
        returnData[i] = dataRead;
        ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x1);

        if (autoIncrement == 0)
        {
            /* AHB address for read */
            /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
            address += 4;
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", ADRV9001_ADDR_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", ADRV9001_ADDR_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", ADRV9001_ADDR_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", ADRV9001_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    ADI_API_RETURN(device);
}

int32_t adrv9001_FlexStreamProcessorMemWrite(adi_adrv9001_Device_t *device,
                                             uint32_t address,
                                             const uint8_t data[],
                                             uint32_t byteCount,
                                             uint8_t flexSpNumber,
                                             adi_adrv9001_ArmSingleSpiWriteMode_e spiWriteMode)
{
    uint32_t i = 0;
    uint8_t regWrite = 0;
    uint8_t autoInc = ADI_ADRV9001_ARM_MEM_AUTO_INCR;
    uint32_t addrIndex = 0;
    uint32_t dataIndex = 0;
    uint32_t spiBufferSize = ((HAL_SPIWRITEARRAY_BUFFERSIZE / 3) - 1);
    uint16_t addrArray[(HAL_SPIWRITEARRAY_BUFFERSIZE / 3)] = { 0 };
    uint8_t  dataArray[(HAL_SPIWRITEARRAY_BUFFERSIZE / 3)] = { 0 };
    uint32_t ADDR_FLEX_SP_ARM_DMA_DATA[4] = { ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA3, ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA2, ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA1, ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0 };
    uint32_t index = 0;
    uint32_t flexSpAddress = address;
    uint8_t regVal = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t singleInstruction = 0;
    uint8_t spiMode = 0;
    uint8_t spiConfig_A = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, data, byteCount);

    ADRV9001_DMAINFO("FLEX_SP_ARM_MEM_WRITE", flexSpAddress, byteCount);

    spiMode = device->spiSettings.enSpiStreaming;
    ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Get, device, &singleInstruction);
    ADRV9001_SPIREADBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, &spiConfig_A);

    ADRV9001_SPIREADBYTEDMA(device, "FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, &regVal);

    if (ADRV9001_BF_EQUAL(regVal, ADRV9001_DMA_CTL_BUS_WAITING))
    {
        ADRV9001_DEBUG_INFO_NUM("FLEX_SP_ARM_MEM_WRITE BUS ERROR ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", regVal);
    }

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (((flexSpAddress & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        if ((ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252 == spiWriteMode) || 
            (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode))
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_ARMCMD,
                spiWriteMode,
                recoveryAction,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                "SPI_WRITE_MODE_STANDARD_BYTES_252 and ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 are not supported if byteCount is not a multiple of 4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        return adrv9001_FlexStreamProcessorMemWriteByte(device, flexSpAddress, data, byteCount, flexSpNumber);
    }

    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regWrite &= ~ADRV9001_DMA_CTL_RD_WRB;

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regWrite |= ADRV9001_BF_ENCODE(2, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != 0)
    {
        regWrite |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the flex SP DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, regWrite);
    
    /* Enable single instruction and disable SPI streaming mode by default.
     * If ADRV9001 SPI streming mode is selected, then single instruction and single instruction are disbled */
    ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x1);
    device->spiSettings.enSpiStreaming = 0;

    if (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode)
    {
        ADRV9001_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, 0x18);
        ADI_EXPECT(adrv9001_NvsRegmapCore_AddrAscension2_Set, device, 0x0);
        ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, 0x0);
        device->spiSettings.enSpiStreaming = 1;
    }

    /* Access the flex SP needed; 0x1 - flex SP0, 0x2 - flex SP1, 0x4 - flex SP2, 0x8 - flex SP3 */
    ADRV9001_SPIWRITEBYTEDMA(device, "FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_SELECT, (0x1 << (flexSpNumber - 1)));

    /* flex SP AHB address for write */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

#if ADI_ADRV9001_SW_TEST > 0
    /* No Cache Enable for SW test */
    if (device->devStateInfo.swTest > 1)
    {
        cacheEnable = 0;
    }
#endif

    if (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252 == spiWriteMode)
    {
        /* Cache Enable and Auto Inc */
         for(i = 0 ; i < byteCount ; i++)
        {
            /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
            /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
            index = flexSpAddress % 4;

            /* Writing byte order: 3,2,1,0 */
            if (index == 0)
            {
                dataIndex = i + 3;
            }
            else
            {
                dataIndex--;
            }

            dataArray[addrIndex] = data[dataIndex];
            addrArray[addrIndex] = ADDR_FLEX_SP_ARM_DMA_DATA[index];

            addrIndex++;
            flexSpAddress++;

            if (addrIndex >= spiBufferSize)
            {
                ADRV9001_SPIWRITEBYTESDMA(device,
                                          "DMA_MEM_WRITE_CACHE",
                                          flexSpAddress - addrIndex,
                                          &addrArray[0],
                                          &dataArray[0],
                                          addrIndex);
                addrIndex = 0;
            }
        }

        if (addrIndex > 0)
        {
            ADRV9001_SPIWRITEBYTESDMA(device,
                                      "DMA_MEM_WRITE_CACHE_LAST",
                                      flexSpAddress - addrIndex,
                                      &addrArray[0],
                                      &dataArray[0],
                                      addrIndex);
        }
    }
    else
    {
        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i += 4)
        {
            if (ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode)
            {
                ADRV9001_SPIWRITEBYTESTREAMDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA3, (data + i), 4);
            }
            else
            {
                /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
                /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA3, data[i + 3]);
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA2, data[i + 2]);
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA1, data[i + 1]);
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0, data[i]);
            }

            if (autoInc == 0)
            {
                /* flex SP AHB address for write */
                /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
                flexSpAddress += 4;
                /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
                ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((flexSpAddress) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
            }
        }
    }

    /* setting up the DMA control register for a read to avoid accidental writing */
    regWrite |= ADRV9001_DMA_CTL_RD_WRB;
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, regWrite);

    ADRV9001_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, spiConfig_A);
    ADI_EXPECT(adrv9001_NvsRegmapCore_SingleInstruction_Set, device, singleInstruction);
    device->spiSettings.enSpiStreaming = spiMode;

    ADI_API_RETURN(device);
}

int32_t adrv9001_FlexStreamProcessorMemRead(adi_adrv9001_Device_t *device,
                                            uint32_t address,
                                            uint8_t returnData[],
                                            uint32_t byteCount,
                                            uint8_t autoIncrement,
                                            uint8_t flexSpNumber)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t regRead = 0;
    uint8_t dataRead = 0;
    uint8_t regVal = 0;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    ADRV9001_DMAINFO("ADRV9001_ADDR_FLEX_SP_ARM_MEM_READ", address, byteCount);

    ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, &regVal);

    if (ADRV9001_BF_EQUAL(regVal, ADRV9001_DMA_CTL_BUS_WAITING))
    {
        ADRV9001_DEBUG_INFO_NUM("ARM_MEM_READ BUS ERROR ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", regVal);
    }

    ADRV9001_DMA_SKIP();

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (((address & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        return adrv9001_FlexStreamProcessorMemReadByte(device, address, returnData, byteCount, autoIncrement, flexSpNumber);
    }

    /*
    Use SPI DMA for Read
    */
    /* Read */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regRead |= ADRV9001_DMA_CTL_RD_WRB;

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regRead |= ADRV9001_BF_ENCODE(2, ADRV9001_DMA_CTL_BUS_SIZE_MASK, ADRV9001_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoIncrement != 0)
    {
        regRead |= ADRV9001_DMA_CTL_AUTO_INCR;
    }

    /* setting up the flex SP DMA control register for a write */
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL", ADRV9001_ADDR_FLEX_SP_ARM_DMA_CTL, regRead);

    /* flex SP AHB address for read */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
    ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    /* starting write at zero address offset */
    for (i = 0; i < byteCount; i += 4)
    {
        /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
        /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
        ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA3, &dataRead);
        returnData[i + 3] = dataRead;
        ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA2, &dataRead);
        returnData[i + 2] = dataRead;
        ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA1, &dataRead);
        returnData[i + 1] = dataRead;
        ADRV9001_SPIREADBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_DATA0, &dataRead);
        returnData[i] = dataRead;

        if (autoIncrement == 0)
        {
            /* flex SP AHB address for read */
            /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
            address += 4;
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_3", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR3, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_2", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR2, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_1", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR1, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
            ADRV9001_SPIWRITEBYTEDMA(device, "ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR_0", ADRV9001_ADDR_FLEX_SP_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV9001_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    ADI_API_RETURN(device);
}

int32_t adrv9001_ArmMailBoxErrCodeGet(adi_adrv9001_Device_t *device, uint16_t *mailboxErrCode)
{
    /* reading the command status register for given opcode */
    uint8_t errCode = 0;
    uint8_t objId = 0;

    ADRV9001_SPIREADBYTE(device, "ARM_CMD_STATUS_10", (uint16_t)ADRV9001_ADDR_ARM_CMD_STATUS_10, &errCode);
    ADRV9001_SPIREADBYTE(device, "ARM_CMD_STATUS_11", (uint16_t)ADRV9001_ADDR_ARM_CMD_STATUS_10 + 1, &objId);

    *mailboxErrCode = ((uint16_t)objId << 8) | errCode;
    ADI_API_RETURN(device);
}

#ifdef ADI_ADRV9001_ARM_VERBOSE
#include "adrv9001_arm_error_mapping.h"
#endif

static const char* adrv9001_CmdErrMsgGet(uint32_t errCode)
{
    if (0x2000 <= errCode && errCode <= 0x3FFF)
    {
        return adrv9001_error_table_CmdError[0];
    }
    else if (0x4000 <= errCode && errCode <= 0x5FFF)
    {
        return adrv9001_error_table_CmdError[1];
    }
    else if (0x6000 <= errCode && errCode <= 0x7FFF)
    {
        return adrv9001_error_table_CmdError[2];
    }
    else if (0x8000 <= errCode && errCode <= 0x9FFF)
    {
        return adrv9001_error_table_CmdError[3];
    }
    else if (0xA000 <= errCode && errCode <= 0xBFFF)
    {
        return adrv9001_error_table_CmdError[4];
    }
    else if (0xC000 <= errCode && errCode <= 0xDFFF)
    {
        return adrv9001_error_table_CmdError[5];
    }
    else if (0xE000 <= errCode && errCode <= 0xFFFF)
    {
        return adrv9001_error_table_CmdError[6];
    }
    
    return NULL;
}

const char* adrv9001_ArmMailBoxErrMsgGet(uint32_t errCode)
{
#ifndef ADI_ADRV9001_ARM_VERBOSE
    return "";
#else
    if (errCode > ADI_ARRAY_LEN(adrv9001_error_table_CmdCtrlMboxCmdError))
    {
        return NULL;
    }
    else
    {
        return adrv9001_error_table_CmdCtrlMboxCmdError[errCode];
    }
#endif
}

int32_t adrv9001_ArmCmdErrorHandler(adi_adrv9001_Device_t *device, uint32_t detErr)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t armErrorFlag = 0;
    uint16_t mailboxErrCode = 0;
    const char *errorString = NULL;
    static const uint8_t ERR_FLAG_MASK = 0x0E;
    static const uint8_t ERR_FLAG_SHIFT = 1;

    ADI_ENTRY_EXPECT(device);

    armErrorFlag = ADRV9001_BF_DECODE((uint8_t)detErr, ERR_FLAG_MASK, ERR_FLAG_SHIFT);

    if (armErrorFlag > 0)
    {
        errorString = adrv9001_ArmMailBoxErrMsgGet(armErrorFlag);

        /* Report Last Command Error information */
        /* TODO: Might be a good idea to report more specific error based on object id and error code */
        if (armErrorFlag == 7)
        {
            /* Clear the error before 'adrv9001_ArmMailBoxErrCodeGet' function is executed;
               Otherwsie, SPI read of ARM command status register will fail and the error code from ARM is not obtained */
            recoveryAction = adi_common_ErrorClear(&device->common);
            if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_ADRV9001_SRC_ARMCMD,
                    detErr,
                    recoveryAction,
                    detErr,
                    errorString);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_EXPECT(adrv9001_ArmMailBoxErrCodeGet, device, &mailboxErrCode);
            errorString = adrv9001_CmdErrMsgGet(mailboxErrCode);
            
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV9001_SRC_ARMCMD,
                             mailboxErrCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             mailboxErrCode,
                             errorString);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

static uint32_t adrv9001_ArmProfileWrite_Validate(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init)
{
    uint32_t i = 0;
    uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    uint32_t RX_CHANNELS[] = {
                               ADI_ADRV9001_RX1,
                               ADI_ADRV9001_RX2,
                               ADI_ADRV9001_ORX1,
                               ADI_ADRV9001_ORX2,
                               ADI_ADRV9001_ILB1,
                               ADI_ADRV9001_ILB2,
                               ADI_ADRV9001_ELB1,
                               ADI_ADRV9001_ELB2
                             };

    static const uint32_t ADRV9001_ADDR_DEVICE_PROFILE = 0x20000000;

    ADI_ENTRY_PTR_EXPECT(device, init);

    if (device->devStateInfo.profileAddr < ADRV9001_ADDR_DEVICE_PROFILE)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device,
            "Invalid Profile Address");

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Range check parameters in adi_adrv9001_ClockSettings_t */
    ADI_RANGE_CHECK(device, init->clocks.clkPllMode, ADI_ADRV9001_CLK_PLL_HP_MODE, ADI_ADRV9001_CLK_PLL_LP_MODE);
    ADI_RANGE_CHECK(device, init->clocks.armPowerSavingClkDiv, 1, 256);
    ADI_RANGE_CHECK(device, init->clocks.rfPllPhaseSyncMode, ADI_ADRV9001_RFPLLMCS_NOSYNC, ADI_ADRV9001_RFPLLMCS_INIT_AND_CONTTRACK);
    ADI_RANGE_CHECK(device, init->clocks.rx1LoSelect, ADI_ADRV9001_LOSEL_LO1, ADI_ADRV9001_LOSEL_LO2);
    ADI_RANGE_CHECK(device, init->clocks.rx2LoSelect, ADI_ADRV9001_LOSEL_LO1, ADI_ADRV9001_LOSEL_LO2);
    ADI_RANGE_CHECK(device, init->clocks.tx1LoSelect, ADI_ADRV9001_LOSEL_LO1, ADI_ADRV9001_LOSEL_LO2);
    ADI_RANGE_CHECK(device, init->clocks.tx2LoSelect, ADI_ADRV9001_LOSEL_LO1, ADI_ADRV9001_LOSEL_LO2);
    ADI_RANGE_CHECK(device, init->clocks.rx1LoDivMode, ADI_ADRV9001_LO_DIV_MODE_HIGH_PERFORMANCE, ADI_ADRV9001_LO_DIV_MODE_LOW_POWER);
    ADI_RANGE_CHECK(device, init->clocks.rx2LoDivMode, ADI_ADRV9001_LO_DIV_MODE_HIGH_PERFORMANCE, ADI_ADRV9001_LO_DIV_MODE_LOW_POWER);
    ADI_RANGE_CHECK(device, init->clocks.tx1LoDivMode, ADI_ADRV9001_LO_DIV_MODE_HIGH_PERFORMANCE, ADI_ADRV9001_LO_DIV_MODE_LOW_POWER);
    ADI_RANGE_CHECK(device, init->clocks.tx2LoDivMode, ADI_ADRV9001_LO_DIV_MODE_HIGH_PERFORMANCE, ADI_ADRV9001_LO_DIV_MODE_LOW_POWER);
    ADI_RANGE_CHECK(device, init->clocks.loGen1Select, ADI_ADRV9001_LOGENPOWER_RFPLL_LDO, ADI_ADRV9001_LOGENPOWER_OFFCHIP);
    ADI_RANGE_CHECK(device, init->clocks.loGen2Select, ADI_ADRV9001_LOGENPOWER_RFPLL_LDO, ADI_ADRV9001_LOGENPOWER_OFFCHIP);
    ADI_RANGE_CHECK(device, init->clocks.rfPll1LoMode, ADI_ADRV9001_INT_LO1, ADI_ADRV9001_EXT_LO_INPUT2);
    ADI_RANGE_CHECK(device, init->clocks.rfPll2LoMode, ADI_ADRV9001_INT_LO2, ADI_ADRV9001_EXT_LO_INPUT2);
    ADI_RANGE_CHECK(device, init->clocks.ext1LoType, ADI_ADRV9001_EXT_LO_DIFFERENTIAL, ADI_ADRV9001_EXT_LO_SINGLE_ENDED);
    ADI_RANGE_CHECK(device, init->clocks.ext2LoType, ADI_ADRV9001_EXT_LO_DIFFERENTIAL, ADI_ADRV9001_EXT_LO_SINGLE_ENDED);
    ADI_RANGE_CHECK(device, init->clocks.rx1RfInputSel, ADI_ADRV9001_RX_A, ADI_ADRV9001_RX_B);
    ADI_RANGE_CHECK(device, init->clocks.rx2RfInputSel, ADI_ADRV9001_RX_A, ADI_ADRV9001_RX_B);
    ADI_RANGE_CHECK(device, init->clocks.auxPllPower, ADI_ADRV9001_COMPONENT_POWER_LEVEL_LOW, ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH);
    ADI_RANGE_CHECK(device, init->clocks.clkPllPower, ADI_ADRV9001_COMPONENT_POWER_LEVEL_LOW, ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH);

    /* PLL LO dividers must be 1 or even numbers between 2 and 1022 */
    if (1 != init->clocks.extLo1Divider)
    {
        if ((1022  < init->clocks.extLo1Divider) ||
               (2  > init->clocks.extLo1Divider) ||
               (0 != init->clocks.extLo1Divider % 2))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             init->clocks.rfPll1LoDivider,
                             "Invalid clock rfPll1LoDivider. Valid values are 1 and even numbers between 2 and 1022, inclusive.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* PLL LO dividers must be 1 or even numbers between 2 and 1022 */
    if (1 != init->clocks.extLo2Divider)
    {
        if ((1022  < init->clocks.extLo2Divider) ||
               (2  > init->clocks.extLo2Divider) ||
               (0 != init->clocks.extLo2Divider % 2))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             init->clocks.rfPll2LoDivider,
                             "Invalid clock rfPll2LoDivider. Valid values are 1 and even numbers between 2 and 1022, inclusive.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }
    
    /* Range check parameters in adi_adrv9001_TxSettings_t */
    for (i = 0; i < ADI_ADRV9001_MAX_TXCHANNELS; i++)
    {
        if ( ADRV9001_BF_EQUAL( init->tx.txInitChannelMask, TX_CHANNELS[i] ) )
        {
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].outputSignaling, ADI_ADRV9001_TX_IQ, ADI_ADRV9001_TX_DIRECT_FM_FSK);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txBbfPower, ADI_ADRV9001_COMPONENT_POWER_LEVEL_LOW, ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txDpProfile.txPreProc.txPreProcMode, ADI_ADRV9001_TX_DP_PREPROC_I_ONLY_DATA_BYPASS_PFIRS, (ADI_ADRV9001_TX_DP_PREPROC_I_ONLY_DATA_CASCADE_PFIRS | ADI_ADRV9001_TX_DP_PREPROC_ENABLE_PFIRS));
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txDpProfile.txPreProc.txPreProcWbNbPfirIBankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txDpProfile.txPreProc.txPreProcWbNbPfirQBankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txDpProfile.txIqdmDuc.iqdmDucMode, ADI_ADRV9001_TX_DP_IQDMDUC_MODE0, ADI_ADRV9001_TX_DP_IQDMDUC_MODE2);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txSsiConfig.ssiType, ADI_ADRV9001_SSI_TYPE_DISABLE, ADI_ADRV9001_SSI_TYPE_LVDS);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txSsiConfig.ssiDataFormatSel, ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txSsiConfig.numLaneSel, ADI_ADRV9001_SSI_1_LANE, ADI_ADRV9001_SSI_4_LANE);
            ADI_RANGE_CHECK(device, init->tx.txProfile[i].txSsiConfig.strobeType, ADI_ADRV9001_SSI_SHORT_STROBE, ADI_ADRV9001_SSI_LONG_STROBE);
        }
    }

    /* Range check parameters in adi_adrv9001_RxSettings_t */
    for (i = 0; i < ADI_ADRV9001_MAX_RXCHANNELS; i++)
    {
        if ( ADRV9001_BF_EQUAL( init->rx.rxInitChannelMask, RX_CHANNELS[i] ) )
        {
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.outputSignaling, ADI_ADRV9001_RX_IQ, ADI_ADRV9001_RX_FM_SYMBOLS);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.adcType, ADI_ADRV9001_ADC_LP, ADI_ADRV9001_ADC_HP);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.lpAdcCalMode, ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, ADI_ADRV9001_ADC_LOWPOWER_CONTINUOUS);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.tiaPower, ADI_ADRV9001_COMPONENT_POWER_LEVEL_LOW, ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.tiaPowerLp, ADI_ADRV9001_COMPONENT_POWER_LEVEL_LOW, ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxSincHBTop.sincGainMux, ADI_ADRV9001_RX_SINC_GAIN_MUX_0_DB, ADI_ADRV9001_RX_SINC_GAIN_MUX_NEG_12_DB);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxSincHBTop.sincMux, ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO, ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxSincHBTop.hbMux, ADI_ADRV9001_RX_HB_MUX_OUTPUT_ZERO, ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.gsOutMuxSel, ADI_ADRV9001_GS_OUT_MUX_GS_OUT, ADI_ADRV9001_GS_OUT_MUX_BYPASS);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.rxOutSel, ADI_ADRV9001_RX_OUT_IQ_SEL, ADI_ADRV9001_RX_OUT_SEL_PFIR);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.rxRoundMode, ADI_ADRV9001_RX_ROUNDMODE_IQ, ADI_ADRV9001_RX_ROUNDMODE_I);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.dpArmSel, ADI_ADRV9001_DP_SEL, ADI_ADRV9001_ARM_SEL);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.dpInFifo.dpInFifoTestDataSel, ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL, ADI_ADRV9001_DP_IN_FIFO_INPUT_SPI_SEL);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.dpInFifo.dpInFifoMode, ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTED);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.rxWbNbCompPFir.rxWbNbCompPFirInMuxSel, ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, ADI_ADRV9001_RP_FIR_IN_MUX_EXT_IN);

            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxDpProfile.rxNbDem.rxWbNbCompPFir.bankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);

            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxSsiConfig.ssiType, ADI_ADRV9001_SSI_TYPE_DISABLE, ADI_ADRV9001_SSI_TYPE_LVDS);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxSsiConfig.ssiDataFormatSel, ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxSsiConfig.numLaneSel, ADI_ADRV9001_SSI_1_LANE, ADI_ADRV9001_SSI_4_LANE);
            ADI_RANGE_CHECK(device, init->rx.rxChannelCfg[i].profile.rxSsiConfig.strobeType, ADI_ADRV9001_SSI_SHORT_STROBE, ADI_ADRV9001_SSI_LONG_STROBE);

        }
    }

    /* Range check parameters in adi_adrv9001_DeviceSysConfig_t */
    ADI_RANGE_CHECK(device, init->sysConfig.duplexMode, ADI_ADRV9001_TDD_MODE, ADI_ADRV9001_FDD_MODE);
    ADI_RANGE_CHECK(device, init->sysConfig.adcTypeMonitor, ADI_ADRV9001_ADC_LP, ADI_ADRV9001_ADC_HP);

    ADI_API_RETURN(device);
}

/*********************************************************************************************
* Refer DeviceProfile_t structure below or in device_profile_t.h file in Navassa ARM firmware
* for the order of transferring the device profile info from API to ARM
**********************************************************************************************
* Profile information provided by BBIC at init via CONFIG command
*
* Profile struct holds clock rates, divider ratios, sampling rates, etc.
* Populated at boot via CONFIG command.
*
* If this structure is initialized to all 0's the linker puts this in the
* stack section therefore initializing devclk_hz to 1
**********************************************************************************************
typedef struct
{

    uint32_t         vcoFreq_kHz;		//!< BBPLL VCO frequency of the system as set by API, in Khz
    uint32_t         hsDigFreq_Hz;		//!< HS Dig as calc by API
    uint32_t         devClock_Hz;		//!< reference clock
    uint8_t          armPowerSavingClkDiv;   //!< B0: ARM power saving clock divider 1/N. (0: Div1 (184MHz); 1: Div2 ... 255: Div256)
    uint8_t          refClockOutEn;          //!< B0: reference clock output disable/enable
    uint8_t          padding[2u];
    uint8_t          clkGenEn;			//!< LP clock or clkPll for HsDigClk
    uint8_t          loPhaseSync;		//!< LO Phase Sync mode
    uint8_t          loSelMask;			//!< RF LO routing select
                                             D0 -  Rx1 Sel      0 = LO1, 1   = LO2
                                             D1 -  Rx2 Sel      0 = LO1, 1   = LO2
                                             D2 -  Tx1 Sel      0 = LO1, 1   = LO2
                                             D3 -  Tx2 Sel      0 = LO1, 1   = LO2
                                             LO1 = PLL1/LOGEN1, LO2 = PLL2/LOGEN2

    uint8_t          loDivMode;         //!< LO divider mode sel (0 = BEST_PHASE_NOISE, 1 = BEST_POWER_SAVING)
                                             D0 -  Rx1 LO divider mode sel
                                             D1 -  Rx2 LO divider mode sel
                                             D2 -  Tx1 LO divider mode sel
                                             D3 -  Tx2 LO divider mode sel

    uint16_t         extLo1Div; 		// 1/2 up to 1/1022. External in divider in ExtIn1 or ExtIn2
    uint16_t         extLo2Div; 		// 1/2 up to 1/1022. External in divider in ExtIn1 or ExtIn2
    uint32_t         extLo1OutFreq_kHz; //!< External LO 1 output freq in kHz
    uint32_t         extLo2OutFreq_kHz; //!< External LO 2 output freq in kHz
    uint8_t          LoGenPower;        //!< D0 - LoGen1   Sel 0 = RFPLL1_LDO, 1 = OffChip
                                        //!< D1 - LoGen2   Sel 0 = RFPLL2_LDO, 1 = OffChip

    uint8_t          extLoSelMask;           D0-1: LO1 Sel 0 = Inter LO1, 1 = Ext OUT, 2 = ExtIn1, 3=ExtIn2
                                             D2-3: LO2 Sel 0 = Inter LO2, 1 = Ext OUT, 2 = ExtIn1, 3=ExtIn2
                                             D4 - Ext1 0 = Differential, 1 = Single Ended
                                             D5 - Ext2 0 = Differential, 1 = Single Ended

    uint16_t         chanConfig;		//!< Channel configuration
                                             D0 -  Rx1;    0 = Disabled, 1 = Enabled
                                             D1 -  Rx2;    0 = Disabled, 1 = Enabled
                                             D2 -  Tx1;    0 = Disabled, 1 = Enabled
                                             D3 -  Tx2;    0 = Disabled, 1 = Enabled
                                             D4 -  ORx1;   0 = Disabled, 1 = Enabled
                                             D5 -  ORx2;   0 = Disabled, 1 = Enabled
                                             D6 -  ILB1;   0 = Disabled, 1 = Enabled
                                             D7 -  ILB2;   0 = Disabled, 1 = Enabled
                                             D8 -  ELB1;   0 = Disabled, 1 = Enabled
                                             D9 -  ELB2;   0 = Disabled, 1 = Enabled
                                             D10 -  Rx1Pin; 0 = Rx1A,     1 = Rx1B
                                             D11 -  Rx2Pin; 0 = Rx2A,     1 = Rx2B

    TxConfig_t       TxConfig[NUM_TX_CHANNELS]; 	//!< TX configuration
    RxConfig_t       RxConfig[NUM_RX_CHANNELS];		//!< RX configuration
    RxConfig_t       OrxConfig[NUM_RX_CHANNELS];	//!< ORX configuration
    RxConfig_t       ELbConfig[NUM_RX_CHANNELS];	//!< External Loopback configuration
    RxConfig_t       ILbConfig[NUM_RX_CHANNELS];	//!< Internal Loopback configuration
    deviceSysConfig_t sysConfig;
    NoiseCanConfig_t NoiseCancelConfig;				//!< Place holder for noise canceler
    uint32_t         checksum;						//!< Device profile checksum
} DeviceProfile_t;
**/
int32_t adrv9001_ArmProfileWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    uint32_t profileAddr = 0;
    uint32_t checksum = 0;
    uint32_t offset = 0;
    uint32_t i = 0;

    uint16_t  armChannels = 0;

    uint8_t cfgData[ADRV9001_PROFILE_CHUNK_MAX + 10] = { 0 };

    static const uint32_t ADRV9001_PROFILE_MAX_SIZE = 1992;
    static const uint32_t ADRV9001_PROFILE_TX_SIZE = 188;
    static const uint32_t ADRV9001_PROFILE_RX_SIZE = 188;
    static const uint32_t ADRV9001_PROFILE_NOISE_CANCEL_SIZE = 28;

    /*!< Bit position of Rx1pin selection in FW struct deviceProfile_t->chanConfig */
    static const uint8_t  RX1PIN_POSITION = 10;

    /*!< Bit position of Rx2pin selection in FW struct deviceProfile_t->chanConfig */
    static const uint8_t  RX2PIN_POSITION = 11;

    ADI_EXPECT(adrv9001_ArmProfileWrite_Validate, device, init);

    adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);

    /* 'clkPllVcoFreq_daHz' is referred as 'vcoFreq_daHz' in FW */
    adrv9001_LoadFourBytes(&offset, &cfgData[0], (init->clocks.clkPllVcoFreq_daHz)); /* CLKPLL VCO frequency is dekaHz (10^1) */

    adrv9001_LoadFourBytes(&offset, &cfgData[0], device->devStateInfo.hsDigClk_Hz); /* HS Dig clock calculated in initialze() */

    adrv9001_LoadFourBytes(&offset, &cfgData[0], KILO_TO_BASE_UNIT(init->clocks.deviceClock_kHz)); /* Device clock frequency */

    cfgData[offset++] = (uint8_t)(init->clocks.armPowerSavingClkDiv - 1);

    cfgData[offset++] = (uint8_t)init->clocks.refClockOutEnable;
    
    cfgData[offset++] = (uint8_t)init->clocks.auxPllPower;
    cfgData[offset++] = (uint8_t)init->clocks.clkPllPower;

    /* CLKGEN PLL or LP CLKGEN PLL for HsDigClk */
    /* 0 = CLKGEN PLL; 1 = LP CLKGEN PLL */
    cfgData[offset++] = (uint8_t)init->clocks.clkPllMode;

    /* LO Phase Sync mode */
    cfgData[offset++]  = init->clocks.rfPllPhaseSyncMode;

    /* LO routing select */
    /* NOTE: LO1 = PLL1 / LOGEN1, LO2 = PLL2 / LOGEN2 */
    cfgData[offset]  =   ((uint8_t)init->clocks.rx1LoSelect - 1) & 0x01; /* D0 - Rx1 Sel; 0 = LO1, 1 = LO2 */
    cfgData[offset] |= ((((uint8_t)init->clocks.rx2LoSelect - 1) & 0x01) << 1); /* D1 - Rx2 Sel; 0 = LO1, 1 = LO2 */
    cfgData[offset] |= ((((uint8_t)init->clocks.tx1LoSelect - 1) & 0x01) << 2); /* D2 - Tx1 Sel; 0 = LO1, 1 = LO2 */
    cfgData[offset] |= ((((uint8_t)init->clocks.tx2LoSelect - 1) & 0x01) << 3); /* D3 - Tx2 Sel; 0 = LO1, 1 = LO2 */
    offset++;

    /* LO divider mode */
    /* 0 = BEST_PHASE_NOISE, 1 = BEST_POWER_SAVING */
    cfgData[offset]  =   (uint8_t)init->clocks.rx1LoDivMode & 0x01;        /* D0 - RX1 LO divider mode sel */
    cfgData[offset] |= (((uint8_t)init->clocks.rx2LoDivMode & 0x01) << 1); /* D1 - RX2 LO divider mode sel */
    cfgData[offset] |= (((uint8_t)init->clocks.tx1LoDivMode & 0x01) << 2); /* D2 - TX1 LO divider mode sel */
    cfgData[offset] |= (((uint8_t)init->clocks.tx2LoDivMode & 0x01) << 3); /* D3 - TX2 LO divider mode sel */
    offset++;

    adrv9001_LoadTwoBytes(&offset, &cfgData[0], init->clocks.extLo1Divider); /* External LO1 In divider setting */

    adrv9001_LoadTwoBytes(&offset, &cfgData[0], init->clocks.extLo2Divider); /* External LO2 In divider setting */

    /* EXT LO1 output frequency in kHz */
    adrv9001_LoadFourBytes(&offset, &cfgData[0], (init->clocks.extLo1OutFreq_kHz));

    /* EXT LO2 output frequency in kHz */
    adrv9001_LoadFourBytes(&offset, &cfgData[0], (init->clocks.extLo2OutFreq_kHz));

    /* LOGEN Power select */
    cfgData[offset]  =   ((uint8_t)init->clocks.loGen1Select - 1) & 0x01;        /* D0 - LoGen1 Sel; 0 = RFPLL1_LDO, 1 = OffChip */
    cfgData[offset] |= ((((uint8_t)init->clocks.loGen2Select - 1) & 0x01) << 1); /* D1 - LoGen2 Sel; 0 = RFPLL2_LDO, 1 = OffChip */
    offset++;

    /* extLoSelMask */
    /* D0-1 - LO1 Sel 0 = Inter LO1, 1 = Ext OUT, 2 = ExtIn1, 3=ExtIn2
     * D2-3 - LO2 Sel 0 = Inter LO2, 1 = Ext OUT, 2 = ExtIn1, 3=ExtIn2
     * D4 - Ext1 0 = Differential, 1= Single Ended
     * D5 - Ext1 0 = Differential, 1= Single Ended */

    cfgData[offset] =  (uint8_t)(init->clocks.rfPll1LoMode & 0x03);        /* D0-1 : LO1 Sel; 0 = Inter LO1, 1 = Ext OUT, 2 = ExtIn1, 3 = ExtIn2 */
    cfgData[offset] |= (uint8_t)((init->clocks.rfPll2LoMode & 0x03) << 2); /* D2-3 : LO1 Sel; 0 = Inter LO1, 1 = Ext OUT, 2 = ExtIn1, 3 = ExtIn2 */
    cfgData[offset] |= (uint8_t)((init->clocks.ext1LoType & 0x03) << 4);   /* D4 : Ext1 0 = Differential, 1= Single Ended */
    cfgData[offset] |= (uint8_t)((init->clocks.ext2LoType & 0x03) << 5);   /* D5 : Ext2 0 = Differential, 1= Single Ended */

    offset++;


    
    
    /* Update 'armChannels' with Tx channel information if the channel is enabled; No action is taken otherwise  */
    if (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_TX_PROFILE_VALID))
    {
        armChannels |= (init->tx.txInitChannelMask & (ADI_ADRV9001_TX1 | ADI_ADRV9001_TX2));
        
        /* Tx channels must have a valid and enabled ILB channel unless the signaling is Direct FM/FSK */
        if(ADRV9001_BF_EQUAL(init->tx.txInitChannelMask, ADI_ADRV9001_TX1) &&
            (init->tx.txProfile[0].outputSignaling != ADI_ADRV9001_TX_DIRECT_FM_FSK))
        {
            armChannels |= (init->rx.rxInitChannelMask & ADI_ADRV9001_ILB1);
        }
            
        if (ADRV9001_BF_EQUAL(init->tx.txInitChannelMask, ADI_ADRV9001_TX2) &&
            (init->tx.txProfile[1].outputSignaling != ADI_ADRV9001_TX_DIRECT_FM_FSK))
        {
            armChannels |= (init->rx.rxInitChannelMask & ADI_ADRV9001_ILB2);
        }
    }

    /* Update 'armChannels' with Rx channel information if the channel is enabled; No action is taken otherwise  */
    if (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_RX_PROFILE_VALID))
    {
        armChannels |= (init->rx.rxInitChannelMask & (ADI_ADRV9001_RX1 | ADI_ADRV9001_RX2));
    }

    /* Update 'armChannels' with ORx channel information if the channel is enabled; No action is taken otherwise  */
    if (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_ORX_PROFILE_VALID))
    {
        armChannels |= (init->rx.rxInitChannelMask & (ADI_ADRV9001_ORX1 | ADI_ADRV9001_ORX2));
    }

    /* Update 'armChannels' with ELB channel information if the channel is enabled; No action is taken otherwise  */
    if (ADRV9001_BF_EQUAL(device->devStateInfo.profilesValid, ADI_ADRV9001_ELB_PROFILE_VALID))
    {
        armChannels |= (init->rx.rxInitChannelMask & (ADI_ADRV9001_ELB1 | ADI_ADRV9001_ELB2));
    }

    /* D10 - Rx1Pin; 0 = Rx1A, 1 = Rx1B */
    armChannels |= (uint16_t)((init->clocks.rx1RfInputSel & 0x03) << RX1PIN_POSITION);
    /* D11 - Rx2Pin; 0 = Rx2A, 1 = Rx2B */
    armChannels |= (uint16_t)((init->clocks.rx2RfInputSel & 0x03) << RX2PIN_POSITION);

    adrv9001_LoadTwoBytes(&offset, &cfgData[0], armChannels);

    profileAddr = device->devStateInfo.profileAddr;

    checksum = adrv9001_Crc32ForChunk(&cfgData[0], offset, checksum, 0);
    recoveryAction = adi_adrv9001_arm_Memory_Write(device, profileAddr, &cfgData[0], offset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += offset;
    offset = 0;

    /* TX CONFIG 188 bytes per TX channel */
    for (i = 0; i < ADI_ADRV9001_MAX_TXCHANNELS; i++)
    {
        adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);
        /* Check whether the Tx channel is valid and write '0' for invalid channel */
        if ( ADRV9001_BF_EQUAL( init->tx.txInitChannelMask, TX_CHANNELS[i] ) )
        {
            adrv9001_TxProfileConfigWrite(device, &(init->tx.txProfile[i]), &cfgData[0], &offset);
        }
        else
        {
            offset = ADRV9001_PROFILE_TX_SIZE;
        }

        checksum = adrv9001_Crc32ForChunk(&cfgData[0], offset, checksum, 0);
        recoveryAction = adi_adrv9001_arm_Memory_Write(device, profileAddr, &cfgData[0], offset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
        ADI_ERROR_RETURN(device->common.error.newAction);

        profileAddr += offset;
        offset = 0;
    }

    /* The profile config structure for Rx, ORx, ILB and ELB is the same. So loop through 8 times */
    /* RX CONFIG 184 bytes per channel */
    for (i = 0; i < ADI_ADRV9001_MAX_RXCHANNELS; i++)
    {
        adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);
        /* Check whether the Rx channel is valid and write '0' for invalid channel */
        if ( ADRV9001_BF_EQUAL( init->rx.rxInitChannelMask, RX_CHANNELS[i] ) )
        {
            adrv9001_RxProfileConfigWrite(device, &(init->rx.rxChannelCfg[i]), &cfgData[0], &offset);
        }
        else
        {
            offset = ADRV9001_PROFILE_RX_SIZE;
        }

        checksum = adrv9001_Crc32ForChunk(&cfgData[0], offset, checksum, 0);
        recoveryAction = adi_adrv9001_arm_Memory_Write(device, profileAddr, &cfgData[0], offset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
        ADI_ERROR_RETURN(device->common.error.newAction);

        profileAddr += offset;
        offset = 0;
    }

    adrv9001_cfgDataSet(&cfgData[0], 0, ADRV9001_PROFILE_CHUNK_MAX);

    adrv9001_DeviceSysConfigWrite(device, &(init->sysConfig), &cfgData[0], &offset);

    /* noiseCanConfig_t (size: 28 bytes) is part of device profile.
     * But this struct is added by FW only as a placeholder, so the memory is filled with '0' at these locations.
     * The calculated 'checksum' is written at the end of FW device profile struct.
     * Hence offset of noiseCanConfig_t is added */
    offset += ADRV9001_PROFILE_NOISE_CANCEL_SIZE;

    /* The checksum calculation will not have any effect as 'cfgData' is filled with '0'.
     * Nevertheless, this is needed in future when noiseCanConfig_t and deviceSysConfig_t have valid values */
    checksum = adrv9001_Crc32ForChunk(&cfgData[0], offset, checksum, 1);

    adrv9001_LoadFourBytes(&offset, &cfgData[0], checksum); /* Copy final Checksum in 'cfgData'*/

    recoveryAction = adi_adrv9001_arm_Memory_Write(device, profileAddr, &cfgData[0], offset, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adi_adrv9001_arm_Memory_Write()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    profileAddr += offset;

    offset = 0;
    if (profileAddr != (device->devStateInfo.profileAddr + ADRV9001_PROFILE_MAX_SIZE))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            profileAddr,
            "Exceed Max Profile bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

/*
* Fetches the (cached) pointer to the PFIR profile buffer on the ARM.
* Returns ADI_COMMON_ERR_API_FAIL if the cached value is invalid - perhaps the program hasn't been loaded yet.
*/
int32_t adrv9001_pFirProfileAddr_Get(adi_adrv9001_Device_t *device, uint32_t *pfirProfileAddr)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    if (device->devStateInfo.pfirProfileAddr < ADRV9001_ADDR_ARM_START_DATA)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_RESET_FULL,
            device,
            "Invalid PFIR Profile Address");

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    *pfirProfileAddr = device->devStateInfo.pfirProfileAddr;

    return recoveryAction;
}

/*********************************************************************************************
* Refer pfirBuffer_t structure below or in t_pfir_common.h file in Navassa ARM firmware
* for the order of transferring the device profile info from API to ARM
**********************************************************************************************
typedef struct {
    //!< During dynamic profile switching, the first two types of PFIRs, RXWbNb and TXWbNbPulShp, would be reloaded for each profile before the switch
    //!<RX WB/NB Compensation PFIR (Channel Filter)coefficient Bank A/B/C/D in rxnb_dem, block30
    pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_A;
    pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_B;
    pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_C;
    pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_D;
    //!<TX WB/NB Preprocessing pulse shaping PFIR coefficient Bank A/B in TX preproc
    pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_A;
    pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_B;
    pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_C;
    pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_D;
    //!< RX NB Pulse Shaping pFIR  128 taps CH1/2 in rxnb_dem
    pfirPulseBuffer_t     pfirRxNbPulShp1[NUM_RX_CHANNELS];
    //!< Channel 1/2 Low/High TIA Bandwidth HP/LP ADC between Mux200 and 201
    pfirMag21Buffer_t     pfirRxMagLowTiaLowSRHp[NUM_RX_CHANNELS];
    pfirMag21Buffer_t     pfirRxMagLowTiaHighSRHp[NUM_RX_CHANNELS];
    pfirMag21Buffer_t     pfirRxMagHighTiaHighSRHp[NUM_RX_CHANNELS];
    pfirMag21Buffer_t     pfirRxMagLowTiaLowSRLp[NUM_RX_CHANNELS];
    pfirMag21Buffer_t     pfirRxMagLowTiaHighSRLp[NUM_RX_CHANNELS];
    pfirMag21Buffer_t     pfirRxMagHighTiaHighSRLp[NUM_RX_CHANNELS];
    //!< TX Magnitude Compensation PFIR 21 taps at Mux410B
    pfirMag21Buffer_t     pfirTxMagComp1;
    pfirMag21Buffer_t     pfirTxMagComp2;
    //!< TX/RX Magnitude Compensation PFIR for NB
    pfirMag13Buffer_t     pfirTxMagCompNb[NUM_TX_CHANNELS];
    pfirMag13Buffer_t     pfirRxMagCompNb[NUM_RX_CHANNELS];
    uint32_t    checksum;
} pfirBuffer_t;
*/
int32_t adrv9001_PfirProfilesWrite(adi_adrv9001_Device_t *device, const adi_adrv9001_Init_t *init)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t profileAddr = 0;
    uint32_t profileStartAddr = 0;
    uint32_t checksum = 0;

    static const uint32_t ADRV9001_PROFILE_MAX_SIZE = 6620;

    ADI_ENTRY_PTR_EXPECT(device, init);
    ADI_EXPECT(adrv9001_pFirProfileAddr_Get, device, &profileStartAddr);
    profileAddr = profileStartAddr;

    recoveryAction = adrv9001_PfirFilterCoeffWrite(device, &(init->pfirBuffer), &profileAddr, &checksum);
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error from adrv9001_PfirFilterCoeffWrite()");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (profileAddr != (profileStartAddr + ADRV9001_PROFILE_MAX_SIZE))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            profileAddr,
            "Exceed Max Profile bytes");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

/****************************************************** Andrew's Stuff *******************************************************/

/* This function constructs an adi_adrv9000_txDynamicProfile_t struct besed on an instance of adi_adrv9001_TxProfile_t */
/*
Transmitter Structure: adi_adrv9000_txDynamicProfile_t
Size : 16 Bytes

typedef struct {
    uint32_t txInputRate;       // Transmitter sample rate in Hz
    uint32_t primaryBw_Hz;      // TX RF bandwidth
    txWbIntTop_t txWbIntTop;    // TX wideband Interpolation top block
}adi_adrv9000_txDynamicProfile_t;
*/
static void adrv9001_LoadTxDynamicProfile(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_TxProfile_t *txConfig)
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, txConfig->txInputRate_Hz);
    adrv9001_LoadFourBytes(&tempOffset, cfgData, txConfig->primarySigBandwidth_Hz);
    adrv9001_LoadTxWbBlockInterpConfig(&tempOffset, cfgData, &txConfig->txDpProfile.txWbIntTop);

    *offset = tempOffset;
}

/* This function constructs an adi_adrv9000_rxDynamicProfile_t struct besed on an instance of adi_adrv9001_RxChannelCfg_t */
/*
Transmitter Structure: adi_adrv9000_rxDynamicProfile_t
Size : 16 Bytes

typedef struct {
    uint32_t rxOutputRate;      // Receiver sample rate in Hz
    uint32_t primaryBw_Hz;      // RX RF bandwidth
    rxWbDecTop_t rxWbDecTop;    // rxwb_dec_top block
}adi_adrv9000_rxDynamicProfile_t;
*/
static void adrv9001_LoadRxDynamicProfile(uint32_t *offset, uint8_t cfgData[], const adi_adrv9001_RxProfile_t *rxConfig)
{
    uint32_t tempOffset = *offset;

    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxConfig->rxOutputRate_Hz);
    adrv9001_LoadFourBytes(&tempOffset, cfgData, rxConfig->primarySigBandwidth_Hz);
    adrv9001_LoadRxWbDecTopConfig(&tempOffset, cfgData, &rxConfig->rxDpProfile.rxWbDecTop);

    *offset = tempOffset;
}

/* This function constructs and sends an adi_adrv9000_dynamicProfileBuf_t struct based on
   an instance of adi_adrv9001_Init_t */
/*
Transmitter Structure: adi_adrv9000_dynamicProfileBuf_t
Size : 164 Bytes

typedef struct {
    uint8_t  profileIndex;
    adi_adrv9000_txDynamicProfile_t tx1Dynamic;
    adi_adrv9000_txDynamicProfile_t tx2Dynamic;
    adi_adrv9000_rxDynamicProfile_t rx1Dynamic;
    adi_adrv9000_rxDynamicProfile_t rx2Dynamic;
    adi_adrv9000_rxDynamicProfile_t orx1Dynamic;
    adi_adrv9000_rxDynamicProfile_t orx2Dynamic;
    adi_adrv9000_rxDynamicProfile_t ilb1Dynamic;
    adi_adrv9000_rxDynamicProfile_t ilb2Dynamic;
    adi_adrv9000_rxDynamicProfile_t elb1Dynamic;
    adi_adrv9000_rxDynamicProfile_t elb2Dynamic;
}adi_adrv9000_dynamicProfileBuf_t;
*/
int32_t adrv9001_DynamicProfile_Write(adi_adrv9001_Device_t *device,
                                      uint8_t dynamicProfileIndex,
                                      const adi_adrv9001_Init_t *init)
{
    enum { ADRV9001_DYNAMIC_PROFILE_BLOB_SIZE = 164,  /* Declare a true constant that can be used as an array size */
           ADRV9001_DYNAMIC_PROFILE_TX_SIZE   = 16,
           ADRV9001_DYNAMIC_PROFILE_RX_SIZE   = 16 };

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    int i = 0;

    uint32_t offset = 0;
    uint8_t cfgData[ADRV9001_DYNAMIC_PROFILE_BLOB_SIZE] = { 0 };

    cfgData[offset++] = dynamicProfileIndex;
    cfgData[offset++] = 0; // padding
    cfgData[offset++] = 0; // padding
    cfgData[offset++] = 0; // padding

    /* Load data for enabled channels; load zeros for disabled channels */
    for (i = 0; i < ADI_ADRV9001_MAX_TXCHANNELS; i++)
    {
        if (ADRV9001_BF_EQUAL( init->tx.txInitChannelMask, TX_CHANNELS[i] ))
        {
            adrv9001_LoadTxDynamicProfile(&offset, cfgData, &init->tx.txProfile[i]);
        }
        else
        {
            offset += ADRV9001_DYNAMIC_PROFILE_TX_SIZE;
        }
    }
    for (i = 0; i < ADI_ADRV9001_MAX_RXCHANNELS; i++)
    {
        if (ADRV9001_BF_EQUAL( init->rx.rxInitChannelMask, RX_CHANNELS[i] ))
        {
            adrv9001_LoadRxDynamicProfile(&offset, cfgData, &init->rx.rxChannelCfg[i].profile);
        }
        else
        {
            offset += ADRV9001_DYNAMIC_PROFILE_RX_SIZE;
        }
    }

    if (offset != sizeof(cfgData))
    {
        ADI_SHOULD_NOT_EXECUTE(device);
    }

    /* Write dynamic profile data to the adrv9001's dynamic profile mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_MAILBOX_DYN_PROF, &cfgData[0], sizeof(cfgData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    return recoveryAction;
}

int32_t adrv9001_PfirWbNbBuffer_Write(adi_adrv9001_Device_t *device,
                                      const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t checksum = 0;  /* Checksum is not used, not sent to ADRV9001 */
    uint8_t cfgData[ADRV9001_PROFILE_CHUNK_MAX + 10] = { 0 };

    /* Get location of the PFIR buffer in ARM memory */
    uint32_t profileAddr = 0;
    ADI_EXPECT(adrv9001_pFirProfileAddr_Get, device, &profileAddr);

    /* Make BLOB of PFIR data and write it to the adrv9001 */
    ADI_EXPECT(adrv9001_PfirWbNbFilterBank_Write,
               device,
               pfirCoeff,
               ADI_ADRV9001_WB_NB_PFIR_COEFS_MAX_SIZE,
               cfgData,
               &profileAddr,
               &checksum);

    return recoveryAction;
}
