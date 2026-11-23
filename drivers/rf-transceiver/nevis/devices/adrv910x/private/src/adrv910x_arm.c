/**
* \file
* \brief Contains ARM private features related function implementation defined in
*        adrv910x_arm.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/
#include "adi_common_error.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_user.h"
#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_bf.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adi_adrv910x_hal.h"

#ifdef __KERNEL__
#include <linux/string.h>
#endif

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/
#ifdef ADI_INIT_DMA_SKIP
#define ADRV910X_DMA_SKIP() \
{ \
 if (device->devStateInfo.swTest) \
 { \
     return (int32_t)recoveryAction; \
 } \
}
#else
#define ADRV910X_DMA_SKIP()
#endif

#define ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT    2
#define ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT   10
#define ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT   18
#define ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT   26
#define ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK 0x3
#define ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BYTE_FILL 0x10

#define ADRV910X_PROFILE_CHUNK_MAX              256u
#define ADRV910X_DYNAMIC_PROFILE_BLOB_SIZE      164
#define ADRV910X_FREQ_HOPPING_MAX_NUM_BYTES     1536u
const char* const adrv910x_error_table_ArmBootStatus[] =
{
    "ARM is powering up",
    "ARM booted with no failure",
    "ARM Firmware checksum error",
    "Efuse data error",
    "Stream image checksum error",
    "Device profile checksum error",
    "Clock PLL initialization error",
    "System clock initialization error",
    "Device profile init setup error",
    "JTAG build status ready indication",
    "Bootup clock LOGEN error",
    "Error initializing RxQEC hardware",
    "Failed to create Health monitor timers",
    "ADC RCAL error",
    "ADC CCAL error",
    "Stream Run error"
};

const char* const adrv910x_error_table_CmdCtrlMboxCmdError[] =
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

const char* const adrv910x_error_table_CmdError[] = 
{ 
    "Error occurred during an Init Calibration. Check that no signal is being applied to the Rx ports. Check that "
        "correct external LOs are applied, and synchronized,  where appropriate",
    "Error occurred during a Tracking Calibration. Disable tracking calibrations, reset and program. If enabled "
        "tracking calibrations require external loopback, check loopback connections. If issue persists, contact "
        "Analog Devices for support",
    "Error occurred during a Firmware Mailbox SET or GET transaction.Check the connection between ADRV910X Evaluation "
        "card and FPGA Platform",
    "Error occurred during a Firmware Mailbox GET transaction.Check the connection between ADRV910X Evaluation card "
        "and FPGA Platform",
    "Error occurred during a Firmware Mailbox CONFIG transaction.Check the connection between ADRV910X Evaluation card"
        " and FPGA Platform",
    "Error occurred in Firmware Driver code",
    "Error occurred in Firmware System code"
};


/*
 * The 'data' passed in to this function is written at cfgData[] array at 'offset' location and
 * increments 'offset' by 2
 **/
void adrv910x_LoadTwoBytes(uint32_t *offset, uint8_t cfgData[], const uint16_t data)
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
void adrv910x_LoadFourBytes(uint32_t *offset, uint8_t cfgData[], const uint32_t data)
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
void adrv910x_LoadEightBytes(uint32_t *offset, uint8_t cfgData[], const uint64_t data)
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

void adrv910x_ParseTwoBytes(uint32_t *offset, uint8_t cfgData[], uint16_t *value)
{
    uint32_t tempOffset = *offset;

    *value = 0;
    *value |= (uint16_t)(cfgData[tempOffset++]) << 0;
    *value |= (uint16_t)(cfgData[tempOffset++]) << 8;

    *offset = tempOffset;
}

void adrv910x_ParseFourBytes(uint32_t *offset, uint8_t cfgData[], uint32_t *value)
{
    uint32_t tempOffset = *offset;

    *value = 0;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 0;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 8;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 16;
    *value |= (uint32_t)(cfgData[tempOffset++]) << 24;

    *offset = tempOffset;
}

