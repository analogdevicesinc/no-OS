################################################################
# Processes
################################################################
# Walk a directory recursively
################################################################
proc RecDirSearch {fromDir pattern} {
    global elfFiles ;
    foreach fname [glob -nocomplain -directory $fromDir *] {
        if {[file isdirectory $fname]} {
            RecDirSearch $fname $pattern
        } elseif {[string match $pattern [file tail $fname]]} {
            set path [file normalize $fname]
            lappend elfFiles $path
        }
    }
}

puts "Running data capture script..."

# Verify the number of arguments
if { $argc != 1 } {
    puts "############### ERROR ##########################"
    puts "This tcl scripts requires at least one argument."
    puts "For example tclsh data_capture.tcl cf_ad7091"
    puts "Please try again..."

    exit -1
}

# read project name from argument
set project [lindex $argv 0]
puts "Current device is $project"

# a list to store the path of the elf files
set elfFiles {}

# define current working directory
set workingDir [string trimright [pwd] "Data_Capture"]
puts "Current working directory is : $workingDir"

puts "Search for elf file ..."

# search elf elf files
RecDirSearch $workingDir sw.elf

# verify the result
if {![llength $elfFiles]} {
    puts "Did not find any elf file!"
    exit -1
}

# if the list has more than one element search with current project name
set elf [lsearch -inline $elfFiles *$project*]

if { [expr [llength $elf] > 1] } {
    puts "################## WARNING #####################"
    puts "You have more than one instance from the project $project in your working directory."
    puts "Elf file will be used from the first instance!"
}

# ensure that $elf is not a list
set elf [lindex $elf 0]

puts " "
puts "################## NOTE #####################"
puts "The used elf file is: $elf"
puts "#############################################"
puts " "

puts "Connecting to the processor..."
    connect mb mdm

puts "Downloading the Microblaze program..."
    rst
    stop
    rst
    dow $elf
    run
    stop

puts "Starting the data acquisition..."
    mwr startAcquisition 1
    con
    after 1000
    stop

puts "Reading data from the system..."
    set rxDataAddr [mrd_var prxData]
    set rxDataAddr [format "0x%08x" $rxDataAddr]
    set conversionData {}
    set nrSample [mrd_var numberOfSample]
    set adcData [mrd $rxDataAddr $nrSample]
    for { set i 1 } { $i < [expr $nrSample * 2] } { incr i 2 } {
        #trim leading zeros
        set val [string trimleft [lindex $adcData $i] 0]
        if { ![string length $val] } {
            set val "0"
        }
        set val [expr 0x$val]
        lappend conversionData $val
        lappend conversionData "\n"
        set addr [expr $rxDataAddr + 4]
        set rxDataAddr [format "0x%08x" $addr]
    }

puts "Ending the Microblaze program..."
    mwr startAcquisition 0
    con
    after 1000
    stop

puts "Moving data into file Acquisition.csv..."
set openFile 0

while { $openFile == 0 } {
    if [catch { open "Acquisition.csv" "w+" } fid ] {
        puts stderr "Error opening the file. Please make sure the file is not \
            opened by another program"
        after 1000
        puts "Trying again..."
    } else {
        set fd $fid
        set openFile 1
        puts "File opened"
    }
}

puts -nonewline $fd [ join $conversionData ]
close $fd

puts "Disconnecting from the processor..."
    stop

puts "Data acquisition is complete. Data is stored in the file Acquisition.csv."
