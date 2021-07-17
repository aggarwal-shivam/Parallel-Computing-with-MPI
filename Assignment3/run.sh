#!/bin/bash
make clean
make

for node in 1 2
do	 
	for core in 1 2 4
	do
		python3 script.py $node $core
		process=$((node*core))
		mpirun -np $process -f hostfile ./src.x tdata.csv
	done
done
