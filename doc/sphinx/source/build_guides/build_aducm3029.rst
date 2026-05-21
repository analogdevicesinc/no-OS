******************************
ADuCM3029 Build Prerequisites
******************************

Prior to building a no-OS project for ADuCM3029 platforms, it is required to
install CrossCore Embedded Studio and the necessary device packs.

Linux
=====

Prerequisites
-------------

- Install CrossCore Embedded Studio 2.10.
- Manually install **ADuCM302x Device Family Pack (DFP 3.2.0+)**.
- Manually install **ARM.CMSIS pack (5.7.0+)**.
- Make sure you don't have multiple versions of ADuCM302x DFP and ARM CMSIS
  packs installed.

.. important::
   Please install all the necessary packs locally and then manually import them
   in CrossCore.

Common issues with environment setup:

- Makefiles search for the CCES_HOME in its default installation directory. It
  may happen that multiple versions are installed and may not work. To select
  a CCES_HOME run:

    .. code-block:: bash

        $ export CCES_HOME=/opt/analog/cces/2.10.0

Building a project
------------------

The ADuCM3029 projects also contain a ``pinmux_config.c`` file which contains
pin configuration instructions.

.. code-block:: bash

    # build an ADuCM3029-only project
    $ make

    # if the platform autodetection picks the wrong platform, explicitly specify
    $ make PLATFORM=aducm3029

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

- Install CrossCore Embedded Studio to a path without whitespaces such as
  ``C:\ADI\cces2.11.1``.
- Manually install **ADuCM302x Device Family Pack (DFP 3.2.0+)**.
- Manually install **ARM.CMSIS pack (5.7.0+)**.
- Make sure you don't have multiple versions of ADuCM302x DFP and ARM CMSIS
  packs installed.
- Set the CCES_HOME environment variable to point to the CrossCore Embedded
  Studio installation directory:

    .. code-block:: bash

        $ export CCES_HOME=/c/ADI/cces2.11.1

Building a project
------------------

.. code-block:: bash

    $ export PLATFORM=aducm3029
    $ make

Running/Debugging
-----------------

Once the **.elf** file has been generated, make sure the board is powered on,
cable connected and use the following commands to upload the program to the
board:

.. code-block:: bash

    $ make run
