#!/bin/bash
# Copyright 2023(c) Analog Devices, Inc.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#     - Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     - Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     - Neither the name of Analog Devices, Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#     - The use of this software may or may not infringe the patent rights
#       of one or more patent holders.  This license does not release you
#       from the requirement that you obtain separate licenses from these
#       patent holders to use this software.
#     - Use of the software either in source or binary form, must be run
#       on or directly connected to an Analog Devices Inc. component.
#
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.
#
# IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
# RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#List of excluded driver folders for the documentation generation.
EXCLUDE_DRV="talise navassa ad9083_api ad5940 madura"

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
