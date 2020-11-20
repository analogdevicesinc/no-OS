# Copyright (C) 2019 Analog Devices, Inc.
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

from decimal import Decimal

import numpy as np
from adi.attribute import attribute
from adi.context_manager import context_manager
from adi.rx_tx import rx


class adxl345(rx, context_manager, attribute):
    """ ADXL345 3-axis accelerometer """

    _device_name = "adxl345"
    _rx_data_type = np.int32
    _rx_unbuffered_data = True
    _rx_data_si_type = np.float

    def __init__(self, uri=""):

        context_manager.__init__(self, uri, self._device_name)
        self._ctrl = self._ctx.find_device("adxl345")
        self.accel_x = self._channel(self._ctrl, "accel_x")
        self.accel_y = self._channel(self._ctrl, "accel_y")
        self.accel_z = self._channel(self._ctrl, "accel_z")
        self._rxadc = self._ctx.find_device("adxl345")
        self._rx_channel_names = ["accel_x", "accel_y", "accel_z"]
        rx.__init__(self)

    @property
    def sampling_frequency_available(self):
        """Provides all available sampling frequency settings for the ADXL345 channels"""
        return self._get_iio_dev_attr("sampling_frequency_available")

    @property
    def sampling_frequency(self):
        """ADXL345 sampling frequency"""
        # Only need to consider one channel, all others follow
        return float(self._get_iio_attr_str("accel_x", "sampling_frequency", False))

    @sampling_frequency.setter
    def sampling_frequency(self, value):
        self._set_iio_attr(
            "accel_x", "sampling_frequency", False, str(Decimal(value).real)
        )

    class _channel(attribute):
        """ADXL345 channel"""

        def __init__(self, ctrl, channel_name):
            self.name = channel_name
            self._ctrl = ctrl

        @property
        def calibbias(self):
            """ADXL345 channel offset"""
            return self._get_iio_attr(self.name, "calibbias", False)

        @calibbias.setter
        def calibbias(self, value):
            self._set_iio_attr(self.name, "calibbias", False, value)

        @property
        def raw(self):
            """ADXL345 channel raw value"""
            return self._get_iio_attr(self.name, "raw", False)

        @property
        def scale(self):
            """ADXL345 channel scale(gain)"""
            return float(self._get_iio_attr_str(self.name, "scale", False))