# Prerequisites
Prior to building the project, a couple steps are necessary in order to get the Maxim Micros SDK and setup the environment. These are presented in the ***Build Prerequisites*** section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build .

The MaximSDK provides distributions of `arm-none-eabi-` GCC compiler + utilities and `OpenOCD`, so you don't have to install these separately.

# Overview

Currently, this project offers 2 firmware examples meant to run on the AD-SWIOT1L-SL board:
	- The first one is based on the IIO framework. It may be used in combination with Scopy (+ the SWIOT1L plugin) or any other IIO client (such as pyadi-iio). This will be referred to as "SWIOT1L default firmware".
	- The other one showcases how you may send data over MQTT to a remote broker connected over the 10Base-T1L port. This will be referred to as "SWIOT1L MQTT firmware".

# Building the SWIOT1L default firmware

This is only intended to be run on the MAX32650 target.

1. Open a terminal and navigate to this project directory (if building on Windows, `Git Bash` has to be used). Please remember that switching between the firmware examples requires you to clean the previous build artifacts. You may do this by running `make reset`.
2.  Type `make RELEASE=y SWIOT1L_STATIC_IP=y -j`, in order to build the project. You may leave out the `SWIOT1L_STATIC_IP=y` flag if you want the IP to be assigned using DHCP. The `RELEASE` flag adds `-O2` optimization and is optional, but without it, the MCU won't be able to keep up with the AD74413R's sampling rate, and some samples will be lost. So, it should only be omitted during debugging.

A successful build should end with the following terminal output:
```
[11:11:27] [HEX] swiot1l.hex
[11:11:27] swiot1l.hex is ready
rm /home/xvr/MaximSDK_new/Libraries/CMSIS/Device/Maxim/MAX32650/Source/GCC/startup_max32650.s
[11:11:21] Done (build/swiot1l.elf)
```
# Building the SWIOT1L MQTT firmware

This is only intended to be run on the MAX32650 target.

1. Open a terminal and navigate to this project directory (if building on Windows, `Git Bash` has to be used). Please remember that switching between the firmware examples requires you to clean the previous build artifacts. You may do this by running `make reset`.
2.  Type `make RELEASE=y SWIOT1L_STATIC_IP=y SWIOT1L_MQTT_EXAMPLE=y SWIOT1L_DEFAULT_FW=n -j`, in order to build the project. The `RELEASE` flag adds `-O2` optimization and is optional, but without it, the MCU won't be able to keep up with the AD74413R's sampling rate, and some samples will be lost. So, it should only be omitted during debugging.

A successful build should end with the following terminal output:
```
[11:11:27] [HEX] swiot1l.hex
[11:11:27] swiot1l.hex is ready
rm /home/xvr/MaximSDK_new/Libraries/CMSIS/Device/Maxim/MAX32650/Source/GCC/startup_max32650.s
[11:11:21] Done (build/swiot1l.elf)
```

The binary and executable files are now available in the `build` directory (`swiot1l.hex` and `swiot1l.elf` files).
# Programming the MCU
Before the MCU can be programmed a few steps are necessary:
1.  Replace the DAPLINK firmware for the MAX32625PICO. This is only required to be done one time.
	- Download the firmware image from the following link: https://github.com/MaximIntegrated/max32625pico-firmware-images/raw/master/bin/max32625_max32650fthr_if_crc_swd_v1.0.6.bin .
	- Make sure the MAX32625PICO is not connected to the PC.
	- Press the button on the MAX32625PICO, and keep it pressed while you plug the USB cable in the MAX32625PICO.
	- Release the button once you can see a `MAINTENANCE` drive being mounted.
	- Copy the firmware binary file to the `MAINTANANCE` drive. It should unmount and a `DAPLINK` drive should appear instead.
2. Connect the MAX32625PICO board to the PC and the SWIOT1L board. If everything went well, you should see a mass storage device named `DAPLINK` in your filesystem.
3. Power on the SWIOT1L board, by providing 24V to the power connector.

