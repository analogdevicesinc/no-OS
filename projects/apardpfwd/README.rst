AD-APARDPFWD-SL no-OS Example Project
======================================

Prerequisites
-------------

Prior to building the project, a couple steps are necessary in order to get the Maxim Micros SDK and setup the environment. These are presented in the ***Build Prerequisites*** section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build .

The MaximSDK provides distributions of `arm-none-eabi-` GCC compiler + utilities and `OpenOCD`, so you don't have to install these separately.

Building the project
--------------------

The project includes 2 different examples:

1. apard_communication_example (selected by default) - This example closes the output port of the AD-APARDPFWD-SL and opens a TCP socket to communicate with the AD-APARD32690-SL. It will start a TCP server using the interface ADIN1110 is connected to (the default settings are IP: 192.168.97.50 port: 10000). It will reply back to the connected client with the characters it receives. The actual IP address, netmask, and gateway used at runtime are printed on the serial port connected through the debug adapter.

2. forward_packets_example - may be selected by setting the EXAMPLE variable to forward_packets_example in the main Makefile.
This example opens the output port of the AD-APARDPFWD-SL to be able to ping the downstream device. It initializes the ADIN1110 Ethernet controller and reads its device ID.

The host running the client may require network settings in order to communicate with a device using the 192.168.97.50 IP. These usually include manually adding a static IP for the host's network interface. You may go through the following guide on how to do this: https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602 (choose an IP in the 192.168.97.x/24 range that's different from the board's address).

The apard_communication_example may be tested by using netcat on the host:

.. code-block:: bash

	netcat 192.168.97.50 10000

Available variants: ``apard_communication_example``, ``forward_packets_example``.
Available boards: ``ad-apard32690-sl``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the project (apard_communication_example example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project apardpfwd --variant apard_communication_example --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project apardpfwd --variant apard_communication_example --board ad-apard32690-sl \
      --probe openocd --flash

Programming the MCU
-------------------

Before the MCU can be programmed a few steps are necessary:

#. Replace the DAPLINK firmware for the MAX32625PICO. This is only required to be done one time.

    * Download the firmware image from the following link: https://github.com/MaximIntegrated/max32625pico-firmware-images/raw/master/bin/max32625_max32650fthr_if_crc_swd_v1.0.6.bin .

    * Make sure the MAX32625PICO is not connected to the PC.

    * Press the button on the MAX32625PICO, and keep it pressed while you plug the USB cable in the MAX32625PICO.

    * Release the button once you can see a `MAINTENANCE` drive being mounted.

    * Copy the firmware binary file to the `MAINTENANCE` drive. It should unmount and a `DAPLINK` drive should appear instead.

#. Connect the MAX32625PICO board to the PC and the AD-APARD32690-SL board. If everything went well, you should see a mass storage device named `DAPLINK` in your filesystem.

#. Power on the AD-APARD32690-SL board using the USB-C port on the AD-APARD32690-SL or the AD-APARDPFWD-SL.

The microcontroller may be programmed by running the build and flash command with the ``--probe`` and ``--flash`` options as shown in the build section above. This method uses OpenOCD in order to load the binary file. If the programming is successful, you should see output indicating successful verification and flashing.
