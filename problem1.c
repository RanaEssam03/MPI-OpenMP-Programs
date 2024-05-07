#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h> // for rand()
#include "mpi.h"

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
                for (int j = 0; j < num_candidates; j++) {
                    fprintf(file, "%d ", rand() % num_candidates);
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
    
    // Each process reads its portion of the file
    int local_num_candidates, local_num_voters;
    int **voter_preferences; // 2D array to store local voter preferences
    
    // Open the file in each process
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        MPI_Finalize();
        return 1;
    }
    
    // Read number of candidates and voters from file
    if (rank == 0) {
        fscanf(file, "%d %d", &num_candidates, &num_voters);
    }
    // Broadcast number of candidates and voters to all processes
    MPI_Bcast(&num_candidates, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_voters, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate the number of voters each process will handle
    int voters_per_process = num_voters / size;
    int remaining_voters = num_voters % size;
    local_num_voters = (rank < remaining_voters) ? voters_per_process + 1 : voters_per_process;
    
    // Allocate memory for local voter preferences
    local_num_candidates = num_candidates;
    voter_preferences = (int **)malloc(local_num_voters * sizeof(int *));
    for (int i = 0; i < local_num_voters; i++) {
        voter_preferences[i] = (int *)malloc(local_num_candidates * sizeof(int));
    }
    
    // Read local voter preferences from file
    for (int i = 0; i < local_num_voters; i++) {
        for (int j = 0; j < local_num_candidates; j++) {
            fscanf(file, "%d", &voter_preferences[i][j]);
        }
    }
    fclose(file);
    // Perform election process based on voter preferences
// (each process calculates its portion of the votes)
int *candidate_votes = (int *)malloc(num_candidates * sizeof(int));
memset(candidate_votes, 0, num_candidates * sizeof(int));

// First round of voting
for (int i = 0; i < local_num_voters; i++) {
    int first_choice = voter_preferences[i][0];
    candidate_votes[first_choice]++;
}

// Determine if a candidate has more than 50% of the votes
int max_votes = 0;
int winning_candidate = -1;
for (int i = 0; i < num_candidates; i++) {
    if (candidate_votes[i] > max_votes) {
        max_votes = candidate_votes[i];
        winning_candidate = i;
    }
}
bool second_round_needed = max_votes <= num_voters / 2;

// If second round is needed, determine top 2 candidates
int second_candidate = -1;
if (second_round_needed) {
    int second_max_votes = 0;
    for (int i = 0; i < num_candidates; i++) {
        if (i != winning_candidate && candidate_votes[i] > second_max_votes) {
            second_max_votes = candidate_votes[i];
            second_candidate = i;
        }
    }
}
    
    // Communicate among processes to determine the winner and round
    // (not shown here, gather results from all processes and determine the winner)
int global_winner, global_round;
MPI_Reduce(&winning_candidate, &global_winner, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
MPI_Reduce(&second_candidate, &global_round, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

if (rank == 0) {
    if (second_round_needed) {
        printf("Winner: Candidate %d, Round: 2\n", global_winner);
    } else {
        printf("Winner: Candidate %d, Round: 1\n", global_winner);
    }
}

free(candidate_votes);
    
    // Free dynamically allocated memory
    for (int i = 0; i < local_num_voters; i++) {
        free(voter_preferences[i]);
    }
    free(voter_preferences);
    
    MPI_Finalize();
    return 0;
}
