AD-PQMON-SL Kit no-OS Example Project
=====================================

Overview
--------

The :adi:`AD-PQMON-SL` provides a complete software and hardware platform for
prototyping and evaluating high performance class S polyphase energy quality
monitors. The design incorporates the ADE9430 high performance, polyphase
energy monitoring IC that has an advanced metrology feature set (total and
fundamental active power, volt amperes reactive (VAR), volt amperes (VA),
watthour, VAR hour, VA hour, total and fundamental IRMS and VRMS, power
factor) and the MAX32650 ultralow power ARM cortex-M4 with FPU-based
microcontroller with 3 MB flash and 1 MB SRAM. The ADE9430 enables accurate
energy monitoring over a wide dynamic range through its superior analog
performance and digital signal processing (DSP) core. The ADE9430 simplifies
the implementation and certification of energy and power quality monitoring
systems by providing tight integration of acquisition and calculation engines.
This solution can be used on a 3-phase system or up to three single-phase
systems.

Preliminary requirements
-------------------------

The licensed software library that works in conjunction with the ADE9430 IC
can be obtained from `here <https://form.analog.com/form_pages/softwaremodules/SRF.aspx>`__.

After obtaining the libraries, the following files need to be added to the
project:

.. code-block:: console

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

It can be integrated into the project by defining the ``PQLIB_PATH`` to point
to the ``pqlib_dir`` path.

Choose interface
----------------

The firmware application can communicate with clients via several interfaces.
In order to use the preferred connection type, set ``INTERFACE`` to the
desired value:

* ``INTERFACE=usb`` (Default value)
* ``INTERFACE=serial`` (Used for 485 communication. Half-duplex communication
  must be handled by user)
* ``INTERFACE=ethernet_t1l`` (Used for communication over T1L)
* ``INTERFACE=ethernet`` (Used for standard Ethernet communication via W5500
  controller)

.. note::

   In case one builds firmware multiple times with different interfaces, make
   sure to delete the ``build`` directory before the new compilation.

.. note::

   For T1L connection the default ip for the device is ``192.168.97.40``. For
   network configuration on client side, use the following steps: `network
   configuration steps
   <https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602>`__.

.. note::

   For standard Ethernet connection, the default IP for the device is
   ``192.168.1.110``. Make sure your network environment is configured
   appropriately to communicate with this IP address.

Build and run
-------------

The project is based on a :adi:`MAX32650` microcontroller. It can
be built and run by running the following script:

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

AFE Calibration
---------------

The firmware supports gain and offset calibration for the ADE9430 AFE.
Calibration coefficients can be stored in internal flash and are automatically
loaded at startup.

**Calibration Procedure:**

1. Set calibration input parameters (nominal voltage/current for gain, or offset
   voltage/current for offset calibration)
2. Select calibration type (GAIN or OFFSET) and phase (A, B, or C)
3. Apply the reference signal and start calibration
4. Wait for calibration to complete (status changes to "done")
5. Save calibration to flash for persistence across power cycles

**Calibration IIO Attributes:**

.. list-table::
   :header-rows: 1

   * - Attribute
     - Description
   * - cal_type
     - Calibration type: GAIN or OFFSET
   * - cal_phase
     - Phase to calibrate: A, B, or C
   * - cal_nominal_voltage
     - Reference voltage for gain calibration (Vrms)
   * - cal_nominal_current
     - Reference current for gain calibration (Arms)
   * - cal_offset_voltage
     - Reference voltage for offset calibration (Vrms)
   * - cal_offset_current
     - Reference current for offset calibration (Arms)
   * - cal_start
     - Write 1 to start calibration
   * - cal_status
     - Calibration status: idle, in_progress, done, error
   * - cal_gain_i_error_before/after
     - Current error before/after gain calibration (%)
   * - cal_gain_v_error_before/after
     - Voltage error before/after gain calibration (%)
   * - cal_offset_i_error_before/after
     - Current error before/after offset calibration (%)
   * - cal_offset_v_error_before/after
     - Voltage error before/after offset calibration (%)
   * - flash_cal_save
     - Write 1 to save calibration to flash
   * - flash_cal_load
     - Write 1 to load calibration from flash
   * - flash_cal_erase
     - Write 1 to erase calibration from flash
   * - flash_status
     - Flash data status: ok, no_data, crc_error, version_error, invalid_magic, error
   * - flash_cal_data
     - Read calibration data summary for all channels (gain, offset, validity)

The ``iio_info`` of the device should look like the following:

.. code-block:: console

    IIO context has 3 attributes:
        uri: serial:/dev/ttyACM0,115200,8n1n
        serial,port: /dev/ttyACM0
        serial,description: MAX32650 CDC-ACM

IIO context has 1 device:

.. code-block:: console

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
                57 device-specific attributes found:
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
                                attr 12:  value: 294.00
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
                                attr 34: cal_type value: GAIN
                                attr 35: cal_type_available value: GAIN OFFSET
                                attr 36: cal_start value: 0
                                attr 37: cal_status value: idle
                                attr 38: cal_phase value: A
                                attr 39: cal_phase_available value: A B C
                                attr 40: cal_nominal_current value: 10.00
                                attr 41: cal_nominal_voltage value: 230.00
                                attr 42: cal_offset_current value: 1.00
                                attr 43: cal_offset_voltage value: 10.00
                                attr 44: cal_gain_i_error_before value: 0.000000
                                attr 45: cal_gain_v_error_before value: 0.000000
                                attr 46: cal_gain_i_error_after value: 0.000000
                                attr 47: cal_gain_v_error_after value: 0.000000
                                attr 48: cal_offset_i_error_before value: 0.000000
                                attr 49: cal_offset_v_error_before value: 0.000000
                                attr 50: cal_offset_i_error_after value: 0.000000
                                attr 51: cal_offset_v_error_after value: 0.000000
                                attr 52: flash_cal_save value: 0
                                attr 53: flash_cal_load value: 0
                                attr 54: flash_cal_erase value: 0
                                attr 55: flash_status value: no_data
                                attr 56: flash_cal_data value: CH_A:gain=0,off=0,i_gain=0x0,...;CH_B:...;CH_C:...
                No trigger on this device


Additional Resources
---------------------

The complete User Guide for the AD-PQMON-SL solution is available at
`AD-PQMON-SL GitHub Page
<https://analogdevicesinc.github.io/documentation/solutions/reference-designs/ad-pqmon-sl/index.html>`__.
