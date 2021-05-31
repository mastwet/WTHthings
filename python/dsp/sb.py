import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft,ifft

x =[1,2,1,2,0,0,0,0]
x =np.array(x)
print(x)
x =fft(x)
print(x)

plt.plot(x)
plt.show()