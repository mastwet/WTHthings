import numpy as np
import matplotlib.pyplot as plt

def signal_xHz(A, fi, time_s, sample):
    return A * np.sin(np.linspace(0, fi * time_s * 2 * np.pi , sample* time_s))

fs = 10000
hz1 = signal_xHz(A=1, fi=20000000,time_s=10,sample=fs)

def add_noise(data):
    wn = np.random.normal(0,1,len(data))
    data_noise = np.where(data != 0.0, data.astype('float64') + 0.1 * wn, 0.0).astype(np.float32)
    return data_noise

x_n = add_noise(hz1)


x = np.arange(0, 10, 1/fs)
hz_fft = np.fft.fft(hz1)
xnf = np.fft.fft(x_n)

hz_fft = abs(hz_fft)
hzn_fft = abs(xnf)

plt.xlabel('t/s')
plt.ylabel('y')
plt.grid()
plt.plot(x, hzn_fft, 'k')
plt.axis([-0, 0.1, 0, 50])
plt.show()





