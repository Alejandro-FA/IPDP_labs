#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // TODO: Is it necessary? Check if performance improves when changing num_threads
#include "mpi.h"

#define ind(i, j, nx) (((i)*(nx)) + (j))

double dot_product(int p_size, double* array1, double* array2, int num_threads) {
    double sum = 0.0;
    #pragma omp parallel for simd reduction(+: sum) num_threads(num_threads)
    for(int i = 0; i < p_size; i++) sum += array1[i]*array2[i];
    
    return sum;
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
    int p_size1, p_size2;
    double * buf_array1 = par_read(file1_name, &p_size1, world_rank, world_size);
    double * buf_array2 = par_read(file2_name, &p_size2, world_rank, world_size);
    
    
    // Free memory allocated in the heap and close framework
    free(buf_array1);
    free(buf_array2);
    
    MPI_Finalize();
    return 0;
}