The microcontroller may be programmed in 2 ways:
1. Drag-and-drop the binary (.hex) file in the `DAPLINK` directory. The drive should be unmounted and mounted again, once the programming is done.
2. While in the project's root directory, type `make RELEASE=y run` (you should keep the flags used to build the project). This method uses OpenOCD in order to load the binary file. If the programming is successful, you should see the following terminal output:
```
** Programming Started **
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
shutdown command invoked
[11:27:42] swiot1l.elf uploaded to board
```
# Network connection for the SWIOT1L default firmware

In order to configure and sample data from the SWIOT1L board, you'll have to connect it to a PC through the single pair Ethernet connector. There are a couple ways to do it:

1. Using a T1L -> Ethernet media converter:
- Direct connection to PC:
Build the firmware with the `SWIOT1L_STATIC_IP=y` flag, will result in using `192.168.97.40` as an IP address. This is the fastest way of testing the platform, since it doesn't require the DHCP configuration or DHCP timeout (in case of AutoIP).
Alternatively, you may omit `SWIOT1L_STATIC_IP=y` (or set it to `n`), in which case, the board will send a DHCP request and try to get an IP that way. If you don't have a DHCP server listening on the PC's network interface (to which SWIOT1L is connected), a link local address (in the 169.254.x.x\16 range) will be used. This may take a while (~10s) the first time you power the board, since there have to be 2 failed DHCP requests.

	In case a link local or static IP address is used for the AD-SWIOT1L-SL, the user may need to manually add a static IP for their PC's network interface which connects to the board. You may do this by going through the following guide https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602 (and choosing an IP in the same subnet as the AD-SWIOT1L-SL board: e.g 192.168.97.1/24).

```mermaid
graph LR
A[SWIOT1L's SPE connector] -- SPE --> B[T1L -> Ethernet media converter]
B -- Ethernet --> D[PC]
```
- Connection through a router:
In this case, you'll most likely have a DHCP server running on your router, which will assign an IP to your board.
 ```mermaid
graph LR
A[SWIOT1L's SPE connector] -- SPE --> B[T1L -> Ethernet media converter]
B -- Ethernet --> C[Router]
D[PC] -- Ethernet --> C[Router]
```
1. Using a T1L -> USB media converter:
 ```mermaid
graph LR
A[SWIOT1L's SPE connector] -- SPE --> B[T1L -> USB media converter]
B -- USB --> D[PC]
```

After the hardware connections are made, you'll have to power cycle the board.
In all these cases, once the TCP/IP software stack is fully configured and the board has an IP assigned to it (this will take ~10s if there is no DHCP server and the `SWIOT1L_STATIC_IP` flag is not set to `y`), the IP, network mask and gateway will be output to the serial interface (baudrate 115200 8N1).

Next, just to make sure that you can communicate with the board, you can try to ping the it using the IP address previously mentioned (either from command line or terminal).
In case the the ping resulted in a "destination unreachable" error, it may be the case that the PC's network interface is not correctly configure to route packets to the board's IP.

# Network connection for the SWIOT1L MQTT firmware

Directly connect the AD-SWIOT1L-SL board to your PC using a SPE -> Ethernet/USB media converter.

By default, the firmware will try to connect to the 192.168.97.1 IP, port 1883. So, you'll need to have an MQTT broker running on the host PC. The broker should listen for connections on an interface which has the 192.168.97.1 IP configured. The IP and port settings may be changed by setting different value to the `SWIOT1L_MQTT_SERVER_IP` and `SWIOT1L_MQTT_SERVER_PORT` make variables when compiling.

The MQTT protocol version used by the AD-SWIOT1L-SL is V3.1.1. The connection is not using TLS and the client does not use an username or password.

After the connection is established, the client will publish data on the following topics:
	- ad74413r/channel0: voltage values measured by the AD74413R's ADC on channel 0.
	- ad74413r/channel1: voltage values measured by the AD74413R's ADC on channel 1, while trying to output a maximum of 3.05 mA.
	- ad74413r/channel2: the value of a resistor connected to the AD74413R's channel 2.
	- ad74413r/channel3: current values measured by the AD74413R's ADC on channel 3, while trying to output a maximum of 4.028 V.

