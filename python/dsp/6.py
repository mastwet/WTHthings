import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

b=np.array([1,0,0,0,0,0,0,0,0])
a=np.array([1,-0.5,0,0,0,0,0,0,0])


x=[1,1,1,0,0,0,1,1,1]
y = signal.lfilter(b, a, x)
c=range(9)
print(c,list(y))
plt.stem(c,list(y))
plt.show()

