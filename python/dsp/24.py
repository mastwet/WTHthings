import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

b,a=signal.butter(1,0.3,'low',analog=True)
w,h=signal.freqs(b,a)
N = 500
t = np.linspace(0,12,num=N)
# w = pi/2
fs = 10

x1 = 5 * np.cos(np.pi * fs * t/10)+2 * np.cos(np.pi * fs * t/5)
x2 = signal.filtfilt(b,a,x1)

plt.plot(x1, label='sinx')
plt.plot(x2)

plt.legend()
plt.show()