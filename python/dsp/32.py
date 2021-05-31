import numpy as np
import matplotlib.pyplot as plt

n0 = 0
x = 0
n = np.arange(-10, 11, 1)
x = (n-n0) >= -3
x1 = (n-n0) >= 7
x2 = x^x1
y2 = 2*x2-1
yf = np.flipud(y2)
plt.stem(n, yf)
plt.axis([-10, 10, 0, 2])
plt.show()
