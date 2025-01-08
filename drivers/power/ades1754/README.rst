ADES1754 no-OS driver
=====================

Supported Devices
-----------------

`ADES1754 <https://www.analog.com/ADES1754>`_

Overview
--------

The ADES1754/ADES1755/ADES1756 are flexible
data-acquisition systems for the management of high-
voltage and low-voltage battery modules. The systems
can measure 14 cell voltages and a combination of six
temperatures or system voltage measurements with fully
redundant measurement engines in 162μs, or perform
all inputs solely with the ADC measurement engine in
99μs. Fourteen internal balancing switches rated
for >300mA for cell-balancing current support extensive
built-in diagnostics.

Cell and bus-bar voltages ranging from -2.5V to +5V are
measured differentially over a 65V common-mode
range, with a typical accuracy of 100μV. If oversampling
is enabled, up to 128 measurements per channel can be
averaged internally with 14-bit resolution and combined
with digital post-processing IIR filtering for increased
noise immunity.

Applications
------------

* Residential Battery Storage Systems
* High-Voltage Battery Stacks
* Battery-Backup Systems (UPS)
* Super-Cap Systems
* Battery-Powered Tools
* EV Charging

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide support
for the communication protocol (UART) which can be done either using the
host UNIT UART peripheral, or by using MAX17851 SPI-to-UART bridge as
an UART peripheral (see MAX17851 device driver and documentation for it).

The first API to be called is **ades1754_init**. Make sure it return 0,
which means that the driver was intialized correctly.

Scan Configuration
------------------

One can set the ADC scaning method by calling **ades1754_set_adc_method**
function, or switching the used scan mode by calling the
**ades1754_switch_scan_mode** function.

More than this the user can trigger the scanning, or stop it by using
**ades1754_start_scan** function.

Cell Configuration
------------------

Cell polarity can be changed by using the **ades1754_set_cell_poll**
function.

Cell voltage can be read back from the accumulator registers after or before
triggering a scan by calling **ades1754_get_cell_data** function.

IIR Configuration
-----------------

ADES1754 also comes along with an integrated IIR filter to be used for
acquisition.

IIR filter coefficient can be set with **ades1754_set_iir** function.
IIR settings such as Alert Issuance to be based on IIR filter results,
IIR data to be stored inside the accumulators or output data to
take or not the IIR data in consideration can be set by using
**ades175_set_iir_ctrl** function.

Buffer Configuration
--------------------

Buffer can be set to be normall sized, or double sized based on the
**ades1754_set_buffer_mode** function that sets the mode.

Alert Configration
------------------

User can set the alert threshold values by using **ades1754_set_alert_thr**
function, and read alert status of specific alerts by suing
**ades1754_get_alert** function.

Cell-Balancing Configuration
----------------------------

Cell-Balancing mode is to be set by using **ades1754_set_balancing_mode**
function.
Cell-Balancing measurement is to be set by using **ades1754_set_balancing_meas**
function.
Cell-Balancing calibration is to be performed by using
**ades1754_set_balancing_calib** function.

ADES1754 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct ades1754_desc *ades1754_desc;
	struct no_os_uart_init_param ades1754_comm_ip = {
		.device_id = 1,
		.baud_rate = 500000,
		.asynchronous_rx = false,
		.size = NO_OS_UART_CS_8,
		.platform_ops = &max_uart_ops,
		.parity = NO_OS_UART_PAR_ODD,
		.stop = NO_OS_UART_STOP_2_BIT,
		.extra = &ades1754_comm_extra,
	};
	struct ades1754_init_param ades1754_ip = {
		.uart_param = &ades1754_comm_ip,
		.id = ID_ADES1754,
		.uart_bridge = true,
		.dev_addr = 0,
		.no_dev = 1,
	};

	ret = ades1754_init(&ades1754_desc, &ades1754_ip);
	if (ret)
		return ret;

	ret = ades1754_set_cell_polarity(ades1754, ADES1754_UNIPOLAR);
	if (ret)
		goto error;
