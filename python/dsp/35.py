from scipy import signal
import numpy as np
import matplotlib.pyplot as plt

den =  np.array([1,-0.7])
num = np.array([1,2])

y0 = np.array([4,2])
y3,_ = signal.dimpulse((num,den,1),8)

w, h = signal.freqz(y3,whole=True)
fig, ax1 = plt.subplots()

ax1.plot(w, 20 * np.log10(abs(h)), 'b')
ax1.set_ylabel('Amplitude [dB]', color='b')
ax1.set_xlabel('Frequency [rad/sample]')
ax2 = ax1.twinx()
angles = np.unwrap(np.angle(h))
ax2.plot(w, angles, 'g')
ax2.set_ylabel('Angle (radians)', color='g')
ax2.grid()
ax2.axis('tight')
plt.show()