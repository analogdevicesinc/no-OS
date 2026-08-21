
#include <common_data.h>
#include <ad5933.h>
#include <iio_ad5933.h>
#include <ad5933.h>
#include <iio_app.h>
#include <no_os_util.h>
#include <no_os_alloc.h>

#define START_FREQ 10000U
#define FREQ_INC 100U
#define FREQ_POINTS 150U
#define SETTLING_CYCLES 15U
#define CALIBRATION_RESISTOR_OHM 500U

static struct ad5933_iio_dev_init_param ad5933_iio_ip = {
	.ad5933_dev_ip = &ad5933_user_init,
	.start_freq = START_FREQ,
	.freq_increment = FREQ_INC,
	.freq_points = FREQ_POINTS,
	.calibration_impedance = CALIBRATION_RESISTOR_OHM,
};

int example_main()
{
	int ret;
	struct ad5933_iio_dev *ad5933_iio_desc;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ad5933_iio_init(&ad5933_iio_desc, &ad5933_iio_ip);


	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad5933",
			.dev = ad5933_iio_desc,
			.dev_descriptor = ad5933_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;
	app_init_param.arg = ad5933_iio_desc;

	ret = iio_app_init(&app, app_init_param);

	if (ret) {
		ad5933_iio_remove(ad5933_iio_desc);
		return ret;
	}


	return iio_app_run(app);
}