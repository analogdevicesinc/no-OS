
#ifndef FTD2XX_SPI_H_
#define FTD2XX_SPI_H_

#include "no_os_spi.h"

#include "ftd2xx.h"
#include "libmpsse_spi.h"

struct ftd2xx_spi_init {
	uint32_t channel_config_pin;
};

struct ftd2xx_spi_desc {
	FT_HANDLE ftHandle;
};

extern const struct no_os_spi_platform_ops ftd2xx_spi_ops;

#endif
