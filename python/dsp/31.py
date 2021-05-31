import numpy as np
import matplotlib.pyplot as plt

n = 0
#n = 3
#n = -3

x = np.linspace(-2,0)
n = -4-2*(x+n)

x1 = np.linspace(0,4)
n1 = -4+3*(x1+n)

x2 = np.linspace(4,8)
n2 = 16-2*(x2+n)

sum = x+x1+x2
sumn = n+n1+n2

plt.stem(sum,sumn)
plt.show()