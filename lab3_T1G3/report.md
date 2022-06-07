# Lab 3 report

Marc Aguilar and Alejandro Fernández

## Exercise 1

There's not much to comment here. Special attention has been put in the order of loops in order to take advantage of data locality. Furthermore, we have also used the directive `collapse(2)` in order to optimize the nested loops that appear when accessing all the pixels of a 2D image.

The last important thing to comment here is that the **time required to complete the filtering process is much higher than the one needed with the CPU** (in our test run, it takes `2.054103 s` with the GPU). We believe that this is due to the **PCIE bus bandwidth limitations**, which creates a huge overhead when compared to the simple computations we need to perform with the proposed filters.

## Exercise 2


## Exercise 3




