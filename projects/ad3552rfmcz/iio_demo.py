from adi.attribute import attribute
from adi.context_manager import context_manager
from adi.rx_tx import rx_tx
import numpy as np

class ad3552r(rx_tx, context_manager):
	_name = "ad3552r"
	out_channels = ["dac0", "dac1"]
	def __init__(self, uri=""):
		context_manager.__init__(self, uri, self._name)
		self._txdac = self._ctx.find_device(self._name)
		self._tx_channel_names = self.out_channels
		rx_tx.__init__(self)


demo = ad3552r(uri="serial:COM19")

demo.tx_buffer_size = 100
demo.tx_enabled_channels = [0, 1]

y = np.linspace(1,1000, 1000, dtype=np.uint16)

demo.tx(y)