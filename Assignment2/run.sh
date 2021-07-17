#!/bin/bash

make

for execution in 1 2 3 4 5 6 7 8 9 10
do
	for P in 4 16
	do	 
		for ppn in 1 8
		do
			process=$((P*ppn))
			size=$((P/4))
			python3 script.py 4 $size $ppn
			for D in 16 256 2048
			do
				mpirun -np $process -f hostfile ./src.x $D $P >> data.txt
			done
		done
	done
done
