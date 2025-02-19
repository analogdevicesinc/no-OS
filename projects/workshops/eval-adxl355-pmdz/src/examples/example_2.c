
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#include <stdio.h>
#include <string.h>


int main()
{
	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

	struct no_os_uart_desc *uart_desc;
	struct adxl355_dev *adxl355_desc;
	struct adxl355_frac_repr temp;
	uint16_t raw_temp;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;
	no_os_uart_stdio(uart_desc);

	ret = adxl355_init(&adxl355_desc, adxl355_ip);
	if (ret < 0)
		goto error_1;
	ret = adxl355_soft_reset(adxl355_desc);
	if (ret)
		goto error_2;
	ret = adxl355_set_odr_lpf(adxl355_desc, ADXL355_ODR_3_906HZ); 
	if (ret)
		goto error_2;
	ret = adxl355_set_op_mode(adxl355_desc, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret < 0)
		goto error_2;

	while(1) {		
		ret = adxl355_get_temp(adxl355_desc, &temp);
		if (ret < 0)
			goto error_2;

		char buff[100];
		int n;
		sprintf(buff, "Current temperature is %d"".%09u millidegrees Celsius\n\r",
			(int)temp.integer, (abs)(temp.fractional));
		n = strlen(buff) + 1;

		ret = no_os_uart_write(uart_desc, buff, n);
		if (ret < 0)
			goto error;
		no_os_mdelay(1000);
	}

	return 0;

error_2:
	pr_info("2Error2\n");
	adxl355_remove(adxl355_desc);
error_1:
	pr_info("1Error1\n");
	no_os_uart_remove(uart_desc);
error:
	no_os_uart_remove(uart_desc);
}