void adrv910x_ParseEightBytes(uint32_t *offset, uint8_t cfgData[], uint64_t *value)
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
#ifndef NEVIS_PS2
static __maybe_unused int32_t adrv910x_DmaMemWriteByte(adi_adrv910x_Device_t *device,
    uint32_t address,
    const uint8_t data[],
    uint32_t byteCount,
	adi_common_Processor_Subsystem_e PS)
{
    uint32_t i = 0;
    uint8_t regWrite = 0;
    uint8_t autoInc = ADI_ADRV910X_ARM_MEM_AUTO_INCR;
	uint32_t DMABase, DMAByteSelect, DMADataBase;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, data, byteCount);

    ADRV910X_DMAINFO("ARM_MEM_WRITE", address, byteCount);

    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regWrite &= ~ADRV910X_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (address >= ADRV910X_ADDR_ARM_START_DATA)
    {
        regWrite |= ADRV910X_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regWrite |= ADRV910X_BF_ENCODE(0, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != 0)
    {
        regWrite |= ADRV910X_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
	if (PS == ADI_PS1)
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV910X_ADDR_ARM_DMA_CTL, regWrite);
		DMABase = ADRV910X_ADDR_ARM_DMA_ADDR0;
		DMAByteSelect = ADRV910X_ADDR_ARM_DMA_BYTE_SEL;
		DMADataBase = ADRV910X_ADDR_ARM_DMA_DATA0;
	}
	else
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_SYS2_DMA_CTL", ADRV910X_ADDR_SYS2_ARM_DMA_CTL, regWrite);
		DMABase = ADRV910X_ADDR_SYS2_ARM_DMA_ADDR0;
		DMAByteSelect = ADRV910X_ADDR_SYS2_ARM_DMA_BYTE_SEL;
		DMADataBase = ADRV910X_ADDR_SYS2_ARM_DMA_DATA0;
	}

    /* AHB address for write */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", DMAByteSelect, (uint8_t)((address & ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    for (i = 0; i < byteCount; i++)
    {
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_0+", DMADataBase + ((address & 0x00000003)), data[i]);
        if (address & 0x00000003)
        {
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_0", DMADataBase, data[i]); /* inc address */
        }

        address++;
        if (autoInc == 0)
        {
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));

	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", DMAByteSelect, (uint8_t)((address & ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));

	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    /* setting up the DMA control register for a read to avoid accidental writing */
    regWrite |= ADRV910X_DMA_CTL_RD_WRB;
	if (PS == ADI_PS1)
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV910X_ADDR_ARM_DMA_CTL, regWrite);
	}
	else
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_SYS2_DMA_CTL", ADRV910X_ADDR_SYS2_ARM_DMA_CTL, regWrite);
	}

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv910x_DmaMemReadByte(adi_adrv910x_Device_t *device,
    uint32_t address,
    uint8_t returnData[],
    uint32_t byteCount,
    uint8_t autoIncrement,
	adi_common_Processor_Subsystem_e PS)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t regRead = 0;
    uint8_t dataRead0 = 0;
	uint32_t DMABase, DMADataBase, DMAByteSelect;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    ADRV910X_DMAINFO("ARM_MEM_READ", address, byteCount);

    /*
    Use SPI DMA for Read
    */
    /* Read */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regRead |= ADRV910X_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (!((address >= ADRV910X_ADDR_ARM_START_PROG) && (address <= ADRV910X_ADDR_ARM_END_PROG)))
    {
        regRead |= ADRV910X_DMA_CTL_SYS_CODEB;
    }
	else
	{
		regRead &= ~ADRV910X_DMA_CTL_SYS_CODEB;
	}
    
    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regRead |= ADRV910X_BF_ENCODE(0, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoIncrement != 0)
    {
        regRead |= ADRV910X_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
	if (PS == ADI_PS1)
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV910X_ADDR_ARM_DMA_CTL, regRead);
		DMABase = ADRV910X_ADDR_ARM_DMA_ADDR0;
		DMAByteSelect = ADRV910X_ADDR_ARM_DMA_BYTE_SEL;
		DMADataBase = ADRV910X_ADDR_ARM_DMA_DATA0;
	}
	else
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_SYS2_DMA_CTL", ADRV910X_ADDR_SYS2_ARM_DMA_CTL, regRead);
		DMABase = ADRV910X_ADDR_SYS2_ARM_DMA_ADDR0;
		DMAByteSelect = ADRV910X_ADDR_SYS2_ARM_DMA_BYTE_SEL;
		DMADataBase = ADRV910X_ADDR_SYS2_ARM_DMA_DATA0;
	}
    

    if (autoIncrement == 0)
    {
        /* AHB address for read */
        /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
        /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", DMAByteSelect, (uint8_t)(((address & ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0) | ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BYTE_FILL));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));


        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i++)
        {
	        ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0+", DMADataBase + (address % 4), &dataRead0);
            returnData[i] = dataRead0;

            if (address & 0x00000003)
            {
	            ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0", DMADataBase, &dataRead0); /* inc address */
            }

            address++;

            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", DMAByteSelect, (uint8_t)(((address & ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0) | ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BYTE_FILL));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }
    else
    {
        /* AHB address for read */
        /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
        /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));


        /* starting write at zero address offset */
        for (i = 0; i < byteCount; i += 4)
        {
	        ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0", DMADataBase, &dataRead0);
            returnData[i] = dataRead0;
	        ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_1", DMADataBase+1, &dataRead0);
            returnData[i + 1] = dataRead0;
	        ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_2", DMADataBase+2, &dataRead0);
            returnData[i + 2] = dataRead0;
            
            /* 'single_instruction' has to be cleared before reading DMA_DATA3 and set back after */
            ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, 0x0);
	        ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_3", DMADataBase+3, &dataRead0);
            returnData[i + 3] = dataRead0;
	        ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, 0x1);
        }
    }

    ADI_API_RETURN(device);
}

