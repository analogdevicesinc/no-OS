ad9081 no-OS Example Project
============================

.. no-os-doxygen::

Supported Evaluation Boards
---------------------------

* `EVAL-AD9081 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9081.html>`_
* `Quad MxFE <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/quad-mxfe.html>`_

Supported Carriers
------------------

.. list-table::
   :widths: 35 35 30
   :header-rows: 1

   * - Evaluation board
     - Carrier
     - FMC slot
   * - `AD9081-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9081.html>`_
     - `VCK190 <https://www.xilinx.com/VCK190>`_
     - FMC0
   * -
     - `VCU118 <https://www.xilinx.com/VCU118>`_
     - FMC+
   * -
     - `VCU128 <https://www.xilinx.com/VCU128>`_
     - FMC+
   * -
     - `ZCU102 <https://www.xilinx.com/ZCU102>`_
     - FMC HPC0
   * -
     - `ZC706 <https://www.xilinx.com/ZC706>`_
     - FMC HPC

.. list-table::
   :widths: 35 35 30
   :header-rows: 1

   * - Evaluation board
     - Carrier
     - FMC slot
   * - `QUAD-MXFE <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/quad-mxfe.html>`_
     - `VCU118 <https://www.xilinx.com/VCU118>`_
     - FMC+

Overview
--------

The AD9081 is a highly integrated, multi-channel mixed-signal front-end (MxFE™)
device designed to deliver a complete radio solution for a wide range of applications,
including 5G wireless infrastructure, broadband communication, and defense electronics.
It combines multiple high-speed ADCs and DACs, a digital up-converter (DUC) and
digital down-converter (DDC), and integrated digital signal processing (DSP) blocks.

The Quad-MxFE System Development Platform contains four MxFE™ software defined,
direct RF sampling transceivers, as well as associated RF front-ends, clocking,
and power circuitry. The target application is phased array radars, electronic
warfare, and ground-based SATCOM, specifically a 16 transmit/16 receive channel
direct sampling phased array at L/S/C band (0.1 GHz to ~5GHz). The Rx & Tx RF
front-end has drop-in configurations that allow for customized frequency ranges,
depending on the user’s application.

Prerequisites
-------------

Prior to building the project, the environment for the development of Xilinx
projects must be set up. These are presented in the *Build Prerequisites*
section of no-OS build guide available :dokuwiki:`here <resources/no-os/build>`.

Reference Input Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For the AD9081-EVALZ, the board can either use an external clock source or the
internal one.

.. note::
  The following rework is required:
   - In order to avoid using an external clock source and fully rely on the
     HMC7044 clock chip, rotate the C6D/C4D caps in C5D/C3D position
     (Please note: In the latest version of the board, this is now the default
     configuration, so this configuration step might not be needed anymore)
   - If LEDS V1P0_LED and VINT_LED are not on please depopulate R22M and populate R2M

For the Quad-MxFE, an additional 500MHz Reference Oscillator or Waveform Generator
is required.

Building and Running the Project
--------------------------------

The steps indicated have to be followed for building the project, for debugging,
and running.

#. Open a terminal and navigate to this project directory (if building on
   Windows, `Git Bash` has to be used).
#. Make sure that the environment is set up for building Xilinx projects.
#. Make sure you have copied the corresponding `system_top.xsa` file of your project to the
   `projects/ad9081/` directory, because it is necessary for the build process.
#. Make sure that the `Makefile` is configured correctly for your platform.
   The default configuration is for the `AD9081-FMCA-EBZ` evaluation board, with
   the profile specified in profiles/vcu118_ad9081_m8_l4/app_config.h:

   .. code-block:: bash

     JESD_MODE=8B10B \ #JESD204B, subclass 1
     TX_MODE=9 \
     RX_MODE=10 \
     RX_LANE_RATE_KHZ=10000000 \ 
     TX_LANE_RATE_KHZ=10000000 \
     RX_JESD_M=8 \
     RX_JESD_L=4 \
     RX_JESD_F=4 \
     RX_JESD_K=32 \
     RX_JESD_S=1 \
     RX_JESD_NP=16 \
     RX_NUM_LINKS=1 \
     TX_JESD_M=8 \
     TX_JESD_L=4 \
     TX_JESD_F=4 \
     TX_JESD_K=32 \
     TX_JESD_S=1 \
     TX_JESD_NP=16 \
     TX_NUM_LINKS=1 \

#. If you want to use the `Quad-MxFE` you can do so with one of the available
   profiles in the `profiles` directory, or you can create your own profile
   by copying one of the existing ones and modifying it. To build the project
   this way you can run the following command:

    .. code-block:: bash

      make -j QUAD_MXFE=y PROFILE=vcu118_quad_ad9081_204b_txmode_9_rxmode_10_revc
#. Otherwise, type ``make -j`` in order to build the project.

A successful build should end with the following terminal output:

