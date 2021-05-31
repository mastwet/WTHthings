import numpy as np
import matplotlib.pyplot as plt

n0 = 0
x = 0
n = np.arange(0, 6, 1)
x = (n-n0) >= 0
x1 = (n-n0) >= 6
x2 = x^x1
x1 = (n + 2)*x2

x3 = [2,4,5,3,2,4]

def circonv(x1,x2,L):
    x = []
    for i in range(0,L-1):
        x.append(np.roll(x1,i))
    return np.dot(x,x3)

res2 = np.convolve(x1,[2,4,5,3])
res = circonv(x1,x2,6)

print('圆周卷积结果：{a}'.format(a=res))
print('线性卷积结果：{a}'.format(a=res2))
plt.plot(res2)
plt.show()
