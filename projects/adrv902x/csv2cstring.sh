#!/bin/sh
#
# Copyright 2024(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
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
