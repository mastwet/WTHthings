
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

#   10
b = [2,1,0,0,0,0,0,0,0,0]
a = [1,-0.6,0,0,0,0,0,0,0,0]

w, h = signal.freqz(b,a)

fig = plt.figure()

ax1 = fig.add_subplot(111)
plt.plot(w, 20 * np.log10(abs(h)), 'b')
plt.ylabel('Amplitude [dB]', color='b')
plt.xlabel('Frequency [rad/sample]')

ax2 = ax1.twinx()
angles = np.unwrap(np.angle(h))
plt.plot(w, angles, 'g')
plt.ylabel('Angle (radians)', color='g')
plt.grid()
plt.axis('tight')
plt.show()