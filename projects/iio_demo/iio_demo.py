from adi.attribute import attribute
from adi.context_manager import context_manager
from adi.rx_tx import rx_tx
import numpy as np

class demo_device(rx_tx, context_manager):
	_in_name = "demo_device_input"
	_out_name = "demo_device_output"
	_in_ch_name = "voltage"
	_out_ch_name = "voltage"
	def __init__(self, uri=""):
		context_manager.__init__(self, uri, self._in_name)
		context_manager.__init__(self, uri, self._out_name)
		self._rxadc = self._ctx.find_device(self._in_name)
		self._txdac = self._ctx.find_device(self._out_name)
		
		self._rx_channel_names = [self._in_ch_name]
		self._tx_channel_names = [self._out_ch_name]
		rx_tx.__init__(self)


demo = demo_device(uri="serial:COM9")
demo.rx_buffer_size = 500
demo.rx_enabled_channels = [0]

demo.tx_buffer_size = 500
demo.tx_enabled_channels = [0]

y = np.linspace(1,1000, 1000, dtype=np.uint16)

demo.tx(y)
print(demo.rx())