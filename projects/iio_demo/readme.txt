Write to device:
cat sample.dat | iio_writedev -u serial:/dev/ttyUSB0,921600 -b 400  demo_device
Read from device:
iio_readdev -u serial:/dev/ttyUSB0,921600 -b 400 -s 6400 demo_device > sample.dat

Windows:
Get-Content ascii.dat | iio_writedev -u serial:COM9,921600 -b 100 -s 100 demo_device_output
iio_readdev -u serial:COM9,921600 -b 100 -s 100 demo_device_input voltage0 voltage1


Raspberry Pi Pico build (CMake):
The Pico platform is built through the CMake build flow (see the Pico CMake
build guide: https://wiki.analog.com/resources/no-os/build). The rpi-pico
preset selects the board and toolchain, while PROJECT_DEFCONFIG selects the
example variant. The following variant is available for the rpi-pico board:
  - iio - basic IIO example (iio_demo/iio.conf)

# Configure: select the rpi-pico preset and the iio example variant.
# Pass -DPROBE=openocd (default) or -DPROBE=jlink to enable the flash and
# debug targets.
cmake --preset rpi-pico -B build-iio_demo-iio-rpi-pico \
    -DPROJECT_DEFCONFIG=iio_demo/iio.conf -DPROBE=openocd

# Build: compile the project (produces .elf and, when picotool is available,
# a .uf2 image under the build directory).
cmake --build build-iio_demo-iio-rpi-pico --target iio_demo

# Change Kconfig options interactively (optional).
cmake --build build-iio_demo-iio-rpi-pico --target menuconfig

# Flash: program the firmware onto the board via the selected probe.
cmake --build build-iio_demo-iio-rpi-pico --target flash

# Debug: start OpenOCD and attach GDB.
cmake --build build-iio_demo-iio-rpi-pico --target debug
