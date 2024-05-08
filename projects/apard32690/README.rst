Evaluating the AD-APARD32690-SL
===============================

Prerequisites
-------------

Prior to building the project, a couple steps are necessary in order to get the Maxim Micros SDK and setup the environment. These are presented in the ***Build Prerequisites*** section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build .

The MaximSDK provides distributions of `arm-none-eabi-` GCC compiler + utilities and `OpenOCD`, so you don't have to install these separately.

Building the project
--------------------

The project includes 2 different examples:

1. basic_example - may be selected by setting the APARD32690_BASIC_EXAMPLE = y (and all the other examples to n) in the main Makefile.
This example is meant to print "Hello world" over UART0. Make sure to have jumpers in the position 2-3 on headers P50 and P55.

2. tcp_echo_server (selected by default) - may be selected by setting the APARD32690_ECHO_SERVER_EXAMPLE = y (and all the other examples to n) in the main Makefile.
This will start a TCP server using the interface ADIN1110 is connected to (the default settings are IP: 192.168.97.40 port: 10000). It will reply back to the connected client with the
characters it receives. The actual IP address, netmask, and gateway used at runtime are printed on the serial port connected through the debug adapter.

The host running the client may require network settings in order to communicate with a device using the 192.168.97.40 IP. These usually include manually adding a static IP for the host's network interface. You may go though the following guide on how to do this: https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602 (choose an IP in the 192.168.97.x/24 range that's different from the board's address).

The project may be tested by using netcat on the host:

.. code-block:: bash

	netcat 192.168.97.40 10000

#. Open a terminal and navigate to this project directory (if building on Windows, `Git Bash` has to be used).

#. Type `make RELEASE=y -j`, in order to build the project. The `RELEASE` flag adds `-O2` optimization. It should be omitted during debugging.

A successful build should end with the following terminal output:

.. code-block:: bash

	[11:11:27] [HEX] apard32690.hex
	[11:11:27] apard32690.hex is ready
	rm /home/xvr/MaximSDK_new/Libraries/CMSIS/Device/Maxim/MAX32690/Source/GCC/startup_max32690.s
	[11:11:21] Done (build/apard32690.elf)

The binary and executable files are now available in the `build` directory (`apard32690.hex` and `apard32690.elf` files).

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
2. While in the project's root directory, type `make RELEASE=y run`. This method uses OpenOCD in order to load the binary file. If the programming is successful, you should see the following terminal output:

.. code-block:: bash

	** Programming Started **
	** Programming Finished **
	** Verify Started **
	** Verified OK **
	** Resetting Target **
	shutdown command invoked
	[11:27:42] apard32690.elf uploaded to board
