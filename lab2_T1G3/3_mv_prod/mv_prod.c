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
    MPI_Offset filesize, read_offset;
    MPI_Status status;
    int bufsize;

    // Open specified file
    MPI_File_open (MPI_COMM_WORLD, in_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    // Get information about the opened file
    MPI_File_get_size (fh, &filesize);
    read_offset = (filesize/nprocs) * rank;
    if (rank == nprocs - 1) bufsize = filesize - read_offset; // Takes into account non-divisible number of elements
    else bufsize = filesize / nprocs;
    *p_size = bufsize / sizeof(double);

    // Allocate buffer to store the doubles read from the file
    double* buf = malloc ( bufsize );

    // Read at the specific position depending on the rank of the process
    MPI_File_read_at (fh, read_offset , buf, *p_size, MPI_DOUBLE, &status);

    // Control print and return
    printf("Process %d : first index %lld value %lf - last index %lld value %lf\n",
        rank, read_offset/sizeof(double), buf[0], read_offset/sizeof(double) + (*p_size) -1, buf[*p_size-1]);
    MPI_File_close (&fh);

    return buf;
}


int main(int argc, char const *argv[]){

    if(argc != 2){
        printf("The number of threads is required as an argument. \n");
        return -1;
    }

    int num_threads = argv[1];

    MPI_Init(NULL, NULL);
    
    // Get world communicator information
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Read the corresponding section of both arrays
    char file1_name[100] = {"/shared/Labs/Lab_2/matrix.bin\0"};
    char file2_name[100] = {"/shared/Labs/Lab_2/matrix_vector.bin\0"};
    int mat_elems, vec_elems;
    double start_time, end_time;

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

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
    double * res_section = mat_prod(matrix_section, vector_complete, mat_elems / (vec_elems * world_size), vec_elems * world_size, num_threads);
    double * res_complete = malloc(vec_elems * world_size * sizeof(double));
    MPI_Allgather (res_section, vec_elems, MPI_DOUBLE, res_complete, vec_elems, MPI_DOUBLE, MPI_COMM_WORLD);

    // No barrier needed since gather already has it 
    end_time = MPI_Wtime();

    if(world_rank == 0) {
        for (int i = 0; i < vec_elems * world_size; ++i) {
            printf("%lf\n", res_complete[i]);
        }
        fflush(stdout);
        printf("It took %f seconds to read the files, to distribute vector `b`, to compute the multiplication and to gather the final result.\n Number of processes: %d \n Number of threads: %d\n", end_time - start_time, world_size, num_threads);
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




