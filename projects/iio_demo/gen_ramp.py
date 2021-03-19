import numpy as np 
import matplotlib.pyplot as plt 

MIN_VAL = 0
MAX_VAL = 1000

periods = 5
NB_SAMPLES = 500
NB_CH = 2
STORAGE_BITS = 16

offset = MAX_VAL / NB_CH

outs = []
for i in range(0, NB_CH):
	sin_offset = offset * i
	in_array = np.linspace(sin_offset, MAX_VAL,NB_SAMPLES)
	outs.append(in_array)
 
# red for numpy.sin() 

plt.plot(in_array, np.transpose(outs)) 
plt.title("numpy.sin()") 
plt.xlabel("X") 
plt.ylabel("Y")
plt.show()


with open('new_ramp.dat', 'wb') as f:
	for i in range (0, NB_SAMPLES):
		for j in range (0, NB_CH):
			int_val = int(outs[j][i])
			val = int_val.to_bytes(int(STORAGE_BITS / 8), byteorder='little', signed=True)
			f.write(val)
