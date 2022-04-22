#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"
// GLOBAL VARIABLES

int * a;
int * b;

// FUNCTIONS
void incorrect_arguments() {
    printf("3 arguments are required:\n");
    printf("   The first argument is the mode of execution. Possible values are '1' (sequential), '2' (parralel) and '3' (parallel with vectorization).\n");
    printf("   The second argument is the number of threads to use. Not required for sequential execution.\n");
    printf("   The third argument (or second one for the sequential mode) defines the size of the vectors. It must be a positive integer value. \n");
}

void init_vectors(int size){
    a = malloc(sizeof(int) * size);
    b = malloc(sizeof(int) * size);
    for(int i = 0; i < size; i++ ){
        a[i] = (int) (rand() % size - size/2);
        b[i] = (int) (rand() % size - size/2);
    }
}

int dot_product(int num_threads ,int size){
    int sum = 0;
    
    // #pragma omp parallel num_threads(num_threads)
    // {
    //     int sum_tmp = 0;
    //     #pragma omp for
    //     for(int i = 0; i < size; i++){
    //         sum_tmp += a[i]*b[i];
    //     }
    //     #pragma omp critical
    //     {
    //         sum += sum_tmp;
    //     }
    // }

    #pragma omp parallel for num_threads(num_threads) reduction(+:sum)
    for(int i = 0; i< size; i++){
        sum += a[i]*b[i];
    }
    
    return sum;
}

// MAIN CODE
int main(int argc, char* argv[]){

    // Retrieve parameters and check that the value is correct
    int exec_type = atoi(argv[1]);
    int num_threads;
    int size;
    switch(exec_type){
        case 1:
            if (argc != 3) {
                incorrect_arguments();
                return -1;
            }
            num_threads = 1;
            size = atoi(argv[2]);
            break;
        case 2:
        case 3:
            if (argc != 4) {
                incorrect_arguments();
                return -1;
            }
            num_threads = atoi(argv[2]);
            size = atoi(argv[3]);
            break;
        default:
            printf("The execution type is incorrect!\nPossible arguments are '1' (sequential), '2' (parrallel) and '3' (parallel with vectorization)\n");
            return -2;
    }

    if (num_threads < 1) {
        printf("The second argument defines the number of threads. It must be a positive (>0) integer value, and it does not have to be passed if execution type is sequential.\nUsing default value of 1...\n");
        num_threads = 1;
    }
    if(size < 1){
        printf("The third argument (or second one for the sequential mode) defines the size of the vectors. It must be a positive (>0) integer value. \n");
        return -1;
    }

    // main program
    init_vectors(size);
    
    clock_t time1, time2;
    double dub_time;
    time1 = clock();
    int sum = dot_product(num_threads, size);
    time2 = clock();
    dub_time = (time2 - time1)/(double)CLOCKS_PER_SEC;
    printf("%lf\t%i\n", dub_time, sum);

    // Free heap memory and exit function
    free(a);
    free(b);
    return 0;
}