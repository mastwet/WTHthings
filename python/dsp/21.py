from scipy import signal
import matplotlib.pyplot as plt
import numpy as np

b1, a1 = signal.cheby1(3, 5, 150, 'low', analog=True)
b2, a2 = signal.cheby1(4, 5, 150, 'low', analog=True)
b3, a3 = signal.cheby1(5, 5, 100, 'low', analog=True)
b4, a4 = signal.cheby1(7, 5, 100, 'low', analog=True)

w1, h1 = signal.freqs(b1, a1)
w2, h2 = signal.freqs(b2, a2)
w3, h3= signal.freqs(b3, a3)
w4, h4 = signal.freqs(b4, a4)


plt.semilogx(w1, 20 * np.log10(abs(h1)))
plt.semilogx(w3, 20 * np.log10(abs(h3)))
plt.semilogx(w4, 20 * np.log10(abs(h4)))
plt.semilogx(w2, 20 * np.log10(abs(h2)))
plt.margins(0, 0.1)
plt.grid(which='both', axis='both')
plt.show()