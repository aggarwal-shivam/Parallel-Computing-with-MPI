#include "mpi.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

int main(int argc, char* argv[]){
	MPI_Init(&argc,&argv);
	int myrank, total_size;
	MPI_Status status;
	MPI_Request request;
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&total_size);

	int D=atoi(argv[1]);
	int P=atoi(argv[2]);
	int ppn=total_size/P;

	int doubles=D*1024/8; //because D is given in KB and sizeof double= 8 B
	int size = ppn*doubles*P/4;

	//declaring buffers used by processes in collective calls
	double *data, *reduce_result,*reduce_result_group, *reduce_result_final;
	double *gather_result,*gather_result_final,*gather_result_group;
	double *alltoallv_recv_buffer_final,*alltoallv_recv_buffer,*alltoallv_recv_buffer_group;
	int *sendcounts,*recvcounts,*send_displs,*recv_displs;

	//dynamic memory allocation for the buffers
	data=(double*)malloc(doubles*sizeof(double));
	reduce_result=(double*)malloc(doubles*sizeof(double));
	reduce_result_group=(double*)malloc(doubles*sizeof(double));
	reduce_result_final=(double*)malloc(doubles*sizeof(double));
	gather_result=(double*)malloc(doubles*ppn*sizeof(double));
	gather_result_group=(double*)malloc(size*sizeof(double));
	gather_result_final=(double*)malloc(doubles*total_size*sizeof(double));
	alltoallv_recv_buffer_final=(double*)malloc(doubles*total_size*sizeof(double));
	alltoallv_recv_buffer=(double*)malloc(doubles*ppn*sizeof(double));
	alltoallv_recv_buffer_group=(double*)malloc(doubles*ppn*(P/4)*sizeof(double));
	sendcounts=(int*)malloc(total_size*sizeof(int));
	recvcounts=(int*)malloc(total_size*sizeof(int));
	send_displs=(int*)malloc(total_size*sizeof(int));
	recv_displs=(int*)malloc(total_size*sizeof(int));

	
	for(int i=0;i<total_size;i++)
	{
		sendcounts[i] = doubles; recvcounts[i] = doubles; send_displs[i] = 0;recv_displs[i]=doubles*i;
	}
	

	for(int i=0;i<doubles;i++)		//initialization of data to random values
		data[i]=(double)(rand()%100);

	double start,end;
	double default_time[4],optimized_time[4];
	//array of sub-communicators
	MPI_Comm intra_comm,inter_comm,group_comm; //sub communicators
// //%%%%%%%%%%%%%%%%%%%%%%%%% Default & Optimized Implementation of Bcast, below %%%%%%%%%%%%%%%%%%%%//
	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call default MPI_Bcast
		MPI_Bcast(data,doubles,MPI_DOUBLE,0,MPI_COMM_WORLD);
	}
	end=MPI_Wtime();
	default_time[0]=(end-start)/5;
    
	
	int intra_color = myrank/ppn;
	MPI_Comm_split(MPI_COMM_WORLD,intra_color,myrank,&intra_comm); //creating intra-node sub communicators
	
	int inter_color = ppn*((4*myrank)/(P*ppn))+ myrank%ppn;
	MPI_Comm_split(MPI_COMM_WORLD,inter_color,myrank,&inter_comm); //creating intra-group sub communicators
	
	int group_color = myrank%((P*ppn)/4);
	MPI_Comm_split(MPI_COMM_WORLD,group_color,myrank,&group_comm); //creating inter group sub communicators

	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call optimized MPI_Bcast
		if(group_color==0)
		{
			MPI_Bcast(data,doubles,MPI_DOUBLE,0,group_comm);
		}
		if(inter_color%ppn==0)
		{
			MPI_Bcast(data,doubles,MPI_DOUBLE,0,inter_comm);
		}
		MPI_Bcast(data,doubles,MPI_DOUBLE,0,intra_comm);
	}
	end=MPI_Wtime();
	optimized_time[0]=(end-start)/5;
