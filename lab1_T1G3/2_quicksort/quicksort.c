#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

struct timeval  stop,
                init;

static inline __attribute__((always_inline)) void start_timer(){gettimeofday(&init, NULL);}

static inline __attribute__((always_inline)) void stop_timer(long long *elp){
    gettimeofday(&stop, NULL);
    *elp = (stop.tv_sec - init.tv_sec) * 1e6 + stop.tv_usec - init.tv_usec;}

// FIXME: remove auxiliary function for tests
void print_vector(int* a, int size) {
    for (int i = 0; i < size; i++) printf("%d ", a[i]);
    printf("\n");
}
    
int partition(int *a, int lo, int hi){
    int     i = lo, 
            j = hi, 
            pivot = lo, 
            temp;

    while(i < j){
        while(a[i] <= a[pivot] && i < hi) i++;
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
        { (void) Quicksort(a, lo, p - 1); } // Left branch
        #pragma omp task
        { (void) Quicksort(a, p + 1, hi); } // Right branch
        #pragma omp taskwait
    }
}
 
int main(int argc, char *argv[])
{
    srand(1);
    
    int size;
    int counter = 0;
    
    long long elp = 0;
    
    if (argc < 1 || sscanf(argv[1], "%i", &size)!=1) {printf("\n\nPlease set the size of the array!\n\n"); return -1;}
    
    
    int *a = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {a[i] = rand() % size;}

    start_timer();

    // Parallelize Quicksort with tasks
    #pragma omp parallel // We create threads outside to avoid massive overhead
    {
        #pragma omp single nowait
        Quicksort(a, 0, size - 1);
    }
    stop_timer(&elp);
    
    double runtime = (double) elp / 1000000.0;

    printf("%lf\t", runtime);
    
    for (int i = 0; i < size - 1; i++){
        if (a[i] > a[i + 1]) counter++;
    }
    if (counter != 0) printf("1\n");
    else printf("0\n");

    free(a);
    
    return 0;
} 
