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
        MPI_Send(&count, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);


	} 
	else // rank 0 sprejeme rezultate in izračuna PI
	{ 
        int global_count = count;
        int recv_count = 0;

        //get count from all other processes
        for (int i = 1; i < num_p; i++)
        {
            MPI_Recv(&recv_count, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            global_count += recv_count;
        }

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
Node name: nsc-msv007.ijs.si | rank: 0
PI = 3.141539 | computed in 52.0203 s
Number of processes: 1

2 procesa:
1 node:
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 0
PI = 3.141552 | computed in 26.0245 s
Number of processes: 2

2 nodes:
Node name: nsc-msv011.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 0
PI = 3.141614 | computed in 26.0202 s
Number of processes: 2

4 procesi:
1 node:
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 3
PI = 3.141617 | computed in 13.0356 s
Number of processes: 4

2 nodes:
Node name: nsc-msv011.ijs.si | rank: 2
Node name: nsc-msv011.ijs.si | rank: 3
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 0
PI = 3.141562 | computed i

8 procesov:
1 node:
Node name: nsc-msv007.ijs.si | rank: 4
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 5
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 6
Node name: nsc-msv007.ijs.si | rank: 7
Node name: nsc-msv007.ijs.si | rank: 3
PI = 3.141479 | computed in 7.9017 s
Number of processes: 8

2 nodes:
Node name: nsc-msv011.ijs.si | rank: 5
Node name: nsc-msv011.ijs.si | rank: 6
Node name: nsc-msv011.ijs.si | rank: 7
Node name: nsc-msv011.ijs.si | rank: 4
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 3
PI = 3.141433 | computed in 6.5220 s
Number of processes: 8

16 procesov:
1 node:
Node name: nsc-msv007.ijs.si | rank: 12
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 13
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 4
Node name: nsc-msv007.ijs.si | rank: 5
Node name: nsc-msv007.ijs.si | rank: 7
Node name: nsc-msv007.ijs.si | rank: 9
Node name: nsc-msv007.ijs.si | rank: 3
Node name: nsc-msv007.ijs.si | rank: 8
Node name: nsc-msv007.ijs.si | rank: 6
Node name: nsc-msv007.ijs.si | rank: 15
Node name: nsc-msv007.ijs.si | rank: 11
Node name: nsc-msv007.ijs.si | rank: 14
Node name: nsc-msv007.ijs.si | rank: 10
PI = 3.141654 | computed in 3.9571 s
Number of processes: 16

2 nodes:
Node name: nsc-msv011.ijs.si | rank: 8
Node name: nsc-msv011.ijs.si | rank: 12
Node name: nsc-msv011.ijs.si | rank: 9
Node name: nsc-msv011.ijs.si | rank: 13
Node name: nsc-msv011.ijs.si | rank: 10
Node name: nsc-msv011.ijs.si | rank: 14
Node name: nsc-msv011.ijs.si | rank: 11
Node name: nsc-msv011.ijs.si | rank: 15
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 4
Node name: nsc-msv007.ijs.si | rank: 6
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 3
Node name: nsc-msv007.ijs.si | rank: 5
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 7
PI = 3.141557 | computed in 3.9558 s
Number of processes: 16

32 procesov:
1 node:
Node name: nsc-msv007.ijs.si | rank: 21
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 8
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 27
Node name: nsc-msv007.ijs.si | rank: 14
Node name: nsc-msv007.ijs.si | rank: 17
Node name: nsc-msv007.ijs.si | rank: 5
Node name: nsc-msv007.ijs.si | rank: 29
Node name: nsc-msv007.ijs.si | rank: 10
Node name: nsc-msv007.ijs.si | rank: 9
Node name: nsc-msv007.ijs.si | rank: 19
Node name: nsc-msv007.ijs.si | rank: 28
Node name: nsc-msv007.ijs.si | rank: 18
Node name: nsc-msv007.ijs.si | rank: 22
Node name: nsc-msv007.ijs.si | rank: 31
Node name: nsc-msv007.ijs.si | rank: 6
Node name: nsc-msv007.ijs.si | rank: 13
Node name: nsc-msv007.ijs.si | rank: 30
Node name: nsc-msv007.ijs.si | rank: 25
Node name: nsc-msv007.ijs.si | rank: 16
Node name: nsc-msv007.ijs.si | rank: 20
Node name: nsc-msv007.ijs.si | rank: 11
Node name: nsc-msv007.ijs.si | rank: 15
Node name: nsc-msv007.ijs.si | rank: 24
Node name: nsc-msv007.ijs.si | rank: 12
Node name: nsc-msv007.ijs.si | rank: 23
Node name: nsc-msv007.ijs.si | rank: 4
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 26
Node name: nsc-msv007.ijs.si | rank: 3
Node name: nsc-msv007.ijs.si | rank: 7
PI = 3.141649 | computed in 2.8574 s
Number of processes: 32

2 nodes:
Node name: nsc-msv007.ijs.si | rank: 3
Node name: nsc-msv007.ijs.si | rank: 9
Node name: nsc-msv007.ijs.si | rank: 12
Node name: nsc-msv007.ijs.si | rank: 13
Node name: nsc-msv007.ijs.si | rank: 7
Node name: nsc-msv007.ijs.si | rank: 11
Node name: nsc-msv007.ijs.si | rank: 15
Node name: nsc-msv007.ijs.si | rank: 2
Node name: nsc-msv007.ijs.si | rank: 6
Node name: nsc-msv007.ijs.si | rank: 10
Node name: nsc-msv007.ijs.si | rank: 14
Node name: nsc-msv007.ijs.si | rank: 1
Node name: nsc-msv007.ijs.si | rank: 5
Node name: nsc-msv007.ijs.si | rank: 8
Node name: nsc-msv007.ijs.si | rank: 0
Node name: nsc-msv007.ijs.si | rank: 4
Node name: nsc-msv011.ijs.si | rank: 28
Node name: nsc-msv011.ijs.si | rank: 24
Node name: nsc-msv011.ijs.si | rank: 19
Node name: nsc-msv011.ijs.si | rank: 23
Node name: nsc-msv011.ijs.si | rank: 29
Node name: nsc-msv011.ijs.si | rank: 26
Node name: nsc-msv011.ijs.si | rank: 30
Node name: nsc-msv011.ijs.si | rank: 16
Node name: nsc-msv011.ijs.si | rank: 20
Node name: nsc-msv011.ijs.si | rank: 27
Node name: nsc-msv011.ijs.si | rank: 31
Node name: nsc-msv011.ijs.si | rank: 17
Node name: nsc-msv011.ijs.si | rank: 21
Node name: nsc-msv011.ijs.si | rank: 25
Node name: nsc-msv011.ijs.si | rank: 18
Node name: nsc-msv011.ijs.si | rank: 22
PI = 3.141573 | computed in 2.2221 s
Number of processes: 32

*/