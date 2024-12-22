# Parallel Convolution

This project aims to implement and evaluate Gaussian blur, a fundamental image processing operation, using three approaches: serial, MPI, and hybrid MPI + OpenMP. The goal is to analyze the performance of parallel programming techniques in optimizing computationally intensive tasks. This work is part of the ME766 course project. For detailed analysis, refer to [Report.pdf](Report.pdf), and the implementation codes can be found in [Code Files](Code%20Files).

----------------------
## Implementation Instructions  

To run the code, ensure that the image and code files are in the same folder. The following instructions demonstrate how to compile and run the code for **50 iterations** on **macOS**:  

### General Note  
- For **grayscale images**, use [`waterfall_grey_1920_2520.raw`](Code%20Files/waterfall_grey_1920_2520.raw) and specify `grey`.  
- For **RGB images**, use [`waterfall_1920_2520.raw`](Code%20Files/waterfall_1920_2520.raw) and specify `rgb`.   

### 1. Serial Implementation  
**Compile:**  
```bash  
/opt/homebrew/bin/gcc-14 serial.c -o serial  
```  
**Run:**  
```bash  
./serial waterfall_grey_1920_2520.raw 1920 2520 50 grey  
```  

### 2. MPI Implementation  
**Compile:**  
```bash  
mpicc -o mpi_conv mpi_conv.c  
```  
**Run (for 2 processes):**  
```bash  
mpirun -np 2 ./mpi_conv waterfall_grey_1920_2520.raw 1920 2520 50 grey
```  


### 3. MPI + OpenMP Hybrid Implementation  
**Compile:**  
```bash  
/opt/homebrew/bin/gcc-14 -fopenmp -o mpi_omp_conv mpi_omp_conv.c -I/opt/homebrew/include -L/opt/homebrew/lib -lmpi  
```  
**Run (for 2 processes):**  
```bash  
mpirun -np 2 ./mpi_omp_conv waterfall_grey_1920_2520.raw 1920 2520 50 grey  
```  
## Results

The table below shows the performance results for various processes, based on the number of iterations and processing approaches for **grayscale images**:

| **N**  | **Serial Time (s)** | **2 Processes MPI Time (s)** | **2 Processes Hybrid Time (s)** | **4 Processes MPI Time (s)** | **4 Processes Hybrid Time (s)** | **6 Processes MPI Time (s)** | **6 Processes Hybrid Time (s)** | **8 Processes MPI Time (s)** | **8 Processes Hybrid Time (s)** |
|--------|---------------------|------------------------------|--------------------------------|------------------------------|--------------------------------|------------------------------|--------------------------------|------------------------------|--------------------------------|
| 20     | 3.034               | 1.549288                     | 0.564127                       | 0.798311                     | 0.506423                       | 0.534291                     | 0.534828                       | 0.534619                     | 0.615634                      |
| 50     | 7.935               | 3.867279                     | 1.396288                       | 1.992304                     | 1.246592                       | 1.334239                     | 1.329070                       | 1.310875                     | 1.526315                      |
| 100    | 16.383              | 7.727510                     | 2.850884                       | 3.986903                     | 2.469982                       | 2.683687                     | 2.741402                       | 2.607915                     | 3.066253                      |
| 200    | 32.291              | 15.493539                    | 5.617990                       | 7.986798                     | 5.052975                       | 5.319520                     | 5.422499                       | 5.076195                     | 6.218083                      |
| 500    | 86.180              | 38.654856                    | 14.047751                      | 19.915830                    | 12.500185                      | 13.675643                    | 13.515103                      | 12.839587                    | 15.371245                     |

## References

1. Mangual, Osvaldo, Teixeira, Marvi, Lopez, Reynaldo, Nevarez, Felix. "Hybrid MPI-OpenMP versus MPI Implementations: A Case Study." *2014*. [DOI: 10.13140/2.1.1219.1364](https://doi.org/10.13140/2.1.1219.1364).

2. Lu, Jin, Zhang, Kaisheng, Chen, Ming, Ma, Ke. "Implementation of parallel convolution based on MPI." *Proceedings of 2013 3rd International Conference on Computer Science and Network Technology*, 2013, pp. 28-31. [DOI: 10.1109/ICCSNT.2013.6967057](https://doi.org/10.1109/ICCSNT.2013.6967057).

3. Jim Ouris. "Parallel Convolution." 2021. Available at: [https://github.com/jimouris/parallel-convolution](https://github.com/jimouris/parallel-convolution). Accessed: 2024-11-29.

4. Panagiotis Petropoulakis. "Parallel Convolution." 2021. Available at: [https://github.com/PetropoulakisPanagiotis/parallel-convolution](https://github.com/PetropoulakisPanagiotis/parallel-convolution). Accessed: 2024-11-29.

