#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // TODO: Is it necessary? Check if performance improves when changing num_threads
#include "mpi.h"
#include <math.h>

void print_matrix(int size, int * matrix){
    
    for(int i = 0; i < size; i++){
       for(int j = 0; j < size; j++){
           printf("%d ", matrix[i+j]);
       }
       printf("\n");
    }
    
    
}

int * new_matrix(int size, int rank){
    int * matrix = malloc(pow(size,2) * sizeof(int));
    
    int nrow = 0;
    for(int i = 0; i < size; i++){
       for(int j = 0; j < size; j++){
           if( i == j) matrix[(size * nrow) + (i+j)] = rank;
           else matrix[i+j] = 0;
       }
        nrow++;
    }

    return matrix;
}



int main()
{
    MPI_Init(NULL, NULL);
    
    // Get world communicator information
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int * matrix = new_matrix(world_size, world_rank);

    if(world_rank == 1) print_matrix(world_size, matrix);
    
    // Free memory allocated in the heap and close framework
    free(matrix);

    MPI_Finalize();
    return 0;
    
}
