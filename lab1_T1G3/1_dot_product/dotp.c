#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

/************************* BENCHMARKING *************************/
struct timeval stop, init;

static inline __attribute__((always_inline)) void start_timer() { gettimeofday(&init, NULL); }

static inline __attribute__((always_inline)) void stop_timer(long long *elp) {
    gettimeofday(&stop, NULL);
    *elp = (stop.tv_sec - init.tv_sec) * 1e6 + stop.tv_usec - init.tv_usec;
}


/********************** AUXILIARY FUNCTIONS *********************/
// Vectors to perform the dot product are global variables
int* a;
int* b;

// Allocate heap memory for the specified vector and initialize it with random numbers
void init_vectors(int size){
    a = malloc(sizeof(int) * size);
    b = malloc(sizeof(int) * size);
    for(int i = 0; i < size; i++ ){
        a[i] = (int) (rand() % size - size/2);
        b[i] = (int) (rand() % size - size/2);
    }
}

// Perform dot product in parallel
int dot_product(int size, int exec_type, int num_threads) { //TODO: Modify function depending on execution type
    int sum = 0;
    #pragma omp parallel for num_threads(num_threads) reduction(+:sum)
    for(int i = 0; i< size; i++) {
        sum += a[i]*b[i];
    }
    return sum;
}

void incorrect_arguments() {
    printf("3 arguments are required:\n");
    printf("   The first argument is the mode of execution. Possible values are '1' (sequential), '2' (parallel) and '3' (parallel with vectorization).\n");
    printf("   The second argument is the number of threads to use. Ignored for sequential execution.\n");
    printf("   The third argument (or second one for the sequential mode) defines the size of the vectors. It must be a positive integer value. \n");
}


/*************************** MAIN CODE **************************/
int main(int argc, char* argv[]){
    // Ensure that we have 3 arguments
    if (argc != 4) { incorrect_arguments(); return -1; }

    // Retrieve parameters and check that the value is correct
    int exec_type = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int size = atoi(argv[3]);

    printf("exec_type: %d, num_threads: %d, size: %d\n", exec_type, num_threads, size);

    if (exec_type != 1 && exec_type != 2 && exec_type != 3) {
        printf("The execution type is incorrect!\nPossible arguments are '1' (sequential), '2' (parallel) and '3' (parallel with vectorization)\n");
        return -1;
    }
    if (num_threads < 1) {
        printf("The second argument defines the number of threads and it must be a positive (>0) integer value.\nUsing default value of 1...\n");
        num_threads = 1;
    }
    if(size < 1){
        printf("The third argument (or second one for the sequential mode) defines the size of the vectors. It must be a positive (>0) integer value. \n");
        return -1;
    }

    // Perform dot product and compute runtime
    init_vectors(size);
    long long elp = 0;
    start_timer();
    int result = dot_product(size, exec_type, num_threads);
    stop_timer(&elp);
    double runtime = (double) elp / 1000000.0;

    printf("%.4e\t%i\n", runtime, result);

    // Free heap memory and exit main function
    free(a);
    free(b);
    return 0;
}