import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import sys

p = np.poly1d([1,7,3,-40])
g = np.roots([1,7,3,-40])
print(p)
a = 0
for i in g:
    a = a+g
print(a)
sys.exit()