// //%%%%%%%%%%%%%%%%%%%%%%%%% Default & Optimized Implementation of MPI_Reduce, below %%%%%%%%%%%%%%%%%%%//
	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call default MPI_Reduce
		MPI_Reduce(data, reduce_result_final,doubles, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	}
	end=MPI_Wtime();
	default_time[1]=(end-start)/5;

	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call optimized MPI_reduce
		MPI_Reduce(data,reduce_result,doubles,MPI_DOUBLE,MPI_SUM,0,intra_comm);
		if(inter_color%ppn==0)
		{
			MPI_Reduce(reduce_result,reduce_result_group,doubles,MPI_DOUBLE,MPI_SUM,0,inter_comm);
		}
		if(group_color==0)
		{
			MPI_Reduce(reduce_result_group,reduce_result_final,doubles,MPI_DOUBLE,MPI_SUM,0,group_comm);
		}
	}
	end=MPI_Wtime();
	optimized_time[1]=(end-start)/5;
// //%%%%%%%%%%%%%%%%%%%%%%%%% Default & Optimized Implementation of MPI_Gather, below %%%%%%%%%%%%%%%%%%%//
	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call default MPI_Gather
		MPI_Gather(data,doubles,MPI_DOUBLE,gather_result_final,doubles,MPI_DOUBLE,0,MPI_COMM_WORLD);
	}
	end=MPI_Wtime();
	default_time[2]=(end-start)/5;

	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call optimized MPI_Gather
		MPI_Gather(data,doubles,MPI_DOUBLE,gather_result,doubles,MPI_DOUBLE,0,intra_comm);
		if(inter_color%ppn==0)
		{
			MPI_Gather(gather_result,ppn*doubles,MPI_DOUBLE,gather_result_group,ppn*doubles,MPI_DOUBLE,0,inter_comm);
		}
		if(group_color==0)
		{
			MPI_Gather(gather_result_group,size,MPI_DOUBLE,gather_result_final,size,MPI_DOUBLE,0,group_comm);
		}
	}
	end=MPI_Wtime();
	optimized_time[2]=(end-start)/5;
// //%%%%%%%%%%%%%%%%%%%%%%%%% Default & Optimized Implementation of MPI_Alltoallv, below %%%%%%%%%%%%%%%%%%%//
	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call default MPI_Alltoallv
		MPI_Alltoallv(data,sendcounts,send_displs,MPI_DOUBLE,alltoallv_recv_buffer_final,recvcounts,recv_displs,MPI_DOUBLE,MPI_COMM_WORLD);
	}
	end=MPI_Wtime();
	default_time[3]=(end-start)/5;
	
	start=MPI_Wtime();
	for(int i=0;i<5;i++){
		//call optimized MPI_Alltoallv
		MPI_Gather(data,doubles,MPI_DOUBLE,alltoallv_recv_buffer,doubles,MPI_DOUBLE,0,intra_comm);
		if(inter_color%ppn==0)
		{
			MPI_Gather(alltoallv_recv_buffer,ppn*doubles,MPI_DOUBLE,alltoallv_recv_buffer_group,ppn*doubles,MPI_DOUBLE,0,inter_comm);
		}
		if(group_color==0)
		{
			MPI_Allgather(alltoallv_recv_buffer_group,(doubles*ppn*(P/4)),MPI_DOUBLE,alltoallv_recv_buffer_final,(doubles*ppn*(P/4)),MPI_DOUBLE,group_comm);
		}
		if(inter_color%ppn==0)
		{
			MPI_Bcast(alltoallv_recv_buffer_final,total_size*doubles,MPI_DOUBLE,0,inter_comm);
		}
		MPI_Bcast(alltoallv_recv_buffer_final,total_size*doubles,MPI_DOUBLE,0,intra_comm);
	}
	end=MPI_Wtime();
	optimized_time[3]=(end-start)/5;
// //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
	// MPI_Barrier(MPI_COMM_WORLD);
	double max_default[4],max_optimum[4];
	for(int i=0;i<4;i++)
	{
		MPI_Reduce(&optimized_time[i],&max_optimum[i],1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD); //finding maximum time taken by a process under default execution
		MPI_Reduce(&default_time[i],&max_default[i],1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD); //Finding maximum time taken by a process under optimized execution
	}
	if(myrank==0){
		for(int i=0;i<4;i++){
			printf("%lf %lf\n",max_optimum[i],max_default[i]);
		}
	}
	MPI_Finalize();
	return 0;
}

