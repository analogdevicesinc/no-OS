AD9545 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD9545`

Overview
--------

The AD9545 is a flexible, quad-input, 10-output DPLL synchronizer and
jitter cleaner designed for telecommunications and networking,
supporting ITU synchronization standards such as ITU-G.8262 and ITU-T
G.812. It synchronizes up to 10 clock outputs from four input
references, ensuring stable operation despite input reference failures.
Key features include programmable reference switching, fast DPLL
locking, and optional use of a crystal resonator or oscillator for
frequency stability. Operating within -40°C to +85°C, it comes in a
48-lead LFCSP package, optimized for compact and advanced designs.
Applications encompass jitter cleanup and synchronization in GPS, PTP
(IEEE 1588), synchronous Ethernet, optical transport networks, and base
station clocking solutions.

Applications
------------

- Global positioning system (GPS), PTP (IEEE 1588), and synchronous
  Ethernet (SyncE) jitter cleanup and synchronization
- Optical transport networks (OTN), synchronous digital hierarchy (SDH),
  and macro and small cell base stations
- Small base station clocking, including baseband and radio Stratum 2,
  Stratum 3e, and Stratum 3 holdover, jitter cleanup, and phase
  transient control
- JESD204B support for analog-to-digital converter (ADC) and
  digital-to-analog converter (DAC) clocking
- Cable infrastructures
- Carrier Ethernet

Operation Modes
---------------

+-----------------+-----------------+--------------------------+-----------------+
| Mode Name       | Description     | Configuration            | Typical Use     |
|                 |                 | Bits / Registers         | Case            |
+-----------------+-----------------+--------------------------+-----------------+
| Active          | The standard    | AD9545_PLL_ACTIVE        | Used in         |
| Closed-Loop     | operational     |                          | scenarios where |
| Operation       | mode where the  |                          | constant        |
|                 | PLL actively    |                          | frequency       |
|                 | adjusts to      |                          | alignment is    |
|                 | maintain lock   |                          | vital, such as  |
|                 | with the        |                          | in data         |
|                 | reference       |                          | transport.      |
|                 | input.          |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Holdover        | Maintains       | AD9545_PLL_HOLDOVER      | Useful in       |
| Operation       | output          |                          | temporary       |
|                 | frequency using |                          | disruptions in  |
|                 | last known good |                          | input reference |
|                 | reference data  |                          | to maintain     |
|                 | when inputs     |                          | system          |
|                 | fail.           |                          | operation.      |
+-----------------+-----------------+--------------------------+-----------------+
| Free-Run Mode   | The PLL outputs | AD9545_PLL_FREERUN       | Applied in      |
|                 | a frequency     |                          | testing or      |
|                 | based on its    |                          | systems         |
|                 | internal        |                          | requiring       |
|                 | settings        |                          | stable outputs  |
|                 | without locking |                          | without active  |
|                 | to a reference. |                          | references.     |
+-----------------+-----------------+--------------------------+-----------------+
| Autosync        | Synchronizes    | Register 0x10B/0x14B     | Optimizes       |
| Trigger Mode    | outputs using   |                          | output          |
|                 | triggers (e.g., |                          | synchronization |
|                 | immediate, on   |                          | after reference |
|                 | phase lock).    |                          | recovery or     |
|                 |                 |                          | during          |
|                 |                 |                          | reconfiguration.|
+-----------------+-----------------+--------------------------+-----------------+
| Single Divider  | Configures      | AD9545_SINGLE_DIV        | Integrated in   |
| Mode            | outputs for     |                          | systems         |
|                 | single          |                          | requiring       |
|                 | frequency       |                          | simple          |
|                 | division.       |                          | frequency       |
|                 |                 |                          | division for    |
|                 |                 |                          | clock           |
|                 |                 |                          | distribution.   |
+-----------------+-----------------+--------------------------+-----------------+
| Dual Divider    | Configures      | AD9545_DUAL_DIV          | Used in         |
| Mode            | outputs for     |                          | multi-phase or  |
|                 | dual frequency  |                          | dual-path clock |
|                 | division,       |                          | structures for  |
|                 | optimizing      |                          | balanced load   |
|                 | resource use in |                          | distribution.   |
|                 | complex setups. |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Automatic,      | Relies on       | DPLLx translation        | Automatically   |
| Priority-Based  | automatic       | profile select mode = 0  | determines the  |
| Translation     | priority        |                          | best profile    |
| Profile         | selection       |                          | without manual  |
| Selection       | process for     |                          | intervention.   |
|                 | choosing a      |                          |                 |
|                 | translation     |                          |                 |
|                 | profile.        |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Manual          | User specifies  | DPLLx translation        | Use when manual |
| Translation     | a translation   | profile select mode = 1  | control is      |
| Profile         | profile         |                          | needed but      |
| Selection with  | manually. If    |                          | priority        |
| Fallback to     | the profile is  |                          | fallback        |
| Priority-Based  | not valid, it   |                          | provides        |
| Selection       | reverts to      |                          | backup.         |
|                 | priority-based  |                          |                 |
|                 | selection       |                          |                 |
|                 | process.        |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Manual          | User manually   | DPLLx translation        | When specific   |
| Translation     | selects a       | profile select mode = 2  | profiles must   |
| Profile         | translation     |                          | be selected     |
| Selection with  | profile, but if |                          | manually, with  |
| Fallback to     | the profile is  |                          | holdover as an  |
| Holdover        | invalid,        |                          | error fallback. |
|                 | switches to     |                          |                 |
|                 | holdover        |                          |                 |
|                 | operation.      |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Manual Only     | Requires manual | DPLLx translation        | Used when       |
| Translation     | profile         | profile select mode = 3  | absolute manual |
| Profile         | management with |                          | control is      |
| Selection       | significant     |                          | essential, but  |
|                 | caveats.        |                          | requires expert |
|                 |                 |                          | knowledge.      |
+-----------------+-----------------+--------------------------+-----------------+
| Phase Buildout  | Used to         | Profile index bit fields | Applications    |
| Mode            | calibrate and   | internally               | where precise   |
|                 | achieve locked  |                          | phase control   |
|                 | state for       |                          | is critical,    |
|                 | active          |                          | like network    |
|                 | profiles,       |                          | timing          |
|                 | providing       |                          | solutions.      |
|                 | specific        |                          |                 |
|                 | control over    |                          |                 |
|                 | phase           |                          |                 |
|                 | transition      |                          |                 |
|                 | processes.      |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+
| Internal Zero   | Allows          | Use register Bit D0 to   | Ideal for       |
| Delay (Hitless) | transitioning   | enable Profile hitless   | environments    |
| Mode            | without phase   | mode                     | needing minimal |
|                 | buildout for    |                          | disturbance     |
|                 | seamless        |                          | during phase    |
|                 | operation.      |                          | transitions.    |
+-----------------+-----------------+--------------------------+-----------------+
| External Zero   | Similar to      | Use register Bit D1 for  | Used when       |
| Delay (Hitless) | internal mode   | enabling Profile         | external sync   |
| Mode            | but for         | external zero delay      | is necessary    |
|                 | external        |                          | without phase   |
|                 | systems.        |                          | delays.         |
|                 |                 |                          |                 |
+-----------------+-----------------+--------------------------+-----------------+

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The essential functions for initializing the AD9545 includes ``ad9545_setup``, 
``ad9545_sys_clk_setup``, and ``ad9545_input_refs_setup``. 
These functions configure system clocks, input references, and auxiliary NCOs
to ensure the device meets system requirements. Additional setups are handled 
by ``ad9545_aux_tdcs_setup`` and ``ad9545_aux_ncos_setup`` for initializing 
auxiliary components like NCOs and TDCs.

Configuration
~~~~~~~~~~~~~

Configuration functions such as ``ad9545_set_r_div``,
``ad9545_set_nco_center_freq``, and ``ad9545_set_q_div`` allow customization
of parameters like dividers and frequencies. These configurations
optimize device performance, enabling features such as zero-delay mode
for synchronization and jitter reduction.

Communication Protocol
~~~~~~~~~~~~~~~~~~~~~~

The driver includes functions for configuring communication interfaces
based on the protocol used, either I2C or SPI. Key functions such as
``ad9545_i2c_reg_read``, ``ad9545_i2c_reg_write``, ``ad9545_spi_reg_read``,
and ``ad9545_spi_reg_write`` handle the reading and writing of device
registers, establishing necessary references to hardware communication
layers.

Data Handling and Register Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Low-level register operations are crucial for interfacing with the
AD9545. Functions like ``ad9545_read_reg``, ``ad9545_write_reg``, and
``ad9545_write_mask`` enable precise control over device registers,
facilitating dynamic updates needed during operation.

Clock Management
~~~~~~~~~~~~~~~~

Clock management functions, including ``ad9545_in_clk_recalc_rate`` and
``ad9545_get_nco_freq_hz``, manage recalibration of clock rates and
frequency settings, which are vital for accurate frequency generation
and adjustments in telecommunications systems.

Auxiliary Features Setup
~~~~~~~~~~~~~~~~~~~~~~~~

Auxiliary setup functions such as ``ad9545_aux_dpll_setup`` and
``ad9545_aux_tdcs_setup`` configure additional components like DPLLs and
TDCs. These setups enhance the device’s performance and versatility in
complex applications.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "common_data.h"
   #include "ad9545.h"
   #include "no_os_clk.h"

   struct no_os_spi_init_param ad9545_spi_ip = {
       .device_id = 0,
       .max_speed_hz = 1000000,
       .mode = NO_OS_SPI_MODE_0,
       .platform_ops = &linux_spi_ops,
       .chip_select = 0,
       .extra = NULL
   };

   struct no_os_i2c_init_param ad9545_i2c_ip = {
       .device_id = 1,
       .max_speed_hz = 400000,
       .slave_address = 0x48,
       .platform_ops = &linux_i2c_ops,
       .extra = &ad9545_i2c_extra_ip
   };

   struct ad9545_init_param ad9545_ip = {
       .spi_init = &ad9545_spi_ip,
       .i2c_init = &ad9545_i2c_ip,
       .comm_type = COMM_TYPE,
   };

   int main() {
       int ret;
       struct ad9545_dev *ad9545_dev;

       ret = ad9545_init(&ad9545_dev, &ad9545_ip);
       if (ret < 0)
           return ret;

       ret = ad9545_setup(ad9545_dev);
       if (ret < 0)
           return ret;

       printf("AD9545 initialization complete.\n");
       return 0;
   }