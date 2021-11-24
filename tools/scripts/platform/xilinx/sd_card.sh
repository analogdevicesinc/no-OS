#!/bin/bash
function fsbl() {
	if [ "$1" = "zynqmp" ]; then
		echo "_a53"
	fi
}

[ ! -f "$1/boot.bif" ] &&\
echo "the_ROM_image:" >> $1/boot.bif && \
echo "{" >> $1/boot.bif && \
echo "	[bootloader] $1/system_top/$2_fsbl/fsbl$(fsbl $2).elf" >> $1/boot.bif && \
echo "	$1/system_top/hw/system_top.bit" >> $1/boot.bif && \
echo "}" >> $1/boot.bif

