#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

#define N 1024

int main(int argc, char *argv[])
{
    omp_set_num_threads(N_THREADS);

    matrix_t *A = matrix_create(MATRIX_SIZE, MATRIX_SIZE);
    matrix_t *B = matrix_create(MATRIX_SIZE, MATRIX_SIZE);
    matrix_t *C = matrix_create(MATRIX_SIZE, MATRIX_SIZE);

    matrix_init(A);
    matrix_init(B);

    double start_time = omp_get_wtime();

    #ifdef NOOPTIMIZE
    matrix_multiply(A, B, C);
    #endif

    #ifdef OPT1_LOOP
    matrix_multiply_opt1(A, B, C);
    #endif

    #ifdef OPT2
    matrix_multiply_opt2(A, B, C);
    #endif

    #ifdef OPT3
    matrix_multiply_opt3(A, B, C);
    #endif

    #ifdef OPT4
    matrix_multiply_opt4(A, B, C);
    #endif

    #ifdef OPT_STRASSEN
    matrix_multiply_strassen(A, B, C);
    #endif

    #ifdef OPT_OFFLOAD
    matrix_multiply_offload(A, B, C);
    #endif

    double exec_time = omp_get_wtime() - start_time;

    const double *C_values = matrix_get_values(C);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            printf("%lf\n", C_values[i * MATRIX_SIZE + j]);
        }
    }

    printf("exec_time %lf\n", exec_time);

    return EXIT_SUCCESS;
}
