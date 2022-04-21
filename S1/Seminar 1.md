# Seminar 1

### SCP (file transfer)

Send files from local to cluster:

```bash
scp test.txt u188315@10.49.0.109:~/sub/folder
```

Receive files from cluster to local:

```bash
scp u188315@10.49.0.109:test.txt ~/Desktop/test.txt
```

==Important==: this commands have to be executed from local terminal, not from opened ssh session.

For sending and retrieving entire folders use the `-r` option. For more information see the documentation: https://www.hypexr.org/linux_scp_help.php



### Exercise 1

> Where are we connected after introducing the SSH command? To a compute node or the login node?

To the login node, shared by all users.

> Do you know what command checks how many users are connected?

The `who` command.

> Run the command *lscpu*. What does it return?

```bash
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                2
On-line CPU(s) list:   0,1
Thread(s) per core:    1
Core(s) per socket:    2
Socket(s):             1
NUMA node(s):          1
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 63
Model name:            Intel(R) Xeon(R) CPU E5-2666 v3 @ 2.90GHz
Stepping:              2
CPU MHz:               1626.708
CPU max MHz:           3500.0000
CPU min MHz:           1200.0000
BogoMIPS:              5800.04
Hypervisor vendor:     Xen
Virtualization type:   full
L1d cache:             32K
L1i cache:             32K
L2 cache:              256K
L3 cache:              25600K
NUMA node0 CPU(s):     0,1
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush acpi mmx fxsr sse sse2 ss ht syscall nx pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology cpuid aperfmperf pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand hypervisor lahf_lm abm cpuid_fault epb invpcid_single pti ssbd ibrs ibpb stibp fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid xsaveopt dtherm ida arat pln pts md_clear flush_l1d
```

> Create a folder in your local computer with a file. Leave it empty.

```bash
alejandro@MacBook-Pro-de-Alejandro ~ % cd ~/Desktop
alejandro@MacBook-Pro-de-Alejandro ~ % mkdir testfolder
```

> Upload it to the cluster using *scp* commands or with a graphical tool.

```bash
[u188315@ip-10-49-0-109 ~]$ scp -r testfolder u188315@10.49.0.109:~
```

> Add some text using VIM. Try some of the describe modes and commands.

```bash
[u188315@ip-10-49-0-109 ~]$ cd testfolder
[u188315@ip-10-49-0-109 ~]$ touch testfile
[u188315@ip-10-49-0-109 ~]$ vim testfile
alejandro@MacBook-Pro-de-Alejandro ~ % scp u188315@10.49.0.109:testfolder/testfile ~/Desktop/testfolder/testfile
```



### Exercise 2

> Create a batch job configuration with 2 nodes, 4 cores per node, 2 task per node and 2 threads per task.

```bash
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
```

