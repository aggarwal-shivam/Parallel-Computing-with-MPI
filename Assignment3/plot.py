import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np


time_data=open("times.txt","r")
time_data=time_data.readlines()
for i in range(len(time_data)):
	time_data[i]=float(time_data[i])

node1_data=time_data[:3]
node2_data=time_data[3:]

fig1=plt.figure()
ax1=plt.axes()

x=[1,2,4]

p1=plt.plot(x,node1_data, label="Scaling on 1 node")
p2=plt.plot(x,node2_data, label="Scaling on 2 nodes")
plt.legend(loc="lower right")
plt.xticks(x)
plt.xlabel("Cores per node")
plt.ylabel("Execution Time")
plt.title("Scaling of Code")
plt.savefig('plot.jpg',dpi=100)
