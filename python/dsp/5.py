import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
t=np.linspace(0,5,13)

x=[1,2,3,4,5]
h=[1,1,2,2,3,4,4,5,5]

y=signal.convolve(x, h, mode='full', method='auto')

plt.stem(t,y)
plt.show()