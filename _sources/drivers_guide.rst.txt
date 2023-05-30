*******************
no-OS drivers guide
*******************

This page represents a quick guide on how to create a no-OS driver and at the same time a generic documentation of the already available drivers.


C code C++ compatible
=====================

The code must be written in C, but be compatible with C + + environments too since some platforms of this type (e.g., Mbed OS)  are already capable of using the no-OS drivers.

Conversion from void*
---------------------

Implicit conversion from void* is forbidden in C + +, so, explicit cast is required.

.. code-block:: C

	struct ad1234_dev *dev;

	dev = (struct ad1234_dev *)malloc(sizeof(*dev));

Coding Style
============

While the preferred no-OS coding style is mainly the Linux kernel one (https://www.kernel.org/doc/html/latest/process/coding-style.html), we should still mention here some important rules.

License Header
--------------

Each source or header file should include a license header - this will allow customers to quickly determine if the module can be used in their system.

.. code-block:: C

	/***************************************************************************//**
	*   @file   ad1234.c
	*   @brief  Implementation of AD1234 Driver
	*   @author Your Name (your.name@analog.com)
	********************************************************************************
	* Copyright 2023(c) Analog Devices, Inc.
	*
	* All rights reserved.
	*
	* Redistribution and use in source and binary forms, with or without
	* modification, are permitted provided that the following conditions are met:
	*  - Redistributions of source code must retain the above copyright
	*    notice, this list of conditions and the following disclaimer.
	*  - Redistributions in binary form must reproduce the above copyright
	*    notice, this list of conditions and the following disclaimer in
	*    the documentation and/or other materials provided with the
	*    distribution.
	*  - Neither the name of Analog Devices, Inc. nor the names of its
	*    contributors may be used to endorse or promote products derived
	*    from this software without specific prior written permission.
	*  - The use of this software may or may not infringe the patent rights
	*    of one or more patent holders.  This license does not release you
	*    from the requirement that you obtain separate licenses from these
	*    patent holders to use this software.
	*  - Use of the software either in source or binary form, must be run
	*    on or directly connected to an Analog Devices Inc. component.
	*
	* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
	* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
	* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
	* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
	* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*******************************************************************************/

Include Guards
--------------

To prevent the inclusion of the header file content more than once, include guards must be used in all .h files.

.. code-block:: C

	#ifndef __AD1234_H__
	#define __AD1234_H__

	// ...

	#endif // __AD1234_H__

Indentation
-----------

Tabs should have a size of 8 characters and should be used instead of spaces for indentation (don't use 8 spaces instead of a tab).

Naming
------

The name of the functions should be prefixed with the part number the driver is created for. If the driver will support multiple parts, the prefix can be the name of one of the parts.

Variables
---------

In order to make the drivers support multiple instances, don't use global variables; use the device handler instead.

Structures
----------

At least 2 structures need to be defined.

1) Initialization structure
...........................

The initialization structure contains all the information required to initialize the device: the interface parameters (e.g., SPI, I2C, GPIOs) or device specific settings.

.. code-block:: C

	struct ad1234_init_param {
		struct no_os_gpio_init_param gpio_reset;
		struct no_os_spi_init_param  spi_init;
		int                          some_settings;
	};

2) Device handler structure
...........................

The device handler structure contains all the information that must be passed between the driver's functions: the interface descriptors (e.g., SPI, I2C, GPIOs) or device specific settings.

.. code-block:: C

	struct ad1234_dev {
		struct no_os_gpio_desc *gpio_reset;
		struct no_os_spi_desc  *spi;
		int                    some_settings;
	};

Functions
---------

Usually, all functions should return an integer error code (a negative number meaning an error).

At least 2 functions need to be defined.

1) Initialization function
..........................

It is mandatory to have this function; its name will be composed by a prefix (usually, the part number) and "_init" (e.g., ad1234_init()).

It should take 2 arguments: a pointer to a pointer (it allocates memory for a structure that will be used outside) of a device handler structure and a pointer to an initialization structure.

.. code-block:: C

	int ad1234_init(struct ad1234_dev **device,
			struct ad1234_init_param *init_param)


It creates the device handler, initializes the communication peripherals, requests the GPIOs and set up the device according to the parameters specified in the dedicated structure.

.. code-block:: C

	{
		struct ad1234_dev *dev;
		int ret;

		dev = (struct ad1234_dev *)malloc(sizeof(*dev));
		if (!dev)
			return -ENOMEM;

		ret = no_os_gpio_get(&dev->gpio_reset, &init_param.gpio_reset);
		if (ret)
			goto error_1;

		ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
		if (ret)
			goto error_2;
		
		dev->some_settings = init_param->some_settings;
		ret =  ad1234_setup(dev);
		if (ret)
			goto error_3;

		*device = dev;
		
		return 0;
		
		error_3:
			spi_remove(dev->spi_desc);
		error_2:
			gpio_remove(dev->gpio_reset);
		error_1:
			free(dev);

			return ret;
	}

2) Remove function
..................

It is mandatory to have this function; its name will be composed by a prefix (usually, the part number) and "_remove" (e.g., ad1234_remove()).

.. code-block:: C

	int ad1234_remove(struct adt1234_dev *dev)

It frees the resources allocated by the initialization function.

.. code-block:: C

	{
		int ret;
		
		ret = spi_remove(dev->spi_desc);
		ret |= gpio_remove(dev->gpio_reset);

		free(dev);

		return ret;
	}

Please note that the remove() function cannot and does not set the provided pointer to NULL after deallocation of resources.

Documenting the Code
--------------------

Doxygen comments of the code are required.

Important Considerations
========================

Avoiding Race Conditions
------------------------

The no-OS software doesn't offer yet full support for Locks, Mutexes or Semaphores. If in the final application there is a risk of having multiple threads accessing the same resource at the same time, the user must deal with this threat so race conditions are avoided.

Avoiding Heap Fragmentation
---------------------------

Since the driver's _init() function is allocating memory on heap and the _remove() one is clearing it, the user must pay attention to how many times and when these two are called in his or her final application so the heap fragmentation won't cause any harmful effect.

Beautifying code
================

Once a new driver or an update is ready to be merged on the main branch, tools that check the coding style compliance can be used. Some examples are given below.

Artistic Style
--------------

Artistic Style is a source code indenter, formatter, and beautifier for the C, C++, C++/CLI, Objective‑C, C# and Java programming languages.

Example:

.. code-block:: bash

	analog@debian:~$ astyle --style=linux --indent=force-tab=8 --max-code-length=80 platform_drivers.c

ClangFormat
-----------

Clang-Format is a tool to format C/C++/Java/JavaScript/Objective-C/Objective-C++/Protobuf code. It can be configured with a config file within the working folder or a parent folder.

Example:

Dump default configuration options to .clang-format.

.. code-block:: bash

	analog@debian:~$ clang-format -dump-config > .clang-format

Modify .clang-format according to the project's specifications. Use -style=file command line option to use the customized style (.clang-format file).

.. code-block:: bash

	analog@debian:~$ clang-format -style=file -i platform_drivers.c

Uncrustify
----------

Source Code Beautifier for C, C++, C#, ObjectiveC, D, Java, Pawn and VALA.

Example:

.. code-block:: bash

	analog@debian:~$ wget https:/raw.githubusercontent.com/uncrustify/uncrustify/master/documentation/htdocs/default.cfg
	analog@debian:~$ uncrustify -c default.cfg -f platform_drivers.c -o platform_drivers.c
