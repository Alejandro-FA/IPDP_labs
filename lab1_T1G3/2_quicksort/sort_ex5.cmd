#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_sort_ex5.txt
#SBATCH --error=out_sort.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=00:20:00

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

make clean >> out_sort.err && make >> out_sort.err || exit 1 

nruns=5

for size in 1000000 100000000
do
    for X in 5 100 1000
    do
        for nthreads in 1 2 4 8 16
        do
            echo "Size: ${size}, Threads: ${nthreads}, X: ${X}"
            for (( i = 0; i < $nruns; i++ ))
            do
                ./a.out 2 $nthreads $size $X
            done
            echo "--------------------"
        done
    done
done