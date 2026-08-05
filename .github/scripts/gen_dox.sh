#!/bin/bash
# Copyright 2023(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause

#List of excluded driver folders for the documentation generation.
EXCLUDE_DRV="talise navassa ad9083_api ad5940 madura koror palma"

#List of folders that are handled differently due to their uncommon structure
uncommon_drv_list=("sd-card" "imu" "api")

# Create a pattern that matches any substring in the list
uncommon_drv_pattern=$(IFS=\|; echo "${uncommon_drv_list[*]}")

#Generate drivers_page.dox and projects_page.dox

#Append string to specific *.dox file
append_to_dox () {
        echo "$1" >> ${TOP_DIR}/doc/doxygen/$2
}

#.dox file generic header
HEADER="/**"

#.dox file generic footer
FOOTER=" */"

#Generate drivers_page.dox file
generate_drivers_page () {

	> ${TOP_DIR}/doc/doxygen/drivers_page.dox

#Page tag and introduction for drivers
	INTRO_DRV="
\page drivers_page Drivers Page

\tableofcontents

The following sections contain code documentation for ADI no-OS drivers.
"
	#add header
	append_to_dox "${HEADER}" drivers_page.dox

	#add introduction
	append_to_dox "${INTRO_DRV}" drivers_page.dox

	#iterate driver types
	for drv_type in ${TOP_DIR}/drivers/*
	do
		#add drivers sections
		if [[ -d "${drv_type}" ]]
		then
			append_to_dox "\section $(basename -- ${drv_type}) $(basename -- ${drv_type^^})" drivers_page.dox
			append_to_dox "" drivers_page.dox
		fi
		#check if the driver type is in the uncommon drivers list
		if echo "${drv_type}" | grep -Eq "${uncommon_drv_pattern}"
		then
			append_to_dox "- \link_to_subdir{/drivers/$(basename -- ${drv_type}) \"$(basename -- ${drv_type^^})\"}" drivers_page.dox
		else
			#iterate drivers per type
			for part in ${drv_type}/*
			do
				if [[ -d "${part}" ]]
				then
					EXCLUDE_PART=0

					for value in $EXCLUDE_DRV
					do
						#exclude drivers that don't have homogenous directories and files structure
						if [[ ${part} == *"$value"* ]]
						then
							EXCLUDE_PART=1
							break
						fi
					done

					if [[ "$EXCLUDE_PART" -eq 0 ]]
					then
						#add link to driver
						append_to_dox "- \link_to_subdir{/drivers/$(basename -- ${drv_type})/$(basename -- ${part}) \"$(basename -- ${part^^})\"}" drivers_page.dox
					fi
				fi
			done
		fi
		#add new line
		append_to_dox "" drivers_page.dox
	done

	#add footer
	append_to_dox "${FOOTER}" drivers_page.dox

}

#Generate projects_page.dox file
generate_projects_page () {

	> ${TOP_DIR}/doc/doxygen/projects_page.dox

#Page tag and introduction for projects
	INTRO_PRJ="
\page projects_page Projects Page

\tableofcontents

The following list contains code documentation for ADI no-OS projects:

"
	#add header
	append_to_dox "${HEADER}" projects_page.dox

	#add introduction
	append_to_dox "${INTRO_PRJ}" projects_page.dox

	#iterate projects
	for prj_type in ${TOP_DIR}/projects/*; do
		if [[ -d "${prj_type}" ]]; then
			#add link to project
			append_to_dox "- \link_to_subdir{/projects/$(basename -- ${prj_type}) \"$(basename -- ${prj_type^^})\"}" projects_page.dox
			#add new line
			append_to_dox "" projects_page.dox
		fi
	done

	#add footer
	append_to_dox "${FOOTER}" projects_page.dox

}

generate_drivers_page
generate_projects_page
