import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft

A = 1
fi = 37
N = 450
fs = 5
time_s = 0.1
x=A * np.sin(np.linspace(0, fi * time_s * 2 * np.pi , N* time_s))

xf = np.fft.fft(x)
axis_xf = np.linspace(-N/2,N/2-1,num=N)
xf_abs = np.fft.fftshift(abs(xf))

plt.plot(xf_abs)
plt.show()