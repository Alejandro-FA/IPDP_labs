#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_sort.out
#SBATCH --error=out_sort.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=00:00:10

export OMP_NUM_THREADS=16

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

make clean >> out_sort.err && make >> out_sort.err || exit 1 

./a.out 1000000 >> results.out

