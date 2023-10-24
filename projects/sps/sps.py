import iio
import time

ACTUATOR_MIN_POS = 1700
ACTUATOR_MAX_POS = 8191

click = 1
up = 2
down = 4
left = 8
right = 16

contexturi = "ip:192.168.20.120"

# apard32690_ctx = iio.Context("ip:192.168.20.112")

# ad7793 = apard32690_ctx.find_device("ad7793")

# chan = ad7793.channels[0]

# calib = -20
# raw = float(chan.attrs["raw"].value)
# offset = float(chan.attrs["offset"].value)
# scale = float(chan.attrs["scale"].value)

# si_val = (raw + offset) * scale
# si_val += calib
# ph_val = 7 - (si_val / 59.71)

# print(ph_val)

swiot1l_ctx = iio.Context(contexturi)
swiot = swiot1l_ctx.find_device("swiot")

swiot.attrs["mode"].value = "config"
swiot1l_ctx = iio.Context(contexturi)
swiot = swiot1l_ctx.find_device("swiot")

print(swiot.attrs["mode"].value)

swiot.attrs["ch0_function"].value = "current_out"
swiot.attrs["ch1_function"].value = "current_in_loop"
swiot.attrs["ch2_function"].value = "digital_input_loop"
swiot.attrs["ch3_function"].value = "voltage_in"

swiot.attrs["ch0_enable"].value = "1"
swiot.attrs["ch1_enable"].value = "1"
swiot.attrs["ch2_enable"].value = "1"
swiot.attrs["ch3_enable"].value = "1"

swiot.attrs["mode"].value = "runtime"

swiot1l_ctx = iio.Context(contexturi)
swiot = swiot1l_ctx.find_device("swiot")
ad74413r = swiot1l_ctx.find_device("ad74413r")

actuator_pos = ACTUATOR_MIN_POS
ad74413r.find_channel("current0", True).attrs["raw"].value = str(1700)
ad74413r.find_channel("current1", True).attrs["raw"].value = str(8191)
ad74413r.find_channel("current2", True).attrs["raw"].value = str(100)

ch1_offset = int(ad74413r.find_channel("current1", False).attrs["offset"].value)
ch1_scale= float(ad74413r.find_channel("current1", False).attrs["scale"].value)

ad74413r.find_channel("voltage2", False).attrs["threshold"].value = str(5000)

ch3_offset = int(ad74413r.find_channel("voltage3", False).attrs["offset"].value)
ch3_scale= float(ad74413r.find_channel("voltage3", False).attrs["scale"].value)

ctx = iio.Context("ip:192.168.20.112")
sps = ctx.find_device("sps")

pos = 0
prevpos = 0
while True:
	g = int(sps.attrs['gestures'].value)
	if g & left:
		pos -= 750
	elif g & right:
		pos += 750
	
	if (pos < 1510):
		pos = 1510
	
	if (pos > 6552):
		pos = 6552

	ch1_raw = int(ad74413r.find_channel("current1", False).attrs["raw"].value)
	sps.attrs['io1'].value = "{:.1f}".format((ch1_raw + ch1_offset) * ch1_scale)
	ch2_raw = int(ad74413r.find_channel("voltage2", False).attrs["raw"].value)
	if (ch2_raw > 32000):
		sps.attrs['io2'].value = 'open'
	else:
		sps.attrs['io2'].value = 'closed'
	ch3_raw = int(ad74413r.find_channel("voltage3", False).attrs["raw"].value)
	sps.attrs['io3'].value = "{:.2f}".format((ch3_raw + ch3_offset) * ch3_scale / 1000)

	if prevpos != pos:
		ad74413r.find_channel("current0", True).attrs["raw"].value = str(pos)
		prevpos = pos
	
	time.sleep(0.1)
