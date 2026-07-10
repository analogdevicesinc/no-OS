AD-APARD32690-SL no-OS Example Project
======================================

.. no-os-doxygen::

Prerequisites
-------------

Prior to building the project, a couple steps are necessary in order to get the Maxim Micros SDK and setup the environment. These are presented in the ***Build Prerequisites*** section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build .

The MaximSDK provides distributions of `arm-none-eabi-` GCC compiler + utilities and `OpenOCD`, so you don't have to install these separately.

Building the project
--------------------

The project provides the following examples, each selected with its own
``--variant`` (the ``.conf`` files in this directory) when building:

1. basic (``--variant basic``)
This example is meant to print "Hello world" over UART0. Make sure to have jumpers in the position 2-3 on headers P50 and P55.

2. tcp_echo_server (``--variant tcp_echo_server``)
This will start a TCP server using the interface ADIN1110 is connected to (the default settings are IP: 192.168.97.40 port: 10000). It will reply back to the connected client with the
characters it receives. The actual IP address, netmask, and gateway used at runtime are printed on the serial port connected through the debug adapter.

3. adin1110_standalone (``--variant adin1110_standalone``)
Standalone ADIN1110 example.

4. esh (``--variant esh``)
Embedded shell example.

The host running the client may require network settings in order to communicate with a device using the 192.168.97.40 IP. These usually include manually adding a static IP for the host's network interface. You may go though the following guide on how to do this: https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602 (choose an IP in the 192.168.97.x/24 range that's different from the board's address).

The project may be tested by using netcat on the host:

.. code-block:: bash

	netcat 192.168.97.40 10000

#. Open a terminal and navigate to the repository root.

#. Build the project with the ``no_os_build.py`` helper, selecting the desired variant (the build defaults to a release, ``-O2``, build):

.. code-block:: bash

	python3 tools/scripts/no_os_build.py build \
		--project apard32690 --variant tcp_echo_server --board ad-apard32690-sl

The binary and executable files are now available in the `build-apard32690-tcp_echo_server-ad-apard32690-sl` directory (`apard32690.hex` and `apard32690.elf` files).

Programming the MCU
-------------------

Before the MCU can be programmed a few steps are necessary:

#. Replace the DAPLINK firmware for the MAX32625PICO. This is only required to be done one time.

    * Download the firmware image from the following link: https://github.com/MaximIntegrated/max32625pico-firmware-images/raw/master/bin/max32625_max32650fthr_if_crc_swd_v1.0.6.bin .

    * Make sure the MAX32625PICO is not connected to the PC.

    * Press the button on the MAX32625PICO, and keep it pressed while you plug the USB cable in the MAX32625PICO.

    * Release the button once you can see a `MAINTENANCE` drive being mounted.

    * Copy the firmware binary file to the `MAINTANANCE` drive. It should unmount and a `DAPLINK` drive should appear instead.

#. Connect the MAX32625PICO board to the PC and the APARD32690 board. If everything went well, you should see a mass storage device named `DAPLINK` in your filesystem.

#. Power on the APARD32690 board.

The microcontroller may be programmed in 2 ways:
1. Drag-and-drop the binary (.hex) file in the `DAPLINK` directory. The drive should be unmounted and mounted again, once the programming is done.
2. From the repository root, re-run the build command for your variant with `--probe openocd --flash` appended. This method uses OpenOCD in order to load the binary file. If the programming is successful, you should see the following terminal output:

.. code-block:: bash

	** Programming Started **
	** Programming Finished **
	** Verify Started **
	** Verified OK **
	** Resetting Target **
	shutdown command invoked
	[11:27:42] apard32690.elf uploaded to board
