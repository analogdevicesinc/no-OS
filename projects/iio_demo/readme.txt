Write to device:
cat sample.dat | iio_writedev -u serial:/dev/ttyUSB0,921600 -b 400  demo_device
Read from device:
iio_readdev -u serial:/dev/ttyUSB0,921600 -b 400 -s 6400 demo_device > sample.dat

Windows:
Get-Content ascii.dat | iio_writedev -u serial:COM9,921600 -b 100 -s 100 demo_device_output
iio_readdev -u serial:COM9,921600 -b 100 -s 100 demo_device_input voltage0 voltage1

