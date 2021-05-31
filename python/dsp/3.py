import numpy as np
import matplotlib.pyplot as plt

plt.subplot(1,1,1)

X = np.linspace(-np.pi*7,np.pi*7,256,endpoint=True)
C,S = 2*np.cos(0.25*X*np.pi),3*np.sin(0.3*X*np.pi)
sum = C+S

plt.plot(X,sum)

plt.show()
