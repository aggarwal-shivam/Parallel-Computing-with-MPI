import subprocess
import sys
import os

try:
    os.remove("hostfile")
except OSError:
    pass
f1 = open("nodefile.txt", "r")
no_of_groups = int(sys.argv[1])
nodes_per_group = int(sys.argv[2])
cores = int(sys.argv[3]) ##ppn

lines = f1.readlines()

total_groups = len(lines)
available_groups = 0
groups_checked = 0
final_result = []

for line in lines:
    nodes = line.strip().split(",")
    reachable = []
    nodes_in_this_group = 0

    for node in nodes:
        status = subprocess.call(["ssh", node, "uptime"])
        if status == 0:
            nodes_in_this_group += 1
            reachable.append(node)
        if nodes_in_this_group == nodes_per_group:
            break

    if nodes_in_this_group == nodes_per_group:
        final_result.append(reachable)
        available_groups += 1
        groups_checked += 1
        if available_groups == no_of_groups:
            break
    else:
        groups_checked += 1


subprocess.call(["clear"])
count = 0
if groups_checked == total_groups and available_groups < no_of_groups:
    print("Not enough nodes available")
else:
    f2 = open("hostfile", "w+")
    for group in final_result:
        count += 1
        for node in group:
            f2.write(node + ":" + str(cores) + "\n")
        