## Using the IIO interface
The firmware is based on a no-OS implementation of the IIO framework from the Linux kernel, which offers similar functionalities. Thus, the board may be configured through the use of pyadi-iio API.

**Note**: the code examples provided are for pyadi-iio, but any IIO client may be used instead to run the equivalent command sequence.

In order to get the pyadi-iio drivers follow the next steps:
1. Clone the pyadi-iio repository: `git clone https://github.com/analogdevicesinc/pyadi-iio` 
2. Follow the installation steps provided in the pyadi-iio's README.md file: https://github.com/analogdevicesinc/pyadi-iio.
3. Update your `PYTHONPATH` environment variable to include the pyadi-iio's root directory.
In Windows you may do this by entering the `Environment Variables` window, and adding (or updating in case you already have it) the `PYTHONPATH` system variable.
In Linux you may open a terminal and either type `export PYTHONPATH=$PYTHONPATH:path/to/pyiio` (which will only be valid in the current terminal) or update your `~/.bashrc` file with the same line. If you chose the second option, you'll also have to type `source ~/.bashrc` to execute it for the current terminal.
4.  Checkout the following branch https://github.com/analogdevicesinc/pyadi-iio/tree/swiot.

## SWIOT device configuration overview

This is a high  level overview of how the firmware manages device configuration. More details about field device usage are presented in the next section.
While the SWIOT1L board is powered up, the system may be in one of 2 states (marked by 2 different IIO contexts):

 1. **config**: all the channels are in a high impedance state, and the user may change their functions and the device to which each SWIOT1L channel is routed to (MAX14906 or AD74413R) using the `swiot` virtual device. It is important to note that when entering this state from the runtime mode, some of the device configurations will reset. The configuration selected in this context will help in generating the runtime IIO context.
The IIO context should look similar to this (displayed by `iio_info`):
```
Library version: 0.24 (git tag: c4498c2)
Compiled with backends: local xml ip usb serial
IIO context created with network backend.
Backend version: 1.1 (git tag: 0000000)
Backend description string: 169.254.97.40 no-OS swiot-rebase-lwip-eda9f736d-modified
IIO context has 2 attributes:
	ip,ip-addr: 169.254.97.40
	uri: ip:169.254.97.40
IIO context has 2 devices:
	iio:device0: swiot
		0 channels found:
		27 device-specific attributes found:
				attr  0: reset ERROR: No such file or directory (2)
				attr  1: serial_id value: 80a4-44044-1f003-2d8104ff-bf
				attr  2: mode value: config
				attr  3: mode_available value: config runtime
				attr  4: identify ERROR: No such file or directory (2)
				attr  5: ext_psu value: 1
				attr  6: signal_mse value: 32
				attr  7: ch0_enable value: 0
				attr  8: ch1_enable value: 0
				attr  9: ch2_enable value: 0
				attr 10: ch3_enable value: 0
				attr 11: ch0_function value: high_z
				attr 12: ch1_function value: high_z
				attr 13: ch2_function value: high_z
				attr 14: ch3_function value: high_z
				attr 15: ch0_device value: ad74413r
				attr 16: ch1_device value: ad74413r
				attr 17: ch2_device value: ad74413r
				attr 18: ch3_device value: ad74413r
				attr 19: ch0_function_available value: high_z voltage_out current_out voltage_in current_in_ext current_in_loop resistance digital_input digital_input_loop current_in_ext_hart current_in_loop_hart
				attr 20: ch1_function_available value: high_z voltage_out current_out voltage_in current_in_ext current_in_loop resistance digital_input digital_input_loop current_in_ext_hart current_in_loop_hart
				attr 21: ch2_function_available value: high_z voltage_out current_out voltage_in current_in_ext current_in_loop resistance digital_input digital_input_loop current_in_ext_hart current_in_loop_hart
				attr 22: ch3_function_available value: high_z voltage_out current_out voltage_in current_in_ext current_in_loop resistance digital_input digital_input_loop current_in_ext_hart current_in_loop_hart
				attr 23: ch0_device_available value: ad74413r max14906
				attr 24: ch1_device_available value: ad74413r max14906
				attr 25: ch2_device_available value: ad74413r max14906
				attr 26: ch3_device_available value: ad74413r max14906
		No trigger assigned to device
	trigger0: sw_trig
		0 channels found:
		No trigger on this device

```

