#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_dotp.out
#SBATCH --error=out_dotp.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:00:10

export N_EXEC=2

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

make clean >> out_dotp.err && make >> out_dotp.err || exit 1 

echo "Sequential execution"
for i in {1..5}
do
    ./a.out 1 1 100000000
done
echo "--------------------"

echo "Parallel execution 1 thread"
for i in {1..5}
do
    ./a.out 2 1 100000000
done
echo "--------------------"

echo "Parallel execution 2 threads"
for i in {1..5}
do
    ./a.out 2 2 100000000
done
echo "--------------------"

echo "Parallel execution 4 threads"
for i in {1..5}
do
    ./a.out 2 4 100000000
done
echo "--------------------"

echo "Parallel execution 8 threads"
for i in {1..5}
do
    ./a.out 2 8 100000000
done
echo "--------------------"

echo "Parallel execution with vectorization 1 thread"
for i in {1..5}
do
    ./a.out 3 1 100000000
done
echo "--------------------"

echo "Parallel execution with vectorization 2 threads"
for i in {1..5}
do
    ./a.out 3 2 100000000
done
echo "--------------------"

echo "Parallel execution with vectorization 4 threads"
for i in {1..5}
do
    ./a.out 3 4 100000000
done
echo "--------------------"

echo "Parallel execution with vectorization 8 threads"
for i in {1..5}
do
    ./a.out 3 8 100000000
done
echo "--------------------"

