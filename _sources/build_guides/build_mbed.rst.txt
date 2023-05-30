************************
Mbed Build Prerequisites
************************

Prior to building a no-OS project for Mbed platforms, it is required to install
the Mbed CLI tools and ARM compiler.

Linux
=====

Prerequisites
-------------

- Install Mbed CLI 1 as per the guide at
  https://os.mbed.com/docs/mbed-os/v6.15/build-tools/install-and-set-up.html.
  Usually the following steps should be sufficient:

    .. code-block:: bash

        $ sudo apt install python3 python3-pip git mercurial gcc-arm-none-eabi
        $ sudo python3 -m pip install mbed-cli pyelftools==0.29

- Configure the compiler location with Mbed CLI. This can be carried out by
  running:

    .. code-block:: bash

        $ mbed config -G GCC_ARM_PATH "path-to-your-gcc-compiler"

Building a project
------------------

To build a project, type:

.. code-block:: bash

    $ make PLATFORM=mbed TARGET_BOARD=...

The **TARGET_BOARD** specifies the board for which the project is built. If not
specified, it defaults to **SDP-K1**.

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
board:

.. code-block:: bash

    $ make run

Windows
=======

Prerequisites
-------------

.. important::
   Use Git Bash to run these commands.

- Initialize the Mbed submodule in no-OS by running:

    .. code-block:: bash

        $ git submodule update --init mbed-os
        $ git submodule update mbed-os

- Install `Python 3.11.2 <https://www.python.org/downloads/release/python-3112/>`_.
- Install the virtual environment package by running:

    .. code-block:: bash

        $ pip install virtualenv

- Create a virtual environment by running:

    .. code-block:: bash

        $ python -m venv <name_of_virtual_environment>

  This will create a virtual environment with the set name in the current
  directory of the Git Terminal.
- Activate the environment by running:

    .. code-block:: bash

        $ source <location_and_name_of_virtual_environment>/Scripts/activate

- Install `GNU Arm Embedded Compiler <https://developer.arm.com/downloads/-/gnu-rm>`_
  (version: **9-2019-q4-major**).
- Remove line 20 of **requirements.txt** inside the mbed-os folder
  (``hidapi>=0.7.99,<0.8.0;platform_system!="Linux"``).
- Install the requirements by running:

    .. code-block:: bash

        $ <location_and_name_of_virtual_environment>/Scripts/pip install -r requirements.txt

  Doing this will run the pip command using the Python inside the virtual
  environment.
- Install the previously removed line from **requirements.txt** by running:

    .. code-block:: bash

        $ <location_and_name_of_virtual_environment>/Scripts/pip install "cython<3.0.0"
        $ <location_and_name_of_virtual_environment>/Scripts/pip install "hidapi>=0.7.99,<0.8.0;platform_system!='Linux'"

- Install Mbed CLI using:

    .. code-block:: bash

        $ <location_and_name_of_virtual_environment>/Scripts/pip install mbed-cli

- Configure the compiler location with Mbed CLI. This can be carried out by
  running in Git Terminal:

    .. code-block:: bash

        $ mbed config -G GCC_ARM_PATH <path_to_your_gcc_compiler>

Building a project
------------------

.. important::
   Make sure that the virtual environment is activated (environment name enclosed
   in parenthesis appears in the git terminal) and that the packages from
   prerequisites were installed.

To build a project, type:

.. code-block:: bash

    $ make PLATFORM=mbed TARGET_BOARD=...

The **TARGET_BOARD** specifies the board for which the project is built. If not
specified, it defaults to **SDP-K1**.

Running/Debugging
-----------------

The running and debugging steps are the same as on Linux. See the Linux section
above for ``make run``.
