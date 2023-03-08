#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

double **A, **B, **C, **D; // Matrix arrays
int size, numThreads;	   // vars for handling arrays

int main(int argc, char *argv[])
{
	// Declare timing vars and initialize them
	double clockstart, clockend, overallTimeS, overallTimeP;
	overallTimeS = 0;
	overallTimeP = 0;

	if (argc == 3) // Sequential Run
	{
		if (strcmp(argv[1], "S") == 0) // Check for proper token
		{
			if (argv[2] > 0) // Check to make sure matrix size > 0
			{
				// Print info about run
				size = atoi(argv[2]);
				printf("========\n");
				printf("mode: Sequential\n");
				printf("thread count: 1\n");
				printf("size: %d\n", size);
				printf("========\n");

				// First throwaway run
				mmm_init();
				mmm_seq();
				mmm_reset(C);

				// 3 subsequent runs, adding total running time
				for (int i = 0; i < 3; i++)
				{
					mmm_reset(C);
					clockstart = rtclock();
					mmm_seq();
					clockend = rtclock();
					overallTimeS += (clockend - clockstart);
				}

				// Free up arrays
				mmm_freeup();

				// Calculate / print final runtime info
				overallTimeS = overallTimeS / (double)3;

				printf("Sequential Time (avg of 3 runs): %f secs\n", overallTimeS);
			}
			else // Throw error if matrix size < 0
			{
				perror("Size of matrices must be greater than 0");
				return -1;
			}
		}
		else if (strcmp(argv[1], "P") == 0) // Throw error if given P with inadequate arg count
		{
			perror("Usage: parallel mode requires [num threads]");
			return -1;
		}
		else // Throw error if S or P is not entered
		{
			perror("Error: mode must be either S (sequential) or P (parallel)");
			return -1;
		}
	}
	else if (argc == 4) // Parallel
	{
		if (strcmp(argv[1], "P") == 0) // Check token for P
		{
			if (argv[2] < 0) // Make sure of positive number of threads, else throw error
			{
				perror("Number of threads must be positive");
				return -1;
			}
			else if (argv[3] < 0) // Make sure matrix size is positive, else throw error
			{
				perror("Size of matrices must be positive");
				return -1;
			}
			else // Run parallel algorithm
			{
				// Initialize vars and print out info about run
				size = atoi(argv[3]);
				mmm_init();
				numThreads = atoi(argv[2]);
				printf("========\n");
				printf("mode: Parallel\n");
				printf("thread count: %d\n", numThreads);
				printf("size: %d\n", size);
				printf("========\n");

				// Initialize overall time var
				overallTimeP = 0;

				// 4 runs, not clocking first run
				for (int run = 0; run < 4; run++)
				{
					mmm_reset(D); // Reset final output mat before each run
					if (run != 0) // Ignore clocking on first run
					{
						clockstart = rtclock();
					}
					// Prep Thread Inputs
					thread_args *args = (thread_args *)malloc(numThreads * sizeof(thread_args));
					for (int i = 0; i < numThreads; i++)
					{
						args[i].tid = i;
						args[i].begin = i * (size / numThreads);
						args[i].end = (i + 1) * (size / numThreads) - 1;
					}

					// Allocate space to hold threads
					pthread_t *threads = (pthread_t *)malloc(numThreads * sizeof(pthread_t));
					for (int i = 0; i < numThreads; i++)
					{
						// printf("Running Thread %d\n", i);
						pthread_create(&threads[i], NULL, mmm_par, &args[i]);
					}

					for (int i = 0; i < numThreads; i++)
					{
						pthread_join(threads[i], NULL);
					}

					// Free args and threads
					free(args);
					args = NULL;

					free(threads);
					threads = NULL;

					if (run != 0) // Don't do final clocking phase on first run
					{
						clockend = rtclock();
						overallTimeP += (clockend - clockstart);
					}
				}

				// Calculate runtime avg
				overallTimeP = overallTimeP / (double)3;

				printf("Parallel Time (avg of 3 runs): %f secs\n", overallTimeP);

				// Sequential runs

				// First throwaway run
				mmm_seq();
				mmm_reset(C);

				// Subsequent 3 runs, clocking and keeping running total
				for (int i = 0; i < 3; i++)
				{
					mmm_reset(C);
					clockstart = rtclock();
					mmm_seq();
					clockend = rtclock();
					overallTimeS += (clockend - clockstart);
				}

				// Calculate runtime avg
				overallTimeS = overallTimeS / (double)3;

				printf("Sequential Time (avg of 3 runs): %f secs\n", overallTimeS);

				// Calculate largest error and report
				double largestError = 0;

				largestError = mmm_verify();

				printf("Largest Error is: %f\n", largestError);

				// Free arrays from heap after checking largestError
				mmm_freeup();

				// Report speedup
				printf("Speedup is: %fx\n", overallTimeS / overallTimeP);
			}
		}
		else // Throw error if incorrect token
		{
			perror("Error: mode must be either S (sequential) or P (parallel)");
			return -1;
		}
	}
	else // Throw error on incorrect arg formatting
	{
		perror("Usage: ./mmm <mode> [num threads] <size>\n");
		return -1;
	}
	return 0;
}
