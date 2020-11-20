NB_SAMPLES = 3000
NB_CH = 2

alfa = range(65, 91)

with open('ascii.dat', 'wb') as f:
	for i in range (0, 3000):
		for j in range (0, NB_CH):
			val = alfa[j].to_bytes(2,byteorder='little', signed=True)
			f.write(val)
