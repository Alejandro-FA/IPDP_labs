#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
#include "aux.h"


#define FILENAME "life1.bin"
#define iter 100

int     row_size;

int * par_read(char * in_file, int * p_size, int rank, int nprocs ) {
    MPI_File fh;
    MPI_Offset filesize, bufsize;
    MPI_Status status;

    // Open specified file
    MPI_File_open (MPI_COMM_WORLD, in_file, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    // Get information about the opened file
    MPI_File_get_size (fh, &filesize);
    bufsize = filesize / nprocs;
    *p_size = bufsize < filesize - rank*bufsize ? bufsize : filesize - rank*bufsize;
    *p_size /= sizeof(int);

    // Allocate buffer to store the doubles read from the file
    int* buf = malloc ( bufsize );

    // Read at the specific position depending on the rank of the process
    MPI_File_seek (fh, rank*bufsize, MPI_SEEK_SET);
    MPI_File_read (fh, buf, *p_size, MPI_INT, &status);

    // Control print and return
    printf("Process %d : first index %d value %d - last index %d value %d\n",
        rank, rank*(*p_size), buf[0], (rank+1)*(*p_size)-1, buf[*p_size-1]);
    MPI_File_close (&fh);

    return buf;
}

/*
* Everything starts from here.
*/
int main (int argc, char** argv){
    
    int         nprocs, 
                rank,
                i = 0;
                
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    /* Local matrix and matrix to update values */
    int local_entries;
    
    /* Ex 5.1 Read the binary file */
    int *local_matrix = par_read(FILENAME, &local_entries, rank, nprocs);
    
    int *update_matrix = calloc(local_entries, sizeof(int));
    int total_entries = local_entries * nprocs;
    
    row_size = sqrt(local_entries * nprocs);
    
    /* Variables to store rows */
    int lower_recv[row_size];       //value from lower neighbor
    int upper_recv[row_size];       //value from upper neighbor
    int lower_send[row_size];       //value to send to lower neighbor
    int upper_send[row_size];       //value to send to upper neighbor
    int lower_row[row_size];
    int upper_row[row_size];
    int current_row[row_size];      //current row to update
    int tmp_row[row_size];          //store update for a row
    

    
    /* Messaging variables */
    MPI_Status stats[4];
    int prev;                       // Stores process previous neighbour
    int next;                       // Stores process next neighbour


    /* Initialize neighbours */
    if ( nprocs == 1 ) {
        next = 0;
        prev = 0;
    }
    else {
        
    /* Ex 5.2: Define neighbours */
        next = rank + 1;
        prev = rank - 1;

        if(rank == 0) prev = nprocs - 1;
        if(rank == nprocs - 1) next = 0;
    }
    
    
    /* Ex 5.3: Make rank 0 print the problem */
    int * received_matrix = malloc(sizeof(int) * total_entries);
    MPI_Gather ( local_matrix, local_entries, MPI_INT, received_matrix, local_entries, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        bitmap(received_matrix, total_entries, row_size, nprocs, 0);
        printf("Received matrix first element: %d, last element: %d", received_matrix[0], received_matrix[total_entries - 1]);
    }
    

    /* Generations */
    for (i = 0; i < iter; i++){
        
        /* Get the buffer to send */
        for (int j = 0; j < row_size; j++) {
            upper_send[j] = local_matrix[j];
            lower_send[row_size - j - 1] = local_matrix[local_entries - j - 1];
        }
        
        /* Send and receive point to point messages */
        if (nprocs == 1) {
            copyRow( lower_send, upper_recv);
            copyRow( upper_send, lower_recv);
        }
        else {
            
            /* Ex 5.4: Communicate upper and lower rows to the neighbouring processes */
            MPI_Request requests[4];
            
            MPI_Isend ( lower_send, row_size, MPI_INT, next, 0, MPI_COMM_WORLD, &requests[0]);
            MPI_Irecv ( lower_recv, row_size, MPI_INT, next, 1, MPI_COMM_WORLD, &requests[1]);
            MPI_Isend ( upper_send, row_size, MPI_INT, prev, 1, MPI_COMM_WORLD, &requests[2]);
            MPI_Irecv ( upper_recv, row_size, MPI_INT, prev, 0, MPI_COMM_WORLD, &requests[3]);

            MPI_Waitall(4, requests, stats);
        }

        /* Update the values */
        getRow(0, local_matrix, current_row);                           // Get the first row
        if (row_size == nprocs){                                        // One row per process, store new values to update_matrix directly
            newValue(lower_recv, current_row, upper_recv, tmp_row);
            copyRow(tmp_row, update_matrix);                            // Update local_matrix values
        }
        else {
            
            /* First row */
            getRow(1, local_matrix, lower_row);
            newValue( lower_row, current_row, upper_recv, tmp_row ); 
            copyRow(tmp_row, update_matrix);

            /* Last row */
            getRow((local_entries/row_size - 1), local_matrix, current_row);
            getRow((local_entries/row_size - 2), local_matrix, upper_row);
            newValue( lower_recv, current_row, upper_row, tmp_row ); 
            copyRow( tmp_row, &update_matrix[local_entries - row_size]);

            /* More than 2 rows per process */
            if ((local_entries / row_size) > 2) {
                for (int j = 1; j < (local_entries / row_size) - 1; j++)
                {
                    getRow(j+1, local_matrix, lower_row);
                    getRow(j, local_matrix, current_row);
                    getRow(j-1, local_matrix, upper_row);
                    newValue( lower_row, current_row, upper_row, tmp_row );
                    copyRow( tmp_row, &update_matrix[ j * row_size ] );
                }
            }
        }
        
        /* Copy over the old local_matrix. */
        for (int j = 0; j < local_entries; j++) {
            local_matrix[j] = update_matrix[j];
        }
    }


    
    /* Ex 5.5: Output the final state */
    MPI_Gather ( local_matrix, local_entries, MPI_INT, received_matrix, local_entries, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank == 0){
        bitmap(received_matrix, total_entries, row_size, nprocs, iter);
    }
    
    free(local_matrix);
    // free(update_matrix);
    free(received_matrix);
    
    MPI_Finalize();
    return 0;
}

