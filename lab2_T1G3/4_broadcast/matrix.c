#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // TODO: Is it necessary? Check if performance improves when changing num_threads
#include "mpi.h"
#include <math.h>

#define ind(i, j, nx) (((i)*(nx)) + (j))

void print_matrix(int size, int * matrix){
    
    for(int i = 0; i < size; i++){
       for(int j = 0; j < size; j++){
           printf("%d ", matrix[ind(i,j,size)]);
       }
       printf("\n");
    }
    printf("\n");
}

int * new_matrix(int size, int rank){
    int * matrix = malloc(pow(size,2) * sizeof(int));
    
    for(int i = 0; i < size; i++){
       for(int j = 0; j < size; j++){
           if( i == j) matrix[ind(i,j,size)] = rank;
           else matrix[ind(i,j,size)] = 0;
       }
    }
    return matrix;
}



int main()
{
    MPI_Init(NULL, NULL);
    double start_time, end_time;
    
    // Get world communicator information
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    int * matrix = new_matrix(world_size, world_rank);
    if(world_rank == 0) {
        printf("Initial matrix (rank 0):\n");
        print_matrix(world_size, matrix);
    }
    
    // Creation of the derived data type which will contain the diagonal elements of the matrix
    MPI_Datatype diagonal;
    MPI_Type_vector (world_size, 1, world_size + 1, MPI_INT, &diagonal );
    MPI_Type_commit (&diagonal);

    // Gathering all the diagonals from all the processes to process 0
    int * matrix_result = malloc(sizeof(int) * pow(world_size,2));
    MPI_Gather ( &matrix[0], 1, diagonal, matrix_result, world_size, MPI_INT, 0, MPI_COMM_WORLD);

    // No barrier needed since the Gather already has one
    end_time = MPI_Wtime();

    // Print result
    if(world_rank == 0) {
        printf("Final matrix (rank 0):\n");
        print_matrix(world_size, matrix_result);
        printf("It took %f seconds to create the individual matrices and gather their diagonals. Performance of %d processes. \n", end_time - start_time, world_size );
    
    }

    // Free memory allocated in the heap and close framework
    free(matrix);
    free(matrix_result);
    MPI_Type_free (&diagonal);

    MPI_Finalize();
    return 0;
    
}
