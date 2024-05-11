#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int totalThreads = omp_get_max_threads(), iter, i, j;
    int row, col, key;

    printf("Enter number of rows: ");
    scanf("%d", &row);
    printf("Enter number of columns: ");
    scanf("%d", &col);


    // Dynamically allocate memory for the matrix
    int **matrix = (int **) malloc(row * sizeof(int *));
    for (i = 0; i < row; i++)
        matrix[i] = (int *) malloc(col * sizeof(int));

    // Initialize the matrix with random numbers
    srand(omp_get_wtime()); // Seed for random number generation
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            matrix[i][j] = rand() % 100; // Generate random numbers between 0 and 99
        }
    }
    // Print the matrix values
    printf("Matrix Values:\n");
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }


    printf("Enter the key to search: ");
    scanf("%d", &key);

    // work on the arrays to store the answer
    // indxArr to store the repetition
    // ans to store the indices
    int *indxArr = (int *) malloc(totalThreads * sizeof(int));
    int **ans = (int **) malloc(totalThreads* sizeof(int *));
    for (i = 0; i < totalThreads; i++)
        ans[i] = (int *) malloc(col * row * 2 * sizeof(int));

    for (i = 0; i < totalThreads; i++) {
        indxArr[i]=0;
        for (j = 0; j < col * row * 2; j++) {
            ans[i][j] = -1;
        }
    }


#pragma omp parallel private (i, iter)
    {
        iter = 0;
#pragma omp for schedule(static)
        for (i = 0; i < row; i++) {
            int id = omp_get_thread_num();
            for (j = 0; j < col; j++) {
                //check, if true save the indices
                if (matrix[i][j] == key) {
                    indxArr[id]++;
                    ans[id][iter++] = j;
                    ans[id][iter++] = i;
                }
            }
        }
    }
    // Barrier ensures synchronization before proceeding
#pragma omp barrier
    {
        for (i = 0; i < totalThreads; i++) {
            printf("At thread %d key=%d appears %d times at indices: ", i, key, indxArr[i]);
            j = 0;
            while (ans[i][j] != -1) {
                printf("(%d,%d) ", ans[i][j++], ans[i][j++]);
            }
            if (j == 0)printf("-1");

            printf("\n");

        }
    }


    // Free dynamically allocated memory
    for (i = 0; i < row; i++)
        free(matrix[i]);
    free(matrix);

     for (i = 0; i < totalThreads; i++)
        free(ans[i]);
    free(ans);
    return 0;
}
