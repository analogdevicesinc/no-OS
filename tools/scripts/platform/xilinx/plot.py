#!/usr/bin/python3
# Copyright 2023(c) Analog Devices, Inc.
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

import matplotlib.pyplot as plt
import csv
import os
import pandas as pd
import sys

def main():
    if len(sys.argv) != 2:
        print("Usage: python plot.py <num_channels>")
        return

    try:
        num_channels = int(sys.argv[1])
    except ValueError:
        print("Error: Invalid number format")

    fig, axs = plt.subplots(num_channels, 1, sharex=True, sharey=True)
    for ch in range(0, num_channels):
        y = []
        csvpath = os.getcwd() + "/" + "capture_ch{}.csv".format(ch+1)
        with open(csvpath, 'r') as file:
          csvreader = csv.reader(file)
          for row in csvreader:
            res = [eval(i) for i in row]
            if (res[0] > 32768):
              res[0] = res[0] - 65535
            y.append(res[0])

        results = pd.read_csv(csvpath)

        x = list(range(len(results)+1))

        axsch = axs if num_channels == 1 else axs[ch]
        axsch.set_ylabel("rx{}{}".format(ch // 2 + 1, 'i' if ch % 2 == 0 else 'q' ))
        axsch.plot(x, y)

    plt.show()

if __name__ == "__main__":
    main()

