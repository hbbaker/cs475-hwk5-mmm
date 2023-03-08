#ifndef MMM_H_
#define MMM_H_

// globals (anything here would be shared with all threads) */
// I would declare the pointers to the matrices here (i.e., extern double **A, **B, **C),
// as well as the size of the matrices, etc.

extern double **A, **B, **C, **D;
extern int size, numThreads;

typedef struct thread_args
{
    int tid;    // the given thread id (0, 1, ...)
    long begin; // where to begin matrix mult
    long end;   // where to end matrix mult
} thread_args;

void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void *mmm_par(void *args);
double mmm_verify();

#endif /* MMM_H_ */
