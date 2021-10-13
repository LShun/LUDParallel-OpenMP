# LU Decomposition using OpenMP

Using OpenMP to Parallelize LU Decomposition. 3 methods are used, loop parallelism, scheduling, and SIMD parallelism. For full results, please look at the PDF report, this is simply an excerpt showing the solutions implemented.

## Solutions

### 2.1.3 Solution 1: Instruction-level, loop parallelism (OMP-ILP)

The first solution proposed is instruction-level loop parallelism, or in OpenMP, “for-loop” parallelism (codenamed: OMP-ILP). Loop parallelism is a common type of parallelism in scientific codes, so OpenMP has an easy mechanism for it. OpenMP parallel loops are a first example of OpenMP ‘work-sharing' constructs: constructs that take an amount of work and distribute it over the available threads in a parallel region, created with the parallel pragma. (Eijkhout 2020) There are 2 places to be parallelized. The first is the lower matrix, or “L” decomposition loop, and the second uses the upper matrix, or “U” decomposition loop. They are then enclosed in a *parallel for* parallel region enclosing all work-sharing for loops. This is because large parallel regions offer more opportunities for using data in cache and provide a bigger context for compiler optimizations (Satoh, Kusano & Sato 2001). The parallel regions are placed outside rather than inside the loops to avoid construction overheads (Satoh, Kusano & Sato 2001). Refer to Figure 8.2 in Appendix 8.1.1 for implementation.

### Solution 2: Solution 1 + OpenMP Scheduling (OMP-DS)

The second solution is to add in dynamic scheduling. By default, OpenMP statically assigns loop iterations to threads (Sobral 2017). When the parallel for block is entered, it assigns each thread the set of loop iterations it is to execute (Sobral 2017). Therefore, assumption can be made that the results for static scheduling is simply OMP-ILP. To ensure this is the case, *pragma omp for schedule(static)* was tested*,* and there is negligible difference compared to no schedule specifier. A static schedule can be non-optimal, however. This is the case when the different iterations take different amounts of time. This is true with the LU decomposition, where sometimes, instead of calculation, the program will assign 0 or 1 directly according to the algorithm. This is true for the diagonals. For the inner *k* loop in the calculation branch, the number of loops is dependent on the location of the location in the matrix. The latter cells in the matrix will require more loops compared to previous loops. Therefore, dynamic scheduling will be implemented and test different chunk sizes. For dynamic scheduling, a chunk size of 1,2,4,8,16, and 32 will be assigned. The chunk size is limited to 32 because that’s exactly half the smallest matrix size that we will benchmark, which is 64x64. For the number of threads, we will use the number of threads that provide the best result from solution 1. The results are then documented. Refer to Figure 8.2 in Appendix 8.1.1 for implementation.

![img](https://lh4.googleusercontent.com/gH8Z3wCy3k6PLE3sMFPoeeS8vA43mZqrDVXtkD1uAz5CWtLK-p2ZH4tmVQaU6LjGjkQWYMZoDOoaG9wAqahzYZfNNiU995xRBw-K6A1UwZF6SL_-Y29PEql46olcY4bvQo6tUQp9=s0)

##### Figure 2.1: Static vs Dynamic Scheduling (Bosio 2017)

### Solution 3: Solution 2 + data-level, SIMD parallelism (OMP-SIMD)

The third solution is to add in Single Instruction, Multiple Data (SIMD), or better known as data-leveling parallelism (SIMD). The *omp simd* directive is applied to a loop to indicate that multiple iterations of the loop can be executed concurrently by using SIMD instructions (IBM 2018). After declaring parallel-for in the outer loop to split the LU decomposition task to threads for multiple cores in CPU to handle them concurrently, our implementation then adds SIMD parallelism to the inner loop to take advantage of SIMD pipelines *within* individual cores of the CPU. To put it simply, this implementation transforms the individual data elements to an operation where a single instruction operates concurrently on multiple data elements (SIMD). Modern Intel processor cores have dedicated vector units supporting SIMD parallel data processing (Reinders & Jeffers 2016). Intel processors that support Intel® Advanced Vector Extensions (Intel® AVX) have one 256-bit vector unit per core. Thus, each core can process eight single-precision (e.g., 32-bit) floating point operations or four double-precision (e.g., 64-bit) floating point operations using a single instruction (Reinders & Jeffers 2016). 

![img](https://lh4.googleusercontent.com/GUOTqbzJ6m9YmXjSHSa3T-8euKCcqAGupYC3RxaoLjxwV0yc8g4YY0uElECh2xPH5cu3-0uMZ5zqGIBxeS7FU8rSAPsPHGdc3iPFhQL9ZbP-cHba_ixjwgWWv_gi2SxlulZKTKsk=s0)

##### Figure 2.2: Example of SIMD Processing. Multiple results are produced from 1 instruction. (Reinders & Jeffers 2016) 

SIMD will be implemented in the LU decomposition for non-diagonals. The inner loop we have in Figure 8.3 in Appendix 8.1.1 is similar to the given example in the picture above. For testing purposes, the thread size, and scheduling algorithm that nets the best scheduling times from solution OMP-DS will be taken to test SIMD instructions.

