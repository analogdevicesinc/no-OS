#include <stdlib.h>
#include <errno.h>
#include "no-os/spi.h"
#include "spi.h"

struct spi_platform_ops maxim_spi_ops;

int32_t spi_init(struct spi_desc **desc, const struct spi_init_param *param)
{
	if(!param)
		return -EINVAL;

	struct spi_desc *descriptor = calloc(1, sizeof(*descriptor));
	if(!descriptor)
		return -ENOMEM;

	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->bit_order = param->bit_order;
	descriptor->platform_ops = &maxim_spi_ops;
	descriptor->extra = param->extra;

	if(SPI_Init(SPI0A, descriptor->mode, param->max_speed_hz) != 0)
		return -1;

	*desc = descriptor;		

	return 0;
}

int32_t spi_remove(struct spi_desc *desc)
{	
	free(desc);
	
	return 0;
}

int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint16_t bytes_number)
{	
	if(!desc || !data)
		return -EINVAL;
	
	uint8_t *tx_buffer = *data;
	uint8_t *rx_buffer = *data;
	spi_req_t req;
	
	req.bits = 8;
	req.ssel = 0;
	req.tx_data = *tx_buffer;
	req.tx_num = 0;
	req.rx_data = *rx_buffer;
	req.rx_num = 0;	
	req.deass = 1;
	req.len = bytes_number;	
	
	SPI_MasterTrans(SPI0A, &req);	

	return 0;
}

int32_t spi_transfer(struct spi_desc *desc, struct spi_msg *msgs, uint32_t len)
{
	if(!desc || !msgs)
		return -EINVAL;
	
	spi_req_t req;
	
	req.bits = 8;
	req.ssel = 0;

	for(uint32_t i = 0; i < len; i++){
		req.tx_data = msgs[i].tx_buff;
		req.tx_num = 0;
		req.deass = msgs[i].cs_change;
		req.len = msgs[i].bytes_number;	
		SPI_MasterTrans(SPI0A, &req);
	}

	return 0;
}

struct spi_platform_ops maxim_spi_ops = {
	.init = &spi_init,
	.write_and_read = &spi_write_and_read,
	.transfer = &spi_transfer,
	.remove = &spi_remove
};

