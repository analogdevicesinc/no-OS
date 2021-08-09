import numpy as np 
import matplotlib.pyplot as plt 

MIN_VAL = 0
MAX_VAL = 50000

#MIN_VAL = 0
#MAX_VAL = 30000

periods = 100
NB_SAMPLES = 100000
NB_CH = 1
STORAGE_BITS = 16

offset = 2 * np.pi * periods / (NB_CH + 1)

def scalae(x):
	return (x + 1) / 2 * (MAX_VAL - MIN_VAL) + MIN_VAL

scale_v = np.vectorize(scalae)

outs = []
for i in range(0, NB_CH):
	sin_offset = offset * i
	in_array = np.linspace(sin_offset, 2 * np.pi * periods + sin_offset,NB_SAMPLES)
	outs.append(scale_v(np.sin(in_array)))
 
# red for numpy.sin() 

plt.plot(in_array, np.transpose(outs)) 
plt.title("numpy.sin()") 
plt.xlabel("X") 
plt.ylabel("Y")
plt.show()

with open('generated_sin.dat', 'wb') as f:
	for i in range (0, NB_SAMPLES):
		for j in range (0, NB_CH):
			int_val = int(outs[j][i])
			val = int_val.to_bytes(int(STORAGE_BITS / 8), byteorder='little', signed=False)
			f.write(val)
