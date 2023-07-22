# CSE330 Summer 2023 Group4 Projects 

## Project1: VM and Linux operations

Configuration Step 

1.a Create a new VM in  Virtual Box 

1.b Install Ubuntu on the VM 

1.c Comiple and install new kernel on the new Ubuntu

1.d Implement a new module and add a simple "Hello world" function in the kernel

1.e Implement a new syscall for the kenel that prints a simple message in the kernel log 


## Project2: Process Management 
In this project, we aim to tackle the producer-consumer problem, a classic synchronization challenge. To achieve this, we will develop a kernel module designed to work with the Linux VM. Solution will involve utilizing kernel threads to represent producers and consumers while employing kernel-space semaphores for efficient synchronization.

### Development Steps 

2.a Implement module interface and module_init

2.b Implement the producer thread which searches the system for all the processes that belong to a given user and
adds their task_struct to the shared buffer.

2.c Implement the consumer thread which  reads out the task_struct of the processes from the buffer and calculates
the elapsed time for each process and the total elapsed time. 

2.d Implement the module_exit signals all the active kernel threads to stop 


## Project3: Memory Management 
Develop a kernel module that effectively traverses the page tables of a specified process. Through this module, you will determine the following key metrics: the number of process pages residing in physical memory (RSS), the count of pages swapped out to disk (SWAP), and the size of the process's working set (WSS). 

Development Steps 

3.a Implement the module_init 

3.b 

