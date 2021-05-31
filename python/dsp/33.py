import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

n0 = 0
x = 0

n = np.arange(-10, 11, 1)
x = (n-n0) >= 0
x1 = (n-n0) >= 7
xf= x^x1
xf = (n+1)*xf

n = np.arange(-10, 11, 1)
h = (n-n0) >= -1
h1 = (n-n0) >= 8
hf= h^h1
hf = (n-1)*hf

nf = np.arange(-20, 21, 1)
j = signal.convolve(xf,hf)

plt.stem(nf, j)
plt.axis([-20, 20, 0, 100])
plt.show()