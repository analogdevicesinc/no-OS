/**
* \file
* \brief Contains private functions related to Radio feature implementation defined in
*        adrv9001_radio.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_tx.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_bf.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_validators.h"
#include "object_ids.h"
#include "adrv9001_radio.h"

#ifdef __KERNEL__
#include <linux/string.h>
#endif

static __maybe_unused int32_t adrv9001_Radio_Pfir_Write_Validate(adi_adrv9001_Device_t *device,
	                                                             const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff,
	                                                             adi_adrv9001_PfirTypeId_e pfirType,
	                                                             adi_common_Port_e port,
	                                                             adi_common_ChannelNumber_e channel)
{
	/* Check input pointers are not null */
	ADI_ENTRY_PTR_EXPECT(device, pfirCoeff);
	
	ADI_EXPECT(adi_adrv9001_Port_Validate, device, port);
	ADI_EXPECT(adi_adrv9001_Channel_Validate, device, channel);
	
	if ((pfirType == ADI_ADRV9001_PFIR_TYPE_RX_NB_PULSE) || (pfirType == ADI_ADRV9001_PFIR_TYPE_MAG_COMP))
	{
		ADI_ERROR_REPORT(&device->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_INV_PARAM,
			ADI_COMMON_ACT_ERR_CHECK_PARAM,
			adrv9001,
			"Selected PFIR ID Type is not supported");

		ADI_API_RETURN(device);
	}

	ADI_API_RETURN(device);
}

int32_t adrv9001_Radio_Pfir_Write(adi_adrv9001_Device_t *device,
	                              const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff,
	                              adi_adrv9001_PfirTypeId_e pfirType,
	                              adi_common_Port_e port,
	                              adi_common_ChannelNumber_e channel)
{
	uint8_t	readval = 0;
	int32_t value = 0;	
	uint32_t i = 0; 
	uint32_t address = 0; 
	uint32_t offset = 0; 
	uint32_t numTaps = 0;

	ADI_PERFORM_VALIDATION(adrv9001_Radio_Pfir_Write_Validate, device, pfirCoeff, pfirType, port, channel);

	if (pfirCoeff->tapsSel == ADI_ADRV9001_PFIR_32_TAPS)
	{
		numTaps = 32u;
	}
	else if (pfirCoeff->tapsSel == ADI_ADRV9001_PFIR_64_TAPS)
	{
		numTaps = 64u;
	}
	else if (pfirCoeff->tapsSel == ADI_ADRV9001_PFIR_96_TAPS)
	{
		numTaps = 96u;
	}
	else
	{
		numTaps = 128u;
	}

	for (i = 0u; i < numTaps; i++)
	{
		/* If exceed numCoeff, then write 0s */
		value = (i < pfirCoeff->numCoeff) ? (int32_t) pfirCoeff->coefficients[i] : 0;

		/* Load 24-bit coefficient data */
		for (offset = 0u; offset < 3u; offset++)
		{
			/* Load address */
			/* REG_CORE_PFIR_CONFIG_PFIR_COEFF_ADDR_MSB */
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffAddr8_Set, device, (address >> 8));
			/* REG_CORE_PFIR_CONFIG_PFIR_COEFF_ADDR_LSB */
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffAddr70_Set, device, address);

			/* Load data  */
			if (offset == 0u)
			{
				ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffData_Set, device, value);
			}
			else if (offset == 1u)
			{
				ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffData_Set, device, (value >> 8));
			}
			else if (offset == 2u)
			{
				ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffData_Set, device, (value >> 16));
			}

			/* Set pfir_coeff_wr_enable */ 
			if (pfirType == ADI_ADRV9001_PFIR_TYPE_RX_WB_NB_COMP)
			{
				ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffWrEnable_Set, device, 0x01);
			}
			else
			{
				if ((port == ADI_TX) && (channel == ADI_CHANNEL_1))
				{
					ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx1PfirCoeffWrEnable_Set, device, 0x01);
				}
				else if ((port == ADI_TX) && (channel == ADI_CHANNEL_2))
				{
					ADI_EXPECT(adrv9001_NvsRegmapCore3_Tx2PfirCoeffWrEnable_Set, device, 0x01);
				}
			}

			/* Wait until previous write is complete, wr_enable is clear (HW ready) */ 
			while (true)
			{
				ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffWrEnable_Get, device, &readval); 
				if (!(readval))
				{
					break;
				}
			}

			/* Increase address */
			address++;
		}
	}

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv9001_Radio_PfirWbNbCompCoeffBankSelMode_Set_Validate(adi_adrv9001_Device_t *device,
	                                                                               adi_adrv9001_PfirBank_e bankSel,
	                                                                               adi_common_Port_e port)
{
	/* Check input pointer is not null */
	ADI_ENTRY_EXPECT(device);
	
	ADI_EXPECT(adi_adrv9001_Port_Validate, device, port);
	
	ADI_RANGE_CHECK(device, bankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);

	ADI_API_RETURN(device);
}

