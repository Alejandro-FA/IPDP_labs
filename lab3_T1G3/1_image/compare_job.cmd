#!/bin/bash

#SBATCH --job-name=compare_image
#SBATCH --output=compare.out
#SBATCH --error=compare.err
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=1
#SBATCH --time=00:01:00

./scripts/compare.sh 

