# Lab 4 report

Marc Aguilar and Alejandro Fernández

## Exercise 1

The functions which apply the filters of both exercises have been implemented similarly. First of all we compute which is the position of the thread executing the filter by its `threadIdx` and `blockIdx`. Then, if the thread is inside the domain of the image we apply the corresponding filter.

In the main part of the code we first allocate space in both the CPU and the GPU. Then, we copy the intial image synchronously from the host to the device and we call the corresponding filters. Then, we perform a synchronous copy of the filtered images from the device to the host.

The execution of the code from the GPU allocation to the last memory transfer takes about **30 ms**.

## Exercise 2



