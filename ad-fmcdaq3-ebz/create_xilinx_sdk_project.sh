hdf_destination=$1
build_destination=$2

missing_arguments()
{
	echo "Insuficient arguments from the command line. Enter the arguments"
	
	echo "Enter the project build folder"
	echo "example => /home/user/workspace/my_proj"
	read -p " : " build_destination
	
	echo Enter the system_top.hdf location
	echo "example => /home/user/workspace/my_proj_hdf/system_top.hdf"
	read -p " : " hdf_destination
}

got_arguments()
{
	if [ "$build_destination" = "" ]
	then
	build_destination="./"
	fi
	echo "$build_destination"
	echo "$hdf_destination"
}

if [ "$hdf_destination" = "" ]
then
	missing_arguments
else
    got_arguments
fi

xsct -s  ../build_scripts/xilinx/create_sdk_project.tcl "$hdf_destination" "$build_destination"

echo "Done!"