#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

double * par_read(char * in_file, int * p_size, int rank, int nprocs ){

    MPI_File fh;
    int filesize, floatsize;

    MPI_File_open (MPI_COMM_WORLD, in_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    MPI_File_get_size (fh, &filesize);
    MPI_Type_size (MPI_INT, &intsize);
    bufsize = filesize/nprocs;
    nints = bufsize/intsize;
    

}


int main()
{

    MPI_Init(NULL, NULL);
    
    int world_rank, world_size;
    char file1_name[100] = {"/shared/Labs/Lab_2/array_p.bin"};
    char file2_name[100] = {"/shared/Labs/Lab_2/array_q.bin"};

    
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    MPI_Finalize();
    return 0;
}

