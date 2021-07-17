import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

sns.set()

fp=open("data.txt","r")
lines=fp.readlines()

name=["Bcast","Reduce","Gather","Alltoallv"]

for collective in range(4):
    #mode : 1->optimized 0->standard
    demo_input_format = pd.DataFrame.from_dict({"D": [], "P": [], "ppn": [], "mode": [], "time": [],})
    row=collective
    for execution in range(9):
        for P in [4, 16]:
            for ppn in [1, 8]:
                for D in [16, 256, 2048]:
                    # Change with the actual data
                    data_from_file=lines[row].split(' ')
                    row+=4
                    demo_input_format = demo_input_format.append({
                        "D": D, "P": P, "ppn": ppn, "mode": 1, "time": float(data_from_file[0])
                    }, ignore_index=True)
                    demo_input_format = demo_input_format.append({
                        "D": D, "P": P, "ppn": ppn, "mode": 0, "time": float(data_from_file[1])
                    }, ignore_index=True)
    demo_input_format["(P, ppn)"] = list(map(lambda x, y: ("(" + x + ", " + y + ")"), map(str, demo_input_format["P"]), map(str, demo_input_format["ppn"])))
    sns.catplot(x="(P, ppn)", y="time", data=demo_input_format, kind="bar", col="D", hue="mode")
    fname=name[collective]
    plt.savefig(f"plot_{fname}.jpg",dpi=100)