int32_t adrv910x_DmaMemWrite(adi_adrv910x_Device_t *device,
	uint32_t address,
	const uint8_t data[],
	uint32_t byteCount,
	adi_adrv910x_ArmSingleSpiWriteMode_e spiWriteMode,
	adi_common_Processor_Subsystem_e PS)
{
    uint32_t i = 0;
    uint8_t regWrite = 0;
    uint8_t autoInc = ADI_ADRV910X_ARM_MEM_AUTO_INCR;
    uint32_t addrIndex = 0;
    uint32_t dataIndex = 0;
    uint32_t spiBufferSize = ((HAL_SPIWRITEARRAY_BUFFERSIZE / 3) - 1);
    uint16_t addrArray[(HAL_SPIWRITEARRAY_BUFFERSIZE / 3)] = { 0 };
    uint8_t  dataArray[HAL_SPIWRITEARRAY_BUFFERSIZE] = { 0 };
    uint32_t index = 0;
    uint32_t armMemAddress = address;
    uint8_t regVal = 0;
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t singleInstruction = 0;
    uint8_t spiMode = 0;
    uint8_t spiConfig_A = 0;
	uint16_t DMABase, DMADataBase, DMAByteSelect, DMACtrl;
    
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, data, byteCount);

    ADRV910X_DMAINFO("ARM_MEM_WRITE", armMemAddress, byteCount);

    spiMode = device->spiSettings.enSpiStreaming;
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Get, device, &singleInstruction);
    ADRV910X_SPIREADBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_A, &spiConfig_A);

	if (PS == ADI_PS1)
	{
		ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_CTL", ADRV910X_ADDR_ARM_DMA_CTL, &regVal);
		DMABase = ADRV910X_ADDR_ARM_DMA_ADDR0;
		DMADataBase = ADRV910X_ADDR_ARM_DMA_DATA0;
		DMAByteSelect = ADRV910X_ADDR_ARM_DMA_BYTE_SEL;
		DMACtrl = ADRV910X_ADDR_ARM_DMA_CTL;
	}
	else
	{
		ADRV910X_SPIREADBYTEDMA(device, "ARM_SYS2_DMA_CTL", ADRV910X_ADDR_SYS2_ARM_DMA_CTL, &regVal);
		DMABase = ADRV910X_ADDR_SYS2_ARM_DMA_ADDR0;
		DMADataBase = ADRV910X_ADDR_SYS2_ARM_DMA_DATA0;
		DMAByteSelect = ADRV910X_ADDR_SYS2_ARM_DMA_BYTE_SEL;
		DMACtrl = ADRV910X_ADDR_SYS2_ARM_DMA_CTL;
	}

    if (ADRV910X_BF_EQUAL(regVal, ADRV910X_DMA_CTL_BUS_WAITING))
    {
        ADRV910X_DEBUG_INFO_NUM("ARM_MEM_WRITE BUS ERROR ADRV910X_ADDR_ARM_DMA_CTL", regVal);
    }

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (((armMemAddress & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
	    if ((ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_CACHED_BYTES_N == spiWriteMode) || 
            (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode))
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV910X_SRC_ARMCMD,
                spiWriteMode,
                recoveryAction,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                "SPI_WRITE_MODE_STANDARD_BYTES_252 and ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 are not supported if byteCount is not a multiple of 4");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        return adrv910x_DmaMemWriteByte(device, armMemAddress, data, byteCount, PS);
    }

    /*
    Use SPI DMA for Write
    */
    /* Write */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regWrite &= ~ADRV910X_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (armMemAddress >= ADRV910X_ADDR_ARM_START_DATA)
    {
        regWrite |= ADRV910X_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
	if (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_1 == spiWriteMode)
	{
		regWrite |= ADRV910X_BF_ENCODE(0, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", DMAByteSelect, ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BYTE_FILL);
	}
	else
	{
		regWrite |= ADRV910X_BF_ENCODE(2, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_BYTE_SEL", DMAByteSelect, 0x00);
	}

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoInc != 0)
    {
        regWrite |= ADRV910X_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", DMACtrl, regWrite);
    
    /* Enable single instruction and disable SPI streaming mode by default.
     * If ADRV910X SPI streming mode is selected, then single instruction and single instruction are disbled */
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, 0x1);
    device->spiSettings.enSpiStreaming = 0;

    if (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode ||
		ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_1 == spiWriteMode)
    {
	    ADRV910X_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_A, ADRV910X_CONFIG_A_SPI_SDO_ACTIVE);
	    
	    ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_AddrAscension2_Set, device, 0x0);
	    ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, 0x0);
        device->spiSettings.enSpiStreaming = 1;
    }

    /* AHB address for write */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

	if (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_CACHED_BYTES_N == spiWriteMode)
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
	        
			addrArray[addrIndex] = DMADataBase + (3 - index);

			addrIndex++;
			armMemAddress++;

			if (addrIndex >= spiBufferSize)
			{
				ADRV910X_SPIWRITEBYTESDMA(device,
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
			ADRV910X_SPIWRITEBYTESDMA(device,
				"DMA_MEM_WRITE_CACHE_LAST",
				armMemAddress - addrIndex,
				&addrArray[0],
				&dataArray[0],
				addrIndex);
		}
	}
	else if (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode ||
			 ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4 == spiWriteMode)
	{
		/* starting write at zero address offset */
		for (i = 0; i < byteCount; i += 4)
		{
			if (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4 == spiWriteMode)
			{
				ADRV910X_SPIWRITEBYTESTREAMDMA(device, "ARM_DMA_DATA_3", DMADataBase + 3, (data + i), 4);
			}
			else
			{
				/* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
				/* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_3", DMADataBase + 3, data[i + 3]);
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_2", DMADataBase + 2, data[i + 2]);
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_1", DMADataBase + 1, data[i + 1]);
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_DATA_0", DMADataBase, data[i]);
			}

			if (autoInc == 0)
			{
				/* AHB address for write */
				/* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
				armMemAddress += 4;
				/* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase + 3, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase + 2, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase + 1, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
				ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((armMemAddress) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
			}
		}
	}
	else if (ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_1 == spiWriteMode)
	{
		int x, amount, arrloc = 0;
		
		/* disable spi auto increment */
		ADRV910X_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_B", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_B, 0x08);
		dataArray[0] = (uint8_t)((DMADataBase >> 8) & 0x7F);
		dataArray[1] = (uint8_t)(DMADataBase);
		do
		{
			amount = byteCount > (HAL_SPIWRITEARRAY_BUFFERSIZE - 2) ? (HAL_SPIWRITEARRAY_BUFFERSIZE - 2) : byteCount;
			if((amount+2) > 8 && ((amount+2) % 8) != 0)
			{
				amount -= (amount + 2) % 8;
			}
			for (x=0; x<amount; x++)
			    dataArray[2+x] = data[arrloc+x];
			byteCount -= amount;
			arrloc += amount;
			amount += 2;
			adi_adrv910x_hal_spi_write(device->common.devHalInfo, dataArray, amount);
		} while (byteCount > 0);
		/* restore spi auto increment */
		ADRV910X_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_B", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_B, 0x00);
	}

    /* setting up the DMA control register for a read to avoid accidental writing */
    regWrite |= ADRV910X_DMA_CTL_RD_WRB;
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", DMACtrl, regWrite);

    ADRV910X_SPIWRITEBYTEDMA(device, "SPI_INTERFACE_CONFIG_A", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_A, spiConfig_A);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, singleInstruction);
    device->spiSettings.enSpiStreaming = spiMode;

    ADI_API_RETURN(device);
}

