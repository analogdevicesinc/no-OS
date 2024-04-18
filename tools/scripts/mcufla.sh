#!/bin/bash
# Copyright 2023(c) Analog Devices, Inc.
# 
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#     - Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     - Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     - Neither the name of Analog Devices, Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#     - The use of this software may or may not infringe the patent rights
#       of one or more patent holders.  This license does not release you
#       from the requirement that you obtain separate licenses from these
#       patent holders to use this software.
#     - Use of the software either in source or binary form, must be run
#       on or directly connected to an Analog Devices Inc. component.
#    
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.
# 
# IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
# RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

USAGE=$(cat <<-END
Flash maxim, stm32 and pico microcontrollers, it downloads specific openocd for Maxim and STM32
platform, for Pico standard openocd is required.
Can be used only with .elf files.

Dependencies (Debian):
	autoconf automake make git libudev-dev libtool libusb-1.0-0-dev pkg-config texinfo
	extra for Pico plaftform: openocd

Usage:
	mcufla.sh path/to/binary.elf [adapter_serial_number]

Authors:
	darius.berghe@analog.com
	andreea.andrisan@analog.com
END

)

print_run() {
	local PS4='Running cmd: '
	local -
	set -o xtrace

	"$@"
}

parse_input() {
	if [ $# -eq 0 ]; then
		echo "$USAGE"
		exit 0
	fi

	if [ $(strings $1 | grep startup_max) ]; then
		PLATFORM=maxim
		TARGET=$(strings $1 | grep startup_max | sed 's/.*startup_\(.*\)\.o/\1/')
	elif [ $(strings $1 | grep startup_stm32) ]; then
		PLATFORM=stm32
		TARGET=$(strings $1 | grep startup_stm32 | sed 's/.*startup_\(stm32..\)[a-zA-Z0-9]*\.o/\1/')
	elif [ $(strings $1 | grep rp2040_chip) ]; then
		PLATFORM=pico
		TARGET="rp2040.cfg"
	else
		echo "Platform not supported"
		exit
	fi

	if [ -z "TARGET" ]; then
		echo "$USAGE"
		exit 0
	fi

	if [ -z "$2" ]; then
		DAP_SERIAL=""
	else
		DAP_SERIAL="cmsis_dap_serial $2"
	fi
}

build_prerequisites_maxim() {
	local HALF_CORES=$(expr $(grep -c '^processor' /proc/cpuinfo) / 2)

	if [ ! -d ~/.mcuflash/hidapi ]; then
		git clone https://github.com/libusb/hidapi.git ~/.mcuflash/hidapi &&
		pushd ~/.mcuflash/hidapi &&
		./bootstrap &&
		./configure &&
		make SHELL='sh' -j$HALF_CORES &&
		sudo -S make install &&
		sudo -S ldconfig &&
		popd

		if [ $? -ne 0 ]; then
			rm -rf ~/.mcuflash/hidapi
			exit
		fi
	fi

	if [ ! -d ~/.mcuflash/maxim/openocd ]; then
		git clone --recursive -b 0.12.0-1.0.0 https://github.com/analogdevicesinc/openocd.git ~/.mcuflash/maxim/openocd &&
		pushd ~/.mcuflash/maxim/openocd &&
		./bootstrap &&
		./configure --enable-cmsis-dap &&
		make SHELL='sh' -j$HALF_CORES &&
		popd
		
		if [ $? -ne 0 ]; then
			rm -rf ~/.mcuflash/maxim/openocd
			exit
		fi
	fi
}

build_prerequisites_stm32() {
	local HALF_CORES=$(expr $(grep -c '^processor' /proc/cpuinfo) / 2)

	if [ ! -d ~/.mcuflash/stm32/openocd ]; then
		git clone https://github.com/STMicroelectronics/OpenOCD.git ~/.mcuflash/stm32/openocd &&
		pushd ~/.mcuflash/stm32/openocd &&
		./bootstrap &&
		./configure &&
		make SHELL='sh' -j$HALF_CORES &&
		popd

		if [ $? -ne 0 ]; then
			rm -rf ~/.mcuflash/stm32/openocd
			exit
		fi
	fi
}

configure_specific_platform() {
	if [ $PLATFORM == 'maxim' ]; then
		echo "Maxim platform detected"
		build_prerequisites_maxim
		print_run ~/.mcuflash/maxim/openocd/src/openocd -s ~/.mcuflash/maxim/openocd/tcl \
			-c "adapter driver cmsis-dap; transport select swd; $DAP_SERIAL" \
			-f "target/$TARGET.cfg"  -c "program $1 verify reset exit"
	elif [ $PLATFORM == 'stm32' ]; then
		echo "STM32 platform detected"
		TAR="$TARGET"x.cfg
		build_prerequisites_stm32
		print_run ~/.mcuflash/stm32/openocd/src/openocd -s ~/.mcuflash/stm32/openocd/tcl \
			-f interface/stlink-dap.cfg -c "transport select dapdirect_swd; $DAP_SERIAL" \
			-f "target/$TAR" -c "program $1 verify reset exit"
	elif [ $PLATFORM == 'pico' ]; then
		# Pico platform is supported by the openocd downloaded for STM32 microcontrollers
		echo "Pico platform detected"
		build_prerequisites_stm32
		print_run ~/.mcuflash/stm32/openocd/src/openocd -s ~/.mcuflash/stm32/openocd/tcl \
			-f interface/cmsis-dap.cfg -f "target/$TARGET" \
			-c "adapter speed 5000; $DAP_SERIAL" -c "program $1 verify reset exit"
	else
		echo "Invalid platform"
		exit
	fi
}

parse_input $@
configure_specific_platform $1
