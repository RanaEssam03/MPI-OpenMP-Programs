#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h> // for rand()
#include "mpi.h"



void randomPermutation(int N, int* array) {
    for (int i = 0; i < N; i++) {
        array[i] = i + 1;
    }
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int num_candidates, num_voters;
    FILE *file;
    char filename[100];
    
    if (rank == 0) {
        // Prompt user for file generation or calculation
        printf("Enter 'generate' to generate file or 'calculate' to compute the result: ");
        scanf("%s", filename);
        if (strcmp(filename, "generate") == 0) {
            // Generate file name for generation
            sprintf(filename, "election_data_%d.txt", size);
            file = fopen(filename, "w");
            if (file == NULL) {
                printf("Error opening file.\n");
                MPI_Finalize();
                return 1;
            }
            // Prompt user for number of candidates and voters
            printf("Enter number of candidates: \n");
            scanf("%d", &num_candidates);
            printf("Enter number of voters: \n");
            scanf("%d", &num_voters);
            // Write number of candidates and voters to file
            fprintf(file, "%d %d\n", num_candidates, num_voters);
            // Generate voter preferences and write to file
            for (int i = 0; i < num_voters; i++) {
                int perm[num_candidates];
                randomPermutation(num_candidates, perm);
                for (int j = 0; j < num_candidates; j++) {
                    fprintf(file, "%d ", perm[j]);
                }
                fprintf(file, "\n");
            }
            fclose(file);
        } else {
            // Read file name for calculation
            printf("Enter filename: ");
            scanf("%s", filename);
        }
    }
    // Broadcast filename to all processes
    MPI_Bcast(filename, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
    

       if(rank == 0){
        file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error opening file.\n");
            MPI_Finalize();
            return 1;
        }
        fseek(file, 0, SEEK_SET);
        fscanf(file, "%d %d", &num_candidates, &num_voters);
        fclose(file);
    }
    MPI_Bcast(&num_candidates, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_voters, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int start = (rank * (num_voters / size))+1;
    int end = (start + (num_voters / size)) - 1;
    if(rank == size - 1) {
        end = num_voters;
    }
    file = fopen(filename, "r");
    fscanf(file, "%d %d", &num_candidates, &num_voters);
    int ** preferences = (int **)malloc((end - start + 1) * sizeof(int *));
    for (int i = 0; i < (end - start + 1); i++) {
        preferences[i] = (int *)malloc(num_candidates * sizeof(int));
    }
    for (int i = 0; i < start - 1; i++) {
        for (int j = 0; j < num_candidates; j++) {
            int temp;
            fscanf(file, "%d", &temp);
        }
    }
    for(int i = start ; i<=end;i++){
        for (int j = 0; j < num_candidates; j++) {
            fscanf(file, "%d", &preferences[i-start][j]);
        }
    }
    fclose(file);

    int * votes = (int *)malloc(num_candidates * sizeof(int));
    memset(votes, 0, num_candidates * sizeof(int));
    for (int i = 0; i < (end - start + 1); i++) {
        votes[preferences[i][0] - 1]++;
    }
    int * global_votes = (int *)malloc(num_candidates * sizeof(int));
    MPI_Reduce(votes, global_votes, num_candidates, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    bool second_round = false;
    int max_votes = 0;
    int max_index = 0;
    if (rank == 0) {
        for (int i = 0; i < num_candidates; i++) {
            if (global_votes[i] > max_votes) {
                max_votes = global_votes[i];
                max_index = i;
            }
        }
        if (max_votes > (num_voters / 2)) {
            printf("Candidate %d wins in the first round.\n", max_index + 1);
        } else {
            second_round = true;
        }
    }
    MPI_Bcast(&second_round, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
    if (second_round) {
       // get max 2 votes with their indices
       int max_votes2 = 0; 
       int max_index2 = 0;
       for(int i = 0; i < num_candidates; i++){
           if(global_votes[i] > max_votes2 && i != max_index){
               max_votes2 = global_votes[i];
               max_index2 = i;
           }
       }
       int * votes2 = (int *)malloc(2 * sizeof(int));
       memset(votes2, 0, 2 * sizeof(int));
       for(int i = start ; i<=end;i++){
        for(int j = 0; j < num_candidates; j++){
          if(preferences[i-start][j] == max_index + 1){
            votes2[0]++;
            break;
          }
        if(preferences[i-start][j] == max_index2 + 1){
                votes2[1]++;
                break;
        }
        }
       }
       int * global_votes2 = (int *)malloc(2 * sizeof(int));
       MPI_Reduce(votes2, global_votes2, 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
       if(rank == 0){
           if(global_votes2[0] > global_votes2[1]){
               printf("Candidate %d wins in the second round.\n", max_index + 1);
           } else {
               printf("Candidate %d wins in the second round .\n", max_index2 + 1);
           }
       }   
    }
    MPI_Finalize();
    return 0;
}