int32_t adrv910x_DmaMemRead(adi_adrv910x_Device_t *device, uint32_t address, uint8_t returnData[], uint32_t byteCount, uint8_t autoIncrement, adi_common_Processor_Subsystem_e PS)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint8_t regRead = 0;
    uint8_t dataRead = 0;
    uint8_t regVal = 0;
	uint32_t DMABase, DMADataBase;

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    ADRV910X_DMAINFO("ARM_MEM_READ", address, byteCount);

    ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_CTL", ADRV910X_ADDR_ARM_DMA_CTL, &regVal);

    if (ADRV910X_BF_EQUAL(regVal, ADRV910X_DMA_CTL_BUS_WAITING))
    {
        ADRV910X_DEBUG_INFO_NUM("ARM_MEM_READ BUS ERROR ADRV910X_ADDR_ARM_DMA_CTL", regVal);
    }

    ADRV910X_DMA_SKIP();

    /* If Address is not on word boundary, Or ByteCount is not on Word boundary */
    if (((address & 0x00000003) > 0) || ((byteCount & 0x00000003) > 0))
    {
        return adrv910x_DmaMemReadByte(device, address, returnData, byteCount, autoIncrement, PS);
    }

    /*
    Use SPI DMA for Read
    */
    /* Read */
    /* 1 = read, 0 = write */
    /* core_bf.rd_wrb.write(bf_status, 1'b1); */
    regRead |= ADRV910X_DMA_CTL_RD_WRB;

    /* system AHB bus slave or code AHB bus slave */
    /* 1'b0=code bus; 1'b1=system bus */
    /* core_bf.sys_codeb.write(bf_status, 1'b1); */
    /* determining the data space and setting dataMem bit accordingly */
    if (!((address >= ADRV910X_ADDR_ARM_START_PROG) && (address <= ADRV910X_ADDR_ARM_END_PROG)))
    {
        regRead |= ADRV910X_DMA_CTL_SYS_CODEB;
    }

    /* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
    /* core_bf.bus_size.write(bf_status, 2'b10); */
    regRead |= ADRV910X_BF_ENCODE(2, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);

    /* address auto incremental, 1'b0=no; 1'b1=yes */
    /* core_bf.auto_incr.write(bf_status, 1'b0); */
    if (autoIncrement != 0)
    {
        regRead |= ADRV910X_DMA_CTL_AUTO_INCR;
    }

    /* setting up the DMA control register for a write */
	if (PS == ADI_PS1)
    {
	    ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_CTL", ADRV910X_ADDR_ARM_DMA_CTL, regRead);
	    DMABase = ADRV910X_ADDR_ARM_DMA_ADDR0;
	    DMADataBase = ADRV910X_ADDR_ARM_DMA_DATA0;
	}
	else
	{
		ADRV910X_SPIWRITEBYTEDMA(device, "ARM_SYS2_DMA_CTL", ADRV910X_ADDR_SYS2_ARM_DMA_CTL, regRead);
		DMABase = ADRV910X_ADDR_SYS2_ARM_DMA_ADDR0;
		DMADataBase = ADRV910X_ADDR_SYS2_ARM_DMA_DATA0;
	}

    /* AHB address for read */
    /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
    /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));

    /* starting write at zero address offset */
    for (i = 0; i < byteCount; i += 4)
    {
        /* Write mem_write_data bitfield (mem_write_data[7:0] has to be the last register written) */
        /* core_bf.mem_write_data.write(bf_status, 32'hxxxxxxxx); */

	    ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_3", DMADataBase+3, &dataRead);
        returnData[i + 3] = dataRead;
	    ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_2", DMADataBase+2, &dataRead);
        returnData[i + 2] = dataRead;
	    ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_1", DMADataBase+1, &dataRead);
        returnData[i + 1] = dataRead;
        
        /* 'single_instruction' has to be cleared before reading DMA_DATA3 and set back after */
	    ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, 0x0);
	    ADRV910X_SPIREADBYTEDMA(device, "ARM_DMA_DATA_0", DMADataBase, &dataRead);
        returnData[i] = dataRead;
	    ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_SingleInstruction_Set, device, 0x1);

        if (autoIncrement == 0)
        {
            /* AHB address for read */
            /* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
            address += 4;
            /* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_3", DMABase+3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_2", DMABase+2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_1", DMABase+1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	        ADRV910X_SPIWRITEBYTEDMA(device, "ARM_DMA_ADDR_0", DMABase, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
        }
    }

    ADI_API_RETURN(device);
}

#endif /* NEVIS_PS2 */
static int32_t adrv910x_ArmMailBoxErrCodeGet(adi_adrv910x_Device_t *device, uint16_t *mailboxErrCode)
{
	/* reading the command status register for given opcode */
	uint8_t errCode = 0;
	uint8_t objId = 0;

	ADRV910X_SPIREADBYTE(device, "ARM_CMD_STATUS_10", (uint16_t)ADRV910X_ADDR_ARM_CMD_STATUS_10, &errCode);
	ADRV910X_SPIREADBYTE(device, "ARM_CMD_STATUS_11", (uint16_t)ADRV910X_ADDR_ARM_CMD_STATUS_10 + 1, &objId);

	*mailboxErrCode = ((uint16_t)objId << 8) | errCode;
	ADI_API_RETURN(device);
}

#ifdef ADI_ADRV910X_ARM_VERBOSE
#include "adrv910x_arm_error_mapping.h"
#endif

static const char* adrv910x_CmdErrMsgGet(uint32_t errCode)
{
    if (0x2000 <= errCode && errCode <= 0x3FFF)
    {
        return adrv910x_error_table_CmdError[0];
    }
    else if (0x4000 <= errCode && errCode <= 0x5FFF)
    {
        return adrv910x_error_table_CmdError[1];
    }
    else if (0x6000 <= errCode && errCode <= 0x7FFF)
    {
        return adrv910x_error_table_CmdError[2];
    }
    else if (0x8000 <= errCode && errCode <= 0x9FFF)
    {
        return adrv910x_error_table_CmdError[3];
    }
    else if (0xA000 <= errCode && errCode <= 0xBFFF)
    {
        return adrv910x_error_table_CmdError[4];
    }
    else if (0xC000 <= errCode && errCode <= 0xDFFF)
    {
        return adrv910x_error_table_CmdError[5];
    }
    else if (0xE000 <= errCode && errCode <= 0xFFFF)
    {
        return adrv910x_error_table_CmdError[6];
    }
    
    return NULL;
}

const char* adrv910x_ArmMailBoxErrMsgGet(uint32_t errCode)
{
#ifndef ADI_ADRV910X_ARM_VERBOSE
    return "";
#else
    if (errCode > ADI_ARRAY_LEN(adrv910x_error_table_CmdCtrlMboxCmdError))
    {
        return NULL;
    }
    else
    {
        return adrv910x_error_table_CmdCtrlMboxCmdError[errCode];
    }
#endif
}

int32_t adrv910x_ArmCmdErrorHandler(adi_adrv910x_Device_t *device, uint32_t detErr)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t armErrorFlag = 0;
    uint16_t mailboxErrCode = 0;
    const char *errorString = NULL;
    static const uint8_t ERR_FLAG_MASK = 0x0E;
    static const uint8_t ERR_FLAG_SHIFT = 1;

    ADI_ENTRY_EXPECT(device);

    armErrorFlag = ADRV910X_BF_DECODE((uint8_t)detErr, ERR_FLAG_MASK, ERR_FLAG_SHIFT);

    if (armErrorFlag > 0)
    {
        errorString = adrv910x_ArmMailBoxErrMsgGet(armErrorFlag);

        /* Report Last Command Error information */
        /* TODO: Might be a good idea to report more specific error based on object id and error code */
        if (armErrorFlag == 7)
        {
            /* Clear the error before 'adrv910x_ArmMailBoxErrCodeGet' function is executed;
               Otherwsie, SPI read of ARM command status register will fail and the error code from ARM is not obtained */
            recoveryAction = adi_common_ErrorClear(&device->common);
            if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_ADRV910X_SRC_ARMCMD,
                    detErr,
                    recoveryAction,
                    detErr,
                    errorString);
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            ADI_EXPECT(adrv910x_ArmMailBoxErrCodeGet, device, &mailboxErrCode);
            errorString = adrv910x_CmdErrMsgGet(mailboxErrCode);
            
            ADI_ERROR_REPORT(&device->common,
                             ADI_ADRV910X_SRC_ARMCMD,
                             mailboxErrCode,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             mailboxErrCode,
                             errorString);
        }

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

static uint32_t adrv910x_ArmProfileWrite_Validate(adi_adrv910x_Device_t *device, const deviceProfile_t *profile)
{
	static const uint32_t ADRV910X_ADDR_DEVICE_PROFILE = 0x20000000;

	ADI_ENTRY_PTR_EXPECT(device, profile);

	if (device->devStateInfo.profileAddr < ADRV910X_ADDR_DEVICE_PROFILE)
	{
		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM,
			device,
			"Invalid Profile Address");

		ADI_ERROR_RETURN(device->common.error.newAction);
	}
	
	ADI_API_RETURN(device);
}

