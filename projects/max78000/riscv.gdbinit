# GDB init for MAX78000 RISC-V (CPU1) live debugging
# Usage (from repo root):
#   riscv-none-elf-gdb \
#     -x projects/max78000/riscv.gdbinit \
#     build-max78000-imu_dual_core-max78000fthr/projects/max78000/coprocessor/riscv.elf

set architecture riscv:rv32

target remote localhost:3334

# Load symbols only -- do NOT 'load', the firmware is already flashed and running.
# 'file' sets the symbol file; the second argument (offset) is 0 since the ELF
# is linked at the correct flash address.
# (riscv.elf is already the file passed on the command line, so just confirm.)

# Optional: break at the RISC-V application entry and let it run there.
# Comment out if you want to attach and inspect wherever the core is right now.
# break main
# continue
