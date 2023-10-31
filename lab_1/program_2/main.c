#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <omp.h>
#include <math.h>

// All variables naming is taken from original task,
// os if you don't like them - I understand you.

#ifndef N_THREADS
    #define N_THREADS 2
#endif

#ifndef ISIZE
    #define ISIZE 5000
#endif

#ifndef JSIZE
    #define JSIZE 5000
#endif

int main(int argc, char **argv)
{
    omp_set_num_threads(N_THREADS);

    double **a = (double **) calloc(ISIZE, sizeof(double *));
    if (a == NULL)
        errx(EX_OSERR, "error: calloc()");

    for (size_t i = 0; i < ISIZE; ++i)
    {
        a[i] = (double *) calloc(JSIZE, sizeof(double));
        if (a[i] == NULL)
            errx(EX_OSERR, "error: calloc()");
    }

    for (size_t i = 0; i < ISIZE; i++)
    {
        for (size_t j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;
    }

    double start_time = omp_get_wtime();

    for (size_t i = 0; i < ISIZE - 4; i++)
    {
        #pragma omp parallel for
        for (size_t j = 5; j < JSIZE; j++)
            a[i][j] = sin(0.1 * a[i + 4][j - 5]);
    }

    double end_time = omp_get_wtime();

#ifdef DEBUG_ON

    for (size_t i = 0; i < ISIZE; i++)
    {
        for (size_t j = 0; j < JSIZE; j++)
            printf("%f ", a[i][j]);

        printf("\n");
    }

#endif // DEBUG_ON

    printf("%lf\n", end_time - start_time);

    for (size_t i = 0; i < JSIZE; ++i)
        free(a[i]);

    free(a);
}
