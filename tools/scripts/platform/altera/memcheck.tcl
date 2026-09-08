# Prove that the System Console master reaches on-chip memory at the address the
# application prints, before trusting anything capture.tcl produces.
#
# Run as:  system-console --cli --script=memcheck.tcl [buffer_addr]
#
# Three checks:
#   1. Every master service is listed, not just the one that gets claimed. The
#      design has more than one (the JTAG-to-Avalon master fpga_m and the Nios V
#      debug module), and capture.tcl silently takes the first.
#   2. The reset vector at the base of on-chip memory is read back. Its first word
#      is a jump whose offset differs per build, so it also identifies which ELF
#      is actually loaded -- a wrong ELF explains a wrong buffer address.
#   3. A pattern is written into the tail of the capture buffer and read back. The
#      tail is chosen because a failed capture leaves it zero anyway, so nothing
#      worth keeping is destroyed. If this round-trips, the address range is
#      reachable and writable, and zeros read from it are real.

set ocm_base 0x10000000
set buf_addr [lindex $argv 0]
if {$buf_addr == ""} {
    set buf_addr 0x1011d400
}
set buf_bytes [expr {256 * 1024}]

set master_paths [get_service_paths master]
puts "Master services found: [llength $master_paths]"
foreach path $master_paths { puts "  $path" }
if {[llength $master_paths] == 0} {
    puts "ERROR: no master service. Is the FPGA configured and the cable visible?"
    exit 1
}
set master [lindex $master_paths 0]
puts "Claiming: $master\n"
set claim [claim_service master $master ""]

puts "On-chip memory base, 16 words at $ocm_base:"
set data [master_read_32 $claim $ocm_base 16]
set k 0
foreach value $data {
    puts [format "  0x%08x = 0x%08X" [expr {$ocm_base + $k * 4}] $value]
    incr k
}
# The jump offset changes whenever .text does, so this only identifies the two
# builds as they stand now. Regenerate with:
#   riscv32-unknown-elf-objcopy -O binary build_<x>/build/ad9088 /tmp/i.bin
#   python3 -c "import struct;print(hex(struct.unpack('<I',open('/tmp/i.bin','rb').read(4))[0]))"
puts "  (word 0 is 0x4595806F for the DDS build, 0x5555806F for the DMA build)\n"

set probe [expr {$buf_addr + $buf_bytes - 16}]
puts "Write/read round trip at the buffer tail, $probe:"
set before [master_read_32 $claim $probe 4]
puts "  before  = $before"
master_write_32 $claim $probe {0xDEADBEEF 0x0BADC0DE 0x12345678 0xA5A5A5A5}
set after [master_read_32 $claim $probe 4]
puts "  after   = $after"
set want {3735928559 195936478 305419896 2779096485}
set ok 1
for {set i 0} {$i < 4} {incr i} {
    if {[expr {[lindex $after $i]}] != [lindex $want $i]} { set ok 0 }
}
if {$ok} {
    puts "  RESULT: the master reaches this address. Zeros read here are real data."
} else {
    puts "  RESULT: FAILED. The master does not write this address -- capture.tcl is\
          reading somewhere other than the buffer."
}
master_write_32 $claim $probe $before

close_service master $claim
puts "\nDone."
