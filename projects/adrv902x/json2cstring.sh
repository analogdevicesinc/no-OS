#!/bin/sh
#
# Copyright 2024(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Usage example:
#     ./json2cstring.sh path/to/file.json
#
# Check if the first argument is equal to a specific string
CFILE="${1%.json}.h"
\cp -f $1 $CFILE
sed -i '$ ! s/$/ \\n\\/' $CFILE
sed -i '$s/$/ \\/' $CFILE
sed -i 's/\r//' $CFILE
sed -i 's/\"/\\\"/g' $CFILE
if [ $(basename "$1") = "ActiveUseCase.profile" ]; then
	sed -i '1s/^/const char *json_profile_active_use_case = "/' $CFILE
fi
if [ $(basename "$1") = "ActiveUtilInit.profile" ]; then
	sed -i '1s/^/const char *json_profile_active_util_init = "/' $CFILE
fi
sed -i '$s/ \\/\\n";/' $CFILE
sed -i -e '$a\' $CFILE
echo $CFILE
