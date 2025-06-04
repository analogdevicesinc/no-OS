make reset
make -j BLINKING=slow LSCRIPT=A.ld bin
cp build/dual-loader-app-A.elf A-slow.elf
cp build/dual-loader-app-A.bin A-slow.bin

make reset
make -j BLINKING=slow LSCRIPT=B.ld bin
cp build/dual-loader-app-B.elf B-slow.elf
cp build/dual-loader-app-B.bin B-slow.bin


make reset
make -j BLINKING=normal LSCRIPT=A.ld bin
cp build/dual-loader-app-A.elf A-normal.elf
cp build/dual-loader-app-A.bin A-normal.bin

make reset
make -j BLINKING=normal LSCRIPT=B.ld bin
cp build/dual-loader-app-B.elf B-normal.elf
cp build/dual-loader-app-B.bin B-normal.bin

make reset
make -j BLINKING=fast LSCRIPT=A.ld bin
cp build/dual-loader-app-A.elf A-fast.elf
cp build/dual-loader-app-A.bin A-fast.bin

make reset
make -j BLINKING=fast LSCRIPT=B.ld bin
cp build/dual-loader-app-B.elf B-fast.elf
cp build/dual-loader-app-B.bin B-fast.bin

cp A-slow.elf A-bad.elf
dd if=/dev/urandom of=A-bad.elf bs=1 count=100000 seek=1000 conv=notrunc
cp A-slow.bin A-bad.bin
dd if=/dev/urandom of=A-bad.bin bs=1 count=100 seek=1000 conv=notrunc

cp B-slow.elf B-bad.elf
dd if=/dev/urandom of=B-bad.elf bs=1 count=100000 seek=1000 conv=notrunc
cp B-slow.bin B-bad.bin
dd if=/dev/urandom of=B-bad.bin bs=1 count=100 seek=1000 conv=notrunc
