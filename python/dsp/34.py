import matplotlib.pyplot as plt
import numpy as np
from scipy import signal


nmin = 0
nmax = 8
n = np.arange(nmin,nmax+1,1)
nl = len(n)

#1号
den = np.array([1,0.35,-0.8])
num = np.array([2.5,-1.8])

#2号
den2 = np.array([0,1,0.35,-0.8])
num2 = np.array([0,2.5,-1.8])

xn = (n-1) == 0

y0 = np.array([0,0])

#计算单位冲激响应
t1,y1 = signal.dimpulse((num,den,1),n=nl)
t2,y2 = signal.dimpulse((num2,den2,1),n=nl)

plt.subplot(211)
plt.stem(t1,np.squeeze(y1))
plt.ylim(-5,5)

plt.subplot(212)
plt.stem(t2,np.squeeze(y2))
plt.ylim(-5,5)

plt.show()