int32_t adrv910x_ArmProfileWrite(adi_adrv910x_Device_t *device, const deviceProfile_t *profile, adi_common_Processor_Subsystem_e PS)
{
	ADI_EXPECT(adrv910x_ArmProfileWrite_Validate, device, profile);
	
	uint8_t *profPtr = (uint8_t*)profile;
	uint32_t profileAddr = 0;
	uint32_t byteCount = 0;
	
	if (PS == ADI_PS1)
	{
		profileAddr = device->devStateInfo.profileAddr;
		byteCount = sizeof(deviceProfile_t);
	}
	else
	{
		profileAddr = 0x20000000;
		byteCount = sizeof(deviceProfilePS2_t);
	}
	
	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, profileAddr, &profPtr[0], byteCount, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, PS);
	
	ADI_API_RETURN(device);
}

/*
* Fetches the (cached) pointer to the PFIR profile buffer on the ARM.
* Returns ADI_COMMON_ERR_API_FAIL if the cached value is invalid - perhaps the program hasn't been loaded yet.
*/
int32_t adrv910x_pFirProfileAddr_Get(adi_adrv910x_Device_t *device, uint32_t *pfirProfileAddr)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    if (device->devStateInfo.pfirProfileAddr < ADRV910X_ADDR_ARM_START_DATA)
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