int32_t adrv9001_Radio_PfirWbNbCompCoeffBankSelMode_Set(adi_adrv9001_Device_t *device, 
	                                                adi_adrv9001_PfirBank_e bankSel,
	                                                uint8_t enable,
	                                                adi_common_Port_e port)
{
	ADI_PERFORM_VALIDATION(adrv9001_Radio_PfirWbNbCompCoeffBankSelMode_Set_Validate, device, bankSel, port);

	if (bankSel == ADI_ADRV9001_PFIR_BANK_A)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffRxASel_Set, device, enable);
		}
		else
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirTxACoeffSel_Set, device, enable);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_B)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffRxBSel_Set, device, enable);
		}
		else
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirTxBCoeffSel_Set, device, enable);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_C)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffRxCSel_Set, device, enable);
		}
		else
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirTxCCoeffSel_Set, device, enable);
		}
	}
	else
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirCoeffRxDSel_Set, device, enable);
		}
		else
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_PfirTxDCoeffSel_Set, device, enable);
		}
	}

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv9001_Radio_PfirWbNbCompGain_Set_Validate(adi_adrv9001_Device_t *device,
	                                                                       adi_adrv9001_PfirBank_e bankSel,
	                                                                       adi_adrv9001_PfirGain_e gain,
	                                                                       adi_common_Port_e port)
{
	/* Check input pointer is not null */
	ADI_ENTRY_EXPECT(device);
	
	ADI_EXPECT(adi_adrv9001_Port_Validate, device, port);

	ADI_RANGE_CHECK(device, bankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);
	ADI_RANGE_CHECK(device, gain, ADI_ADRV9001_PFIR_GAIN_NEG_12_DB, ADI_ADRV9001_PFIR_GAIN_MAX);

	ADI_API_RETURN(device);
}

int32_t adrv9001_Radio_PfirWbNbCompGain_Set(adi_adrv9001_Device_t *device,
	                                        adi_adrv9001_PfirBank_e bankSel,
	                                        adi_adrv9001_PfirGain_e gain,
	                                        adi_common_Port_e port)
{
	ADI_PERFORM_VALIDATION(adrv9001_Radio_PfirWbNbCompGain_Set_Validate, device, bankSel, gain, port);
	
	if (bankSel == ADI_ADRV9001_PFIR_BANK_A)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankAPfirGain_Set, device, (uint8_t)gain);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankAPfirGain_Set, device, (uint8_t)gain);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_B)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankBPfirGain_Set, device, (uint8_t)gain);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankBPfirGain_Set, device, (uint8_t)gain);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_C)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankCPfirGain_Set, device, (uint8_t)gain);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankCPfirGain_Set, device, (uint8_t)gain);
		}
	}
	else
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankDPfirGain_Set, device, (uint8_t)gain);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankDPfirGain_Set, device, (uint8_t)gain);
		}
	}
	
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv9001_Radio_PfirWbNbCompTaps_Set_Validate(adi_adrv9001_Device_t *device,
	                                                                       adi_adrv9001_PfirBank_e bankSel,
	                                                                       adi_adrv9001_PfirNumTaps_e taps,
	                                                                       adi_common_Port_e port)
{
	/* Check input pointer is not null */
	ADI_ENTRY_EXPECT(device);
	
	ADI_EXPECT(adi_adrv9001_Port_Validate, device, port);
	
	ADI_RANGE_CHECK(device, bankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);
	ADI_RANGE_CHECK(device, taps, ADI_ADRV9001_PFIR_32_TAPS, ADI_ADRV9001_PFIR_TAPS_MAX_ID);

	ADI_API_RETURN(device);
}

