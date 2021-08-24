NB_SAMPLES = 3000
NB_CH = 1

alfa = range(65, 91)
lena = len(alfa)

with open('ascii.dat', 'wb') as f:
	for i in range (0, int(3000 / lena)):
		for j in range(0, lena):
			val = alfa[j].to_bytes(1,byteorder='little', signed=False)
			f.write(val)
