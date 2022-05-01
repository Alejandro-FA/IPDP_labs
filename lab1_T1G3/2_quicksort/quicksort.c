#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/********************** AUXILIARY FUNCTIONS *********************/
void incorrect_arguments() {
    printf("3 arguments are required:\n");
    printf("   The first argument is the mode of execution. Possible values are '1' (exercise 4) and '2' (exercise 5).\n");
    printf("   The second argument is the number of threads to use. It cannot be greater than 24.\n");
    printf("   The third argument defines the size of the array to sort. It must be a positive integer value.\n");
    printf("   The fourth argument is X, the condition value for exercise 5. Ignored for execution mode 1.\n");
}

int partition(int *a, int lo, int hi){
    int     i = lo, 
            j = hi, 
            pivot = lo, 
            temp;

    while(i < j){
        // Find first element from the left greater than the pivot
        while(a[i] <= a[pivot] && i < hi) i++;
        // Find first element from the right smaller than the pivot
        while(a[j] > a[pivot]) j--;
        if(i < j){
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }
    temp        = a[pivot];
    a[pivot]    = a[j];
    a[j]        = temp;
    return j;
}

void Quicksort(int *a, int lo, int hi){
    if ( lo < hi ) {
        int p = partition(a, lo, hi);
        
        #pragma omp task
        { (void) Quicksort(a, lo, p - 1); } // Left subarray
        #pragma omp task
        { (void) Quicksort(a, p + 1, hi); } // Right subarray
    }
}

void Quicksort_EX5(int *a, int lo, int hi, int X) {
    if ( lo < hi ) {
        int p = partition(a, lo, hi);
        
        #pragma omp task if (hi - lo <= X)
        { (void) Quicksort_EX5(a, lo, p - 1, X); } // Left subarray
        #pragma omp task if (hi - lo <= X)
        { (void) Quicksort_EX5(a, p + 1, hi, X); } // Right subarray
    }
}

/*************************** MAIN CODE **************************/
int main(int argc, char *argv[])
{   
    // Ensure that we have 4 arguments
    if (argc != 5) { incorrect_arguments(); return -1; }

    // Retrieve arguments
    int exec_type = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int size = atoi(argv[3]);
    int X = atoi(argv[4]);
    
    // Check that arguments are correct
    if (exec_type != 1 && exec_type != 2) {
        printf("The execution type is incorrect!\nPossible arguments are '1' (exercise 4) and '2' (exercise 5).\n");
        return -1;
    }
    if (num_threads < 1 || num_threads > 24) {
        printf("The second argument defines the number of threads and it must be between 1 and 24.\nUsing default value of 1...\n");
        num_threads = 1;
    }
    if (size < 1) {
        printf("The second argument defines the size of the array to sort. It must be a positive integer value.\n");
        return -1;
    }
    if(exec_type == 2 && X < 1) {
        printf("The third argument defines X. It must be a positive (>0) integer value.\n");
        return -1;
    }

    // Initialize array randomly with the specified size
    srand(1);
    int *a = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {a[i] = rand() % size;}


    // Parallelize Quicksort with tasks
    double start = omp_get_wtime();
    #pragma omp parallel num_threads(num_threads) // We create threads outside to avoid massive overhead
    {
        #pragma omp single // Includes implicit synchronization
        { 
            switch(exec_type){
            case 1:
                Quicksort(a, 0, size - 1);
                break;
            case 2:
                Quicksort_EX5(a, 0, size-1, X);
                break;
            }
        }
    }
    double end = omp_get_wtime();
    printf("%lf\t", end - start);
    
    // Check that the result is correct
    int counter = 0;
    for (int i = 0; i < size - 1; i++) if (a[i] > a[i + 1]) counter++;
    if (counter != 0) printf("1\n");
    else printf("0\n");

    // Free heap memory and exit
    free(a);
    return 0;
} 