2. **runtime**: all the channel functions have been configured and the devices are out of reset and running (may sample or output signals, report faults and allow register readings and writings). Channel direction and functions may not change during this state, with the exception of the AD74413R's diagnostics channels.

For the complete documentation of the AD74413R, MAX14906 and ADT75 IIO drivers (for no-OS) you may access the following:

 - [AD74413R](https://wiki.analog.com/resources/tools-software/uc-drivers/ad74413r)
 - [MAX14906](https://wiki.analog.com/resources/tools-software/uc-drivers/max14906)
 - [ADT75](https://wiki.analog.com/resources/tools-software/uc-drivers/adt75)

## Field devices configuration and usage overview
This section details how the IIO interface may be used to configure and and sample data from the field devices.
The system has the following IIO devices:

 1. **swiot**

As previously mentioned, field devices will need to have their channels configured prior to being used in the runtime context. This is done through the `swiot` virtual IIO device, which is present in both the config and runtime specific IIO contexts.

*Device attributes*:

 - reset: write any value to this attribute in order to reset the firmware. Can't be read.
 - serial_id: unique ID specific to the microcontroller. May only be read.
 - mode: reflects system's state (config or runtime). May be written in order to change the state. The possible values are those of the `mode_available` attribute. Writing a value specific to the current state is a no op. Values outside of those available will be ignored (the mode won't change).
 - mode_available: valid mode values: `config`, `runtime`.
 - identify: write any value to this in order to blink LED2. Useful for identifying which board has a specific context (or IP) if there are multiple of them in a network.
 - ext_psu: mirrors the external power supply switch. Note that a value of 1 does not guarantee that power supply is actually connected or that it can provide the expected 156W. It's the user's responsibility to ensure this condition. May only be read.
 Possible values: 0 - the field devices are only powered by the LT8304 (through the 24V terminal block) and/or by the LTC9111 (through SPoE). 1 - The MAX14906 is powered directly by the external power supply.
 - signal_mse: indicator of the single pair Ethernet signal quality. May only be read.
 - chX_enable: channel specific enable condition. The `X` index refers to the number of the SWIOT1L's terminal block channel (0 based). When set to 0, the specific channel "won't be controlled by any device", and will be left in a high impedance state, regardless of other attributes. The available values are either 0 or 1.
 - chX_function: the I/O function for a SWIOT1L terminal block channel. May either be specific to the AD74413R or the MAX14906, depending on the `chX_device` attribute. The possible values are those of the `chX_function_available` attribute. It's important to note that the available values change based on the `chX_device` attribute. So it's necessary to write that one before, since the provided value is checked by the driver when written. Otherwise `-EINVAL` (-22) will be returned.
 - chX_function_available: available values for the `chX_function` attribute. May only be read.
 - chX_device: the field device which will control a terminal block channel. The possible values may be read from the `chX_device_available` attribute.
 - chX_device_available: contains the available values for the `chX_device` attribute: `ad74413r`, `max14906`.

In the pyadi-iio example script, the channel functions may be changed using the `channel_enable`,  `channel_config` and `channel_device` lists. For example, the default configuration is:

```
"""
Possible values:
- max14906 selected as device: input, output
- ad74413r selected as device: voltage_out, current_out,
voltage_in, current_in_ext, current_in_loop,
resistance, digital_input, digital_input_loop,
current_in_ext_hart, current_in_loop_hart
"""
channel_config = ["voltage_in", "current_in_ext", "current_out", "output"]
# Possible values: 0, 1
channel_enable = [1, 1, 1, 1]
# Possible values: "ad74413r", "max14906"
channel_device = ["ad74413r", "ad74413r", "ad74413r", "max14906"]
```

This may be interpreted as such:
 - Channel 1: routed to the channel 1 of AD74413R, measuring the input voltage.
 - Channel 2: routed to the channel 2 of AD74413R, measuring the input current.
 - Channel 3: routed to the channel 3 of AD74413R, measuring the voltage resulted from the current limited output through the load connected on the channel's terminals.
 - Channel 4: routed to the channel 4 of MAX14906, configured as digital output.

In order to make sure the config context is selected, the `mode` attribute may be written prior to the next snippet:

`swiot.mode =  "config"`

The configuration sequence using the `swiot`'s attributes is the following:
```
swiot.ch0_device = channel_device[0]
swiot.ch0_function = channel_config[0]
swiot.ch0_enable = channel_enable[0]
swiot.ch1_device = channel_device[1]
swiot.ch1_function = channel_config[1]
swiot.ch1_enable = channel_enable[1]
swiot.ch2_device = channel_device[2]
swiot.ch2_function = channel_config[2]
swiot.ch2_enable = channel_enable[2]
swiot.ch3_device = channel_device[3]
swiot.ch3_function = channel_config[3]
swiot.ch3_enable = channel_enable[3]
```
It's important to note the order in which the attributes are set. `chX_device` should be written before `chX_function`, otherwise an error will occur when setting the latter.

The indexes in these lists correspond to the terminal block channel number (0 based). 0 is the first channel of the SWIOT, 1 is the second, and so on. 

When using these configuration lists, you don't have to worry about the case when 2 channels connected in parallel have "incompatible" functions (e.g both channel 1's of AD74413R and MAX14906 configured to output a voltage). When the channel of a device is configured, it's correspondent is placed in high impedance by the firmware.

In order to switch from one context to the other, the `mode` attribute is written, after the channel configuration shown above:

`swiot.mode =  "runtime"`

After writing this attribute, the IIOD server running on the MAX32650 MCU will restart, which results in loosing the connection to the board and the IIO context being destroyed. As such, the new context will have to be read:

```
ad74413r = adi.ad74413r(uri=dev_uri)
max14906 = adi.max14906(uri=dev_uri)
adt75 = adi.lm75(uri=dev_uri)
swiot = adi.swiot(uri=dev_uri)
```

Note that the new runtime context also contains the field devices, aside from swiot. These may be used to sample data, as is shown in the next sections, detailing the usage of the IIO interface for each of them.

Controlling the MAX14906:
-
**Direct register access**:
The device's registers may be read or written with arbitrary values using the IIO direct register access interface:

 1. Reading a value:
`value = max14906.reg_read(0x1))`
The above snippet may be used to read the value of the `0x1` register
 2. Writing a value:
  `max14906.reg_write(0x1, 0xff))`
The above snippet may be used to set the value of the `0x1` register to `0xff`

**Note**: updating an existing value (by using a mask) is not implemented using this interface, and requires a read->apply mask->write sequence.

This method is primarily intended to be used when a (channel or device) attribute for a specific field does not exist. It is important to note that it should be used with caution, since it will overwrite settings previously done by the firmware, which do not expect to change when the device is in the runtime context. One example of this is channel functions (directions), which the IIO driver assumes to be constant.

**Using channel attributes**:
This method is intended to be used for reading/writing channel specific settings or values (such as the channel state).
The MAX14906 has the following IIO channels, each corresponding to one of the 4 channels of the device:
 1. voltageX: either input or output depending on how it was configured. The `X` value is the index of the physical channel (starting from 0) of the SWIOT board (which is the same as the MAX14906 index).
	The channel has the following attributes:
	 - *raw*: either 0 or 1, representing the low/high state of a channel. The value of an output channel may be configured by writing this attribute: 
	
	`max14906.channels["voltage3"].raw = 1`
	or
	`max14906.channels["voltage3"].raw = 0`
	
	May also be read in both input and output configuration modes to monitor the state of the channel: 	 
	
	 `state = max14906.channels["voltage3"].raw`
	
	Usually (for ADC and DAC devices), a real value may be converted by applying the 
	`value = (raw + offset) * scale` formula. In this case this doesn't quite apply. Since this is a digital-io device, the values it can interpret are binary (and cannot be ), so the raw attribute will either be 0 for a low, or 1 for a high logic level.
	 - *scale*:  constant value of 1 (since the chip is a digital IO).
	 - *offset*: constant value of 0.
	 - *current_limit*: only available for output channels. May be set from a list of predefined values, you may get by reading the `current_limit_available` attribute. The values are provided as integers (current values in mA), and set as such:
		 
		`max14906.channels["voltage3"].current_limit = 130`
	 - *current_limit_available*: list of available values to be set for the `current_limit` attribute (mA values): `600`, `130`, `300`, `1200`.
	 - *do_mode*: only available for output channels. Configures the output driver for a specific channel. May be set from a list of predefined values, you may get by reading the `do_mode_available` attribute. May be set as such:
	 
		 `max14906.channels["voltage3"].do_mode = "High_side"`
	 - *do_mode_available*: list of available values to be set for the `do_mode` attribute: `High_side`, `High_side_2x_inrush`, `Push_pull_clamp`, `Push_pull`.
	 - *IEC_type*: only available for input channels. Used to setup the digital-io input type.
	 - *IEC_type_available*: list of available values: `Type_1_3`, `Type_2`.

**Note**: For a more comprehensive description of each setting, you may access the [MAX14906 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX14906.pdf).
	  
Controlling the AD74413R:
 -
 **Direct register access**:
The device's registers may be read or written with arbitrary values using the IIO direct register access interface:

 1. Reading a value:
`value = ad74413r.reg_read(0x1))`
The above snippet may be used to read the value of the `0x1` register
 2. Writing a value:
  `ad74413r.reg_write(0x1, 0xff))`
The above snippet may be used to set the value of the `0x1` register to `0xff`

**Note**: updating an existing value (by using a mask) is not implemented using this interface, and requires a read->apply mask->write sequence.

This method is primarily intended to be used when a (channel or device) attribute for a specific field does not exist. It is important to note that it should be used with caution, since it will overwrite settings previously done by the firmware, which do not expect to change when the device is in the runtime context. One example of this is channel functions (directions), which the IIO driver assumes to be constant.

**Using channel attributes**:
 Each channel of the AD74413R may be configured using one of these functions: `voltage_out, current_out,voltage_in, current_in_ext, current_in_loop, resistance, digital_input, digital_input_loop, current_in_ext_hart, current_in_loop_hart`. (write these to the `channel_config` list in the Python script). Depending on this configuration, there may be 1 or 2 IIO channels generated, each with a specific set of attributes.
 

 

#### 1. Voltage input:

This configuration creates one IIO channel:

 - _Input voltage_:

| Attribute name               	| Description                                                                                                	| Access type 	| Shared                 	|
|------------------------------	|------------------------------------------------------------------------------------------------------------	|-------------	|------------------------	|
| raw                          	| value between 0 - 8191, corresponding to the  DAC code. Vout = (raw + offset) * scale (mV)                 	| Read/Write  	|            -           	|
| scale                        	| millivolts/LSB. Constant value of 1.342                                                                    	| Read        	|            -           	|
| offset                       	| offset to be applied to the raw value. Constant value of 0.                                                	| Read        	|            -           	|
| sampling_frequency           	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write  	|            -           	|
| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled)                                                 	| Read        	| Shared by all channels 	|
---
#### 2. Current output:
This configuration creates 2 IIO channels:
	 
 - *Input voltage* (used to measure the voltage output by the DAC channel in order to generate the configured current value):

