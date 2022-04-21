#!/bin/bash
#SBATCH --output=example_%j.out
#SBATCH --error=example_%j.err
#SBATCH --time=00:00:05
#SBATCH --nodes=2
#SBATCH --ntasks=4
#SBATCH --tasks-per-node=2
#SBATCH --cpus-per-task=2

echo "Name of the cluster that has executed the script: " $(hostname)
echo "Number of nodes: " $SLURM_JOB_NUM_NODES
echo "Number of cores: " $(( $SLURM_CPUS_ON_NODE * $SLURM_JOB_NUM_NODES ))
echo "Number of tasks: " $SLURM_NTASKS
echo "Number of cores per task: " $SLURM_CPUS_PER_TASK

