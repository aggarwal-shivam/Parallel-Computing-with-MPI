import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sns
import os
import sys

data_path="data_files/"
P=[16,36,49,64]
N=['16^2','32^2','64^2','128^2','256^2','512^2','1024^2']
E=[1,2,3,4,5]
cols=['E1','E2','E3','E4','E5']

file_name="data_files/data_E{}_P{}.txt"


mode1=pd.DataFrame(columns=cols,index=N)
mode2=pd.DataFrame(columns=cols,index=N)
mode3=pd.DataFrame(columns=cols,index=N)

for process in P:
    f1=file_name.format(1,process)
    fp1=open(f1,'r')
    lines1=fp1.readlines()
    
    f2=file_name.format(2,process)
    fp2=open(f2,'r')
    lines2=fp2.readlines()
    
    f3=file_name.format(3,process)
    fp3=open(f3,'r')
    lines3=fp3.readlines()

    f4=file_name.format(4,process)
    fp4=open(f4,'r')
    lines4=fp4.readlines()
    
    f5=file_name.format(5,process)
    fp5=open(f5,'r')
    lines5=fp5.readlines()
    
    for i in range(0,len(lines1),3):
        row=i//3
        mode1.iloc[row,0]=float(lines1[i][:-2])
        mode2.iloc[row,0]=float(lines1[i+1][:-2])
        mode3.iloc[row,0]=float(lines1[i+2][:-2])
        
    for i in range(0,len(lines2),3):
        row=i//3
        mode1.iloc[row,1]=float(lines2[i][:-2])
        mode2.iloc[row,1]=float(lines2[i+1][:-2])
        mode3.iloc[row,1]=float(lines2[i+2][:-2])
        
    for i in range(0,len(lines3),3):
        row=i//3
        mode1.iloc[row,2]=float(lines3[i][:-2])
        mode2.iloc[row,2]=float(lines3[i+1][:-2])
        mode3.iloc[row,2]=float(lines3[i+2][:-2])
    
    for i in range(0,len(lines4),3):
        row=i//3
        mode1.iloc[row,3]=float(lines4[i][:-2])
        mode2.iloc[row,3]=float(lines4[i+1][:-2])
        mode3.iloc[row,3]=float(lines4[i+2][:-2])
        
    for i in range(0,len(lines5),3):
        row=i//3
        mode1.iloc[row,4]=float(lines5[i][:-2])
        mode2.iloc[row,4]=float(lines5[i+1][:-2])
        mode3.iloc[row,4]=float(lines5[i+2][:-2])
        
    outname='P{}'.format(process)
    
#     logN=np.log(np.array(N,dtype=float))
    
    mode1_np=mode1.values.T
    mode2_np=mode2.values.T
    mode3_np=mode3.values.T
    
   
    mode1_np = np.vstack(mode1_np[:, :]).astype(np.float)
    mode2_np = np.vstack(mode2_np[:, :]).astype(np.float)
    mode3_np = np.vstack(mode3_np[:, :]).astype(np.float)
    
    
#     mode1_np=np.log(mode1_np)
#     mode2_np=np.log(mode2_np)
#     mode3_np=np.log(mode3_np)
    
    mode1_med=np.median(mode1_np,axis=0)
    mode2_med=np.median(mode2_np,axis=0)
    mode3_med=np.median(mode3_np,axis=0)
#     print(np.log(float_arr))
    #plt.figure(figsize=(12,18))
    
    plt.figure(figsize=(12,12))
    
    bp1 = plt.boxplot(mode1_np,labels=N,patch_artist=True)
    plt.plot([1,2,3,4,5,6,7],mode1_med,label="Multiple MPI Sends & Recvs")
    bp2 = plt.boxplot(mode2_np,labels=N,patch_artist=True)
    plt.plot([1,2,3,4,5,6,7],mode2_med,label="MPI Pack & Unpack")
    bp3 = plt.boxplot(mode3_np,labels=N,patch_artist=True)

    for box in bp1["boxes"]:
    	box.set(facecolor = (0.0, 1.0, 0.0, 0.1))
    	
    for box in bp2["boxes"]:
    	box.set(facecolor = (1.0, 0.0, 0.0, 0.1))
    	
    for box in bp3["boxes"]:
    	box.set(facecolor = (0.0, 0.0, 1.0, 0.1))
    	
    plt.plot([1,2,3,4,5,6,7],mode3_med,label="MPI Derived data types")
    
    plt.legend(loc="upper left")
    plt.title("Time vs Number of data points for P={}".format(process))
    plt.xlabel('Number of data points(N)')
    plt.ylabel('time (seconds)')
    
    plt.savefig('plot{}.jpg'.format(process),dpi=100)
    
    
    
