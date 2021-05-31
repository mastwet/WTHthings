import numpy as np
import matplotlib.pyplot as plt


def e(x):
    return np.exp(x*(0.2+0.3j))

x = np.linspace(-10,10)
nr = e(x).real
ni = e(x).imag

#plt.stem(x,nr)
plt.stem(x,ni)
plt.show()
