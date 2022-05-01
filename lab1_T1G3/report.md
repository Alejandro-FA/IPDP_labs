# Lab 1 report

Marc Aguilar and Alejandro Fernández

## Task 1: Dot product

### Exercise 2.2

> Submit a job executing the code with a vector size of 100M integers and obtain results for 1, 2, 4 and 8 threads (you can use the sample slurm script dotp job.cmd as a base for your job files). How does it scale? Plot the strong speedup.

| Number of threads | Sequential (ms) | Parallel time (ms) |
| ----------------- | --------------- | ------------------ |
| 1                 | 64.302          | 64,465             |
| 2                 | 64.302          | 32,65              |
| 4                 | 64.302          | 21,609             |
| 8                 | 64.302          | 21,468             |

All the times presented in the previous table correspond to an average of 5 runs.

### Exercise 2.3

> Using the time spent in the dot product function compute throughput in *GBytes/s* and *GFLOP/s* for each configuration (present them in a table).

| Number of threads | GBytes/s | GFLOP/s |
| ----------------- | -------- | ------- |
| 1                 | 64.302   | 64,465  |
| 2                 | 64.302   | 32,65   |
| 4                 | 64.302   | 21,609  |
| 8                 | 64.302   | 21,468  |

All the times presented in the previous table correspond to an average of 5 runs.

### Exercise 3.2

> Submit a job executing the code with a vector size of 100M integers and obtain results for 1, 2, 4 and 8 cores with the option of vectorization inside the parallel region. How does it scale? Add a table to your report comparing the values with the simple parallelization.

| Number of threads | Parallel time (ms) | Vectorized time (ms) |
| ----------------- | ------------------ | -------------------- |
| 1                 | 64,465             | 31,537               |
| 2                 | 32,65              | 21,775               |
| 4                 | 21,609             | 21,312               |
| 8                 | 21,468             | 21,526               |

All the times presented in the previous table correspond to an average of 5 runs.  

### Exercise 3.3

> Change the flag *`-O2`* in the Makefile for *`-O3`*. Submit a job executing the code with a vector size of 100M integers and obtain results for 1, 2, 4 and 8 cores with the options of Exercise 2 and Exercise 3. Compare the results. What does *`-O3`* do?

The flag `-O3`just sets a better optimization level for the compiler. 

## Task 2: Sorting with Quicksort

### Exercise 4.2

> Submit a job executing the code with a vector size of 1M doubles and obtain results for 1, 2, 4, 8 and 16 cores (you can use the sample job file dotp sort.cmd as a base for your job files). How does it scale? Plot the speedup.



### Exercise 4.3

> Submit a job executing the code with a vector size of 100M doubles and obtain results for 1, 2, 4, 8 and 16 cores (you can use the sample job file dotp sort.cmd as a base for your job files). Does it scale better or worse than before? Why?



### Exercise 5

> Add the clause `if(hi − lo ≤ (X))` to the pragmas of the recursive calls to *Quicksort* and repeat the tests in Exercises 4.2 and 4.3. Try different values for (X) (5, 10 and 1000) and different sizes for the problem. What do you observe? What does it do? Add a table to your report with the different values you have obtained.



## Task 3: N-queens with Genetic Algorithm

### Exercise 6.1

> Use *`gprof`* to get a profile of the code. What is the function that takes more time? You may need to modify the Makefile and the job script.

The function that takes more time is the `Fitness()` one. 


### Exercise 6.2

> Parallelize the most time consuming function and run the code requesting 4 cores. Is the new code faster?

After parallelizing the function `Fitness()` we can observe that the new code is quite faster than the previous one. The parallelization we have implemented consists on:

1. Dividing the iterations of the for loop between 4 threads.
2. Applying a reduction to the variable `attack`.
3. Setting a static scheduling, which is the one that improves the performance the most in this case. 

