#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_nqueens.out
#SBATCH --error=out_nqueens.err
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:00:10

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0


gprof -b a.out gmon.out > analysis2.out
