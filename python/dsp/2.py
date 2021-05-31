import numpy as np
import matplotlib.pyplot as plt

n0 = 0
x = 0
n = np.arange(-10, 11, 1)
x = (n-n0) >= 0
x1 = (n-n0) >= 7
x2 = x^x1
plt.stem(n, x2)
plt.axis([-10, 10, 0, 1.2])
plt.show()
