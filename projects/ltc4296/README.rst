LTC4296 no-OS Example Project
=============================

.. no-os-doxygen::


Contents
--------

.. contents:: Table of Contents
	:depth: 3


Overview
--------

The evaluation board allows the LTC4296-1 to be exercised without the need for
external components.

Full performance details are provided in the LTC4296 data sheet, which should
be consulted in conjunction with user guide.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4296/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4296/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTC4296, unlock the chip and enables power on all
avaliable ports.


This is the only example in this project, so it builds by default:

.. code-block:: bash

	make

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
