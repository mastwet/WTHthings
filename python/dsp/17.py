import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft

A1 = 8
fi1 = 10
N1 = 450
fs1 = 5
time_s1 = 0.1

A2 = 8
fi2 = 20
N2 = 450
fs2 = 5
time_s2 = 0.1

x=A1 * np.sin(np.linspace(0, fi1 * time_s1 * 2 * np.pi , N1* time_s1))
y=A2 * np.sin(np.linspace(0, fi2 * time_s2 * 2 * np.pi , N2* time_s2))

r = x+y
xf = np.fft.fft(r)
xf_abs = np.fft.fftshift(abs(xf))
plt.stem(r)
plt.show()