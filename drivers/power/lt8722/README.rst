LT8722 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`LT8722 <https://www.analog.com/LT8722>`_

Overview
--------

The LT8722 is a high performance, high efficiency, monolithic full bridge DC/DC
converter. One side of the full bridge is driven by a pulse width modulation
(PWM) buck power stage, while the other side of the full bridge is driven by
a linear power stage. The LT8722 can deliver up to ±54W of power to its load
while only requiring a single inductor. An integrated 25-bit digital-to-analog
converter (DAC) is used to control the LT8722 output voltage. Two additional
9-bit DACs control the positive and negative output current limits. An analog
output telemetry pin can be used to monitor SPI selectable parameters such as
VIN, VOUT, IOUT or the LT8722 junction temperature. The serial peripheral
interface (SPI) can be used to configure and control the LT8722 allowing for
flexibility to set the desired output voltage, output current limits, voltage
limits, switching frequency and control ON/OFF behavior. The SPI operates at up
to 10MHz allowing for fast readback and control. The LT8722 operates from a
single 3.1V to 15V supply. Silent Switcher techniques are used to minimize
EMI/EMC emissions while delivering high efficiency at high switching
frequencies. The LT8722 is available in a 3mm × 3mm LQFN package.

Applications
------------

LT8722
-------

* Driving a Thermo Electric Cooler (TEC) with Fine Control
* Transmit Optical Sub-Assembly (TOSA) Cooling
* Erbium Doped Fiber Amplifier (EDFA) Temperature Regulation
* Photonic Integrated Circuit (PIC) Cooling
* LiDAR Mirror Control
* Motor Control

LT8722 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI) alongside and for SWEN and EN GPIO pins.

The first API to be called is **lt8722_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter, which contains a boolean variable start_up_sequence, which if set
to true will do the start-up sequence of the device during initialization.

Setting the Switching Frequency
-------------------------------

The LT8722 uses a constant frequency PWM architecture that can be programmed
to switch from 500kHz to 3MHz through the SW_FRQ_SET register bits and further
be adjusted by ±15% through the SW_FRQ_ADJ register bits.

The switching frequency can be set using **lt8722_set_sw_frq_set** API and
the switching frequency can be adjusted using **lt8722_set_sw_frq_adj** API.

VCC LDO Regulator
-----------------

An internal low dropout (LDO) regulator produces a 3.4V supply to the VCC pin
from VIN when the VCC_VREG register bit is 1. To improve overall efficiency,
an external supply between 3.4V to 3.8V can be applied to the VCC pin.
When an external supply is used, the VCC_VREG register bit needs to be
configured to 0. With this setting, the VCC LDO’s regulation voltage will be
reduced to 3.1V. The VCC pin can then be overdriven with an external supply
between 3.4V and 3.8V.

The VCC LDO regulator output voltage can be set using **lt8722_set_vcc_vreg**
API.

Setting the Initial Peak Inductor Current
-----------------------------------------

When the PWM driver is enabled, the initial peak inductor current can cause
some transient behavior to the output voltage and current for a short period
of time. The optimal initial peak inductor current is different for different
VIN, switching frequency and inductor values.

The initial peak inductor current can be set using **lt8722_set_vc_int** API.

LDR Driver Internal Power Mitigation
------------------------------------

In some conditions, the power dissipation of the LDR driver can be quite high.
The LT8722 integrates power dissipation feedback loops to limit the maximum
power dissipation of the LDR driver's top (M1) and bottom (M2) power devices.

The LDR driver internal power mitigation can be set using **lt8722_set_pwr_lim**
API.

Setting the Output Voltage
--------------------------

The output voltage can be set using **lt8722_set_output_voltage** API.

PWM Duty Cycle Configuration
----------------------------

The PWM duty cycle can be set using **lt8722_set_sys_dc** API.

Maximum/Minimum TEC Voltage Limits
----------------------------------

