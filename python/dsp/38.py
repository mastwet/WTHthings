import numpy as np
import matplotlib.pyplot as plt

n0 = 0
x = 0
n = np.arange(-16, 32, 1)
x = (n-n0) >= 0
x1 = (n-n0) >= 17
x2 = x^x1
y = (2*n - 1)*x2
y1 = np.roll(y,-16) + y + np.roll(y,16)
#y1 = np.roll(y1,-7)
y1 = np.roll(y1,7)


plt.stem(n, y1)
plt.axis([-10, 20, 0, 100])
plt.show()
