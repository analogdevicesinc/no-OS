#include "basic_example.h"
#include "common_data.h"
#include "cn0391.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

int basic_example_main(void)
{
	struct cn0391_dev *dev;
	double hot_temp, cold_temp, tc_voltage, rtd_res;
	int ret;

	ret = cn0391_init(&dev, &cn0391_ip);
	if (ret) {
		pr_info("CN0391 init failed: %d\n", ret);
		return ret;
	}

	pr_info("CN0391 Thermocouple Measurement\n");

	while (1) {
		ret = cn0391_read_temperature(dev, CN0391_CH3_ID, &hot_temp, &cold_temp,
					      &tc_voltage, &rtd_res);
		if (ret) {
			pr_info("Temperature read failed: %d\n", ret);
		} else {
			printf("Hot Junction:  %.2f C\n", hot_temp);
			printf("Cold Junction: %.2f C\n", cold_temp);
			printf("TC Voltage:    %.4f mV\n", tc_voltage);
			printf("RTD Resistance: %.1f Ohms\n", rtd_res);
			printf("---\n");
		}

		no_os_mdelay(5000);
	}

	cn0391_remove(dev);

	return 0;
}