The maximum positive and negative TEC voltages are set in the SPIS_OV_CLAMP
and SPIS_UV_CLAMP registers respectively. These two registers set the maximum
and minimum SPIS_DAC register values and, in turn, set the maximum positive
and negative TEC voltages.

The maximum/minimum TEC voltage limits can be set using
**lt8722_set_spis_ov_clamp** and **lt8722_set_spis_uv_clamp** APIs respectively.

Positive/Negative Output Current Limits
---------------------------------------

To protect the load, the LT8722 integrates two 9-bit DACs to limit the maximum
output currents in both directions independently. Positive current refers to
current flowing from LDR to SFB. The current limits can be set in the
SPIS_DAC_ILIMP and SPIS_DAC_ILIMN registers.

The positive/negative output current limits can be set using
**lt8722_set_spis_dac_ilimp** and **lt8722_set_spis_dac_ilimn** APIs
respectively.

Status Monitoring
-----------------

The LT8722 has a status word that can be read using **lt8722_get_status** API.

Analog Monitoring
-----------------

Several analog signals can be monitored through the AOUT pin. The signal
selection is made in the SPIS_AMUX register. When AOUT_EN = 0, the AOUT pin is
tri-stated. The AMUX_TEST bits can be used to confirm the AOUT signal integrity
by changing the AOUT pin voltage by a pre-defined amount for the selected
signal.

Analog monitoring can be enabled using **lt8722_set_aout_en** API. The signal
selection can be set using **lt8722_set_amux** API.

Software Reset
--------------

Software Reset operation is available through **lt8722_reset** API.

Shutdown Mode
-------------

Shutdown Mode operation is available through **lt8722_shutdown** API.


LT8722 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct lt8722_dev *lt8722_dev;
	struct no_os_uart_init_param lt8722_uart_ip = {
		.device_id = 1,
		.baud_rate = 57600,
		.size = NO_OS_UART_CS_8,
		.platform_ops = &max_uart_ops,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &lt8722_uart_extra,
	};
	struct no_os_spi_spi_param lt8722_spi_ip = {
		.device_id = 1,
		.extra = &lt8722_spi_extra,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0
	};
	struct no_os_gpio_init_param lt8722_swen_ip = {
		.port = 0,
		.pull = NO_OS_PULL_NONE,
		.number = 21,
		.platform_ops = &max_gpio_ops,
		.extra = &lt8722_gpio_extra
	};
	struct no_os_gpio_init_param lt8722_en_ip = {
		.port = 0,
		.pull = NO_OS_PULL_NONE,
		.number = 20,
		.platform_ops = &max_gpio_ops,
		.extra = &lt8722_gpio_extra
	};
	struct lt8722_init_param lt8722_ip = {
		.spi = &lt8722_spi_ip,
		.gpio_swen = &lt8722_swen_ip,
		.gpio_en = &lt8722_en_ip,
		.start_up_sequence = true
	};
	ret = lt8722_init(&lt8722_dev, &lt8722_ip);
	if (ret)
		goto error;

LT8722 no-OS IIO support
-------------------------

The LT8722 IIO driver comes on top of the LT8722 driver and offers support
for interfacing IIO clients through libiio.

LT8722 IIO Device Configuration
--------------------------------

Output Channel Attributes
-------------------------

LT8722 IIO device has a single output channel (voltage_0) for the output
voltage. The output channel has a total of 5 channel attributes:

* ``output - the output voltage value in millivolts``
* ``min - the minimum output voltage value in millivolts``
* ``min_available - the list of available minimum output voltage for the output channel``
* ``max - the maximum output voltage value in millivolts``
* ``max_available - the list of available maximum output voltage for the output channel``

Global Attributes
-----------------

The device has a total of 31 global attributes:

