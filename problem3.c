// OpenMP header
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <time.h>

 
int main(int argc, char* argv[])
{
    /*Seed the random number generator ONCE*/
    srand(time(NULL)); 
    int N = 5, i;
    printf("Please enter a Number: \n");
    scanf("%d", &N);
    
    int totalThreads =  omp_get_max_threads();
    int* localSums = malloc(totalThreads * sizeof(int));

    /* Begin of parallel region*/
    #pragma omp parallel private(i)
    {
        int* nums = malloc(N * sizeof(int));
        double localSum = 0;
        int thNum = omp_get_thread_num();
    
        // 1. Generate n random elements
        // 2. Calculate local sum of the generated n elements
        for ( i = 0; i < N; i++)
        {
            nums[i] = rand() % 100;
            localSum += nums[i];
        }

        // 3. Calculate local mean of the generated n elements
        double mean = (double)localSum / N;

        localSum = 0;
        // 4. Calculate local sum of squared differences from the mean 
        for(i = 0 ; i < N ; i++)
        {
            localSum += pow((nums[i]-mean), 2);
        }
        // gather all local sums in one array 
        localSums[thNum] = localSum;        
    }


    /*Barrier ensures synchronization before proceeding*/ 
    #pragma omp barrier
    {
    // 5. Add local sum of squared differences to global sum.     
    int globalMean = 0;
    for(i = 0 ; i < totalThreads ; i++)
    {
        globalMean += localSums[i];
    }
    // 6. Calculate the global mean. (global sum / n * numOfProcesses).
    double variance = (double)globalMean / (N * totalThreads); 

    // 7. Calculate the standard deviation
    double stdDeviation = sqrt(variance);

    printf("Standard Deviation = %f\n", stdDeviation);
    
    }
    
    return 0;
}
