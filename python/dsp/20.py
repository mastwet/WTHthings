import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft,ifft

N = signal.buttord(0.2, 0.3, 1, 30, True)
b, a = signal.butter(1, N, 'band', True)
w, h = signal.freqs(b, a, np.logspace(0.01, 2, 500))
plt.semilogx(w, 20 * np.log10(abs(h)))
plt.grid(which='both', axis='both')
plt.axis()
plt.show()