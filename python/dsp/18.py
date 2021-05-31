import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft

def add_noise(data):
    wn = np.random.normal(0,1,len(data))
    data_noise = np.where(data != 0.0, data.astype('float64') + 0.1 * wn, 0.0).astype(np.float32)
    return data_noise

A = 1
fi = 37
N = 410
fs = 5
time_s = 0.1
x=A * np.sin(np.linspace(0, fi * time_s * 2 * np.pi , N* time_s))
x_n = add_noise(x)

xnf = np.fft.fft(x_n)
xf = np.fft.fft(x)
axis_xf = np.linspace(-N/2,N/2-1,num=N)
xf_abs = np.fft.fftshift(abs(xf))

plt.plot(x_n)
plt.show()