int32_t adrv910x_PfirProfilesWrite(adi_adrv910x_Device_t *device, const pfirBuffer_t *pfirBuffer)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t profileAddr = 0;
    uint32_t profileStartAddr = 0;

	ADI_ENTRY_PTR_EXPECT(device, pfirBuffer);
    ADI_EXPECT(adrv910x_pFirProfileAddr_Get, device, &profileStartAddr);
    profileAddr = profileStartAddr;

	uint8_t *pfirProfPtr = (uint8_t*)pfirBuffer;
	
	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, profileAddr, &pfirProfPtr[0], sizeof(pfirBuffer_t), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
	
	ADI_API_RETURN(device);

    return recoveryAction;
}

int32_t adi_adrv910x_arm_MailBox_Get(adi_adrv910x_Device_t *device, uint8_t objectId, uint8_t subObjectId, uint8_t channelMask, uint16_t byteOffset, uint8_t returnData[], uint32_t byteCount)
{
    uint8_t extendedData[5] = { 0 };

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, returnData, byteCount);

    /* TODO: Validate objectId? */

    extendedData[0] = (uint8_t)((byteCount >> 0) & 0xFF);
    extendedData[1] = (uint8_t)((byteCount >> 8) & 0xFF);
    extendedData[2] = (uint8_t)((byteCount >> 16) & 0xFF);
    extendedData[3] = (uint8_t)((byteCount >> 24) & 0xFF);

    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_MAILBOX_GET, &extendedData[0], 4, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    /* ARM Object id, byte offset LSB, offset MSB = 0, byteCount will read that number of bytes */
    extendedData[0] = channelMask;
    extendedData[1] = objectId;
    extendedData[2] = subObjectId;
    extendedData[3] = (byteOffset & 0xFF);
    extendedData[4] = ((byteOffset >> 8) & 0xFF);

    /* send the read config ARM opcode */
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_GET_OPCODE, &extendedData[0], 5);

    /* check the command status for timeout */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV910X_ARM_GET_OPCODE,
                                        objectId,
                                        ADI_ADRV910X_READARMCFG_TIMEOUT_US,
                                        ADI_ADRV910X_READARMCFG_INTERVAL_US);
    if (byteCount % 4 != 0)
    {
        ADI_EXPECT(adi_adrv910x_arm_Memory_Read, device, ADRV910X_ADDR_ARM_MAILBOX_GET, returnData, byteCount, false, ADI_PS1);
    }
    else
    {
        ADI_EXPECT(adi_adrv910x_arm_Memory_Read, device, ADRV910X_ADDR_ARM_MAILBOX_GET, returnData, byteCount, ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_arm_Config_Write(adi_adrv910x_Device_t *device, const uint8_t armData[], uint32_t armDataSize, const uint8_t mailboxCmd[], uint32_t mailboxCmdSize)
{
#if !ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
	int32_t halError = ADI_COMMON_ACT_NO_ACTION;
#endif

	ADI_ENTRY_PTR_ARRAY_EXPECT(device, armData, armDataSize);
	ADI_ENTRY_PTR_ARRAY_EXPECT(device, mailboxCmd, mailboxCmdSize);

	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], armDataSize, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_SET_OPCODE, &mailboxCmd[0], mailboxCmdSize);

#if ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
										ADRV910X_ARM_SET_OPCODE,
										mailboxCmd[1],
										ADI_ADRV910X_DEFAULT_TIMEOUT_US,
										ADI_ADRV910X_DEFAULT_INTERVAL_US);
