Name: Bao Nguyen </br>
ID: 107171106 </br>
Email: bang4255@colorado.edu </br>

# Files Needed: </br>
Makefile: Builds all code listed below </br>

multi-lookup.c : Application that takes set of file names for input. Contains 1 hostname per line. Each file is serviced by requestor thread and servicedby max # of resolver threads. Outputs hostname with IP address</br>

multi-lookup.h : Header file for multi-lookup.c </br>

queue.c : The requester pushes to queue and the resolver pops from queue </br>

queue.h : Header file for queue.c </br>

util.c : Utility function that performs DNS lookup. Returns resolved IP address </br>

util.h : Header fil for util.h </br>

input folder : Contains .txt files with given hostnames </br>


# How to use files above: </br>

Run "make" command in terminal to build all files. </br>

Run application by using following command template: ./multi-lookup <# requester> <# resolver> <requester log> <resolverl log> [<data file>...] </br>

For memory leaks, use: "valgrind ./multi-lookup 1 1 serviced.txt results.txt input/names1.txt input/names2.txt... input/namesn.txt" </br>

Run "make clean" to remove files. </br>