int32_t adrv9001_Radio_PfirWbNbCompTaps_Set(adi_adrv9001_Device_t *device,
	                                        adi_adrv9001_PfirBank_e bankSel,
	                                        adi_adrv9001_PfirNumTaps_e taps,
	                                        adi_common_Port_e port)
{
	ADI_PERFORM_VALIDATION(adrv9001_Radio_PfirWbNbCompTaps_Set_Validate, device, bankSel, taps, port);
	
	if (bankSel == ADI_ADRV9001_PFIR_BANK_A)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankAPfirTaps_Set, device, (uint8_t)taps);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankAPfirTaps_Set, device, (uint8_t)taps);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_B)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankBPfirTaps_Set, device, (uint8_t)taps);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankBPfirTaps_Set, device, (uint8_t)taps);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_C)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankCPfirTaps_Set, device, (uint8_t)taps);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankCPfirTaps_Set, device, (uint8_t)taps);
		}
	}
	else
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankDPfirTaps_Set, device, (uint8_t)taps);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankDPfirTaps_Set, device, (uint8_t)taps);
		}
	}
	
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv9001_Radio_PfirWbNbCompSymmetric_Set_Validate(adi_adrv9001_Device_t *device,
	                                                                            adi_adrv9001_PfirBank_e bankSel,
	                                                                            adi_adrv9001_PfirSymmetric_e symmetric,
	                                                                            adi_common_Port_e port)
{
	/* Check input pointer is not null */
	ADI_ENTRY_EXPECT(device);
	
	ADI_EXPECT(adi_adrv9001_Port_Validate, device, port);
	
	ADI_RANGE_CHECK(device, bankSel, ADI_ADRV9001_PFIR_BANK_A, ADI_ADRV9001_PFIR_BANK_D);
	ADI_RANGE_CHECK(device, symmetric, ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, ADI_ADRV9001_PFIR_COEF_SYMMETRIC);

	ADI_API_RETURN(device);
}

int32_t adrv9001_Radio_PfirWbNbCompSymmetric_Set(adi_adrv9001_Device_t *device, 
	                                             adi_adrv9001_PfirBank_e bankSel, 
	                                             adi_adrv9001_PfirSymmetric_e symmetric,
	                                             adi_common_Port_e port)
{
	ADI_PERFORM_VALIDATION(adrv9001_Radio_PfirWbNbCompSymmetric_Set_Validate, device, bankSel, symmetric, port);

	if (bankSel == ADI_ADRV9001_PFIR_BANK_A)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankASymmetric_Set, device, (uint8_t)symmetric);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankAPfirSymmetric_Set, device, (uint8_t)symmetric);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_B)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankBSymmetric_Set, device, (uint8_t)symmetric);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankBPfirSymmetric_Set, device, (uint8_t)symmetric);
		}
	}
	else if (bankSel == ADI_ADRV9001_PFIR_BANK_C)
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankCSymmetric_Set, device, (uint8_t)symmetric);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankCPfirSymmetric_Set, device, (uint8_t)symmetric);
		}
	}
	else
	{
		if (port == ADI_RX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_RxBankDSymmetric_Set, device, (uint8_t)symmetric);
		}
		else if (port == ADI_TX)
		{
			ADI_EXPECT(adrv9001_NvsRegmapCore_TxBankDPfirSymmetric_Set, device, (uint8_t)symmetric);
		}
	}

	ADI_API_RETURN(device);
}

