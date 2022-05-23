#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // TODO: Is it necessary? Check if performance improves when changing num_threads
#include <unistd.h>

#include "mpi.h"

#define ind(i, j, nx) (((i)*(nx)) + (j))

double* mat_prod(double* matrix, double* vector, int mat_rows, int vec_size, int num_threads) {
    double * result = malloc(sizeof(double) * mat_rows);
    
    // #pragma omp parallel for simd num_threads(num_threads)
    for (int i = 0; i < mat_rows; i++) {
        double sum = 0.0;
        for(int j = 0; j < vec_size; j++) sum += matrix[ind(i,j,vec_size)] * vector[j];
        
        result[i] = sum;
    }

    return result;
}

double * par_read(char * in_file, int * p_size, int rank, int nprocs, int loglevel ) {
    MPI_File fh;
    MPI_Offset filesize, read_offset;
    MPI_Status status;

    // Open specified file
    MPI_File_open (MPI_COMM_WORLD, in_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    // Get information about the opened file
    MPI_File_get_size (fh, &filesize);
    filesize /= sizeof(double);
    read_offset = (filesize/nprocs) * rank;
    if (rank == nprocs - 1) *p_size = filesize - read_offset; // Takes into account non-divisible number of elements
    else *p_size = filesize / nprocs;


    // Allocate buffer to store the doubles read from the file
    double* buf = malloc ( (*p_size) * sizeof(double));

    // Read at the specific position depending on the rank of the process
    MPI_File_read_at (fh, read_offset * sizeof(double), buf, *p_size, MPI_DOUBLE, &status);

    // Control print and return
    if (loglevel == 0) {
        printf("Process %d : first index %lld value %lf - last index %lld value %lf\n",
            rank, read_offset, buf[0], read_offset + (*p_size) -1, buf[*p_size-1]);
    }
    MPI_File_close (&fh);

    return buf;
}


int main(int argc, char const *argv[]){

    /********************** Get the number of threads to use *********************/
    if (argc != 3) { printf("The number of threads and the log level (0 for debug or 1 for results only) have to be passed as arguments (e.g. './mpi_dotp 4 0')\n"); return -1; }
    int nthreads = atoi(argv[1]);
    int loglevel = atoi(argv[2]);
    if (nthreads < 1 || nthreads > 24) {
        printf("The argument defines the number of threads and it must be between 1 and 24.\nUsing default value of 1...\n");
        nthreads = 1;
    }
    if (loglevel != 0 && loglevel != 1) {
        printf("The log level determines the amount of information to print, and it has to be either 0 (debug) or 1 (just results).\nUsing default value of 0...\n");
        loglevel = 0;
    }



    MPI_Init(NULL, NULL);
    
    // Get world communicator information
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Read the corresponding section of both arrays
    char file1_name[100] = {"/shared/Labs/Lab_2/matrix.bin\0"};
    char file2_name[100] = {"/shared/Labs/Lab_2/matrix_vector.bin\0"};
    int mat_elems, vec_elems;

    double * matrix_section = par_read(file1_name, &mat_elems, world_rank, world_size, loglevel);
    double * vector_section = par_read(file2_name, &vec_elems, world_rank, world_size, loglevel);
    fflush(stdout);
    sleep(1);
    MPI_Barrier(MPI_COMM_WORLD);

    // Distribute b to all processes
    double * vector_complete = malloc(vec_elems * world_size * sizeof(double));
    MPI_Allgather (vector_section, vec_elems, MPI_DOUBLE, vector_complete, vec_elems, MPI_DOUBLE, MPI_COMM_WORLD);
    if(world_rank == 0 && loglevel == 0){
        printf("Complete vector elements: %d. Complete matrix rows: %d, columns: %d\n",
            vec_elems * world_size, mat_elems / vec_elems, vec_elems * world_size);
        fflush(stdout);
    }

    // Compute the matrix - vector product
    double start_time, end_time;
    double * res_complete = malloc(vec_elems * world_size * sizeof(double));
    start_time = MPI_Wtime();
    double * res_section = mat_prod(matrix_section, vector_complete, mat_elems / (vec_elems * world_size), vec_elems * world_size, nthreads);
    MPI_Allgather (res_section, vec_elems, MPI_DOUBLE, res_complete, vec_elems, MPI_DOUBLE, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    // No barrier needed since gather already has it 
    if(world_rank == 0) {
        if (loglevel == 0) {
            for (int i = 0; i < vec_elems * world_size; ++i) printf("%lf\n", res_complete[i]);
        }
        else printf("First element of matrix * vector product: %lf\n", res_complete[0]);
        printf("Execution time: %lf s\n", end_time - start_time);
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




