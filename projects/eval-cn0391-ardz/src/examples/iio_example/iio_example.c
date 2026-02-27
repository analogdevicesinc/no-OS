#include "iio_example.h"
#include "common_data.h"
#include "iio_ad7124.h"
#include "iio_cn0391.h"
#include "iio_app.h"
#include "no_os_util.h"
#include "no_os_print_log.h"

int iio_example_main(void)
{
	struct cn0391_dev *dev;
	struct iio_app_init_param app_init_param = {0};
	struct iio_app_desc *app;
	int ret;

	ret = cn0391_init(&dev, &cn0391_ip);
	if (ret) {
		pr_info("CN0391 init failed: %d\n", ret);
		return ret;
	}

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("cn0391", dev,
			       &iio_cn0391_device, NULL, NULL, NULL),
		IIO_APP_DEVICE("ad7124-8", dev->ad7124_dev,
			       &iio_ad7124_device, NULL, NULL, NULL),
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = cn0391_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret) {
		pr_info("IIO app init failed: %d\n", ret);
		cn0391_remove(dev);
		return ret;
	}

	ret = iio_app_run(app);
	if (ret)
		pr_info("IIO app run failed: %d\n", ret);

	iio_app_remove(app);
	cn0391_remove(dev);

	return ret;
}
