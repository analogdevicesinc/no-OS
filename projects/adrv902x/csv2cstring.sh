#!/bin/sh
#
# Copyright 2024(c) Analog Devices, Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of Analog Devices, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Usage example:
#     ./csv2cstring.sh path/to/file.csv
#
CFILE="${1%.csv}.h"
\cp -f $1 $CFILE
sed -i '$ ! s/$/\\n\\/' $CFILE
sed -i '$s/$/ \\/' $CFILE
sed -i 's/\r//' $CFILE
sed -i 's/\"/\\\"/g' $CFILE
if [ "$1" = "ADRV9025_RxGainTable.csv" ]; then
sed -i '1s/^/const char *ADRV9025_RxGainTable_text ="/' $CFILE
fi
if [ "$1" = "ADRV9025_TxAttenTable.csv" ]; then
sed -i '1s/^/const char *ADRV9025_TxAttenTable_text ="/' $CFILE
fi
sed -i '1s/=/&\n\t/' $CFILE 
sed -i '$s/ \\/\\n";/' $CFILE
echo >> $CFILE
sed -i -e '$a#endif' $CFILE
echo >> $CFILE
if [ "$1" = "ADRV9025_RxGainTable.csv" ]; then
sed -i '1i\
#ifndef ADRV9025_RX_GAIN_TABLE_H\n#define ADRV9025_RX_GAIN_TABLE_H\n' $CFILE
fi
if [ "$1" = "ADRV9025_TxAttenTable.csv" ]; then
sed -i '1i\
#ifndef ADRV9025_TX_ATTEN_TABLE_H\n#define ADRV9025_TX_ATTEN_TABLE_H\n' $CFILE
fi
echo $CFILE
