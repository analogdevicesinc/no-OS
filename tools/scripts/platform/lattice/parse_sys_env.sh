#!/usr/bin/env bash
set -euo pipefail

# Parse Lattice Propel sys_env.xml for versioning and RISC-V extensions
# Usage: parse_sys_env.sh <sys_env.xml> <LATTICE_RISCV_ARCH> <required_propel_version> <required_riscv_rtos_version>

xml_path=${1:?xml path is required}
lattice_riscv_arch=${2:?LATTICE_RISCV_ARCH is required}
req_propel=${3:?required Propel version is required}
req_riscv_rtos=${4:?required riscv_rtos version is required}

fail() {
    echo "error: $*" >&2
    exit 1
}

[ -f "$xml_path" ] || fail "missing xml: $xml_path"

propel_version=""
riscv_vendor=""
riscv_library=""
riscv_name=""
riscv_version=""

# Pure grep/PCRE parsing (no external dependencies). Avoid lookbehind limits.
propel_version=$(grep -oP '<lattice:version>\K[^<]+' "$xml_path" | head -n1 || true)

# Extract the riscv_rtos instanceName tag, then pull attributes with shell regex.
inst_tag=$(grep -oP '<lattice:instanceName[^>]*name="riscv_rtos"[^>]*>' \
    "$xml_path" | head -n1 || true)
[[ $inst_tag =~ vendor=\"([^\"]*)\" ]] && riscv_vendor=${BASH_REMATCH[1]}
[[ $inst_tag =~ library=\"([^\"]*)\" ]] && riscv_library=${BASH_REMATCH[1]}
[[ $inst_tag =~ name=\"([^\"]*)\" ]] && riscv_name=${BASH_REMATCH[1]}
[[ $inst_tag =~ version=\"([^\"]*)\" ]] && riscv_version=${BASH_REMATCH[1]}

# Pull extension flags with awk across lines (no multiline PCRE needed).
ext_val() {
    local key=$1
    awk -v key="$key" '
        found && match($0, /<lattice:value>([^<]+)/, m) { print m[1]; exit }
        match($0, "<lattice:name[^>]*>" key "</lattice:name>") { found=1 }
    ' "$xml_path" | head -n1
}

riscv_vlnv="${riscv_vendor}:${riscv_library}:${riscv_name}:${riscv_version}"
ext_c=$(ext_val C_EXT)
ext_m=$(ext_val M_EXT)
ext_a=$(ext_val A_EXT)
ext_f=$(ext_val F_EXT)
jtag_type=$(ext_val JTAG_TYPE)
jtag_soft=$(ext_val SOFT_JTAG)
jtag_chan_ext=$(ext_val JTAG_CHANNEL_EXTEND)
jtag_chan=$(ext_val JTAG_CHANNEL)

missing=()
require_val() {
    local key=$1 val=$2
    [ -n "$val" ] || missing+=("$key")
}

require_val C_EXT "$ext_c"
require_val M_EXT "$ext_m"
require_val A_EXT "$ext_a"
require_val F_EXT "$ext_f"
require_val JTAG_TYPE "$jtag_type"
require_val SOFT_JTAG "$jtag_soft"
require_val JTAG_CHANNEL_EXTEND "$jtag_chan_ext"
require_val JTAG_CHANNEL "$jtag_chan"

if [ ${#missing[@]} -ne 0 ]; then
    printf 'error: missing keys in %s: %s\n' "$xml_path" "${missing[*]}" >&2
    exit 1
fi

status=0
lc() { printf '%s' "$1" | tr 'A-Z' 'a-z'; }
ext_enabled() {
    case "$(lc "$1")" in
        true|1|yes) return 0 ;;
    esac
    return 1
}
arch_from_ext="rv32i"
ext_enabled "$ext_m" && arch_from_ext+="m"
ext_enabled "$ext_a" && arch_from_ext+="a"
ext_enabled "$ext_f" && arch_from_ext+="f"
ext_enabled "$ext_c" && arch_from_ext+="c"
arch_req=$(lc "$lattice_riscv_arch")
if [ "$arch_from_ext" != "$arch_req" ]; then
    echo "arch mismatch: sys_env encodes $arch_from_ext" \
        "(C=$ext_c M=$ext_m A=$ext_a F=$ext_f)," \
        "expected $arch_req" >&2
    status=1
fi
if [ "$propel_version" != "$req_propel" ]; then
    echo "version mismatch: Propel is $propel_version, expected $req_propel" >&2
    status=1
fi
if [ "$riscv_version" != "$req_riscv_rtos" ]; then
    echo "version mismatch: riscv_rtos is $riscv_version, expected $req_riscv_rtos" >&2
    status=1
fi

cat <<EOF
PROPEL_VERSION=$propel_version
RISCV_RTOS_VLNV=$riscv_vlnv
RISCV_RTOS_VERSION=$riscv_version
LATTICE_RISCV_ARCH=$lattice_riscv_arch
EXT_C=$ext_c
EXT_M=$ext_m
EXT_A=$ext_a
EXT_F=$ext_f
JTAG_TYPE=$jtag_type
SOFT_JTAG=$jtag_soft
JTAG_CHANNEL_EXTEND=$jtag_chan_ext
JTAG_CHANNEL=$jtag_chan
EOF

exit $status
