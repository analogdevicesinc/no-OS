Write to device:
cat sample.dat | iio_writedev -u serial:/dev/ttyUSB0,921600 -b 400  demo_device
Read from device:
iio_readdev -u serial:/dev/ttyUSB0,921600 -b 400 -s 6400 demo_device > sample.dat

Windows:
Get-Content ascii.dat | iio_writedev -u serial:COM9,921600 -b 100 -s 100 demo_device_output
iio_readdev -u serial:COM9,921600 -b 100 -s 100 demo_device_input voltage0 voltage1


Raspberry Pi Pico build (CMake):
The Pico platform is built through the CMake build flow (see the Pico CMake
build guide: https://wiki.analog.com/resources/no-os/build).

cd no-OS

# build the project (iio example on the Raspberry Pi Pico)
python tools/scripts/no_os_build.py build \
	--project iio_demo --variant iio --board rpi-pico

# build and flash (requires a connected debug probe)
python tools/scripts/no_os_build.py build \
	--project iio_demo --variant iio --board rpi-pico \
	--probe openocd --flash

Available variants: iio. Replace --variant accordingly.