| Attribute name               	| Description                                                                                                	| Access type 	| Shared                 	|
|------------------------------	|------------------------------------------------------------------------------------------------------------	|-------------	|------------------------	|
| raw                          	| value between 0 - 8191, corresponding to the  DAC code. Vout = (raw + offset) * scale (mV)                 	| Read/Write  	|            -           	|
| scale                        	| millivolts/LSB. Constant value of 1.342                                                                    	| Read        	|            -           	|
| offset                       	| offset to be applied to the raw value. Constant value of 0.                                                	| Read        	|            -           	|
| sampling_frequency           	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write  	|            -           	|
| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled)                                                 	| Read        	| Shared by all channels 	|
---

- _Output current_:

| Attribute name 	| Description 	| Access type 	| Shared 	|
|---	|---	|---	|---	|
| raw 	| value between 0 - 8191, corresponding to the  DAC code. Iout = (raw + offset) * scale (mA) 	| Read/Write 	| - 	|
| scale 	| milliamperes/LSB. Constant value of 0.003051 	| Read 	| - 	|
| offset 	| offset to be applied to the raw value. Constant value of 0. 	| Read 	| - 	|
| slew_en 	| enable slew rate control. If this is set to 1, the DAC will configure it's output slew rate using the values of the slew_rate and slew_step attributes. 	| Read/Write 	| - 	|
| slew_rate 	| configures the DAC's output slew rate 	| Read/Write 	| - 	|
| slew_rate_available 	| available values for the slew_rate attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
| slew_step 	| configures the DAC's output slew step 	| Read/Write 	| - 	|
| slew_step_available 	| available values for the slew_step attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|

