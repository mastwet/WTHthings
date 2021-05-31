
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from matplotlib.patches import Circle

b = [1,1,1,0,0,0,0,0,0,0]
a = [1,0.5,-0.25,0,0,0,0,0,0,0]

w, h = signal.freqz(b,a) #freq and angle
z, p, k = signal.tf2zpk(b,a)



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

fig, ax = plt.subplots()
circle = Circle(xy = (0.0, 0.0), radius = 1, alpha = 0.9, facecolor = 'white')
ax.add_patch(circle)
for i in p:
    ax.plot(np.real(i), np.imag(i), 'bx')  # pole before quantization
for i in z:
    ax.plot(np.real(i), np.imag(i), 'bo')  # zero before quantization

ax.set_xlim(-1.8, 1.8)
ax.set_ylim(-1.2, 1.2)
ax.grid()
plt.show()
