// module load OpenMPI/4.1.0-GCC-10.2.0 
// mpicc mpi_test.c -o mpi_test
// srun --reservation=fri --mpi=pmix -n2 -N2 ./mpi_test

#include <stdio.h>
#include <string.h> 
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLES 500000000

int main(int argc, char *argv[]) 
{ 
	int rank; // process rank 
	int num_p; // total number of processes 
	int source; // sender rank
	int destination; // receiver rank 
	int tag = 0; // message tag 
	char message[100]; // message buffer 
	MPI_Status status; // message status 
	char node_name[MPI_MAX_PROCESSOR_NAME]; //node name
	int name_len; //true length of node name
    
	MPI_Init(&argc, &argv); // initialize MPI (more bit prva MPI funkcija ki jo kličeš v programu)
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get process rank (oz pač id od prej)
	MPI_Comm_size(MPI_COMM_WORLD, &num_p); // get number of processes
	MPI_Get_processor_name( node_name, &name_len ); // get node name (samo za zanimivost)

    // ###################### računanje pi ############################
    //vsak izračuna svj delj PI po monte carlo metodi
    int count = 0;
    double x, y, z, pi;
    
    // seed za random generator more bit različen za vsak proces uporabi time pomnože s rankom
    srandom((unsigned)time(NULL) * rank);

    //merjeneje časa
    double start = MPI_Wtime();

    // koliko bo računal vsak proces
    int samples_per_process = SAMPLES / num_p;

    // računanje PI
    for (int i = 0; i < samples_per_process; i++)
    {
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        if (z <= 1.0) // preveri če je razdalja do izhodišča večja ali manjša od 1
        {
            count++;
        }
    }
    
    //pi = ((double)count / SAMPLES) * 4.0; // to je za en proces pri več bo to na koncu zračunal root proces

    //###################### konec računanja pi ########################
    //print node name and rank
    printf("Node name: %s | rank: %d\n", node_name, rank);

    //send count to root process
	if( rank != 0 ) // ločimo procese glede na rank (0 je root proces običajno)
	{ 
		//vsi ki niso root pošlejo root procesu svoj del PI
        // printf("Sending count %d to root process\n", count);
        MPI_Send(&count, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);


	} 
	else // rank 0 sprejeme rezultate in izračuna PI
	{ 
        int global_count = count;
        int recv_count = 0;
        // printf("Global count is %d\n", count);
        //get count from all other processes
        for (int i = 1; i < num_p; i++)
        {
            MPI_Recv(&recv_count, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            global_count += recv_count;
            // printf("Receiving count %d from process %d\n", recv_count, i);
        }

        // printf("Global count after recv is %d\n", global_count);
        //calculte PI
        pi = ((double)global_count / SAMPLES) * 4.0;

        //print result and time
        double end = MPI_Wtime();
        printf("PI = %lf | computed in %.4f s\n", pi, end-start);

        //print number of processes
        printf("Number of processes: %d\n", num_p);

	}

    
	MPI_Finalize(); 

	return 0; 
} 
/*
1 proces: 


2 procesa:
1 node:


2 nodes:

*/