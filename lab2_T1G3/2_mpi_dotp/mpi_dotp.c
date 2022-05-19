#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // TODO: Is it necessary? Check if performance improves when changing num_threads
#include "mpi.h"

double dot_product(int p_size, double* array1, double* array2, int num_threads) {
    double sum = 0.0;
    #pragma omp parallel for simd reduction(+: sum) num_threads(num_threads)
    for(int i = 0; i < p_size; i++) sum += array1[i]*array2[i];
    
    return sum;
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



int main() {
    MPI_Init(NULL, NULL);
    
    // Get world communicator information
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Read the corresponding section of both arrays
    char file1_name[100] = {"/shared/Labs/Lab_2/array_p.bin\0"};
    char file2_name[100] = {"/shared/Labs/Lab_2/array_q.bin\0"};
    int p_size1, p_size2;
    // TODO: compute execution time
    double * buf_array1 = par_read(file1_name, &p_size1, world_rank, world_size);
    double * buf_array2 = par_read(file2_name, &p_size2, world_rank, world_size);
    
    // Compute the dot product using OpenMP
    int nthreads = 4;
    double sum = 0.0;
    if (p_size1 == p_size2) {
        sum = dot_product(p_size1, buf_array1, buf_array2, nthreads);
        printf("Process with rank %d: sum = %lf\n", world_rank, sum);
    } else printf("Process %d: array sizes do not match. Array 1 size: %d, array 2 size: %d\n", world_rank, p_size1, p_size2);
    fflush(stdout);

    // FIXME: Fix the number of threads in and outside the reduction
    // Combine the results of each process
    double solution = 0.0;
    MPI_Reduce( &sum, &solution, 4, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (world_rank == 0) printf("The result of the dot product is %lf\n", solution);

    // Free memory allocated in the heap and close framework
    free(buf_array1);
    free(buf_array2);
    
    MPI_Finalize();
    return 0;
}




