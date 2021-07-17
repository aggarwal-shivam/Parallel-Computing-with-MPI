#!/bin/bash

make

python3 ~/UGP/eagle/monitor/monitord.py start
chmod u+x ~/UGP/allocator/src/allocator.out
~/UGP/allocator/src/allocator.out 64 8
mkdir data_files

for execution in 1 2 3 4 5
do
        for P in 16 36 49 64
        do
                for N in 256 1024 4096 16384 65536 262144 1048576
                do
                       for MODE in 1 2 3
                       do
                       	mpirun -np $P -hostfile hostsimproved ./src.x $N 50 $MODE >> ./data_files/data_E${execution}_P${P}.txt
                       done
                done
        done
done
python3 plot.py
