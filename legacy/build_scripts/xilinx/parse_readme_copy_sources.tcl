#
# Parse README.md and copy the source files
#

set fp [open $source_directory/README.md r]
set data [read $fp]
set start_sign "\[."
set end_sign "\]"
set start_path_index 0
set end_path_index 1
set index 1

while {1} {
	if {[string first $start_sign $data $end_path_index] != -1 } {
	set start_path_index [string first $start_sign $data $end_path_index]
	incr start_path_index 1
		while {1} {
			if {[string first $end_sign $data $start_path_index] != -1 } {
				set end_path_index [string first $end_sign $data $start_path_index]
				 incr end_path_index -1
				 set path($index) [string range $data $start_path_index $end_path_index]
				incr index 1
				break
			} else {
				puts "ERROR IN PARSING README FILE"
				exit
			}
		}
	} break
}
close $fp

for { set i 1 } { $i < $index} { incr i 1 } {
	puts $path($i)
	if {$path($i)=="./"} {
		eval file copy -force -- [glob $source_directory/$path($i)*.c]  $project_destination
		if { [catch {eval file copy -force -- [glob $source_directory/$path($i)*.h]  $project_destination} fid] } {
			puts "no header file in $source_directory/$path($i)"
		}
	} else {
		eval file copy -force -- [glob $source_directory/$path($i)/*.c]  $project_destination
		eval file copy -force -- [glob $source_directory/$path($i)/*.h]  $project_destination
	}
 }