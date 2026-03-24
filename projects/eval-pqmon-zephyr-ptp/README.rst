EVAL-PQMON Zephyr PTP — Build & Setup Guide
=============================================

Power Quality Monitor on AD-PQMON-SL board with IEEE 1588 PTP synchronization
over 10BASE-T1L (ADIN1110), running on Zephyr RTOS.

Hardware
--------

- **Board**: AD-PQMON-SL (2x boards for PTP master/slave setup)
- **MCU**: MAX32650 (ARM Cortex-M4F)
- **AFE**: ADE9430 on SPI1 — power quality analog frontend
- **Network**: ADIN1110 on SPI2 — 10BASE-T1L Ethernet with hardware PTP clock
- **RTC**: MAX31343 on I2C
- **Connection**: Two boards connected via 10BASE-T1L (single twisted pair)

Features
--------

- IEC 61000-4-30 Class S power quality measurements via PQLib
- IEEE 1588 PTP synchronization (<100ns offset between boards)
- Hardware syntonized counter timestamps (TS_ADDEND frequency control)
- 1PPS output via TS_TIMER for cross-board AFE cycle alignment
- IIO (libiio) interface over TCP and USB CDC/ACM
- UART2 CSV export at 460800 baud (1Hz)
- NVS flash calibration storage
- Synchronized AFE restart via UDP + PTP 1PPS

Prerequisites
-------------

Host System
^^^^^^^^^^^

Tested on Ubuntu 22.04 LTS (x86_64). Other Linux distributions should work
with equivalent packages.

Install base dependencies::

    sudo apt update
    sudo apt install --no-install-recommends \
        git cmake ninja-build gperf \
        ccache dfu-util device-tree-compiler wget \
        python3-dev python3-pip python3-setuptools python3-tk python3-wheel \
        xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev \
        libmagic1 picocom minicom

Zephyr SDK 0.17.0
^^^^^^^^^^^^^^^^^^

Download and install the Zephyr SDK::

    cd ~
    wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.17.0/zephyr-sdk-0.17.0_linux-x86_64.tar.xz
    tar xf zephyr-sdk-0.17.0_linux-x86_64.tar.xz
    cd zephyr-sdk-0.17.0
    ./setup.sh

Add to your shell profile (``~/.bashrc`` or ``~/.zshrc``)::

    export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
    export ZEPHYR_SDK_INSTALL_DIR=$HOME/zephyr-sdk-0.17.0

