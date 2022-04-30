#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_dotp.out
#SBATCH --error=out_dotp.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --time=00:10:00

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

make clean >> out_dotp.err && make >> out_dotp.err || exit 1

nruns=5

echo "Sequential execution"
for (( i = 0; i < $nruns; i++ ))
do
    ./a.out 1 1 100000000
done
echo "--------------------"

for nthreads in 1 2 4 8
do
    echo "Parallel execution, ${nthreads} threads"
    for (( i = 0; i < $nruns; i++ ))
    do
        ./a.out 2 $nthreads 100000000
    done
    echo "--------------------"
done

for nthreads in 1 2 4 8
do
    echo "Parallel execution with vectorization, ${nthreads} threads"
    for (( i = 0; i < $nruns; i++ ))
    do
        ./a.out 3 $nthreads 100000000
    done
    echo "--------------------"
done
