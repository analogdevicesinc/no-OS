AD-PQMON-SL kit no-OS Example Project
=====================================

## Overview

The [AD-PQMON-SL](https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-pqmon-sl.html) provides a complete software and hardware platform for prototyping and evaluating high performance class S polyphase energy quality monitors. The design incorporates the ADE9430 high performance, polyphase energy monitoring IC that has an advanced metrology feature set (total and fundamental active power, volt amperes reactive (VAR), volt amperes (VA), watthour, VAR hour, VA hour, total and fundamental IRMS and VRMS, power factor) and the MAX32650 ultralow power ARM cortex-M4 with FPU-based microcontroller with 3 MB flash and 1 MB SRAM. The ADE9430 enables accurate energy monitoring over a wide dynamic range through its superior analog performance and digital signal processing (DSP) core. The ADE9430 simplifies the implementation and certification of energy and power quality monitoring systems by providing tight integration of acquisition and calculation engines. This solution can be used on a 3-phase system or up to three single-phase systems.


## Preliminary requirements

The licensed software library that works in conjunction with the ADE9430 IC can be obtained from [here](https://form.analog.com/form_pages/softwaremodules/SRF.aspx).

After obtaining the libraries, the following files need to be added to the project:

```console
pqlib_dir
|   libadi_pqlib_cm4_gcc.a
└───include
	|   ade9430.h
	|   adi_pqlib_debug.h
	|   adi_pqlib_error.h
	|   adi_pqlib_memory.h
	|   adi_pqlib_profile.h
	|   adi_pqlib_version.h
	|   adi_pqlib.h
	└───config
		└───adi_pqlib_cfg.h
```

It can be integrated into the project by defining the `PQLIB_PATH` to point to the `pqlib_dir` path. 


## Choose interface

The firmware application can communicate with clients via several interfaces. In order to use the preferred connection type, set `INTERFACE` to the desired value:

* ```INTERFACE=usb``` (Default value)
* ```INTERFACE=serial``` (Used for 485 communication. Half-duplex communication must be handled by user)
* ```INTERFACE=ethernet``` (Used for communication over T1L)

NOTE: In case one builds firmware multiple times with different interfaces, make sure to delete the `build` directory before the new  compilation.

NOTE 2: For T1L connection the deffault ip for the device is `192.168.97.40`. For network configuration on client side, use the following steps: [network configuration steps](https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602).

## Build and run

The project is based on a [MAX32650](https://www.analog.com/en/products/max32650.html) microcontroller. It can be built and run by running the following script:

```bash
# remove build directory
make reset
# select platform
export PLATFORM=maxim
# select controller type
export TARGET=max32650
# select interface
export INTERFACE=usb
# build and flash the code
make PQLIB_PATH=<path_to_library> run
```

The `iio_info` of the device should look like the following:

```console
IIO context has 3 attributes:
        uri: serial:/dev/ttyACM0,115200,8n1n
        serial,port: /dev/ttyACM0
        serial,description: MAX32650 CDC-ACM
IIO context has 1 devices:
        iio:device0: pqm (buffer capable)
                11 channels found:
                        current0: ia (input, index: 1, format: be:U16/16>>0)
                        8 channel-specific attributes found:
                                attr  0: rms value: 0.01
                                attr  1: angle value: 0
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.01616
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                        voltage0: ua (input, index: 2, format: be:U16/16>>0)
                        15 channel-specific attributes found:
                                attr  0: rms value: 0.25
                                attr  1: angle value: 0
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.05501
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                                attr  8: deviation_under value: 0.25
                                attr  9: deviation_over value: 229.97
                                attr 10: pinst value: 0
                                attr 11: pst value: 0
                                attr 12: plt value: 0
                                attr 13: magnitude1012 value: 0
                                attr 14: maxMagnitude value: 0
                        current1: ib (input, index: 3, format: be:U16/16>>0)
                        8 channel-specific attributes found:
                                attr  0: rms value: 0.10
                                attr  1: angle value: 128
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.01616
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                        voltage1: ub (input, index: 4, format: be:U16/16>>0)
                        15 channel-specific attributes found:
                                attr  0: rms value: 0.09
                                attr  1: angle value: 0
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.05501
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                                attr  8: deviation_under value: 0.09
                                attr  9: deviation_over value: 229.97
                                attr 10: pinst value: 0
                                attr 11: pst value: 0
                                attr 12: plt value: 0
                                attr 13: magnitude1012 value: 0
                                attr 14: maxMagnitude value: 0
                        current2: ic (input, index: 5, format: be:U16/16>>0)
                        8 channel-specific attributes found:
                                attr  0: rms value: 0.12
                                attr  1: angle value: 42
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.01616
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                        voltage2: uc (input, index: 6, format: be:U16/16>>0)
                        15 channel-specific attributes found:
                                attr  0: rms value: 0.30
                                attr  1: angle value: 0
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.05501
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                                attr  8: deviation_under value: 0.30
                                attr  9: deviation_over value: 229.97
                                attr 10: pinst value: 0
                                attr 11: pst value: 0
                                attr 12: plt value: 0
                                attr 13: magnitude1012 value: 0
                                attr 14: maxMagnitude value: 0
                        current3: in (input, index: 7, format: be:U16/16>>0)
                        8 channel-specific attributes found:
                                attr  0: rms value: 0.06
                                attr  1: angle ERROR: Invalid argument (22)
                                attr  2: harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  3: inter_harmonics value: 0.000000 100.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 
                                attr  4: scale value: 0.01616
                                attr  5: offset value: 0
                                attr  6: thd value: 0.000000
                                attr  7: raw value: 0
                        count0: dips (input, index: 8, format: be:U16/16>>0)
                        5 channel-specific attributes found:
                                attr  0: countEvent value: 0
                                attr  1: startTime value: 0 0 0 0 0 0
                                attr  2: endTime value: 0 0 0 0 0 0
                                attr  3: durationInCycles value: 0 0 0 0 0 0
                                attr  4: minMag value: 0 0 0 0 0 0
                        count1: swells (input, index: 9, format: be:U16/16>>0)
                        5 channel-specific attributes found:
                                attr  0: countEvent value: 0
                                attr  1: startTime value: 0 0 0 0 0 0
                                attr  2: endTime value: 0 0 0 0 0 0
                                attr  3: durationInCycles value: 0 0 0 0 0 0
                                attr  4: maxMag value: 0 0 0 0 0 0
                        count2: rvc (input, index: 10, format: be:U16/16>>0)
                        6 channel-specific attributes found:
                                attr  0: countEvent value: 0
                                attr  1: startTime value: 0 0 0 0 0 0
                                attr  2: endTime value: 0 0 0 0 0 0
                                attr  3: durationInCycles value: 0 0 0 0 0 0
                                attr  4: deltaUmax value: 0 0 0 0 0 0
                                attr  5: deltaUss value: 0 0 0 0 0 0
                        count3: intrpt (input, index: 11, format: be:U16/16>>0)
                        4 channel-specific attributes found:
                                attr  0: countEvent value: 0
                                attr  1: startTime value: 0 0 0 0 0 0
                                attr  2: endTime value: 0 0 0 0 0 0
                                attr  3: durationInCycles value: 0 0 0 0 0 0
                34 device-specific attributes found:
                                attr  0: u2 value: 0.00
                                attr  1: u0 value: 0.00
                                attr  2: sneg_voltage value: 0.00
                                attr  3: spos_voltage value: 0.00
                                attr  4: szro_voltage value: 0.00
                                attr  5: i2 value: 0.00
                                attr  6: i0 value: 0.00
                                attr  7: sneg_current value: 0.00
                                attr  8: spos_current value: 0.00
                                attr  9: szro_current value: 0.00
                                attr 10: nominal_voltage value: 0.32
                                attr 11: voltage_scale value: 1001.00
                                attr 12: current_scale value: 294.00
                                attr 13: i_consel_en value: 0.00
                                attr 14: dip_threshold value: 90.00
                                attr 15: dip_hysteresis value: 2.00
                                attr 16: swell_threshold value: 110.00
                                attr 17: swell_hysteresis value: 2.00
                                attr 18: intrp_threshold value: 5.00
                                attr 19: intrp_hysteresis value: 2.00
                                attr 20: rvc_threshold value: 6.00
                                attr 21: rvc_hysteresis value: 3.00
                                attr 22: msv_carrier_frequency value: 330.00
                                attr 23: msv_record_length value: 120.00
                                attr 24: msv_threshold value: 0.30
                                attr 25: sampling_frequency value: 0.00
                                attr 26: v_consel value: 4W_WYE
                                attr 27: v_consel_available value: 4W_WYE 3W_DELTA_VA_VB_NEGVC 4W_WYE_VB_NEGVA_NEGVC 4W_DELTA_VB_NEGVA 4W_DELTA_VA_VB_VC
                                attr 28: flicker_model value: 230V_50HZ
                                attr 29: flicker_model_available value: 230V_50HZ 230V_60HZ 120V_50HZ 120V_60HZ
                                attr 30: nominal_frequency value: 50
                                attr 31: nominal_frequency_available value: 50 60
                                attr 32: process_data value: 1
                                attr 33: fw_version value: 2.1
                No trigger on this device


```
