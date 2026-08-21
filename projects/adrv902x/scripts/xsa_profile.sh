#!/bin/sh
# Derive the ADRV902x JESD use-case profile from a bitstream .xsa.
#
# The two bits that select the profile are embedded in the hardware handoff
# (system.hwh) inside the .xsa:
#   * LINK_MODE 2                         -> JESD204C   (else JESD204B)
#   * an axi_adrv9026_rx_os_jesd_rx core  -> ORx present (else no ORx)
# The main-Rx converter count is the NUM_CHANNELS of the main Rx ADC-TPL core
# (rx_adrv9026_tpl_core_adc_tpl_core; the ORx TPL is rx_os_* and is skipped).
#
# Usage:  xsa_profile.sh /abs/path/system_top....xsa
# Output: PROFILE=<firmware-dir-name>;RX_CONVS=<n>
set -eu

xsa="${1:?usage: xsa_profile.sh <path-to-.xsa>}"
hwh=$(unzip -p "$xsa" '*.hwh')

# Main Rx ADC-TPL channel count (I+Q per Rx lane). Fall back to 8 if absent.
convs=$(printf '%s\n' "$hwh" | awk -F'"' '
    /INSTANCE="rx_adrv9026_tpl_core_adc_tpl_core"/ { f = 1 }
    f && /NAME="NUM_CHANNELS"/ { print $4; exit }')
[ -n "${convs:-}" ] || convs=8

if printf '%s\n' "$hwh" | grep -q 'NAME="LINK_MODE" VALUE="2"'; then
	link=204c
else
	link=204b
fi
if printf '%s\n' "$hwh" | grep -q 'INSTANCE="axi_adrv9026_rx_os_jesd_rx"'; then
	orx=1
else
	orx=0
fi

if [ "$link" = 204c ]; then
	profile=JESD204C_ORx
elif [ "$orx" = 1 ]; then
	profile=JESD204B_ORx
else
	profile=JESD204B_no_ORx
fi

echo "PROFILE=${profile};RX_CONVS=${convs}"