---
#### 3. Voltage output:
This configuration creates 2 IIO channels:
 - *Input current* (used to measure the current output by the DAC channel in order to generate the configured voltage value).
	
	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value between 0 - 65535, corresponding to the  ADC code. I = (raw + offset) * scale (mA). Positive values mean that the current is sourced, and negative that the current is sinked by the AD74413R. 	| Read/Write 	| - 	|
	| scale 	| milliamperes/LSB. Constant value of 0.000762951 	| Read 	| - 	|
	| offset 	| offset to be applied to the raw value. Constant value of -32768. 	| Read 	| - 	|
	| sampling_frequency 	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write 	| - 	|
	| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled) 	| Read 	| Shared by all channels |
---
 - *Output voltage*:

	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value between 0 - 8191, corresponding to the DAC code. Vout = (raw + offset) * scale (mV) 	| Read/Write 	| - 	|
	| scale 	| millivolts/LSB. Constant value of 1.22070 	| Read 	| - 	|
	| offset 	| offset to be applied to the raw value. Constant value of 0. 	| Read 	| - 	|
	| slew_en 	| enable slew rate control. If this is set to 1, the DAC will configure it's output slew rate using the values of the slew_rate and slew_step attributes. 	| Read/Write 	| - 	|
	| slew_rate 	| configures the DAC's output slew rate 	| Read/Write 	| - 	|
	| slew_rate_available 	| available values for the slew_rate attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
	| slew_step 	| configures the DAC's output slew step 	| Read/Write 	| - 	|
	| slew_step_available 	| available values for the slew_step attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
