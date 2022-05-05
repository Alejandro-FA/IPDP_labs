#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void printPhase(int phase, int world_rank) {
    if(world_rank == 0) printf("Phase %d:\n", phase);
}

void printSeparator(int world_rank) {
    if(world_rank == 0) printf("---------------------\n\n");
}

int main(int argc, char ** argv)
{
    /****************** PHASE 1 ********************/
    int world_rank, world_size;
    char world_name[100] = {"MPI_COMM_WORLD"};
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_set_name(MPI_COMM_WORLD, world_name);

    MPI_Barrier(MPI_COMM_WORLD);
    printPhase(1, world_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Hi, I'm rank %d. My comunicator is %s and has a size of %d processes.\n", world_rank, world_name, world_size);
    MPI_Barrier(MPI_COMM_WORLD);
    printSeparator(world_rank);

    /*****************+ PHASE 2 **********************/
    int  color, split_rank, split_size;
    MPI_Comm SPLIT_COMM;
    char split_name[100] = {"SPLIT_COMM"};
    
    /* Determine color based on row */
    color = world_rank / 4;
    
    /* Split the communicator based on the color and use the original rank for ordering */
    MPI_Comm_split (MPI_COMM_WORLD, color, world_rank, &SPLIT_COMM);
    MPI_Comm_rank (SPLIT_COMM, &split_rank);
    MPI_Comm_size (SPLIT_COMM, &split_size);
    MPI_Comm_set_name(MPI_COMM_WORLD, world_name);

    MPI_Barrier(MPI_COMM_WORLD);
    printPhase(2, world_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Hi, I was rank %d in comunicator %s which had %d processes. Now I'm rank %d in comunicator %s_%d which has %d processes.\n",
        world_rank, world_name, world_size, split_rank, split_name, color, split_size);
    MPI_Barrier(MPI_COMM_WORLD);
    printSeparator(world_rank);

    /*****************+ PHASE 3 **********************/
    MPI_Group group;
    MPI_Group even_group;
    MPI_Comm EVEN_COMM;
    MPI_Comm_group(MPI_COMM_WORLD, &group); // Get the group of the World communicator

    int n = world_size/2;
    int* even_ranks = malloc( sizeof(int) * n );
    for(int i = 0; i < n; i++) even_ranks[i] = 2*i;
    
    MPI_Group_incl (group, n, even_ranks, &even_group );
    MPI_Comm_create ( MPI_COMM_WORLD, even_group, &EVEN_COMM );

    int even_rank, even_size;
    char even_name[100] = {"EVEN_COMM"};

    MPI_Barrier(MPI_COMM_WORLD);
    printPhase(3, world_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank % 2 == 0) {
        MPI_Comm_rank (EVEN_COMM, &even_rank);
        MPI_Comm_size (EVEN_COMM, &even_size);
        printf("Hi, I was rank %d in comunicator %s_%d which had %d processes. Now I'm rank %d in communicator %s which has %d processes. \n", split_rank, split_name, color, split_size, even_rank, even_name, even_size );
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printSeparator(world_rank);


    /*****************+ PHASE 4 **********************/
    MPI_Group odd_group;
    MPI_Comm ODD_COMM;
    
    MPI_Group_excl (group, n, even_ranks, &odd_group );
    MPI_Comm_create ( MPI_COMM_WORLD, odd_group, &ODD_COMM );

    int odd_rank, odd_size;
    char odd_name[100] = {"ODD_COMM"};

    MPI_Barrier(MPI_COMM_WORLD);
    printPhase(4, world_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank % 2 != 0) {
        MPI_Comm_rank (ODD_COMM, &odd_rank);
        MPI_Comm_size (ODD_COMM, &odd_size);
        printf("Hi, I was rank %d in comunicator %s which had %d processes. Now I'm rank %d in communicator %s which has %d processes. \n", 
        world_rank, world_name, world_size, odd_rank, odd_name, odd_size );
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printSeparator(world_rank);


    // Close MPI framework
    MPI_Barrier(MPI_COMM_WORLD);
    free(even_ranks);
    if(world_rank % 2 == 0) {
        MPI_Group_free(&even_group);
        MPI_Comm_free (&EVEN_COMM);
    }
    else {
        MPI_Comm_free (&ODD_COMM); 
        MPI_Group_free(&odd_group);
    }
    MPI_Comm_free (&SPLIT_COMM);
    MPI_Finalize();
    return 0;
}
