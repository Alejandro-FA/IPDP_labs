#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_dotp_16P1T.out
#SBATCH --error=out_dotp.err
#SBATCH --ntasks=16
#SBATCH --cpus-per-task=1
#SBATCH --time=00:05:00

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0
module load OpenMPI/4.1.0-GCC-10.2.0


make clean >> make.out && make >> make.out || exit 1 


nruns=5
for nprocs in 1 2 4 8 16
do
    echo "Processes: ${nprocs}"
    for (( i = 0; i < $nruns; i++ ))
    do
        mpirun -np $nprocs --bind-to none -x OMP_NUM_THREADS=4 ./a.out 1 1
    done
    echo "--------------------"
done
