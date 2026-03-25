Display Demo no-OS Example Project
==================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Overview
--------

The Display Demo project demonstrates driving an SSD1306 OLED display
from a Xilinx Zedboard using the no-OS SPI and GPIO drivers. The
application initializes the SSD1306 display controller, powers on the
display via dedicated VDD and VBAT GPIO controls, and prints a text
message on screen.

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/display_demo/src/app>`__.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* SSD1306 OLED Display
* `Zedboard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # copy the Xilinx hardware description file
   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