West and Python Dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Install west (Zephyr's meta-tool)::

    pip3 install --user west

Ensure ``~/.local/bin`` is in your ``PATH``::

    export PATH="$HOME/.local/bin:$PATH"

Zephyr Workspace Setup
-----------------------

Initialize the Zephyr workspace::

    mkdir ~/zephyrproject
    cd ~/zephyrproject
    west init --mr v4.3.0-4613-ga04d8957c21
    west update
    pip3 install -r zephyr/scripts/requirements.txt

.. note::

    The exact Zephyr revision used is ``v4.3.0-4613-ga04d8957c21``
    (commit ``a04d8957c21``). Using a different version may require
    re-adapting the patches.

Alternatively, if you want the v4.0 branch tip::

    west init
    west update

But then you must verify the patches apply cleanly.

Project Installation
--------------------

Copy the project into the Zephyr workspace::

    cp -r <path-to-no-OS>/projects/eval-pqmon-zephyr-ptp \
          ~/zephyrproject/pqmon_zephyr_iio_t1l

Install PQLib (Closed Source)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ADI Power Quality Library (PQLib) is not included in this repository.
You must obtain it separately from Analog Devices and place the files in
the project's ``lib/pqlib/`` directory.

Contact your ADI representative or download from the ADI website to obtain:

- ``libadi_pqlib_cm4_gcc.a`` — precompiled static library (Cortex-M4, soft-float ABI)
- Header files (``include/`` directory)

The expected directory layout after installation::

    pqmon_zephyr_iio_t1l/lib/pqlib/
    ├── libadi_pqlib_cm4_gcc.a
    └── include/
        ├── ade9430.h
        ├── adi_pqlib.h
        ├── adi_pqlib_cfg.h
        ├── adi_pqlib_debug.h
        ├── adi_pqlib_error.h
        ├── adi_pqlib_memory.h
        ├── adi_pqlib_profile.h
        └── adi_pqlib_version.h

.. note::

    The project builds and runs with the unmodified PQLib distribution.
    No changes to the PQLib headers or library are required.
    The ``adi_pqlib_cfg.h`` from the PQLib distribution can be used as-is.

Install Board Definition
^^^^^^^^^^^^^^^^^^^^^^^^

The AD-PQMON-SL board definition must be placed in the Zephyr tree::

    mkdir -p ~/zephyrproject/zephyr/boards/adi/ad_pqmon_sl
    cp ~/zephyrproject/pqmon_zephyr_iio_t1l/patches/board_ad_pqmon_sl/* \
       ~/zephyrproject/zephyr/boards/adi/ad_pqmon_sl/

Apply Zephyr Patches
^^^^^^^^^^^^^^^^^^^^

Two Zephyr source files require modifications:

1. **eth_adin2111.c** — Adds hardware PTP frame timestamp extraction
   (8-byte big-endian seconds+nanoseconds parsing) and calls
   ``adin1110_ptp_update_hw_time()`` on every RX/TX timestamp capture.

2. **ptp/clock.c** — Reduces the PTP step threshold from 1s to 100ms.
   The ADIN1110 TS_ADDEND supports +/-10% frequency adjustment; offsets
   above 100ms produce PI servo ratios that cannot converge. Also adds
   offset+delay to log messages.

Apply both patches from the Zephyr root::

    cd ~/zephyrproject/zephyr
    git apply ~/zephyrproject/pqmon_zephyr_iio_t1l/patches/zephyr_eth_adin2111_ptp.patch
    git apply ~/zephyrproject/pqmon_zephyr_iio_t1l/patches/zephyr_ptp_clock_step_threshold.patch

Verify the patches applied::

    git diff --stat
    # Should show:
    #  drivers/ethernet/eth_adin2111.c | 298 ++++++++++++++++++++++++++++-
    #  subsys/net/lib/ptp/clock.c      |  16 +-

Building
--------

All build commands run from the workspace root (``~/zephyrproject``).

IIO Transport Selection
^^^^^^^^^^^^^^^^^^^^^^^^

The firmware supports two IIO transport options, selected at build time
via Kconfig:

============================  =================================================
Transport                     Description
============================  =================================================
``CONFIG_IIO_TRANSPORT_USB``  IIO over USB CDC/ACM (default). Host connects
                              via ``/dev/ttyACMx``. Requires USB cable to each
                              board. Independent of T1L network.
``CONFIG_IIO_TRANSPORT_TCP``  IIO over TCP port 30431 on the 10BASE-T1L link.
                              Host connects via ``ip:192.0.2.x``. Requires the
                              host to have an IP on the same subnet (e.g. via
                              a T1L-to-Ethernet media converter).
============================  =================================================

The default is USB (set in ``Kconfig`` and ``prj.conf``). To switch to
TCP over T1L, add the Kconfig override to the build command::

    -DCONFIG_IIO_TRANSPORT_TCP=y -DCONFIG_IIO_TRANSPORT_USB=n

Or create a conf overlay file (e.g. ``prj_tcp.conf``)::

    # Use TCP IIO transport instead of USB
    CONFIG_IIO_TRANSPORT_USB=n
    CONFIG_IIO_TRANSPORT_TCP=y

    # USB device stack not needed for TCP-only builds
    # CONFIG_USB_DEVICE_STACK_NEXT=n
    # CONFIG_USBD_CDC_ACM_CLASS=n

And append it to the build::

    -DCONF_FILE="prj.conf;prj_master.conf;prj_tcp.conf"

.. note::

    The two transports are mutually exclusive (Kconfig ``choice``).
    PTP synchronization and the DAPLink serial console work regardless
    of which IIO transport is selected.

Build Commands — USB IIO (Default)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Master (Board 1)::

    cd ~/zephyrproject
    west build -p -b ad_pqmon_sl pqmon_zephyr_iio_t1l \
        -- -DCONF_FILE="prj.conf;prj_master.conf" \
           -DDTC_OVERLAY_FILE=boards/ad_pqmon_sl_board1.overlay

Slave (Board 2)::

    cd ~/zephyrproject
    west build -p -b ad_pqmon_sl pqmon_zephyr_iio_t1l \
        -- -DCONF_FILE="prj.conf;prj_slave.conf" \
           -DDTC_OVERLAY_FILE=boards/ad_pqmon_sl_board2.overlay

Build Commands — TCP IIO over T1L
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Master (Board 1)::

    cd ~/zephyrproject
    west build -p -b ad_pqmon_sl pqmon_zephyr_iio_t1l \
        -- -DCONF_FILE="prj.conf;prj_master.conf" \
           -DDTC_OVERLAY_FILE=boards/ad_pqmon_sl_board1.overlay \
           -DCONFIG_IIO_TRANSPORT_TCP=y -DCONFIG_IIO_TRANSPORT_USB=n

Slave (Board 2)::

    cd ~/zephyrproject
    west build -p -b ad_pqmon_sl pqmon_zephyr_iio_t1l \
        -- -DCONF_FILE="prj.conf;prj_slave.conf" \
           -DDTC_OVERLAY_FILE=boards/ad_pqmon_sl_board2.overlay \
           -DCONFIG_IIO_TRANSPORT_TCP=y -DCONFIG_IIO_TRANSPORT_USB=n

.. note::

    The ``-p`` flag does a pristine build (full rebuild). Omit it for
    incremental builds after the first successful build.

Flashing
--------

Connect the board via DAPLink USB and flash::

    west flash

For two-board setups, flash each board separately. After each flash/reset,
set USB CDC/ACM permissions::

    sudo chmod 666 /dev/ttyACM2 /dev/ttyACM3

USB Port Mapping (Two Boards Connected)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

==========  ==========================================
Port        Description
==========  ==========================================
ttyACM0     Board 1 DAPLink serial console (master)
ttyACM1     Board 2 DAPLink serial console (slave)
ttyACM2     Board 1 IIO CDC/ACM (master)
ttyACM3     Board 2 IIO CDC/ACM (slave)
==========  ==========================================

.. note::

    Port assignments may vary. Check ``dmesg`` after connecting to confirm.

Network Configuration
---------------------

The boards use static IPv4 addresses on the 10BASE-T1L link:

- **Master**: 192.0.2.1/24
- **Slave**: 192.0.2.2/24

PTP synchronization starts automatically. The master becomes GRAND_MASTER
and the slave becomes TIME_RECEIVER via the IEEE 1588 BMCA.

IIO Interface
-------------

Access measurements via libiio (version 0.25 recommended).

Install libiio on the host::

    sudo apt install libiio-utils
    # Or build from source: https://github.com/analogdevicesinc/libiio

USB IIO (CONFIG_IIO_TRANSPORT_USB)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The firmware exposes a USB CDC/ACM serial port. After connecting the
board via USB, the IIO endpoint appears as ``/dev/ttyACMx``.

Set permissions after each flash/reset::

    sudo chmod 666 /dev/ttyACM2 /dev/ttyACM3

Connect to master (Board 1)::

    iio_info -u serial:/dev/ttyACM2,115200,8n1

Connect to slave (Board 2)::

    iio_info -u serial:/dev/ttyACM3,115200,8n1

Read channel attributes::

    URI="serial:/dev/ttyACM2,115200,8n1"
    iio_attr -u $URI -c pqm voltage0 rms

TCP IIO over T1L (CONFIG_IIO_TRANSPORT_TCP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The firmware listens on TCP port 30431 on the 10BASE-T1L network.
The host must have an IP address on the same 192.0.2.0/24 subnet
(e.g. via a T1L-to-Ethernet media converter or a third ADIN1110 adapter).

Connect to master::

    iio_info -u ip:192.0.2.1

Connect to slave::

    iio_info -u ip:192.0.2.2

Read channel attributes::

    URI="ip:192.0.2.1"
    iio_attr -u $URI -c pqm voltage0 rms

Calibration (works with either transport)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    # Set URI to match your transport:
    URI="serial:/dev/ttyACM2,115200,8n1"    # USB
    # URI="ip:192.0.2.1"                    # TCP

    iio_attr -u $URI -D pqm cal_start 1       # start calibration
    iio_attr -u $URI -D pqm cal_status         # poll: RUNNING -> DONE
    iio_attr -u $URI -D pqm flash_cal_save 1   # save to NVS flash
    iio_attr -u $URI -D pqm flash_cal_load 1   # load from flash

UART2 CSV Export
----------------

Connect to UART2 at 460800 baud for 1Hz CSV data output::

    picocom -b 460800 /dev/ttyUSB0

CSV format::

    PQM,VA,VB,VC,IA,IB,IC,IN,THD_V*3,THD_I*3,angles*4,
    harmonics(6x49),event_counts(4),event_details(4x3),
    lifetime_totals(4),power/energy/pf(8x3)\r\n

Cross-Board Event Correlation Test
-----------------------------------

With both boards connected and running::

    python3 tools/pqmon_timestamp_test.py \
        --uri serial:/dev/ttyACM2,115200,8n1 \
        --uri2 serial:/dev/ttyACM3,115200,8n1 \
        --continuous --interval 2.0 --event-interval 1

Project Structure
-----------------

::

    eval-pqmon-zephyr-ptp/
    ├── CMakeLists.txt              # Build system
    ├── Kconfig                     # Kconfig options
    ├── prj.conf                    # Shared Zephyr configuration
    ├── prj_master.conf             # Master overrides (IP, PTP priority)
    ├── prj_slave.conf              # Slave overrides (IP, PTP priority)
    ├── boards/
    │   ├── ad_pqmon_sl_board1.overlay  # DT overlay for board 1
    │   └── ad_pqmon_sl_board2.overlay  # DT overlay for board 2
    ├── drivers/usb_compat/
    │   ├── mcr_regs.h              # MAX32650 USB register compat shim
    │   └── wrap_max32_usb.h        # USB HAL wrapper
    ├── dts/bindings/
    │   ├── rtc/adi,max31343.yaml   # MAX31343 RTC binding
    │   └── sensor/adi,ade9430.yaml # ADE9430 AFE binding
    ├── lib/pqlib/                  # NOT INCLUDED — obtain from ADI
    │   ├── libadi_pqlib_cm4_gcc.a  #   (see "Install PQLib" section)
    │   └── include/                #   Place headers + library here
    ├── patches/
    │   ├── board_ad_pqmon_sl/      # Board definition (copy to Zephyr tree)
    │   ├── zephyr_eth_adin2111_ptp.patch
    │   └── zephyr_ptp_clock_step_threshold.patch
    ├── src/
    │   ├── main.c                  # Boot sequence, main loop
    │   ├── pqlib_example.c/.h      # AFE thread, PQLib processing
    │   ├── pqlib_afe.c/.h          # AFE data polling, timestamps
    │   ├── afe_config.c/.h         # ADE9430 init, power/energy reads
    │   ├── afe_calibration.c/.h    # Calibration routines
    │   ├── afe_sync.c/.h           # Cross-board AFE sync (UDP + 1PPS)
    │   ├── pqlib_convert.c/.h      # Unit conversions (RMS, power, PF)
    │   ├── ptp_clock_adin1110_zephyr.c  # PTP clock driver (HW timestamps)
    │   ├── ptp_clock_adin1110_hw.h # PTP driver API header
    │   ├── ptp_status.c/.h         # PTP sync status query
    │   ├── adin1110_early_init.c   # ADIN1110 bootstrap (MSSEL pin)
    │   ├── rtc_max31343.c/.h       # MAX31343 RTC driver
    │   ├── iio_pqm.c/.h           # IIO device attrs and channels
    │   ├── iio_server.c/.h        # IIOD server (TCP + USB)
    │   ├── uart_export.c/.h       # UART2 CSV export
    │   ├── flash_storage.c/.h     # NVS calibration storage
    │   ├── no_os_shim.c/.h        # no-OS API shim for Zephyr
    │   ├── pqm.h                  # Channel attribute defines
    │   ├── status.h               # Status codes
    │   └── iio/                   # IIO framework (iiod, types)
    └── tools/
        └── pqmon_timestamp_test.py # Cross-board test script

PTP Synchronization Architecture
---------------------------------

The PTP implementation uses the ADIN1110 hardware syntonized counter
exclusively — no software time extrapolation.

**Hardware registers used:**

- ``TS_ADDEND`` (0x80): Frequency control — PI servo writes this to
  syntonize the slave's counter to the master's crystal
- ``TS_SEC_CNT`` / ``TS_NS_CNT`` (0x82/0x83): Write-only time set
  (step corrections for offsets > 100ms)
- ``TS_CFG`` (0x84): Counter enable/clear/timer control
- ``TS_1SEC_CMP`` (0x81): Nanosecond rollover at 1,000,000,000
- ``TS_TIMER`` (0x85-0x88): 1PPS output signal generation
- ``TS_CAPT`` (0x89-0x8A): On-demand counter capture via GPIO pulse
- ``CONFIG0`` FTSE/FTSS: Frame timestamp enable (8-byte PTP format)

**Sync flow:**

1. ADIN1110 stamps every RX/TX frame with syntonized counter value
2. Zephyr PTP stack computes offset via IEEE 1588 protocol
3. PI servo feeds offset into ``rate_adjust()`` → writes ``TS_ADDEND``
4. Large offsets (>100ms) corrected via ``clock_set()`` → writes
   ``TS_SEC_CNT`` / ``TS_NS_CNT``
5. After convergence (<200ns offset), TS_TIMER 1PPS is re-aligned
6. 1PPS triggers cross-board AFE synchronization via UDP handshake

Tested Versions
---------------

=========================  =========================
Component                  Version
=========================  =========================
Zephyr RTOS                v4.3.0-4613-ga04d8957c21
Zephyr SDK                 0.17.0
West                       1.5.0
CMake                      3.22.1
Python                     3.10.12
Host OS                    Ubuntu 22.04 LTS
=========================  =========================

Known Issues
------------

- **PQLib flicker crash**: Rare intermittent crash in
  ``arm_biquad_cascade_df1_q31`` inside ``PQLIBFlickerProcess``
  (unaligned memory access). (I only caught it during tests where the events are of interrupts type Vin = 0)

- **i194 bug**: PQLib occasionally reports ``intrpCount=194`` on every 3rd
  interrupt event (the first 3rd event always). The firmware detects and rejects this via
  ``events_counts_valid()``. Do not call SetConfig/Reset after i194 — it
  corrupts the PQLib handle.

- **PQLib is precompiled**: ``libadi_pqlib_cm4_gcc.a`` is a binary blob
  compiled for Cortex-M4 with soft-float ABI. ``CONFIG_FP_SOFTABI=y`` is
  required.

License
-------

SPDX-License-Identifier: Apache-2.0
