import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

a = 7
b = 27
c = 47

def gen_rn(a):
    n0 = 0
    x = 0
    n = np.arange(-10, 11, 1)
    x = (n - n0) >= 0
    x1 = (n - n0) >= a
    x2 = x ^ x1
    return x2

def draw(w,h):
    fig, ax1 = plt.subplots()
    ax1.plot(w, 20 * np.log10(abs(h)), 'b')
    ax1.set_ylabel('X3 Amplitude [dB]', color='b')
    ax1.set_xlabel('Frequency [rad/sample]')
    plt.show()

w, h = signal.freqz(gen_rn(c),whole=True)

draw(w,h)
