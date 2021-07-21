# README

This repository is the collection of all the Assignments completed in the course CS633-Parallel-Computing at IIT Kanpur. All assignments are done in the Message Passing Interface (MPI) paradigm for parallel computing.

## Contributors
1. Shivam Aggarwal (shivama20@iitk.ac.in)
2. Boppana Tej Kiran (tejkiranb20@iitk.ac.in)
   
## Details about the Assignments

### Assignment 1
In this assignment, we implemented the Halo Exhange program with the help of MPI. The assignment consisted of implementing the program using three methods:
- Multiple MPI_Sends and MPI_Recv to transmit the data items
- MPI_Pack/MPI_Unpack and MPI_Send and MPI_Recv to transmit the multiple elements at the same time
- MPI_Send and MPI_Recv using the MPI derived datatypes
  
We compared the performance of each of the three methods, and presented a detailed study about the same. More details can be found in the Assignment 1 directory.
### Assignment 2

In this assignment, we tried different optimizations to optimize the performance of MPI collective operations by making use of the network topology of 'csews' cluster at IITK. We optimized the following collective operations:
- MPI_Bcast
- MPI_Reduce
- MPI_Gather
- MPI_Alltoallv

We compared the effects of different optimizations and presented our results. More details can be found in the Assignment 2 directory.

### Assignment 3

In this assignment, we obtained the data using the sequential I/O and distributed this data to different processes for further computation. We experimented with different data distribution startegies, and studied the scaling of the code on different number of processors. 

More details can be found in the Assignment 3 directory.