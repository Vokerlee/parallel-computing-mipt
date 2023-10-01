#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    double A[MATRIX_SIZE][MATRIX_SIZE];
    double B[MATRIX_SIZE][MATRIX_SIZE];
    double C[MATRIX_SIZE][MATRIX_SIZE] = {0};

    matrix_init(A);
    matrix_init(B);

    double start_time = omp_get_wtime();

    matrix_multiply_seq(A, B, C);

    double exec_time = omp_get_wtime() - start_time;
    printf("%lf\n", exec_time);

    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            printf("%lf\n", C[i][j]);
        }
    }

    return EXIT_SUCCESS;
}
