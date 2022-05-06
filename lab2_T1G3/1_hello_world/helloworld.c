#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define MAX_MESSAGE_SIZE 400

void printMessage(char* message, MPI_Comm comm) {
    MPI_Status status;
    int comm_rank, comm_size;
    MPI_Comm_rank(comm, &comm_rank);
    MPI_Comm_size(comm, &comm_size);

    if (comm_rank == 0) {
        printf("%s", message);
        for (int i = 0; i < comm_size - 1; ++i) {
            MPI_Recv(message, MAX_MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, 0, comm, &status);
            printf("%s", message);
        }
        printf("---------------------\n\n");
    }
    else MPI_Send(message, MAX_MESSAGE_SIZE, MPI_CHAR, 0, 0, comm);
}

int main(int argc, char ** argv)
{
    MPI_Init(NULL, NULL);

    /******************************** PHASE 1 ********************************/
    // Get world info
    int world_rank, world_size;
    char world_name[100] = {"MPI_COMM_WORLD"};
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_set_name(MPI_COMM_WORLD, world_name);

    // Set message to print
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, MAX_MESSAGE_SIZE,
        "Hi, I'm rank %d. My comunicator is %s and has a size of %d processes.\n",
        world_rank, world_name, world_size);

    // Print message
    printMessage(message, MPI_COMM_WORLD);

    /******************************** PHASE 2 ********************************/
    int  color, split_rank, split_size;
    MPI_Comm SPLIT_COMM;
    char split_name[100] = {"SPLIT_COMM"};
    
    // Determine color based on row
    color = world_rank / 4;
    
    // Split the communicator based on the color and use the original rank for ordering
    MPI_Comm_split (MPI_COMM_WORLD, color, world_rank, &SPLIT_COMM);
    MPI_Comm_rank (SPLIT_COMM, &split_rank);
    MPI_Comm_size (SPLIT_COMM, &split_size);
    MPI_Comm_set_name(MPI_COMM_WORLD, world_name);

    // Set message to print
    snprintf(message, MAX_MESSAGE_SIZE,
        "Hi, I was rank %d in comunicator %s which had %d processes. Now I'm rank %d in comunicator %s_%d which has %d processes.\n",
        world_rank, world_name, world_size, split_rank, split_name, color, split_size);

    // Print message
    printMessage(message, MPI_COMM_WORLD);

    /******************************** PHASE 3 ********************************/
    MPI_Group world_group;
    MPI_Group even_group;
    MPI_Comm EVEN_COMM;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group); // Get the group of the World communicator

    // Create Group for processors with even rank
    int n = world_size/2;
    int* even_ranks = malloc( sizeof(int) * n );
    for(int i = 0; i < n; i++) even_ranks[i] = 2*i;
    MPI_Group_incl (world_group, n, even_ranks, &even_group );
    MPI_Comm_create ( MPI_COMM_WORLD, even_group, &EVEN_COMM );

    // Print message
    int even_rank, even_size;
    char even_name[100] = {"EVEN_COMM"};

    if(world_rank % 2 == 0) {
        MPI_Comm_rank (EVEN_COMM, &even_rank);
        MPI_Comm_size (EVEN_COMM, &even_size);
        snprintf(message, MAX_MESSAGE_SIZE,
            "Hi, I was rank %d in comunicator %s_%d which had %d processes. Now I'm rank %d in communicator %s which has %d processes. \n",
            split_rank, split_name, color, split_size, even_rank, even_name, even_size );
        printMessage(message, EVEN_COMM);
    }


    /******************************** PHASE 4 ********************************/
    // Create Group for processors with odd rank
    MPI_Group odd_group;
    MPI_Comm ODD_COMM;
    MPI_Group_excl (world_group, n, even_ranks, &odd_group );
    MPI_Comm_create ( MPI_COMM_WORLD, odd_group, &ODD_COMM );

    // Print message
    int odd_rank, odd_size;
    char odd_name[100] = {"ODD_COMM"};

    if(world_rank % 2 != 0) {
        MPI_Comm_rank (ODD_COMM, &odd_rank);
        MPI_Comm_size (ODD_COMM, &odd_size);
        snprintf(message, MAX_MESSAGE_SIZE,
            "Hi, I was rank %d in comunicator %s which had %d processes. Now I'm rank %d in communicator %s which has %d processes. \n", 
            world_rank, world_name, world_size, odd_rank, odd_name, odd_size );
        printMessage(message, ODD_COMM);
    }


    /************************** Close MPI Framework **************************/
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
