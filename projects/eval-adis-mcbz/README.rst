Running no-OS on EVAL-ADIS-MCBZ
===============================

Prerequisites
-------------

Prior to building the project, the environment should be set up for being able to use the STM32 tools. These steps are presented in the ***Build Prerequisites*** section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build .

Example:

.. code-block:: bash

	analog@linux:~/no-OS/projects/eval-adis-mcbz$ export STM32CUBEIDE=/opt/st/stm32cubeide
	analog@linux:~/no-OS/projects/eval-adis-mcbz$ export STM32CUBEMX=/opt/st/stm32cubemx
	analog@linux:~/no-OS/projects/eval-adis-mcbz$ export PLATFORM=stm32

Building the project
--------------------

Example:

.. code-block:: bash

	analog@linux:~/no-OS/projects/eval-adis-mcbz$ make

Programming the MCU
-------------------

Example:

.. code-block:: bash

	analog@linux:~/no-OS/projects/eval-adis-mcbz$ make run
