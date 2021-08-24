import numpy as np 
import matplotlib.pyplot as plt 
#from dumper import dump

MIN_VAL = 0
MAX_VAL = 50000

periods = 100
NB_SAMPLES = 10000
NB_CH = 2
STORAGE_BITS = 16

offset = (MAX_VAL - MIN_VAL)/ NB_CH

outs = []
for i in range(0, NB_CH):
	in_array = []
	
	for j in range(0, periods):
		tmp = np.linspace(MIN_VAL + offset * i, MAX_VAL,int(NB_SAMPLES / periods))
		for k in tmp:
    			in_array.append(k)
	while len(in_array) < NB_SAMPLES:
    		in_array.append(0)
	outs.append(in_array)
 
# red for numpy.sin() 

tmp = np.linspace(MIN_VAL, MAX_VAL,int(NB_SAMPLES))
plt.plot(tmp, np.transpose(outs)) 
plt.title("numpy.sin()") 
plt.xlabel("X") 
plt.ylabel("Y")
plt.show()


with open('new_ramp.dat', 'wb') as f:
	for i in range (0, NB_SAMPLES):
		for j in range (0, NB_CH):
			int_val = int(outs[j][i])
			val = int_val.to_bytes(int(STORAGE_BITS / 8), byteorder='little', signed=False)
			f.write(val)
