#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
#include "aux.h"


#define FILENAME "life1.bin"
#define iter 100

int     row_size;

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
    int *local_matrix = /* */
    
    
    
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
    MPI_Status stat;
    int prev;                       // Stores process previous neighbour
    int next;                       // Stores process next neighbour


    /* Initialize neighbours */
    if ( nprocs == 1 ) {
        next = 0;
        prev = 0;
    }
    else {
        
    /* Ex 5.2: Define neighbours */
    
    //
    //
    //
    }
    
    
    /* Ex 5.3: Make rank 0 print the problem */
    
    //
    //
    //
    

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
            
            //
            //
            //
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
    
    //
    //
    //
    
    MPI_Finalize();
    return 0;
}

