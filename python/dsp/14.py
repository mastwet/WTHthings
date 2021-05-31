# coding=utf-8
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft


def signal_xHz(A, fi, time_s, sample):
    return A * np.sin(np.linspace(0, fi * time_s * 2 * np.pi , sample* time_s))

sin = signal_xHz(3,10,1,400)
sin_fft = fft(sin)

plt.plot(sin_fft)
plt.show()