
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

a = [1,9,6,-56]
p = (np.poly1d(a))
print(np.poly1d(p))
print("m={},y(m)={}".format(37,p(37)))
