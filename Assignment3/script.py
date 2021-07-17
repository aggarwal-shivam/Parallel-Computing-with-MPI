import subprocess
import sys
import os

try:
  os.remove("hostfile")
except OSError:
  pass

no_of_nodes = int(sys.argv[1])
cores = int(sys.argv[2]) ##ppn

list_of_nodes = ['csews1','csews2','csews3','csews4','csews5','csews6','csews7','csews8','csews9','csews10',\
                  'csews11','csews12','csews13','csews14','csews15','csews16','csews17','csews18','csews19','csews20','csews21','csews22','csews23','csews24', \
                  'csews25','csews26','csews27','csews28','csews29','csews30','csews31','csews32','csews33','csews34','csews35','csews36','csews37']

reachable = []
for node in list_of_nodes:
  status = subprocess.call(["ssh", node, "uptime"])
  if status == 0:
    reachable.append(node)
    if len(reachable) == no_of_nodes:
      break;
    


f2 = open("hostfile", "w+")
for node in reachable:
  f2.write(node + ":" + str(cores) + "\n")

f2.close()
        




