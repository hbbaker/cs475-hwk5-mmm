#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

int main(int argc, char *argv[])
{
	double clockstart, clockend;
	double **A, **B;
	int size, numThreads;
	// clockstart = rtclock(); // start clocking

	// start: stuff I want to clock
	if (argc == 3)
	{
		// printf("Sequential MMM\n");
		// printf("%s\n", argv[1]);
		if (strcmp(argv[1], "S") == 0)
		{
			if (argv[2] > 0)
			{
				size = atoi(argv[2]);
				printf("========\n");
				printf("mode: Sequential\n");
				printf("thread count: 1\n");
				printf("size: %d\n", size);
				printf("========\n");
			}
			else
			{
				perror("Size of matrices must be greater than 0");
				return -1;
			}
		}
		else if (strcmp(argv[1], "P") == 0)
		{
			perror("Usage: parallel mode requires [num threads]");
			return -1;
		}
		else
		{
			perror("Error: mode must be either S (sequential) or P (parallel)");
			return -1;
		}
	}
	else if (argc == 4)
	{
		// printf("Parallel MMM\n");
		// printf("%s\n", argv[1]);
		if (strcmp(argv[1], "P") == 0)
		{
			if (argv[2] < 0)
			{
				perror("Number of threads must be positive");
				return -1;
			}
			else if (argv[3] < 0)
			{
				perror("Size of matrices must be positive");
				return -1;
			}
			else
			{
				size = atoi(argv[3]);
				numThreads = atoi(argv[2]);
				printf("========\n");
				printf("mode: Parallel\n");
				printf("thread count: %d\n", numThreads);
				printf("size: %d\n", size);
				printf("========\n");
			}
		}
		else
		{
			perror("Error: mode must be either S (sequential) or P (parallel)");
			return -1;
		}
	}
	else
	{
		perror("Usage: ./mmm <mode> [num threads] <size>\n");
		return -1;
	}

	// end: stuff I want to clock

	// clockend = rtclock(); // stop clocking
	// printf("Time taken: %.6f sec\n", (clockend - clockstart));

	return 0;
}
