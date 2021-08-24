from ad3552r import ad3552r
import matplotlib.pyplot as plt
import numpy as np

NB_CH = 1
#CH_SEL = 0 #Enable ch 0
#CH_SEL = 1 #Enable ch 1
CH_SEL = 2 #Enable ch 2
AMP = 60000
SAMPLES = 4000
PERIODS = 20
NB_TIMES = 1
PRINT_SIN = 0
MAX_SIZE = 5000

def create_sin():
        N = SAMPLES
        fc = PERIODS
        ts = 1 / float(N)
        t = np.arange(0, N * ts, ts)
        d1 = (np.sin(2 * np.pi * t * fc) + 1) / 2 * AMP
        d2 = (np.cos(2 * np.pi * t * fc) + 1) / 2 * AMP
        #print(len(d1))
        if NB_CH == 1:
                return (t, d1)
        else:
                return (t, [d1, d2])

def ramp(t):
        return t % 20000

def create_ramp():
        N = SAMPLES
        MAX = AMP
        t = np.arange(0, N * 2)
        d = np.arange(0, MAX, MAX / N)
        d2 = np.arange(MAX, 0, -MAX / N)
        d3 = np.append(d, d2)
        dd3 = d3 / 2
        #return (t, ramp(t))
        if NB_CH == 1:
                return (t, d3)
        else:
                return (t, [d3, dd3])

def print_sin(t, data):
        #plt.plot(t, np.transpose(outs)) 
        plt.plot(t, np.transpose(data))
        plt.title("numpy.sin()") 
        plt.xlabel("X") 
        plt.ylabel("Y")
        plt.show()

(t, _sin) = create_sin()
#(t, _sin) = create_ramp()
if PRINT_SIN == 1:
        print_sin(t, _sin)

# Set up AD7124
#dev = ad3552r(uri="serial:COM19")
dev = ad3552r(uri="ip:192.168.100.100")
print('Conected')

if NB_CH == 1:
        dev.tx_enabled_channels = [CH_SEL]
else:
        dev.tx_enabled_channels = [0, 1]


i = 0
while i < NB_TIMES:
        j = 0
        sz = len(_sin)
        while j < sz:
                tmp = min(sz - j, MAX_SIZE)
                print('j: %d, sz: %d, tmp: %d' % (j, sz,tmp))
                new_data = _sin[j : j + tmp]
                #print(new_data)

                j = j + tmp

        if i % 1000 == 0 and i != 0:
                print("sending...")
        i = i + 1
