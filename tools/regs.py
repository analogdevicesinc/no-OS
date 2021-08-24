import serial
import time
import numpy as np 

ser = serial.Serial('COM19', 115200, timeout=1)

def _write_serial(msg):
	print('-> ' + msg.decode('ascii', errors="backslashreplace"))
	ser.write(msg)

def _read_serial():
	while (True):
		#time.sleep(1)
		line = ser.readline()
		if (line == b''):
			break
		print('<- ' + line.decode('ascii', errors="backslashreplace"))

def _to_byte(str):
	return int(str)

def _str_to_bytes(din):
	data = din.split()
	bytes = [int(d, 0) for d in data]
	return bytes

def _send_write(din):
	cmd = b'WRITE device0 spi_write_and_read %d\r\n'%len(din)
	_write_serial(cmd)
	din = bytes(din.encode('ascii'))
	_write_serial(din)

def _send_read():
	cmd = b'READ device0 spi_write_and_read\r\n'
	_write_serial(cmd)

while (True):
	print('Data to write:')
	data = input()
	#data = _str_to_bytes(data)
	_send_write(data)
	time.sleep(0.2)
	_read_serial()
	_send_read()
	time.sleep(0.2)
	_read_serial()
	
