#!/bin/bash

#SBATCH --job-name=sieve
#SBATCH --output=sieve_%j.out
#SBATCH --error=sieve_%j.err
#SBATCH --cpus-per-task=
#SBATCH --ntasks=
#SBATCH --nodes=
#SBATCH --time=

make || exit 1  # Exit if make fails


