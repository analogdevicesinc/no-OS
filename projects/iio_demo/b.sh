make ca PLATFORM=maxim
make --debug=v PLATFORM=maxim
objcopy -O ihex build/iio_demo.elf build/iio_demo.hex
cp build/iio_demo.hex /media/sf_shared/iio_demo.hex
cp build/iio_demo.elf /media/sf_shared/iio_demo.elf
