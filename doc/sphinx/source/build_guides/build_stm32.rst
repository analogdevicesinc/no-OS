*************************
STM32 Build Prerequisites
*************************

Prior to building a no-OS project for STM32 platforms, it is required to install
the STM32 development tools and set up the environment.

Linux
=====

Prerequisites
-------------

- Install `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_
  (latest version) to default location ``/opt/stm32cubeide``. If you'd rather
  install it at a different location, run
  ``export STM32CUBEIDE=/path/to/your/stm32cubeide`` in the terminal used for
  building.
- Install `STM32CubeMX <https://www.st.com/en/development-tools/stm32cubemx.html>`_
  version 6.5.0 to default location ``/opt/stm32cubemx``. If you'd rather
  install it at a different location, run
  ``export STM32CUBEMX=/path/to/your/stm32cubemx`` in the terminal used for
  building.
- Install Python3 (if not already present) and make sure the ``python`` command
  executes Python3 (not Python2). This can be easily achieved by running:

    .. code-block:: bash

        $ sudo apt install python-is-python3

Building a project
------------------

Make sure you have the **.ioc** file in the project directory, then type:

.. code-block:: bash

    $ make PLATFORM=stm32

.. note::
   If during the project generation you get a dialog saying that you are using
   an .ioc file generated with an old CubeMX version, click **Continue**.
   **Migrate** is also a valid option but only if you know what you are doing.

   If you're trying to use an .ioc file generated with a newer CubeMX than the
   one installed on your machine, you will get a prompt that asks you to upgrade
   your installation to the new version. There is no other choice than to click
   **OK** and then manually upgrade.

The build process creates a **build** directory in the project folder:

.. code-block::

    build
    ├── app
    ├── bsp
    ├── obj
    ├── project_name.elf
    └── tmp

Running/Debugging
-----------------

Once the **.elf** file has been generated, make sure the board is powered on,
cable connected and use the following commands to upload the program to the
board or debug.

Uploading the binary to target is achieved with:

.. code-block:: bash

    $ make run

Windows
=======

Prerequisites
-------------

.. important::
   Use Git Bash to run these commands.

- Install `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_
  (latest version) to your desired location like ``C:\stm32cubeide``.
- Install `STM32CubeMX <https://www.st.com/en/development-tools/stm32cubemx.html>`_
  version 6.5.0 to your desired location like ``C:\stm32cubemx``.
- Install `Python <https://www.python.org/downloads/>`_ and make sure it's
  available in Git Bash by adding it to the Windows Path, if needed.

Building a project
------------------

Assuming you've installed STM32CubeMX at ``C:\stm32cubemx`` and STM32CubeIDE at
``C:\stm32cubeide``, run these commands prior to building to let the build system
know where they are installed:

.. code-block:: bash

    $ export STM32CUBEMX=/c/stm32cubemx
    $ export STM32CUBEIDE=/c/stm32cubeide

Make sure you have the **.ioc** file in the project directory, then type:

.. code-block:: bash

    $ make PLATFORM=stm32

.. note::
   If during the project generation you get a dialog saying that you are using
   an .ioc file generated with an old CubeMX version, click **Continue**.
   **Migrate** is also a valid option but only if you know what you are doing.

   If you're trying to use an .ioc file generated with a newer CubeMX than the
   one installed on your machine, you will get a prompt that asks you to upgrade
   your installation to the new version. There is no other choice than to click
   **OK** and then manually upgrade.

Running/Debugging
-----------------

The running and debugging steps are the same as on Linux. See the Linux section
above for ``make run``.

Debugging with PowerShell (OpenOCD)
-----------------------------------

.. important::
   This section is for running built no-OS projects natively under Windows
   using PowerShell and OpenOCD.

- Install `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_.
- In PowerShell, set the variables below, correcting with the absolute paths
  of your STM32CubeIDE install:

.. code-block:: powershell

    $stm32cubeide="C:\ST\STM32CubeIDE_1.16.1\STM32CubeIDE"
    $openocd_bin="$stm32cubeide\plugins\com.st.stm32cube.ide.mcu.externaltools.openocd.win32_2.3.200.202404091248\tools\bin\openocd.exe"
    $openocd_scripts="$stm32cubeide\plugins\com.st.stm32cube.ide.mcu.debug.openocd_2.2.100.202406131243\resources\openocd\st_scripts"

- Extract the pair of deliverables (e.g., ``some_project.elf.openocd``,
  ``project.elf``) in a folder. The ``.openocd`` file will be the same
  regardless of the Makefile configuration.
- Navigate to the folder in PowerShell:

.. code-block:: powershell

    cd ~\path\to\my_project

- Set the project files:

.. code-block:: powershell

    $openocd_cmd=".\some_project.elf.openocd"
    $openocd_elf="./some_project.elf"

- And run:

.. code-block:: powershell

    &"$openocd_bin" -s "$openocd_scripts" -f $openocd_cmd -c "program $openocd_elf verify reset exit"
