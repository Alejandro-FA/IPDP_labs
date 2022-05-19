# Lab 2 report

Marc Aguilar and Alejandro Fernández

## Exercise 1: Hello World

There's not much to comment here. The code directly addresses all the tasks proposed by the exercise. In order to decide whether a process has an even or an odd rank, we've used the **modulo 2 operation**.

## Exercise 2: Dot product


## Exercise 4: Broadcast (Gather)

The main aspect to highlight of this exercise is the fact that we created a derived datatype called `diagonal`. It is used to make easier the gathering process (despite the fact that the exercise was called broadcast we assumed it was a typo). 

After trying with 4 and 8 processes we got the results we expected. The execution with 4 processes takes **0.000040 seconds** and with 8 processes it takes **0.000090 seconds**, twice the first one. These results make sense since it is a weak scaling case because the size of the problem increases as the resources do. 

The overhead of having to send all the diagonals from each process will be larger as the number of processes grow. We have also executed it with 16 processes and the results are worse.

## Exercise 5: Game of Life

This exercise was very straightforward since the algorithm was already implemented. We developed all the data management as it was detailed in both the report and the comments in the code. 