* ``enable_req - ENABLE_REQ state of the device``
* ``enable_req_available - ENABLE_REQ available states of the device``
* ``swen_req - SWEN_REQ state of the device``
* ``swen_req_available - SWEN_REQ available states of the device``
* ``sw_frq_set - SW_FRQ_SET state of the device``
* ``sw_frq_set_available - SW_FRQ_SET available states of the device``
* ``sw_frq_adj - SW_FRQ_ADJ state of the device``
* ``sw_frq_adj_available - SW_FRQ_ADJ available states of the device``
* ``sys_dc - SYS_DC state of the device``
* ``sys_dc_available - SYS_DC available states of the device``
* ``vcc_vreg - VCC_VREG state of the device``
* ``vcc_vreg_available - VCC_VREG available states of the device``
* ``sw_vc_int - SW_VC_INT state of the device``
* ``sw_vc_int_available - SW_VC_INT available states of the device``
* ``spi_rst - SPI_RST state of the device``
* ``spi_rst_available - SPI_RST available states of the device``
* ``pwr_lim - PWR_LIM state of the device``
* ``pwr_lim_available - PWR_LIM available states of the device``
* ``spis_dac - SPIS_DAC state of the device``
* ``spis_dac_ilimn - SPIS_DAC_ILIMN state of the device``
* ``spis_dac_ilimp - SPIS_DAC_ILIMP state of the device``
* ``amux - AMUX state of the device``
* ``amux_available - AMUX available states of the device``
* ``amux_test - AMUX_TEST state of the device``
* ``amux_test_available - AMUX_TEST available states of the device``
* ``aout_en - AOUT_EN state of the device``
* ``aout_en_available - AOUT_EN available states of the device``
* ``en_pin - EN_PIN state of the device``
* ``en_pin_available - EN_PIN available states of the device``
* ``swen_pin - SWEN_PIN state of the device``
* ``swen_pin_available - SWEN_PIN available states of the device``

Debug Attributes
----------------

* ``swen - indicates whether the PWM is switching``
* ``swen_available - SWEN available states of the device``
* ``srvo_ilim - indicates whether the output current limit state of the device is active``
* ``srvo_ilim_available - SRVO_ILIM available states of the device``
* ``srvo_plim - indicates whether the linear regulator power dissapation limit is active``
* ``srvo_plim_available - SRVO_PLIM available states of the device``
* ``min_ot - indicates whether the PWM switching is limited by the minimum on-time of the device or minimum off-time of the device``
* ``min_ot_available - MIN_OT available states of the device``
* ``por_occ - indicates whether the a reset has happened since last cleared``
* ``por_occ_available - POR_OCC available states of the device``
* ``over_current - indicates whether the linear driver over-current fault happened since last cleared``
* ``over_current_available - OVER_CURRENT available states of the device``
* ``tsd - indicates whether the over-temperature fault happened since last cleared``
* ``tsd_available - TSD available states of the device``
* ``vcc_uvlo - indicates whether the VCC regulator under-voltage lockout happened since last cleared``
* ``vcc_uvlo_available - VCC_UVLO available states of the device``
* ``vddio_uvlo - indicates whether the VDDIO regulator under-voltage lockout happened since last cleared``
* ``vddio_uvlo_available - VDDIO_UVLO available states of the device``
* ``cp_uvlo - indicates whether the charge pump under-voltage lockout happened since last cleared``
* ``cp_uvlo_available - CP_UVLO available states of the device``
* ``v2p5_uvlo - indicates whether the 2.5V reference under-voltage lockout happened since last cleared``
* ``v2p5_uvlo_available - V2P5_UVLO available states of the device``

LT8722 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct lt8722_iio_dev *lt8722_iio_dev;
	struct lt8722_iio_dev_init_param lt8722_iio_ip = {
		.lt8722_init_param = &lt8722_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt8722_iio_init(&lt8722_iio_dev, &lt8722_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt8722",
			.dev = lt8722_iio_dev,
			.dev_descriptor = lt8722_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt8722_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_lt8722;

	return iio_app_run(app);