#else
	halError = adi_common_hal_Wait_us(&device->common, ADI_ADRV910X_ARM_SET_OPCODE_WAIT_INTERVAL_US);
	if (halError != ADI_COMMON_ACT_NO_ACTION)
	{
		ADI_ERROR_REPORT(&device->common,
						 ADI_COMMON_ERRSRC_ADI_HAL,
						 halError,
						 ADI_COMMON_ACT_ERR_CHECK_TIMER,
						 device,
						 "Timer not working");
		ADI_ERROR_RETURN(device->common.error.newAction);
	}
#endif

	ADI_API_RETURN(device);
}

#ifdef API_MUTEX

#define ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, addr, data) \
	do { \
		uint16_t txByteCount = 0; \
		adi_adrv910x_spi_DataPack(device->common.devHalInfo, spiBuf, &txByteCount, addr, 0xFF, data, ADRV910X_SPI_WRITE_POLARITY); \
		adi_adrv910x_hal_spi_write(device->common.devHalInfo, spiBuf, txByteCount); \
	} while (0)

#define ADRV910X_SPIREADBYTEDMA_UNLOCKED(device, addr, data) \
	do { \
		uint16_t txByteCount = 0; \
		uint8_t rdBuff[3]; \
		adi_adrv910x_spi_DataPack(device->common.devHalInfo, spiBuf, &txByteCount, addr, 0xFF, data, ~ADRV910X_SPI_WRITE_POLARITY); \
		adi_adrv910x_hal_spi_read(device->common.devHalInfo, spiBuf, rdBuff, txByteCount); \
		data = rdBuff[2]; \
	} while (0)

