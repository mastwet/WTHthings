# coding=utf-8
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft

# 学号37

a =np.arange(99)
b = np.roll(a, 37)
c = np.roll(a, -37)
print(b,c)
plt.plot(b)
plt.plot(c)
plt.show()
