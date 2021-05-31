import matplotlib.pyplot as plt
import numpy as np
import scipy.signal

from scipy import signal

x=[1,0,0,0,0,0,0,0,0,0,0]

ys=[0]
xs=[0]
A=[1,-0.5]
B=[1,0]
zi=signal.lfiltic(B,A,xs,ys)

y=signal.lfilter(B,A,x,zi)
n=np.linspace(0,10,11)

plt.subplot(122)
plt.stem(n,y)


C=signal.dlti([1,0],[1,-0.5])
t,hn=signal.dimpulse(C,n=11)
plt.subplot(121)
plt.stem(np.squeeze(hn))
plt.show()
