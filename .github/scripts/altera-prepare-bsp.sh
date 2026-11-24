#!/bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Analog Devices, Inc. All rights reserved.
#
# Prepare the Nios V BSP bundle used by the Altera CI build.
#
# The no-OS Altera build links against a BSP (system.h, linker.x, HAL
# headers and libhal2_bsp.a) generated from a Nios V hardware design.
# Generating it needs the full Quartus install, so it is done once per
# (design, toolchain) pair and cached on the runner. Later runs verify the
# cached bundle's checksums and reuse it.
#
# Required environment:
#   ALTERA_QUARTUS_ROOT   Quartus Pro install (contains qinit.sh and niosv/)
#   ALTERA_TOOLCHAIN_BIN  directory holding riscv32-unknown-elf-gcc/g++
#   ALTERA_DESIGN_URL     zip of the Nios V reference design
#   ALTERA_DESIGN_NAME    short design name, used in the cache path
# Optional:
#   ALTERA_BSP_CACHE      cache root (default: $HOME/noos-bsp/altera)
#   ALTERA_DESIGN_QPF     project file, relative to the design root
#                         (default: hw/top.qpf)
#   ALTERA_DESIGN_VDS     system file, relative to the design root
#                         (default: hw/src/vds/qsys_top/qsys_top.vds)
#
# On success, CMAKE_{C,CXX,ASM}_COMPILER, ALTERA_BSP_DIR and ALTERA_BSP_LIB
# are appended to $GITHUB_ENV, or printed when run outside GitHub Actions.

set -euo pipefail

: "${ALTERA_QUARTUS_ROOT:?ALTERA_QUARTUS_ROOT is not set}"
: "${ALTERA_TOOLCHAIN_BIN:?ALTERA_TOOLCHAIN_BIN is not set}"
: "${ALTERA_DESIGN_URL:?ALTERA_DESIGN_URL is not set}"
: "${ALTERA_DESIGN_NAME:?ALTERA_DESIGN_NAME is not set}"

QPF_REL=${ALTERA_DESIGN_QPF:-hw/top.qpf}
VDS_REL=${ALTERA_DESIGN_VDS:-hw/src/vds/qsys_top/qsys_top.vds}
CACHE_ROOT=${ALTERA_BSP_CACHE:-$HOME/noos-bsp/altera}

CC="$ALTERA_TOOLCHAIN_BIN/riscv32-unknown-elf-gcc"
CXX="$ALTERA_TOOLCHAIN_BIN/riscv32-unknown-elf-g++"
for tool in "$CC" "$CXX"; do
	if [ ! -x "$tool" ]; then
		echo "ERROR: $tool not found or not executable" >&2
		exit 1
	fi
done
if [ ! -f "$ALTERA_QUARTUS_ROOT/qinit.sh" ]; then
	echo "ERROR: $ALTERA_QUARTUS_ROOT/qinit.sh not found" >&2
	exit 1
fi

# libhal2_bsp.a is compiled by this gcc, so the bundle is keyed on its version.
GCC_VERSION=$("$CC" -dumpversion)
BUNDLE="$CACHE_ROOT/$ALTERA_DESIGN_NAME-gcc$GCC_VERSION"
echo "Toolchain: $("$CC" --version | head -1)"
echo "BSP bundle: $BUNDLE"

export_env() {
	local vars=(
		"CMAKE_C_COMPILER=$CC"
		"CMAKE_CXX_COMPILER=$CXX"
		"CMAKE_ASM_COMPILER=$CC"
		"ALTERA_BSP_DIR=$BUNDLE/bsp"
		"ALTERA_BSP_LIB=$BUNDLE/libhal2_bsp.a"
	)
	local v
	for v in "${vars[@]}"; do
		echo "$v"
		if [ -n "${GITHUB_ENV:-}" ]; then
			echo "$v" >> "$GITHUB_ENV"
		fi
	done
}

mkdir -p "$CACHE_ROOT"
# Serialize generation in case two jobs reach an empty cache at once.
exec 9> "$CACHE_ROOT/.lock"
flock 9

if [ -f "$BUNDLE/SHA256SUMS" ]; then
	echo "Reusing cached BSP bundle; verifying checksums"
	(cd "$BUNDLE" && sha256sum --quiet -c SHA256SUMS)
	export_env
	exit 0
fi

echo "No cached BSP bundle for this design and toolchain; generating it"
WORK=$(mktemp -d "$CACHE_ROOT/.work.XXXXXX")
trap 'rm -rf "$WORK"' EXIT

wget -q -O "$WORK/design.zip" "$ALTERA_DESIGN_URL"
python3 -m zipfile -e "$WORK/design.zip" "$WORK/design"

# The zip holds a single top-level folder; find the design root by its
# project file rather than assuming the folder name.
QPF=$(find "$WORK/design" -path "*/$QPF_REL" -print -quit)
if [ -z "$QPF" ]; then
	echo "ERROR: $QPF_REL not found in $ALTERA_DESIGN_URL" >&2
	exit 1
fi
DESIGN=${QPF%/"$QPF_REL"}
if [ ! -f "$DESIGN/$VDS_REL" ]; then
	echo "ERROR: $VDS_REL not found under $DESIGN" >&2
	exit 1
fi

# niosv-bsp and niosv-app need the Quartus environment. qinit.sh is not
# written for set -eu, so relax it while sourcing. The chosen gcc goes first
# on PATH so the app build below compiles the BSP library with it.
set +eu
# shellcheck source=/dev/null
source "$ALTERA_QUARTUS_ROOT/qinit.sh"
set -eu
# qinit.sh sets PATH but not this; the generated app Makefile requires it.
export QUARTUS_ROOTDIR="$ALTERA_QUARTUS_ROOT"
export PATH="$ALTERA_TOOLCHAIN_BIN:$PATH"

STAGE="$WORK/bundle"
mkdir -p "$STAGE/bsp" "$WORK/app"
(cd "$DESIGN" && niosv-bsp -c -t=hal -p="$QPF_REL" -s="$VDS_REL" \
	"$STAGE/bsp/settings.bsp")
if [ ! -f "$STAGE/bsp/system.h" ]; then
	echo "ERROR: niosv-bsp did not produce system.h" >&2
	exit 1
fi

# libhal2_bsp.a is only produced by building an application against the BSP,
# so build a minimal one.
printf 'int main(void)\n{\n\treturn 0;\n}\n' > "$WORK/app/main.c"
niosv-app --bsp-dir="$STAGE/bsp" --app-dir="$WORK/app" --srcs="$WORK/app/main.c"
cmake -S "$WORK/app" -B "$WORK/app/build" -G "Unix Makefiles" \
	-DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX"
# Only the BSP library is needed. The app's default target also converts the
# ELF to hex, which needs QUARTUS_ROOTDIR (exported above).
cmake --build "$WORK/app/build" --target hal2_bsp
LIB=$(find "$WORK/app/build" -name libhal2_bsp.a -print -quit)
if [ -z "$LIB" ]; then
	echo "ERROR: libhal2_bsp.a was not produced" >&2
	exit 1
fi
cp "$LIB" "$STAGE/libhal2_bsp.a"

# The design ships a ready .sof; keep it with the BSP it matches, for
# hardware tests.
SOF=$(find "$DESIGN" -name '*.sof' -print -quit)
if [ -n "$SOF" ]; then
	cp "$SOF" "$STAGE/top.sof"
fi

(cd "$STAGE" && find . -type f ! -name SHA256SUMS -print0 | sort -z |
	xargs -0 sha256sum > SHA256SUMS)
mv "$STAGE" "$BUNDLE"
echo "BSP bundle ready"
export_env
