#!/bin/bash
#SBATCH --job-name=test
#SBATCH --output=out_dotp_6P4T.out
#SBATCH --error=out_dotp.err
#SBATCH --ntasks=6
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00

source /etc/profile.d/z00-global-profile.sh

module load GCC/10.2.0
module load OpenMPI/4.1.0-GCC-10.2.0


make clean >> make.out && make >> make.out || exit 1 


nruns=5

# echo "Processes: 2, Threads: 12"
# for (( i = 0; i < $nruns; i++ ))
# do
#     mpirun -np 2 --bind-to none -x OMP_NUM_THREADS=4 ./a.out 12 1
# done
# echo "--------------------"

# echo "Processes: 4, Threads: 6"
# for (( i = 0; i < $nruns; i++ ))
# do
#     mpirun -np 4 --bind-to none -x OMP_NUM_THREADS=4 ./a.out 6 1
# done
# echo "--------------------"

echo "Processes: 6, Threads: 4"
for (( i = 0; i < $nruns; i++ ))
do
    mpirun -np 6 --bind-to none -x OMP_NUM_THREADS=4 ./a.out 4 1
done
echo "--------------------"

# echo "Processes: 12, Threads: 2"
# for (( i = 0; i < $nruns; i++ ))
# do
#     mpirun -np 12 --bind-to none -x OMP_NUM_THREADS=4 ./a.out 2 1
# done
# echo "--------------------"
