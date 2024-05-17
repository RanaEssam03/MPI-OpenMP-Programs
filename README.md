# MPI_OpenMP_Programs

This repository contains solutions for three computational problems using MPI and OpenMP. The problems are as follows:
1. **Election to Choose a New President (using MPI)**
2. **Search in a 2D Array (using OpenMP)**
3. **Calculating Standard Deviation (using OpenMP)**

## Table of Contents
- [Problem 1: Election to Choose a New President](#problem-1-election-to-choose-a-new-president-using-mpi)
  - [Description](#description)
  - [Input Format](#input-format)
  - [Output Format](#output-format)
  - [Execution Instructions](#execution-instructions)
- [Problem 2: Search in a 2D Array](#problem-2-search-in-a-2d-array-using-openmp)
  - [Description](#description-1)
  - [Input Format](#input-format-1)
  - [Output Format](#output-format-1)
  - [Execution Instructions](#execution-instructions-1)
- [Problem 3: Calculating Standard Deviation](#problem-3-calculating-standard-deviation-using-openmp)
  - [Description](#description-2)
  - [Input Format](#input-format-2)
  - [Output Format](#output-format-2)
  - [Execution Instructions](#execution-instructions-2)
- [Compilation and Running Instructions](#compilation-and-running-instructions)
- [Prerequisites](#prerequisites)


## Problem 1: Election to Choose a New President (using MPI)

### Description
This problem simulates an election process to determine the president based on voter preferences. The election can consist of up to two rounds. If a candidate receives more than 50% of the votes in the first round, they win. Otherwise, a second round is conducted with the top two candidates.

### Input Format
The input file should be formatted as follows:
- The first line contains the number of candidates `C`.
- The second line contains the number of voters `V`.
- The next `V` lines contain the voters' preference lists.

Example:
3 5
1 2 3
1 2 3
2 1 3
2 3 1
3 2 1

### Output Format
The output will indicate the winning candidate and the round in which they won.

Example:
2 2

### Execution Instructions
1. **Generate Voter Preferences File:**
   Run the program and choose the option to generate the file. Enter the number of candidates and voters, followed by the voters' preference lists.
2. **Calculate Election Results:**
   Run the program and choose the option to calculate the results. Enter the filename of the voter preferences file. The program will distribute the file reading process across MPI processes, compute the results, and display the steps and the final outcome.

## Problem 2: Search in a 2D Array (using OpenMP)

### Description
This problem involves searching for a key in a 2D array using OpenMP for parallel processing.

### Input Format
- Number of rows and columns in the matrix.
- The key to search for.

Example:
5 5 42


### Output Format
The output will display the indices where the key is found or `-1` if the key is not found.

Example:
Process 0 found key at index (2, 3)
Indices: [(2, 3)]



### Execution Instructions
1. Compile the program with OpenMP support.
2. Run the program, enter the matrix dimensions, and the key to search for. The program will initialize the matrix with random numbers, perform the search in parallel, and display the results.

## Problem 3: Calculating Standard Deviation (using OpenMP)

### Description
This problem calculates the standard deviation of a set of randomly generated numbers using OpenMP for parallel computation.

### Input Format
- An integer `n` representing the number of elements per process.

Example:
5


### Output Format
The output will display the standard deviation of the generated numbers.

Example:
Standard Deviation = 24.658

### Execution Instructions
1. Compile the program with OpenMP support.
2. Run the program and enter the number of elements per process. The program will generate the elements, compute the local sums, and aggregate the results to calculate and display the standard deviation.

## Compilation and Running Instructions

### Prerequisites
- MPI Library (e.g., MPICH or OpenMPI)
- OpenMP support in your C compiler (e.g., GCC)

### Compilation
To compile the programs, use the following commands:


#### For MPI program (Problem 1):
To Compile:
```sh
mpicc file_name.c -o output_file -lm
```
To Run:
```sh
 mpiexec -n <num_processes> ./output_file
```

#### For OpenMP (Problem 2 & 3):
To Compile:
```sh
gcc -o out -fopenmp file_name.c
```
To Run:
```sh
 ./out
```
