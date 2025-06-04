#!/bin/bash

# Default to MAX32672 if not specified
TARGET=${1:-max32672}

if [ "$TARGET" != "max32672" ] && [ "$TARGET" != "max32690" ]; then
    echo "Usage: $0 [max32672|max32690]"
    exit 1
fi

echo "Building binaries for $TARGET..."

make reset
make -j TARGET=$TARGET BLINKING=slow LSCRIPT=A_${TARGET}.ld bin
cp build/dual-loader-app-A_${TARGET}.elf ${TARGET}-A-slow.elf
cp build/dual-loader-app-A_${TARGET}.bin ${TARGET}-A-slow.bin

make reset
make -j TARGET=$TARGET BLINKING=slow LSCRIPT=B_${TARGET}.ld bin
cp build/dual-loader-app-B_${TARGET}.elf ${TARGET}-B-slow.elf
cp build/dual-loader-app-B_${TARGET}.bin ${TARGET}-B-slow.bin


make reset
make -j TARGET=$TARGET BLINKING=normal LSCRIPT=A_${TARGET}.ld bin
cp build/dual-loader-app-A_${TARGET}.elf ${TARGET}-A-normal.elf
cp build/dual-loader-app-A_${TARGET}.bin ${TARGET}-A-normal.bin

make reset
make -j TARGET=$TARGET BLINKING=normal LSCRIPT=B_${TARGET}.ld bin
cp build/dual-loader-app-B_${TARGET}.elf ${TARGET}-B-normal.elf
cp build/dual-loader-app-B_${TARGET}.bin ${TARGET}-B-normal.bin

make reset
make -j TARGET=$TARGET BLINKING=fast LSCRIPT=A_${TARGET}.ld bin
cp build/dual-loader-app-A_${TARGET}.elf ${TARGET}-A-fast.elf
cp build/dual-loader-app-A_${TARGET}.bin ${TARGET}-A-fast.bin

make reset
make -j TARGET=$TARGET BLINKING=fast LSCRIPT=B_${TARGET}.ld bin
cp build/dual-loader-app-B_${TARGET}.elf ${TARGET}-B-fast.elf
cp build/dual-loader-app-B_${TARGET}.bin ${TARGET}-B-fast.bin

# Create bad binaries for testing
cp ${TARGET}-A-slow.elf ${TARGET}-A-bad.elf
dd if=/dev/urandom of=${TARGET}-A-bad.elf bs=1 count=100000 seek=1000 conv=notrunc
cp ${TARGET}-A-slow.bin ${TARGET}-A-bad.bin
dd if=/dev/urandom of=${TARGET}-A-bad.bin bs=1 count=100 seek=1000 conv=notrunc

cp ${TARGET}-B-slow.elf ${TARGET}-B-bad.elf
dd if=/dev/urandom of=${TARGET}-B-bad.elf bs=1 count=100000 seek=1000 conv=notrunc
cp ${TARGET}-B-slow.bin ${TARGET}-B-bad.bin
dd if=/dev/urandom of=${TARGET}-B-bad.bin bs=1 count=100 seek=1000 conv=notrunc

echo "Done building binaries for $TARGET"
