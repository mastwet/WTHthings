import numpy as np
import matplotlib.pyplot as plt

def generate_sinusoid(N, A, f0, fs, phi):
    T = 1/fs
    n = np.arange(N)    # [0,1,..., N-1]
    x = A * np.cos( 2*f0*np.pi*n*T + phi )
    return x

N = 511
A = 0.8
f0 = 440
fs = 44100
phi = 0


def generate_complex_sinusoid(k, N):
    n = np.arange(N)

    c_sin = np.exp(1j * 2 * np.pi * k * n / N)

    return np.conjugate(c_sin)

def generate_complex_sinusoid_matrix(N):
    n = np.arange(N)
    n = np.expand_dims(n, axis=1)      # 扩充维度，将1D向量，转为2D矩阵，方便后面的矩阵相乘

    k = n

    m = n.T * k / N                    # [N,1] * [1, N] = [N,N]

    S = np.exp(1j * 2 * np.pi * m)     # 计算矩阵 S

    return np.conjugate(S)

# 生成信号，用于测试
N = 511
A = 0.8
f0 = 440
fs = 44100
phi = 1.0
x = generate_sinusoid(N, A, f0, fs, phi)

def mydft(x,N):
# 第一种方式计算DFT
    X_1 = np.array([])
    for k in range(N):
        s = generate_complex_sinusoid(k, N)
        X_1 = np.append(X_1, np.sum(x * s))

    mX = np.abs(X_1)
    pX = np.angle(X_1)
    return mX,pX

mX,pX = mydft(x,N)
# plot the magnitude and phase
plt.subplot(2,1,1)
plt.plot(mX)

plt.plot(pX)
plt.show()
