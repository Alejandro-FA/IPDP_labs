# Lab 2 report

Marc Aguilar and Alejandro Fernández

## Exercise 1: Hello World

There's not much to comment here. The code directly addresses all the tasks proposed by the exercise. In order to decide whether a process has an even or an odd rank, we've used the **modulo 2 operation**.

## Exercise 2: Dot Product

## Exercise 3: Matrix Vector Product

In this exercise we considered that it would be a good approach to implement the matrix vector product computation in parallel with a fixed number of threads. Therefore we used a similar approach to the last exercise with OpenMP with **4 threads** in all the cases.

| Number of processes | Execution time (s) |
| ------------------- | ------------------ |
| 1                   | 1.245931           |
| 2                   | 1.133708           |
| 4                   | 1.084926           |
| 8                   | 1.100226           |
| 16                  | 1.195487           |

As we can observe the scaling is pretty decent until we reach 8 processes. Then, the overhead of managing all the data transfer between processes is greater than the problem itself. Consequently the execution time decreases for 8 and 16 processes.

*The execution times are an average of 5 runs.*

## Exercise 4: Broadcast (Gather)

The main aspect to highlight of this exercise is the fact that we created a derived datatype called `diagonal`. It is used to make easier the gathering process (despite the fact that the exercise was called broadcast we assumed it was a typo). 

After trying with 4 and 8 processes we got the results we expected. The execution with 4 processes takes **0.04 ms** and with 8 processes it takes **0.09 ms**, twice the first one. These results make sense since it is a weak scaling case because the size of the problem increases as the resources do. 

The overhead of having to send all the diagonals from each process and managing the data will be larger as the number of processes grow. We have also executed it with 16 processes and the results are worse than before.

## Exercise 5: Game of Life

This exercise was very straightforward since the algorithm was already implemented. We developed all the data management as it was detailed in both the report and the comments in the code. 
