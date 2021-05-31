import numpy as np
import matplotlib.pyplot as plt


n=np.arange(-2,10,1)
x=(n-0) == 0
x = [False,False,True,True,True,True,True,True,True,False,False,False]
print(x)

plt.stem(n,x)
plt.axis([-2,10,0,2])
plt.show()