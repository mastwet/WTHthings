import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft,ifft

n1 = [1,3,2,4,5,6,0,0,0,0,0,0,0,0,0,0]
n2 = []
for i in range(0,16):
    n2.append(2*i-3)

n1f = fft(np.array(n1))
n2f = fft(np.array(n2))
n3 = n1f*n2f
re = ifft(n3)
re2 = signal.convolve(n1,n2)
print(re)
print(re2)
plt.stem(re2)
plt.show()