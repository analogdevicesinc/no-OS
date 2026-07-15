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
project (version 4.0.0 or later) on the Windows side. Installation can be done
with the MSI installer, or from a PowerShell prompt with:

.. code-block::

    C:\> winget install usbipd

As of usbipd-win 4.0.0 you no longer need to install any client-side USB/IP
tooling inside WSL — the Windows-side commands below are all that is required.

Attach USB devices to WSL
=========================

Listing available devices
-------------------------

Open **Command Prompt** or **PowerShell** and run:

.. code-block::

    C:\> usbipd list
    BUSID  VID:PID    DEVICE                                  STATE
    10-1   0403:6014  USB Serial Converter                    Not shared
    ...

This lists all connected USB devices and their state.

From WSL, you can see currently attached USB devices with:

.. code-block:: bash

    $ lsusb

Binding a device
----------------

Before a device can be attached to WSL it must be shared once with ``bind``.
Open **Command Prompt** or **PowerShell** in Administrator mode and run:

.. code-block::

    C:\> usbipd bind --busid <BUSID>

For example:

.. code-block::

    C:\> usbipd bind --busid 10-1

Binding is persistent and survives reboots, so this only needs to be done once
per device. After binding, ``usbipd list`` shows the device as **Shared**.

Force binding
^^^^^^^^^^^^^

A plain ``bind`` leaves the device usable by Windows until it is attached to
WSL, which is what you normally want. If a Windows driver holds the device and
prevents it from being shared or attached, add ``--force`` to bind it anyway:

.. code-block::

    C:\> usbipd bind --force --busid <BUSID>

With ``--force`` the host can no longer use the device while it is bound. Only
use it when a normal ``bind`` does not work.

Attaching a device
------------------

To attach a shared USB device to WSL, run in the Windows Command Prompt (no
administrator privileges required):

.. code-block::

    C:\> usbipd attach --wsl --busid <BUSID>

For example:

.. code-block::

    C:\> usbipd attach --wsl --busid 10-1

After attaching, ``usbipd list`` will show the device as **Attached**, and
``lsusb`` inside WSL will show the device.

Detaching a device
------------------

To return the USB device back to Windows, either physically disconnect and
reconnect it, or run in the Windows Command Prompt:

.. code-block::

    C:\> usbipd detach --busid <BUSID>

Additional Resources
====================

- `Connecting USB devices to WSL <https://devblogs.microsoft.com/commandline/connecting-usb-devices-to-wsl/>`_
- `USB/IP client tools <https://github.com/dorssel/usbipd-win/wiki/WSL-support#usbip-client-tools>`_
