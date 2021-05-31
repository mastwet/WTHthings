
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

#   10
a = [1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
b = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
c = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]

w1, h1 = signal.freqz(a)
w2, h2 = signal.freqz(b)
w3, h3 = signal.freqz(c)

fig = plt.figure()

ax1 = fig.add_subplot(111)
plt.plot(w1, 20 * np.log10(abs(h1)), 'b')
plt.plot(w2, 20 * np.log10(abs(h2)), 'g')
plt.plot(w3, 20 * np.log10(abs(h3)), 'r')

plt.grid()
plt.axis('tight')
plt.show()