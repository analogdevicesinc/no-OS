/***************************************************************************//**
 *   @file   filter.c
 *   @brief  Implementation of Filter Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <matio.h>
#include "filter.h"
#include "../ad9361_api.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
char rx_fields[11][16] = {"BBPLL", "ADC", "R2", "R1",
						  "RF", "RXSAMP", "Coefficient", "CoefficientSize",
						  "Decimation", "Gain", "RFBandwidth"};
char tx_fields[11][16] = {"BBPLL", "DAC", "T2", "T1",
						  "TF", "TXSAMP", "Coefficient", "CoefficientSize",
						  "Interp", "Gain", "RFBandwidth"};

/***************************************************************************//**
 * @brief parse_mat_fir_file
*******************************************************************************/
int32_t parse_mat_fir_file(char *mat_fir_filename,
						   uint8_t *fir_type,
						   FIRConfig *fir_config,
						   uint32_t *bandwidth,
						   uint32_t *path_clks)
{
	mat_t *matfp;
	matvar_t *matvar;
	matvar_t *struct_field;
	char (*fields)[16];
	uint8_t field_index;
	uint8_t coeff_index;
	int32_t ret = 0;

	matfp = Mat_Open(mat_fir_filename, MAT_ACC_RDONLY);
	if(matfp == NULL)
	{
		printf("Error opening MAT file \"%s\".\n", mat_fir_filename);
		return -1;
	}
	else
	{
		matvar = Mat_VarRead(matfp,"tohwrx");
		if(matvar == NULL)
		{
			matvar = Mat_VarRead(matfp,"tohwtx");
			if(matvar != NULL)
			{
				fields = tx_fields;
				*fir_type = 1;
			}
			else
			{
				printf("Invalid MAT file.\n");
				return -1;
			}
		}
		else
		{
			fields = rx_fields;
			*fir_type = 0;
		}
		for(field_index = 0; field_index < 11; field_index++)
		{
			struct_field = Mat_VarGetStructField(matvar, fields[field_index],
												 1, 0);
			switch (field_index)
			{
			case 0 ... 5:
				path_clks[field_index] = *(double*) struct_field->data;
				break;
			case 6:
				for(coeff_index = 0; coeff_index < 128; coeff_index++)
				{
					fir_config->coef[coeff_index] =
									(int16_t)(*(double*) (struct_field->data +
											  coeff_index * sizeof(double)));
				}
				break;
			case 7:
				fir_config->coef_size = 
								(uint8_t)(*(double*) struct_field->data);

				break;
			case 8:
				fir_config->dec_int =
								(uint32_t)(*(double*) struct_field->data);
				break;
			case 9:
				fir_config->gain = (int32_t)(*(double*) struct_field->data);

				break;
			case 10:
				*bandwidth = (uint32_t)(*(double*) struct_field->data);
				break;
			default:
				ret = -1;
				goto error;
			}
		}
	}

	fir_config->ch = 3;

error:
	Mat_VarFree(matvar);
	Mat_Close(matfp);

	return ret;
}

/***************************************************************************//**
 * @brief load_fir_files
*******************************************************************************/
int32_t load_enable_fir_files(struct ad9361_rf_phy *phy,
							  char *tx_mat_fir_filename,
							  char *rx_mat_fir_filename)
{
#ifdef DEBUG
	uint8_t index;
#endif
	uint8_t fir_type;
	uint32_t tx_bandwidth;
	uint32_t rx_bandwidth;
	uint32_t tx_path_clks[6];
	uint32_t rx_path_clks[6];
	AD9361_RXFIRConfig rx_fir;
	AD9361_TXFIRConfig tx_fir;

	parse_mat_fir_file(tx_mat_fir_filename, &fir_type, (FIRConfig*)&tx_fir, &tx_bandwidth, tx_path_clks);

	if (!fir_type) {
		printf("Invalid TX filter configuration.\n");
		return -1;
	}
#ifdef DEBUG
	printf("channels_selection: %d\n", tx_fir.tx);

	printf("gain = %d\n", tx_fir.tx_gain);

	printf("int = %d\n", tx_fir.tx_int);

	for (index = 0; index < tx_fir.tx_coef_size; index ++) {
		if (index == 0)
			printf("coeff = ");
		printf("%d", tx_fir.tx_coef[index]);
		if (index == (tx_fir.tx_coef_size - 1))
			printf("\n");
		else
			printf(", ");
	}

	printf("coeff_size = %d\n", tx_fir.tx_coef_size);

	printf("bandwidth = %d\n", tx_bandwidth);

	for (index = 0; index < 6; index ++) {
		if (index == 0)
			printf("path_clks = ");
		printf("%d", tx_path_clks[index]);
		if (index == 5)
			printf("\n");
		else
			printf(", ");
	}
#endif
	parse_mat_fir_file(rx_mat_fir_filename, &fir_type, (FIRConfig *)&rx_fir, &rx_bandwidth, rx_path_clks);

	if (fir_type) {
		printf("Invalid RX filter configuration.\n");
		return -1;
	}
#ifdef DEBUG
	printf("channels_selection: %d\n", rx_fir.rx);

	printf("gain = %d\n", rx_fir.rx_gain);

	printf("dec = %d\n", rx_fir.rx_dec);

	for (index = 0; index < rx_fir.rx_coef_size; index ++) {
		if (index == 0)
			printf("coeff = ");
		printf("%d", rx_fir.rx_coef[index]);
		if (index == (rx_fir.rx_coef_size - 1))
			printf("\n");
		else
			printf(", ");
	}

	printf("coeff_size = %d\n", rx_fir.rx_coef_size);

	printf("bandwidth = %d\n", rx_bandwidth);

	for (index = 0; index < 6; index ++) {
		if (index == 0)
			printf("path_clks = ");
		printf("%d", rx_path_clks[index]);
		if (index == 5)
			printf("\n");
		else
			printf(", ");
	}
#endif
	ad9361_set_tx_fir_config(phy, tx_fir);
	ad9361_set_rx_fir_config(phy, rx_fir);
	ad9361_set_tx_fir_en_dis(phy, 1);
	ad9361_set_rx_fir_en_dis(phy, 1);
	ad9361_set_tx_rf_bandwidth(phy, tx_bandwidth);
	ad9361_set_rx_rf_bandwidth(phy, rx_bandwidth);
	ad9361_set_trx_path_clks(phy, rx_path_clks, tx_path_clks);

	return 0;
}
