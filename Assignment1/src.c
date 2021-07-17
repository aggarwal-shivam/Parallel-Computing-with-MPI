#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<math.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc,&argv);

    int mode; // to decide the execution of program

    int myrank,size_total;
    MPI_Status status;
    MPI_Request request;
    double maxTime;
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&size_total);
    int n=atoi(argv[1]);
    int N,steps;
    N = sqrt((double)n);
    steps=atoi(argv[2]);
    mode=atoi(argv[3]);
    MPI_Datatype row_vector,col_vector;
    MPI_Type_vector(N,1,1,MPI_DOUBLE,&row_vector);
    MPI_Type_commit(&row_vector);
    MPI_Type_vector(N,1,N,MPI_DOUBLE,&col_vector);
    MPI_Type_commit(&col_vector);

    double* N_matrix;
    N_matrix=(double*)malloc(N*N*sizeof(double));
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
        {
            // N_matrix[i][j] = rand()%100;
            N_matrix[i*N+j] = (double)rand();
        }
    }

    int size=sqrt((double)size_total);	//this is P value...later it will be taken via cmd
    int Pi,Pj;
    Pi = myrank/size; Pj=myrank%size;
    int lflag=0,rflag=0,uflag=0,dflag=0;
    if(Pi>0)
    {
        uflag=1;
    }
    if(Pi<size-1)
    {
        dflag=1;
    }
    if(Pj>0)
    {
        lflag=1;
    }
    if(Pj<size-1)
    {
        rflag=1;
    }

    double left[N],right[N],up[N],down[N];
    double send_left[N],send_right[N],send_up[N],send_down[N];
    double recv_left[N],recv_right[N],recv_up[N],recv_down[N];
    int t=0;
    double sTime,eTime,total_Time;
    int sends=0;
    sTime = MPI_Wtime();
    while(t<steps)
    {
        //Step:1 performing halo exchange
        //sending element by element to neighbouring processes
        if(mode==1){        // mode 1 is used when we want to use Multiple MPI sends and receives
            if(uflag) //if there is process above
            {
                for(int i=0;i<N;i++) //sending row 0
                {
                    MPI_Isend(N_matrix+i,1,MPI_DOUBLE,myrank-size,myrank,MPI_COMM_WORLD,&request);
                    sends++;
                }
            }
            if(dflag) //if there is a process below  
            {
                for(int i=0;i<N;i++) //sending row (N-1)
                {
                    MPI_Isend(N_matrix+(N-1)*N+i,1,MPI_DOUBLE,myrank+size,myrank,MPI_COMM_WORLD,&request);
                    sends++;
                }
            }
            if(lflag) //if there is a process on left
            {
                for(int i=0;i<N;i++) //sending column 0
                {
                    MPI_Isend(N_matrix+i*N,1,MPI_DOUBLE,myrank-1,myrank,MPI_COMM_WORLD,&request);
                    sends++;
                }
            }
            if(rflag) //if there is a process on right
            {
                for(int i=0;i<N;i++) //sending column (n-1)
                {
                    sends++;
                    MPI_Isend(N_matrix+(i*N+N-1),1,MPI_DOUBLE,myrank+1,myrank,MPI_COMM_WORLD,&request);
                }
            }
            //receiving data from neighbourig processes
            if(uflag)
            {
                for(int i=0;i<N;i++)
                {
                    MPI_Recv(up+i,1,MPI_DOUBLE,myrank-size,myrank-size,MPI_COMM_WORLD,&status);
                }
            }
            if(dflag)
            {
                for(int i=0;i<N;i++)
                {
                    MPI_Recv(down+i,1,MPI_DOUBLE,myrank+size,myrank+size,MPI_COMM_WORLD,&status);
                }
            }
            if(lflag)
            {
                for(int i=0;i<N;i++)
                {
                    MPI_Recv(left+i,1,MPI_DOUBLE,myrank-1,myrank-1,MPI_COMM_WORLD,&status);
                }
            }
            if(rflag)
            {
                for(int i=0;i<N;i++)
                {
                    MPI_Recv(right+i,1,MPI_DOUBLE,myrank+1,myrank+1,MPI_COMM_WORLD,&status);
                }
            }
        }

        if(mode==2){        // mode2 is for using MPI Pack and Unpack
            int position=0;
            if(uflag) //if there is process above
            {
                for(int i=0;i<N;i++) //sending row 0
                {
                    MPI_Pack(N_matrix+i,1,MPI_DOUBLE,send_up,N*sizeof(double),&position,MPI_COMM_WORLD);
                }
                MPI_Isend(send_up,position,MPI_PACKED,myrank-size,myrank,MPI_COMM_WORLD,&request);
                sends++;
            }
            if(dflag) //if there is a process below  
            {
                position=0;
                for(int i=0;i<N;i++) //sending row (N-1)
                {
                    MPI_Pack(N_matrix+(N-1)*N+i,1,MPI_DOUBLE,send_down,N*sizeof(double),&position,MPI_COMM_WORLD);
                }
                MPI_Isend(send_down,position,MPI_PACKED,myrank+size,myrank,MPI_COMM_WORLD,&request);
                sends++;
                
            }
            if(lflag) //if there is a process on left
            {
                position=0;
                for(int i=0;i<N;i++) //sending column 0
                {
                    MPI_Pack(N_matrix+i*N,1,MPI_DOUBLE,send_left,N*sizeof(double),&position,MPI_COMM_WORLD);
                }
                MPI_Isend(send_left,position,MPI_PACKED,myrank-1,myrank,MPI_COMM_WORLD,&request);
                sends++;
            }
            if(rflag) //if there is a process on right
            {
                position=0;
                for(int i=0;i<N;i++) //sending column (n-1)
                {
                    MPI_Pack(N_matrix+(i*N+N-1),1,MPI_DOUBLE,send_right,N*sizeof(double),&position,MPI_COMM_WORLD);
                }
                sends++;
                MPI_Isend(send_right,position,MPI_PACKED,myrank+1,myrank,MPI_COMM_WORLD,&request);
            }
            //receiving data from neighbouring processes
            if(uflag)
            {
                position=0;
                MPI_Recv(recv_up,N*sizeof(double),MPI_PACKED,myrank-size,myrank-size,MPI_COMM_WORLD,&status);
                for(int i=0;i<N;i++)
                {
                    MPI_Unpack(recv_up,N,&position,up+i,1,MPI_DOUBLE,MPI_COMM_WORLD);
                }
            }
            if(dflag)
            {
                position=0;
                MPI_Recv(recv_down,N*sizeof(double),MPI_PACKED,myrank+size,myrank+size,MPI_COMM_WORLD,&status);
                for(int i=0;i<N;i++)
                {
                    MPI_Unpack(recv_down,N,&position,down+i,1,MPI_DOUBLE,MPI_COMM_WORLD);
                }
            }
            if(lflag)
            {
                position=0;
                MPI_Recv(recv_left,N*sizeof(double),MPI_PACKED,myrank-1,myrank-1,MPI_COMM_WORLD,&status);
                for(int i=0;i<N;i++)
                {
                    MPI_Unpack(recv_left,N,&position,left+i,1,MPI_DOUBLE,MPI_COMM_WORLD);
                }
            }
            if(rflag)
            {
                position=0;
                MPI_Recv(recv_right,N*sizeof(double),MPI_PACKED,myrank+1,myrank+1,MPI_COMM_WORLD,&status);
                for(int i=0;i<N;i++)
                {
                    MPI_Unpack(recv_right,N,&position,right+i,1,MPI_DOUBLE,MPI_COMM_WORLD);
                }
            }
        }

        if(mode==3){    //mode3 is used for MPI derived datatypes
            if(uflag) //if there is process above
            {
                MPI_Isend(N_matrix,1,row_vector,myrank-size,myrank,MPI_COMM_WORLD,&request);
            }
            if(dflag){
                MPI_Isend(N_matrix+(N-1)*N,1,row_vector,myrank+size,myrank,MPI_COMM_WORLD,&request);
            }
            if(lflag){
                MPI_Isend(N_matrix,1,col_vector,myrank-1,myrank,MPI_COMM_WORLD,&request);
            }
            if(rflag){
                MPI_Isend(N_matrix+(N-1),1,col_vector,myrank+1,myrank,MPI_COMM_WORLD,&request);
            }
            // receiving data from neighbours
            if(uflag){
                MPI_Recv(up,N,MPI_DOUBLE,myrank-size,myrank-size,MPI_COMM_WORLD,&status);
            }
            if(dflag){
                MPI_Recv(down,N,MPI_DOUBLE,myrank+size,myrank+size,MPI_COMM_WORLD,&status);
            }
            if(lflag){
                MPI_Recv(left,N,MPI_DOUBLE,myrank-1,myrank-1,MPI_COMM_WORLD,&status);
            }
            if(rflag){
                MPI_Recv(right,N,MPI_DOUBLE,myrank+1,myrank+1,MPI_COMM_WORLD,&status);
            }

        }

        N_matrix[0*N+0] = (up[0]+left[0]+N_matrix[0*N+1]+N_matrix[1*N+0])/4;
        N_matrix[0*N+N-1] = (up[N-1]+right[0]+N_matrix[0*N+N-2]+N_matrix[1*N+N-1])/4;
        N_matrix[(N-1)*N+0] = (down[0]+left[N-1]+N_matrix[(N-1)*N+1]+N_matrix[(N-2)*N+0])/4;
        N_matrix[(N-1)*N+N-1] = (down[N-1]+right[N-1]+N_matrix[(N-2)*N+N-1]+N_matrix[(N-1)*N+N-2])/4;        

        for(int i=1;i<N-1;i++)
        {
            N_matrix[0*N+i]= (N_matrix[0*N+i+1]+N_matrix[0*N+i-1]+N_matrix[1*N+i]+up[i])/4;
            N_matrix[(N-1)*N+i]= (N_matrix[(N-1)*N+i+1]+N_matrix[(N-1)*N+i-1]+N_matrix[(N-1)*N+i]+down[i])/4;
            N_matrix[i*N+0]= (N_matrix[(i+1)*N+0]+N_matrix[(i-1)*N+0]+N_matrix[i*N+1]+left[i])/4;
            N_matrix[i*N+N-1]=(N_matrix[(i+1)*N+N-1]+N_matrix[(i-1)*N+N-1]+N_matrix[i*N+N-2]+right[i])/4;
        }
        //at interior points
        for(int i=1;i<N-1;i++)
        {
            for(int j=1;j<N-1;j++)
            {
                N_matrix[i*N+j]=(N_matrix[(i-1)*N+j]+N_matrix[(i+1)*N+j]+N_matrix[i*N+j-1]+N_matrix[i*N+j+1])/4;
            }
        }
        //incrementing the steps
        t++;
    }
    eTime = MPI_Wtime();
    double time = eTime-sTime;
    MPI_Reduce (&time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(myrank==0){
		printf("%lf\n",maxTime);    	
    }
    MPI_Finalize();
    return 0;
}
