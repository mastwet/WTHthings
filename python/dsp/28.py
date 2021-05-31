import matplotlib.pyplot as plt
from scipy import signal
import numpy as np
from numpy.fft import fft, fftshift

window = np.blackman(51)

fp = 0.3*np.pi
fs = 0.5*np.pi
rp = 0.1
_as = 50

wc = (fp+fs)/1000
B = 2*np.pi*(fs-fp)/1000
nb = np.ceil(11*np.pi/B)
hn = signal.firwin(nb,0.7,B,window="hamming")
w,h = signal.freqz(hn,1,1e4)

plt.semilogx(w/2/np.pi*1000,20*np.log10(abs(h)))
plt.margins(0,0.1)
plt.show()

