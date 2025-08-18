Hockey Puck no-OS Example Project
======================================

.. no-os-doxygen::

Prerequisites
-------------

Prior to building the project, a couple steps are necessary in order to get the Maxim Micros SDK and setup the environment. These are presented in the ***Build Prerequisites*** section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build .

The MaximSDK provides distributions of `arm-none-eabi-` GCC compiler + utilities and `OpenOCD`, so you don't have to install these separately.

Project setup
----------------

The project includes 5 different examples:

1. **ad7124_app**: Prints data sampled on channel 0 of the AD7124.
2. **hello_world**: Uses the STDIO redirection to UART feature to print a message over the serial connection.
3. **iio_example**: Creates an IIO server and exposes the AD7124 as an IIO device. Uses the network backend of IIOD for data transmission (through the ADIN1110).

The host running the client may require specific network settings to communicate with a device using the 192.168.97.60 IP. These usually include manually adding a static IP for the host's network interface. You can follow this guide on how to do this: `Static IP Setting Guide <https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602>`_ (choose an IP in the 192.168.97.x/24 range that's different from the board's address).

The project may be tested by using netcat on the host:

.. code-block:: bash

    iio_info -u ip:192.168.97.60

4. **max6613_app**: Prints the temperature data sampled by the MAX6613.
5. **tcp_echo_server**: This will start a TCP server using the interface ADIN1110 is connected to (the default settings are IP: 192.168.97.60 port: 10000). It will reply back to the connected client with the characters it receives. The actual IP address, netmask, and gateway used at runtime are printed on the serial port connected through the debug adapter.

The host running the client may require specific network settings to communicate with a device using the 192.168.97.60 IP. These usually include manually adding a static IP for the host's network interface. You can follow this guide on how to do this: `Static IP Setting Guide <https://wiki.analog.com/resources/no-os/misc_guides/static_ip_setting?rev=1715173602>`_ (choose an IP in the 192.168.97.x/24 range that's different from the board's address).

The project may be tested by using netcat on the host:

.. code-block:: bash

    netcat 192.168.97.60 10000

Building the project
--------------------

#. Open a terminal and navigate to this project directory (if building on Windows, `Git Bash` must be used).
#. The project is built using make:

.. code-block:: bash

    make EXAMPLE=<example> HP_STATIC_IP=<y/n> BOARD_REV=<HP_IS/HP_NON_IS> <RELEASE=y/n>

Make Flags
^^^^^^^^^^

* **RELEASE**: Controls the optimization level.
    * `y`: Compiles with `-O2`.
    * `n`: (Default) Compiles without optimization, which is recommended for debugging.
* **EXAMPLE**: Selects the project version.
    * `ad7124_app`
    * `hello_world`
    * `iio_example`
    * `max6613_app`
    * `tcp_echo_server`
* **HP_STATIC_IP**: Selects between a static IP and DHCP for samples involving a network connection.
    * `y`: Configures the IP specified using the NO_OS_IP make variable. By default, it is 192.168.97.60
    * `n`: (Default) Let the DHCP server running on the host PC assign an IP to the device.
* **BOARD_REV**: Selects the board revision.
    * `HP_IS`: (Default) Configure the peripheral interfaces for the intrinsic safety version of the Hockey Puck
    * `HP_NON_IS`: Configure the peripheral interfaces for the non intrinsic safety version of the Hockey Puck

A successful build should end with the following terminal output:

.. code-block:: bash

    [11:11:27] [HEX] hockey_puck.hex
    [11:11:27] hockey_puck.hex is ready
    [11:11:21] Done (build/hockey_puck.elf)

The binary and executable files are now available in the `build` directory (`hockey_puck.hex` and `hockey_puck.elf` files).

Programming the MCU
-------------------

Before the MCU can be programmed a few steps are necessary (only for the Drag-and-drop method detailed below):

#. Replace the DAPLINK firmware for the MAX32625PICO. This is only required to be done one time.

    * Download the firmware image from the following link: https://github.com/MaximIntegrated/max32625pico-firmware-images/raw/master/bin/max32625_max32650fthr_if_crc_swd_v1.0.6.bin .

    * Make sure the MAX32625PICO is not connected to the PC.

    * Press the button on the MAX32625PICO, and keep it pressed while you plug the USB cable in the MAX32625PICO.

    * Release the button once you can see a `MAINTENANCE` drive being mounted.

    * Copy the firmware binary file to the `MAINTANANCE` drive. It should unmount and a `DAPLINK` drive should appear instead.

#. Connect the MAX32625PICO board to the PC and the Hockey Puck board. If everything went well, you should see a mass storage device named `DAPLINK` in your filesystem.

#. Power on the Hockey Puck board.

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
	[11:27:42] hockey_puck.elf uploaded to board