---

#### 4. Current input externally powered:
This configuration creates one IIO channel:
 - *Input current*

	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value between 0 - 65535, corresponding to the  ADC code. I = (raw + offset) * scale (mA). 	| Read/Write 	| - 	|
	| scale 	| milliamperes/LSB. Constant value of 0.000381476 	| Read 	| - 	|
	| offset 	| offset to be applied to the raw value. Constant value of 0. 	| Read 	| - 	|
	| sampling_frequency 	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write 	| - 	|
	| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled) 	| Read 	| Shared by all channels |
---


#### 5. Current input loop powered:
This configuration creates 2 IIO channels:
 - *Input current* (used to measure the current sourced by the AD74413R)

	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value between 0 - 65535, corresponding to the  ADC code. I = (raw + offset) * scale (mA). 	| Read/Write 	| - 	|
	| scale 	| milliamperes/LSB. Constant value of 0.000381476 	| Read 	| - 	|
	| offset 	| offset to be applied to the raw value. Constant value of 0. 	| Read 	| - 	|
	| sampling_frequency 	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write 	| - 	|
	| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled) 	| Read 	| Shared by all channels |
---

- *Output current* (used to set the limit of the current sourced by the AD74413R)

	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value between 0 - 8191, corresponding to the  DAC code. Iout = (raw + offset) * scale (mA) 	| Read/Write 	| - 	|
	| scale 	| milliamperes/LSB. Constant value of 0.003051 	| Read 	| - 	|
	| offset 	| offset to be applied to the raw value. Constant value of 0. 	| Read 	| - 	|
	| slew_en 	| enable slew rate control. If this is set to 1, the DAC will configure it's output slew rate using the values of the slew_rate and slew_step attributes. 	| Read/Write 	| - 	|
	| slew_rate 	| configures the DAC's output slew rate 	| Read/Write 	| - 	|
	| slew_rate_available 	| available values for the slew_rate attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
	| slew_step 	| configures the DAC's output slew step 	| Read/Write 	| - 	|
	| slew_step_available 	| available values for the slew_step attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
