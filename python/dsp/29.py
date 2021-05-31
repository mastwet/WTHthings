from scipy import signal

a = [1,2,3]
b = [-0.6,-0.7,-0.8]
c = [7]

b,a = signal.invresz(a,b,c)
print(b)
print(a)