/* Simplified version of DMA Byte Read/Write for accessing HW semaphore */
static __maybe_unused int32_t adrv910x_lockDmaMemReadByte(adi_adrv910x_Device_t *device,
	uint32_t address,
	uint8_t *returnData)
{
	int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
	uint8_t regWrite = 0;
	uint8_t dataRead0 = 0;
	uint8_t spiBuf[3];

	*returnData = 0;
	/* Read */
	/* 1 = read, 0 = write */
	/* core_bf.rd_wrb.write(bf_status, 1'b1); */
	regWrite |= ADRV910X_DMA_CTL_RD_WRB | ADRV910X_DMA_CTL_SYS_CODEB;
    
	/* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
	/* core_bf.bus_size.write(bf_status, 2'b10); */
	regWrite |= ADRV910X_BF_ENCODE(0, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);

	/* setting up the DMA control register for a write, always PS1 */
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_CTL, regWrite);

	/* AHB address for read */
	/* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
	/* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_BYTE_SEL, (uint8_t)(((address & ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0) | ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BYTE_FILL));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0 + 3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0 + 2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0 + 1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
	

	/* read back value */
	ADRV910X_SPIREADBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_DATA0 + (address % 4), dataRead0);
	*returnData = dataRead0;
	ADRV910X_SPIREADBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_DATA0, dataRead0);

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv910x_lockDmaMemWriteByte(adi_adrv910x_Device_t *device,
	uint32_t address,
	const uint8_t *data)
{
	uint8_t regWrite = 0;
	uint8_t spiBuf[3];

	/*
	Use SPI DMA for Write
	*/
	/* Write */
	/* 1 = read, 0 = write */
	/* core_bf.rd_wrb.write(bf_status, 1'b1); */
	regWrite &= ~ADRV910X_DMA_CTL_RD_WRB;
	regWrite |= ADRV910X_DMA_CTL_SYS_CODEB;


	/* bus size, 2'b00=byte; 2'b01=half-word; 2'b10=full-word; 2'b11=invalid */
	/* core_bf.bus_size.write(bf_status, 2'b10); */
	regWrite |= ADRV910X_BF_ENCODE(0, ADRV910X_DMA_CTL_BUS_SIZE_MASK, ADRV910X_DMA_CTL_BUS_SIZE_SHIFT);

	/* setting up the DMA control register for a write */
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_CTL, regWrite);

	/* AHB address for write */
	/* core_bf.bus_addr.write(bf_status, 32'hxxxxxxxx); */
	/* Refer nvs_spi.yda for more info about the byte shifting logic below on DMA address input */
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0 + 3, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR3_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0 + 2, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR2_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0 + 1, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR1_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_BYTE_SEL, (uint8_t)((address & ADRV910X_ADDR_ARM_DMA_BYTE_SEL_BIT_MASK) >> 0));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_ADDR0, (uint8_t)((address) >> ADRV910X_ADDR_ARM_DMA_ADDR0_BYTE_SHIFT));
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_DATA0 + ((address & 0x00000003)), *data);
	if ((address & 0x00000003) != 0)
	{
		ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_DATA0, *data);
	}

	/* setting up the DMA control register for a read to avoid accidental writing */
	regWrite |= ADRV910X_DMA_CTL_RD_WRB;
	ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(device, ADRV910X_ADDR_ARM_DMA_CTL, regWrite);

	ADI_API_RETURN(device);
}

#ifdef LOCK_DEBUG
#define LOCK_LOG(...) printf(__VA_ARGS__)
#else /* LOCK_DEBUG */
#define LOCK_LOG(...)
#endif /* LOCK_DEBUG */

int adi_bbic_sem_unlock(void * device)
{
	uint32_t lock = 0, lock2 = 0;
	uint8_t spiBuf[3];
	adi_adrv910x_Device_t *dev = (adi_adrv910x_Device_t *)device;
	if ((dev->devStateInfo.devState & ADI_ADRV910X_STATE_ARM_LOADED) != 0)
	{
        LOCK_LOG("r-%s\r\n", dev->common.error.funcName);
		do
		{
			adrv910x_lockDmaMemWriteByte(dev, 0x4600018A, (uint8_t*)&lock/*, 1, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1*/);
			ADRV910X_SPIREADBYTEDMA_UNLOCKED(dev, 0x018A, lock2);
			if (lock2 != 0)
			{
				ADRV910X_SPIWRITEBYTEDMA_UNLOCKED(dev, 0x018A, 0);
				adi_common_hal_Wait_us(&dev->common, 10000);
			}
		} while (lock2 != 0);
        LOCK_LOG("lock released\r\n");
	}
  return 0;
}

int adi_bbic_sem_lock(void *device)
{
	uint32_t lock = 0;
	uint32_t lock2 = 0, reg = 0;
	uint8_t spiBuf[3];
	adi_adrv910x_Device_t *dev = (adi_adrv910x_Device_t *)device;

	if ((dev->devStateInfo.devState & ADI_ADRV910X_STATE_ARM_LOADED) != 0)
	{
		do
		{
			adrv910x_lockDmaMemReadByte(dev, 0x4600018A, (uint8_t*)&lock/*, 1, 0, ADI_PS1*/);
            LOCK_LOG("first tap: %02X\r\n", lock);
			if (lock == 0)
			{
				/* lock aquired, apply rule #2... */
				adrv910x_lockDmaMemReadByte(dev, 0x4600018A, (uint8_t*)&lock/*, 1, 0, ADI_PS1*/);
			}
			else
			{
                LOCK_LOG("waiting for lock: %02X\r\n", lock);
				adi_common_hal_Wait_us(&dev->common, 10000);
			}
		} while (lock != 0x11);
	}
	return 1;
}

#endif /* API_MUTEX */

