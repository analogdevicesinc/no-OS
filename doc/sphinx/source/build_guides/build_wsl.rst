***************************
WSL2 USB Passthrough Guide
***************************

If you use WSL you cannot access USB devices directly because WSL does not
natively support USB. If you try to load the binary into the target with
``make run``, you will encounter errors about missing JTAG targets.

This guide explains how to pass USB devices from Windows to WSL2 so that
JTAG debugging and flashing work from within WSL.

Requirements
============

- Windows 10 or Windows 11 with all updates installed.
- WSL2 with kernel version 5.10.60.1 or later.

You can check your kernel version from WSL with:

.. code-block:: bash

    $ uname -a
    Linux 5.15.90.1-microsoft-standard-WSL2 ...

Install usbipd-win
==================

Install the `usbipd-win <https://github.com/dorssel/usbipd-win/releases>`_
project on the Windows side. Installation can be done with the MSI installer.

Install Linux USB/IP tools
==========================

From WSL, install the user space tools for USB/IP and a database of USB hardware
identifiers:

.. code-block:: bash

    $ sudo apt update
    $ sudo apt upgrade
    $ sudo apt install linux-tools-virtual hwdata
    $ sudo update-alternatives --install /usr/local/bin/usbip usbip $(command -v ls /usr/lib/linux-tools/*/usbip | tail -n1) 20

If the last command does not work, try:

.. code-block:: bash

    $ sudo update-alternatives --install /usr/local/bin/usbip usbip `ls /usr/lib/linux-tools/*/usbip | tail -n1` 20

Attach USB devices to WSL
=========================

Listing available devices
-------------------------

Open **Command Prompt** or **PowerShell** in Administrator mode and run:

.. code-block::

    C:\> usbipd wsl list
    BUSID  VID:PID    DEVICE                                  STATE
    10-1   0403:6014  USB Serial Converter                    Not attached
    ...

This lists all connected USB devices and their attachment state.

From WSL, you can see currently attached USB devices with:

.. code-block:: bash

    $ lsusb

Attaching a device
------------------

To attach a USB device to WSL, run in the Windows Command Prompt:

.. code-block::

    C:\> usbipd wsl attach -b <BUSID>

For example:

.. code-block::

    C:\> usbipd wsl attach -b 10-1

After attaching, ``usbipd wsl list`` will show the device as
**Attached - Ubuntu** (or your WSL distribution name), and ``lsusb`` inside
WSL will show the device.

Detaching a device
------------------

To return the USB device back to Windows, either physically disconnect and
reconnect it, or run in the Windows Command Prompt:

.. code-block::

    C:\> usbipd wsl detach -b <BUSID>

Additional Resources
====================

- `Connecting USB devices to WSL <https://devblogs.microsoft.com/commandline/connecting-usb-devices-to-wsl/>`_
- `USB/IP client tools <https://github.com/dorssel/usbipd-win/wiki/WSL-support#usbip-client-tools>`_
