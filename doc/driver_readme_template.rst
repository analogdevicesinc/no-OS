.. Driver documentation template for no-OS device drivers.
..
.. HOW TO USE THIS TEMPLATE
.. ------------------------
.. 1. Copy this file to ``drivers/<category>/<part>/README.rst``
..    (e.g. ``drivers/temperature/max31827/README.rst``).
.. 2. Replace every ``<...>`` placeholder and delete the guidance comments
..    (lines starting with ``..``) as you fill each section in.
.. 3. Keep the section order and heading underline styles below. RST underline
..    characters used here, in order of depth:
..       ``=`` title       (level 1)
..       ``-`` section      (level 2)
..       ``~`` subsection   (level 3)
..       ``^`` sub-sub      (level 4)
..    Underlines must be at least as long as the text above them.
.. 4. A stub at ``doc/sphinx/source/drivers/<category>/<part>.rst`` must
..    ``.. include::`` this README so Sphinx renders it. Create it if missing:
..       .. include:: ../../../../../drivers/<category>/<part>/README.rst
..    The category's glob toctree in ``doc/sphinx/source/drivers_doc.rst``
..    then picks it up automatically.
.. 5. Drop the IIO sections if the driver has no IIO layer. Keep everything
..    else; prefer completeness.

<PART> no-OS driver
===================
.. Underline the title with ``=`` for the full width of the text above.

.. no-os-doxygen::

Supported Devices
-----------------

.. One entry per device this driver supports. Prefer the ``:adi:`` role,
.. which links to the ADI product page, e.g. ``:adi:`MAX31827```.

- :adi:`<PART>`

Overview
--------

.. Two or three short paragraphs describing the device: what it is, key
.. specs (resolution, channels, interfaces, supply rails), notable on-chip
.. features, and how it is typically used. Pull the numbers from the data
.. sheet; do not invent specs.

<Describe the device: function, key electrical characteristics, interface,
and primary use.>

Applications
------------

.. Bullet list of typical end applications, usually lifted from the data
.. sheet's "Applications" section.

* <Application 1>
* <Application 2>
* <Application 3>

.. Optional: include an Operation Modes table when the device is
.. software-configurable into distinct modes. Use an RST grid table:
..
.. Operation Modes
.. ---------------
..
.. +-----------+-----------------+---------------------+------------------+
.. | Mode Name | Description     | Configuration Bits  | Typical Use Case |
.. +===========+=================+=====================+==================+
.. | <MODE>    | <what it does>  | <bits / mask>       | <when to use it> |
.. +-----------+-----------------+---------------------+------------------+

<PART> Device Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

.. Describe the communication protocol the caller must provide (SPI/I2C),
.. the first API to call (``<part>_init``), what it returns on success, and
.. any helper it calls internally to set up registers.

In order to be able to use the device, you will have to provide the support
for the communication protocol (<SPI/I2C>).

The first API to be called is **<part>_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

.. Add one ``~~~`` subsection per functional group of the driver, e.g.
.. register access, measurement/conversion, channel configuration, GPIO,
.. diagnostics. Name each after what it does and reference the relevant
.. **<part>_<api>** functions.

<Feature Group 1>
~~~~~~~~~~~~~~~~~

<Describe the feature and the **<part>_<api>** functions that implement it.>

<PART> Driver Initialization Example
------------------------------------

.. A minimal, copy-pasteable example that initializes the device. Show the
.. UART/SPI/I2C init params and the ``<part>_init`` call. Match the style of
.. existing driver READMEs.

.. code-block:: bash

	struct <part>_desc *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	struct <part>_init_param <part>_ip = {
		/* fill in the device init parameters */
	};

	ret = <part>_init(&dev, &<part>_ip);
	if (ret)
		goto error;

.. Keep the following IIO sections only if the driver has an IIO layer
.. (``drivers/.../iio_<part>.c``). Otherwise delete everything below.

<PART> no-OS IIO support
------------------------

The <PART> IIO driver comes on top of the <PART> driver and offers support
for interfacing IIO clients through libiio.

<PART> IIO Device Configuration
-------------------------------

.. One ``~~~`` subsection per attribute group the IIO driver exposes
.. (e.g. Channel Attributes, Global Attributes, Debug Attributes). List
.. each attribute as an inline-literal bullet with a short description.

<Attribute Group>
~~~~~~~~~~~~~~~~~

* ``<attribute_name> - <what it does>``

<PART> IIO Driver Initialization Example
----------------------------------------

.. code-block:: bash

	int ret;

	struct <part>_iio_device *<part>_iio_dev;
	struct <part>_iio_init_param <part>_iio_ip = {
		.<part>_init_param = &<part>_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = <part>_iio_init(&<part>_iio_dev, &<part>_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "<part>",
			.dev = <part>_iio_dev,
			.dev_descriptor = <part>_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_<part>;

	return iio_app_run(app);
