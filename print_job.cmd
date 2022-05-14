#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=means_ex4.txt
#SBATCH --error=out_sort.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0

gcc print.c -o print_file
./print_file
