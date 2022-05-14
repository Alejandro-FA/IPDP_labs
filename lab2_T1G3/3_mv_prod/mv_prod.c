#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // TODO: Is it necessary? Check if performance improves when changing num_threads
#include <unistd.h>

#include "mpi.h"

#define ind(i, j, nx) (((i)*(nx)) + (j))

double* mat_prod(double* matrix, double* vector, int mat_rows, int vec_size, int num_threads) {
    double * result = malloc(sizeof(double) * mat_rows);
    
    #pragma omp parallel for simd num_threads(num_threads)
    for (int i = 0; i < mat_rows; i++) {
        double sum = 0.0;
        for(int j = 0; j < vec_size; j++) sum += matrix[ind(i,j,vec_size)] * vector[j];
        
        result[i] = sum;
    }

    return result;
}

double * par_read(char * in_file, int * p_size, int rank, int nprocs ) {
    MPI_File fh;
    MPI_Offset filesize, bufsize;
    MPI_Status status;
    int doublesize;

    // Open specified file
    MPI_File_open (MPI_COMM_WORLD, in_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    // Get information about the opened file
    MPI_File_get_size (fh, &filesize);
    MPI_Type_size (MPI_DOUBLE, &doublesize);
    bufsize = filesize / nprocs;
    int ndoubles = bufsize / doublesize;
    *p_size = bufsize < filesize - rank*bufsize ? bufsize : filesize - rank*bufsize;
    *p_size /= sizeof(MPI_DOUBLE);

    // Allocate buffer to store the doubles read from the file
    double* buf = malloc ( bufsize );

    // Read at the specific position depending on the rank of the process
    MPI_File_seek (fh, rank*bufsize, MPI_SEEK_SET);
    MPI_File_read (fh, buf, ndoubles, MPI_DOUBLE, &status);

    // Control print and return
    printf("Process %d : first index %d value %lf - last index %d value %lf\n",
        rank, rank*ndoubles, buf[0], (rank+1)*ndoubles-1, buf[ndoubles-1]);
    MPI_File_close (&fh);

    return buf;
}


int main() {
    MPI_Init(NULL, NULL);
    
    // Get world communicator information
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Read the corresponding section of both arrays
    char file1_name[100] = {"/shared/Labs/Lab_2/matrix.bin\0"};
    char file2_name[100] = {"/shared/Labs/Lab_2/matrix_vector.bin\0"};
    int mat_elems, vec_elems;
    double * matrix_section = par_read(file1_name, &mat_elems, world_rank, world_size);
    double * vector_section = par_read(file2_name, &vec_elems, world_rank, world_size);
    fflush(stdout);
    sleep(1);
    MPI_Barrier(MPI_COMM_WORLD);

    // Distribute b to all processes
    double * vector_complete = malloc(vec_elems * world_size * sizeof(double));
    MPI_Allgather (vector_section, vec_elems, MPI_DOUBLE, vector_complete, vec_elems, MPI_DOUBLE, MPI_COMM_WORLD);
    if(world_rank == 0){
        printf("Complete vector elements: %d. Complete matrix rows: %d, columns: %d\n",
            vec_elems * world_size, mat_elems / vec_elems, vec_elems * world_size);
        fflush(stdout);
    }

    // Compute the matrix - vector product
    int num_threads = 1;
    double * res_section = mat_prod(matrix_section, vector_complete, mat_elems / (vec_elems * world_size), vec_elems * world_size, num_threads);
    double * res_complete = malloc(vec_elems * world_size * sizeof(double));
    MPI_Allgather (res_section, vec_elems, MPI_DOUBLE, res_complete, vec_elems, MPI_DOUBLE, MPI_COMM_WORLD);
    if(world_rank == 0) {
        for (int i = 0; i < vec_elems * world_size; ++i) {
            printf("%lf\n", res_complete[i]);
        }
        fflush(stdout);
    }
    
    // Free memory allocated in the heap and close framework
    free(matrix_section);
    free(vector_section);
    free(vector_complete);
    free(res_section);
    free(res_complete);
    
    MPI_Finalize();
    return 0;
}



