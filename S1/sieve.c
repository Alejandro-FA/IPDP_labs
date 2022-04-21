#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

int main(int argc, char** argv) {
    int n;
    int i;
    int j;
    int *primes;
    int cnt=0;
    
    srand(time(NULL)+getpid());
    n = rand() % 10000;
    primes = (int*) malloc(sizeof(int) * (n+1));

    printf("PID %d: Calculating primes for %d\n",getpid(), n);

    for(i=2; i<=n; i++) {
        primes[i] = i;
    }
    
    i = 2;
    while ((i*i) <= n) {
        if (primes[i] != 0) {
            for(j=2; j<n; j++) {
                if (primes[i]*j > n) {
                    break;
                } else {
                    primes[primes[i]*j]=0;
                }
            }
        }
        i++;
    }

    for(i=2; i<=n; i++) {
        if(primes[i] != 0) {
            cnt++;
        }
    }
    
    printf("PID %d: There are %d primes smaller than %d\n",getpid(), cnt, n);
    
    return 0;
}