.. code-block:: bash

  [14:15:11] Creating archive with files
    text    data     bss     dec     hex filename
  240676    2092 1057008 1299776  13d540 /home/ramona/workspace/no-OS/projects/ad9081/build/ad9081.elf
  [14:14:38] Done (build/ad9081.elf)

Available profiles:
  * vcu118_ad9081_m8_l4 - default profile
  * vcu118_quad_ad9081_204b_txmode_9_rxmode_10_revc - Quad-MxFE profile
    .. code-block:: bash
  
      JESD_MODE=8B10B \ #JESD204B, subclass 1
      TX_MODE=9 \
      RX_MODE=10 \
      RX_LANE_RATE_KHZ=10000000 \ 
      TX_LANE_RATE_KHZ=10000000 \
      RX_JESD_M=8 \
      RX_JESD_L=4 \
      RX_JESD_F=4 \
      RX_JESD_K=32 \
      RX_JESD_S=1 \
      RX_JESD_NP=16 \
      RX_NUM_LINKS=4 \
      TX_JESD_M=8 \
      TX_JESD_L=4 \
      TX_JESD_F=4 \
      TX_JESD_K=32 \
      TX_JESD_S=1 \
      TX_JESD_NP=16 \
      TX_NUM_LINKS=4 \
  * vcu118_quad_ad9081_204c_txmode_11_rxmode_4_revc - Quad-MxFE profile
    .. code-block:: bash
  
      JESD_MODE=64B66B \ #JESD204C, subclass 1
      TX_MODE=11 \
      RX_MODE=4 \
      RX_LANE_RATE_KHZ=16500000 \ 
      TX_LANE_RATE_KHZ=16500000 \
      RX_JESD_M=8 \
      RX_JESD_L=2 \
      RX_JESD_F=8 \
      RX_JESD_K=32 \
      RX_JESD_S=1 \
      RX_JESD_NP=16 \
      RX_NUM_LINKS=4 \
      TX_JESD_M=16 \
      TX_JESD_L=4 \
      TX_JESD_F=8 \
      TX_JESD_K=32 \
      TX_JESD_S=1 \
      TX_JESD_NP=16 \
      TX_NUM_LINKS=4 \

These profiles correspond some of the possible HDL configurations. For creating
a profile for your specific project needs, please visit
:git-hdl:`QUAD-MxFE hdl <projects/ad_quadmxfe1_ebz>` or 
:git-hdl:`QUAD-MxFE hdl <projects/ad9081_fmca_ebz>` 
documentation files for your platform, as well as `the AD9081 user guide <https://www.analog.com/media/en/technical-documentation/user-guides/ad9081-ad9082-ug-1578.pdf>` 

Fore more details about the available make rules, check out
:dokuwiki:`this page <resources/no-os/make>`.

.. note::
  The QUAD-MXFE project is configured for rev C boards, in case of older revisions,
  please check out older releases.

Running the Project
-------------------
Once the project is built, you can run it on your target platform. You can either
run using Vitis or by using the command line:
... code-block:: bash

  make run

No-OS Supported Examples
------------------------

The various example modes can be selected via Makefile, or when running the make
command while buidling the project

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	IIOD ?= n
  QUAD_MXFE = n

  # Uncomment to select the profile:
  #PROFILE = vcu118_quad_ad9081_204c_txmode_11_rxmode_4_revc
  #PROFILE = vcu118_quad_ad9081_204b_txmode_9_rxmode_10_revc
  PROFILE = vcu118_ad9081_m8_l4

or
.. code-block:: bash

  make run IIOD=y QUAD_MXFE=y PROFILE=vcu118_quad_ad9081_204b_txmode_9_rxmode_10_revc

AD9081 example
^^^^^^^^^^^^^^
The AD9081 example is a simple application that initializes the AD9081 device, 
in case IIOD is enabled, it will also configure the IIO interface and you can
use `libiio <https://wiki.analog.com/resources/tools-software/linux-software/libiio>`_ command line tools / `IIO-Oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope/releases>`_ / `Scopy2 v2 <https://github.com/analogdevicesinc/scopy/releases/tag/v2.0.0>`_ to control the device.

QUAD-MXFE example
^^^^^^^^^^^^^^^^^
In this example all four AD9081 devices are initialized and configured, in order
to be able to fully use the eval board.

.. important::

   The QUAD-MXFE project is configured for rev C boards and later, which implies 
   the use of specific HMC7043 channels (as opposed to the rev B boards), along
   with some specific pin configurations for AD9081. 

Just like in the case of single MxFE, if IIOD is enabled, it will also provide
the iio support so that the board can be used with any of the ADI iio tools:
`libiio <https://wiki.analog.com/resources/tools-software/linux-software/libiio>`_ command line tools / `IIO-Oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope/releases>`_ / `Scopy v2 <https://github.com/analogdevicesinc/scopy/releases/tag/v2.0.0>`_ to control the device.
