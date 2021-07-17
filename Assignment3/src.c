#include "mpi.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<limits.h>

int main(int argc, char* argv[]){
	MPI_Init(&argc,&argv);
	int myrank, total_size;
	MPI_Status status;
	MPI_Request request;
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&total_size);
	int max_row=0, max_column=0;
	float *all_data, *local_data, *local_data_final;

	// reading number of rows and columns in the input csv file
	if(myrank==0)
	{
		char* filename=argv[1];
		int line_num=0;
		
		FILE* fp=fopen(filename,"r");
		if(!fp)
		{
			printf("Can't open the CSV file, please check!\n");
		}
		else
		{
			char buffer[8192];
			while(fgets(buffer,8192,fp))
			{
				char* value=strtok(buffer,", ");
				int fields=0;
				if(max_row==0)
				{
					while(value)
					{
						max_column++;
						value=strtok(NULL, ", ");
					}
				}
				max_row++;
			}
			fclose(fp);
			// printf("rows: %d columns: %d\n",max_row, max_column);
		}
		//====================================================================//
		//creating the buffer to read temperature  data from CSV file by rank 0
		max_row = max_row-1; max_column = max_column-2; //skipping header row, lattitude& longitude columns
		all_data = (float*)malloc(sizeof(float)*(max_row)*(max_column)); // buffer that stores temperature values
		// reading the input csv file by rank 0 process
		int row=0, column=0;
		fp=fopen(filename,"r");
		if(!fp)
		{
			printf("Can't open the CSV file, please check!\n");
		}
		else
		{
			char buffer[8192];
			int row=0, column=0;
			while(fgets(buffer,8192,fp))
			{
				column=0;
				char* value=strtok(buffer,", ");
				int fields=0;
				if(row!=0)
				{
					while(value)
					{
						column++;
						if(column>2)
						{
							all_data[(row-1)*(max_column)+column-3] = strtof(value, NULL);  // reading temperature values
							
						}
						value=strtok(NULL, ", ");
					}
				}
				row++;
			}
			fclose(fp);
		}
	}
		// printf("%f\n",all_data[20]);
	double sTime,eTime,Time; 
	sTime = MPI_Wtime(); // startinng the timer for code
	if(myrank==0)
	{
		for(int i=1;i<total_size;i++)
		{
			MPI_Send(&max_row,1,MPI_INT,i,0,MPI_COMM_WORLD);   // sening information about number of rows in inut data  to other processes
			MPI_Send(&max_column,1,MPI_INT,i,0,MPI_COMM_WORLD); // sening information about number of columns in inut data  to other processes
		} 
	}
	if(myrank!=0)
	{
		MPI_Recv(&max_row,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);  
		MPI_Recv(&max_column,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
	}

	int position;
	int column_per_process = max_column/total_size;
	
	// printf("here1\n");printf("column_per_process: %d,max_row: %d\n",column_per_process,max_row);
	local_data = (float*)malloc(sizeof(float)*(max_row)*(column_per_process));    // local buffers used by each process to perform computation
	local_data_final = (float*)malloc(sizeof(float)*(max_row)*(column_per_process));
	
	if(myrank==0)
	{
		for(int i=1;i<total_size;i++)
		{
			position = 0;
			for(int j=0;j<max_row;j++)   // packing the data for each process by rank 0
			{
				MPI_Pack(all_data+(j*max_column)+(i-1)*column_per_process,column_per_process,MPI_FLOAT,local_data,max_row*column_per_process*sizeof(float),&position,MPI_COMM_WORLD);
			}
			MPI_Send(local_data,position,MPI_PACKED,i,0,MPI_COMM_WORLD); //sending packed temperature data to all other processes by rank 0  
		}
	}
	if(myrank!=0)
	{
		position = 0;
		MPI_Recv(local_data,max_row*column_per_process*sizeof(float),MPI_PACKED,0,0,MPI_COMM_WORLD,&status);
		for(int j=0;j<max_row;j++) // unpacking of data done by all processes except rank 0
		{
			MPI_Unpack(local_data,max_row*column_per_process*sizeof(float),&position,(local_data_final+j*(column_per_process)),column_per_process,MPI_FLOAT,MPI_COMM_WORLD);
		}
	}

	// performing minimum finding operation by each process except root//
	float min_array[column_per_process];
	for(int i=0;i<column_per_process;i++)
	{
		min_array[i] = INT_MAX;
	}
	if(myrank!=0)
	{
		for(int i=0;i<max_row;i++)
		{
			for(int j=0;j<column_per_process;j++)
			{
				if(local_data[i*column_per_process+j]<min_array[j])
				{
					min_array[j] = local_data[i*column_per_process+j]; // computation of corresponding year wise minimum temperatures by each process except rank 0
				}
			}
		}
	}
	
	int root_column = max_column - (total_size-1)*column_per_process;
	int row_column_start = (total_size-1)*column_per_process;
	float root_min_array[root_column];
	if(myrank==0)
	{
		for(int i=0;i<root_column;i++)
		{
			root_min_array[i] = INT_MAX;
		}
		for(int i=0;i<max_row;i++)
		{
			for(int j=row_column_start;j<max_column;j++)
			{
				if(all_data[i*max_column+j]<root_min_array[j-row_column_start])
				{
					root_min_array[j-row_column_start] = all_data[i*max_column+j]; // computation of corresponding year wise minimum temperatures by rank 0
				}
			}
		}	
	}
	MPI_Datatype min_vector;
	MPI_Type_vector(column_per_process,1,1,MPI_FLOAT,&min_vector);
    MPI_Type_commit(&min_vector);
    if(myrank!=0)
    {
    	MPI_Isend(min_array,1,min_vector,0,myrank,MPI_COMM_WORLD,&request);  // sending calculated year wise min. temperatues to root process
    }
    float global_min=INT_MAX;
    float* collection_buffer = (float*)malloc(sizeof(float)*(total_size-1)*column_per_process);	
    if(myrank == 0)
    {
    	for(int i=1;i<total_size;i++)
    	{
    		MPI_Recv(collection_buffer+(i-1)*column_per_process,column_per_process,MPI_FLOAT,i,i,MPI_COMM_WORLD,&status); // collecting year wise min. temp. by root (rank 0)
    	}
    	for(int i=0;i<(total_size-1)*column_per_process;i++)
    	{
    		if(collection_buffer[i]<global_min)
    		{
    			global_min = collection_buffer[i]; // computation of global min. temperature by root (rank 0)
    		}
    	}
    	for(int i=0;i<root_column;i++)
    	{
    		if(root_min_array[i]<global_min)
    		{
    			global_min = root_min_array[i]; // computation of global min. temperature by root (rank 0)
    		}
    	}

    }

    eTime = MPI_Wtime(); // end of timer
    Time = eTime - sTime;
    double max_Time;
    MPI_Reduce(&Time,&max_Time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD); // findin maximum time across all processes
    // writing values to output.txt and times.txt
    if(myrank ==0)
    {
    	FILE *fp = fopen("output.txt","w");
    	for(int i=0;i<(total_size-1)*column_per_process;i++)
    	{
    		fprintf(fp,"%0.2f, ",collection_buffer[i]);
    	}
    	for(int i=0;i<root_column;i++)
    	{
    		fprintf(fp,"%0.2f, ",root_min_array[i]);
    	}	
    	fprintf(fp,"\n");
    	fprintf(fp,"%0.2f\n",global_min);
    	fprintf(fp,"%f\n",max_Time);
   		fclose(fp);
   		FILE *time_file = fopen("times.txt","a");
   		fprintf(time_file,"%f\n",max_Time);
   		fclose(time_file);
    }
	MPI_Finalize();
	return 0;
}
