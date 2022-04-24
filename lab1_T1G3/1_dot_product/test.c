#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "omp.h"
#include <unistd.h>

struct timeval  stop,
                init;

static inline __attribute__((always_inline)) void start_timer(){gettimeofday(&init, NULL);}

static inline __attribute__((always_inline)) void stop_timer(long long *elp){
    gettimeofday(&stop, NULL);
    *elp = (stop.tv_sec - init.tv_sec) * 1e6 + stop.tv_usec - init.tv_usec;}

int main(int argc, char const *argv[])
{
    long long elp = 0;
    start_timer();
    sleep(5);
    stop_timer(&elp);
    
    double runtime = (double) elp / 1000000.0;

    printf("%lf\t", runtime);
    return 0;
}
