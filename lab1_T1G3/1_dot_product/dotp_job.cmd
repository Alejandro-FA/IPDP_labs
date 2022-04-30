#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_dotp.out
#SBATCH --error=out_dotp.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --time=00:00:10

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

make clean >> out_dotp.err && make >> out_dotp.err || exit 1 

echo "Sequential execution"
for i in {1..20}
do
    ./a.out 1 1 100000000
done

echo "\nParallel execution 1 thread"
for i in {1..20}
do
    ./a.out 1 1 100000000
done

echo "\nParallel execution 2 threads"
for i in {1..20}
do
    ./a.out 1 1 100000000
done

echo "\nParallel execution 4 threads"
for i in {1..20}
do
    ./a.out 1 1 100000000
done

echo "\nParallel execution 8 threads"
for i in {1..20}
do
    ./a.out 1 1 100000000
done

