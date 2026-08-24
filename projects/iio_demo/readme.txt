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

ADuCM3029 build (CMake, EVAL-ADICUP3029):
The aducm3029 platform is built through the CMake build flow. Point at the
CrossCore Embedded Studio install if it is not auto-detected:
export CCES_HOME=/opt/analog/cces/3.0.3

cd no-OS

# build the iio example on the EVAL-ADICUP3029
python tools/scripts/no_os_build.py build \
	--project iio_demo --variant iio --board eval-adicup3029

# build the iio example over Wi-Fi (ESP8266)
python tools/scripts/no_os_build.py build \
	--project iio_demo --variant iio_wifi --board eval-adicup3029

# build the sw-trigger iio example over Wi-Fi (ESP8266)
python tools/scripts/no_os_build.py build \
	--project iio_demo --variant iio_wifi_with_sw_trig --board eval-adicup3029

The iio_wifi variants run the IIOD transport over an ESP8266 Wi-Fi module
(UART AT commands) instead of the serial backend. The board connects to the
configured access point and the IIO client connects to the reported IP
address. The software trigger example requires networking (it is not
supported over UART).

The EVAL-ADICUP3029 does not carry an on-board Wi-Fi module: an external
ESP8266 module has to be connected to the EVAL-ADICUP3029 P1 connector. Set
the UART switch S2 to the "WiFi Module" (right) position to route UART0 to
the P1 Wi-Fi connector (UART0_TX = P0.10, UART0_RX = P0.11); the module is
driven over UART0 at 115200 baud and its reset line is not connected (the
module is reset in software). The ESP8266 must run AT firmware configured
for 115200 baud, and its CH_PD/EN pin must be tied high (and GPIO0 left high
at boot) for the module to start; otherwise initialization times out.

Set the network credentials in the iio_wifi (or iio_wifi_with_sw_trig)
defconfig via CONFIG_WIFI_SSID and CONFIG_WIFI_PWD before building, for
example:
	CONFIG_WIFI_SSID="MyNetwork"
	CONFIG_WIFI_PWD="MyPassword"

Available variants:
  rpi-pico:        iio
  eval-adicup3029: iio, iio_wifi, iio_wifi_with_sw_trig
Replace --variant / --board accordingly.
