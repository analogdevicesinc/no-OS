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
Flash maxim microcontrollers using Maxim's own openocd.
Only use .elf files, won't work with .hex files.

Dependencies (Debian):
	autoconf automake make git libudev-dev libtool libusb-1.0-0-dev pkg-config texinfo

Usage:
	maxfla.sh path/to/binary.elf [adapter_serial_number]

Author:
	darius.berghe@analog.com
END

)

HALF_CORES=$(expr $(grep -c '^processor' /proc/cpuinfo) / 2)

function print_run
{
	local PS4='Running cmd: '
	local -
	set -o xtrace

	"$@"
}

if [ $# -eq 0 ]; then
	echo "$USAGE"
	exit 0
fi

if [ ! -d ~/.maxflash/hidapi ]; then
	git clone https://github.com/libusb/hidapi.git ~/.maxflash/hidapi &&
	pushd ~/.maxflash/hidapi &&
	./bootstrap &&
	./configure &&
	make SHELL='sh' -j$HALF_CORES &&
	sudo -S make install &&
	sudo -S ldconfig &&
	popd

	if [ $? -ne 0 ]; then
		rm -rf ~/.maxflash/hidapi
		exit
	fi
fi

if [ ! -d ~/.maxflash/openocd ]; then
	git clone https://github.com/MaximIntegratedMicros/openocd.git ~/.maxflash/openocd &&
	pushd ~/.maxflash/openocd &&
	./bootstrap &&
	./configure --enable-cmsis-dap &&
	make SHELL='sh' -j$HALF_CORES &&
	popd
	
	if [ $? -ne 0 ]; then
		rm -rf ~/.maxflash/openocd
		exit
	fi
fi

TARGET=$(strings $1 | grep startup_max | sed 's/.*startup_\(.*\)\.o/\1/')
if [ -z "TARGET" ]; then
	echo "$USAGE"
	exit 0
fi

if [ -z "$2" ]; then
	DAP_SERIAL=""
else
	DAP_SERIAL="cmsis_dap_serial $2"
fi
		
print_run ~/.maxflash/openocd/src/openocd -s ~/.maxflash/openocd/tcl -c "adapter driver cmsis-dap; transport select swd; $DAP_SERIAL" -f "target/$TARGET.cfg"  -c "program $1 verify reset exit"
