# coding=utf-8


import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft


def circonv(a,b,n):
    s = []
    for i in range(0,n):
        a = np.roll(a,1)
        s.append(a)
    s = np.array(s)
    d = np.array(b)
    c = np.dot(s,d)
    return c

x1 =np.array([1,4,3,2,3,2])
x2=np.array([2,1,3,4,0,0])
N=6

b = circonv(x1,x2,N)
print(b)


