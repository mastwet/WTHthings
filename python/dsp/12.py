import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft,ifft


num = [1, 2, 3, 4, 1, 2, 3, 4]
num2 = [2, 3, 1, 4, 2, 4, 3, 4]

sum = [2, 6, 3, 16, 2, 8, 9, 16]
sum_fft = fft(sum) #快速傅里叶变换
print(sum)
print(list(sum_fft))

num = np.array(num)
num2 = np.array(num2)
num_fft = fft(num)    #分别进行快速傅里叶变换
num2_fft = fft(num2)
freq_convole = signal.convolve(num_fft, num2_fft) / (2 * np.pi)  #进行卷积，对比结果
print(list(freq_convole))