---

#### 6. Current input loop powered with HART compatibility:

The channels and their attributes are the same as in the **Current input externally powered** mode.


#### 7. Current input externally powered with HART compatibility:

The channels and their attributes are the same as in the **Current input externally powered** mode.

#### 8. Resistance:
This configuration creates one IIO channel:
 - *Resistance*:

| Attribute name               	| Description                                                                                                	| Access type 	| Shared                 	|
|------------------------------	|------------------------------------------------------------------------------------------------------------	|-------------	|------------------------	|
| raw                          	| value between 0 - 65535, corresponding to the raw value read from the ADC's result register                 	| Read/Write  	|            -           	|
| sampling_frequency           	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write  	|            -           	|
| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled)                                                 	| Read        	| Shared by all channels 	|
---

#### 9. Digital input:
This configuration creates one IIO channel:
 - *Voltage input*:
 
	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value of either 0 or 1, corresponding to the comparator's output 	| Read/Write 	| - 	|
	| scale 	| constant value of 1 	| Read 	| - 	|
	| offset 	| Constant value of 0. 	| Read 	| - 	|
	| threshold 	| The threshold value (in millivolts) to be used by the comparator. 	| Read/Write 	| - 	|
	| sampling_frequency 	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write 	| - 	|
	| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled) 	| Read 	| Shared by all channels |
---

#### 10. Digital input loop powered:
This configuration creates 2 IIO channels:
 - *Voltage input*:

	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value of either 0 or 1, corresponding to the comparator's output 	| Read/Write 	| - 	|
	| scale 	| constant value of 1 	| Read 	| - 	|
	| offset 	| Constant value of 0. 	| Read 	| - 	|
	| threshold 	| The threshold value (in millivolts) to be used by the comparator. 	| Read/Write 	| - 	|
	| sampling_frequency 	| The number of samples the ADC will convert in 1 second. This is divided by the number of enabled channels. 	| Read/Write 	| - 	|
	| sampling_frequency_available 	| 4800, 1200 (filtering disabled) 20, 10 (filtering enabled) 	| Read 	| Shared by all channels |
---

 - *Current output* (used to set the limit of the current to be sourced by the AD74413R):

	| Attribute name 	| Description 	| Access type 	| Shared 	|
	|---	|---	|---	|---	|
	| raw 	| value between 0 - 8191, corresponding to the  DAC code. Iout = (raw + offset) * scale (mA) 	| Read/Write 	| - 	|
	| scale 	| milliamperes/LSB. Constant value of 0.003051 	| Read 	| - 	|
	| offset 	| offset to be applied to the raw value. Constant value of 0. 	| Read 	| - 	|
	| slew_en 	| enable slew rate control. If this is set to 1, the DAC will configure it's output slew rate using the values of the slew_rate and slew_step attributes. 	| Read/Write 	| - 	|
	| slew_rate 	| configures the DAC's output slew rate 	| Read/Write 	| - 	|
	| slew_rate_available 	| available values for the slew_rate attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
	| slew_step 	| configures the DAC's output slew step 	| Read/Write 	| - 	|
	| slew_step_available 	| available values for the slew_step attribute. Trying to set values outside this range will result in an error, and the field won't change. 	| Read 	| Shared by all channels 	|
---

## Sampling data from the AD74413R

After the configuration step, pyadi-iio may be used to sample buffered data from the AD74413R. This is done by running the following:

```
ad74413r.rx_enabled_channels = ["voltage0"]
ad74413r.sample_rate =  4800
ad74413r.rx_buffer_size = 4800
data = ad74413r.rx()
```

The channel from which the data is sampled may be changed to another input channel like this:
```
ad74413r.rx_enabled_channels = ["current0"]
```
Of course, in the example above, the channel 0 of the SWIOT1L has to be configured to measure current.
Finally, the samples are stored in the `data` array.


