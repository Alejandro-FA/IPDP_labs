#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=means_ex4.txt
#SBATCH --error=out_sort.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

gcc mean_calc.c -o mean_calc
./mean_calc /shared/home/u188315/IPDP_labs/lab1_T1G3/2_quicksort/out_sort_ex4.txt 12 5

