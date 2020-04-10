/*
 * spi.c
 *
 *  Created on: Apr 10, 2020
 *      Author: amiclaus
 */
#include <inttypes.h>
#include "spi.h"
#include <stdlib.h>
#include "error.h"

int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param){
	(*desc)->dev = calloc(1, sizeof(struct spi_dev));
	if(!((*desc)->dev))
		return FAILURE;

	(*desc)->dev->spi_dev_init = param->dev.spi_dev_init;
	(*desc)->dev->spi_dev_remove = param->dev.spi_dev_remove;
	(*desc)->dev->spi_dev_write_and_read = param->dev.spi_dev_write_and_read;

	return ((*desc)->dev->spi_dev_init(desc, param));
}


/* Free the resources allocated by spi_init(). */
int32_t spi_remove(struct spi_desc *desc){
	if(desc->dev->spi_dev_remove(desc))
		return FAILURE;

	free(desc->dev);

	return SUCCESS;
}

/* Write and read data to/from SPI. */
int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint16_t bytes_number){
	return (desc->dev->spi_dev_write_and_read(desc, data, bytes_number));
}
