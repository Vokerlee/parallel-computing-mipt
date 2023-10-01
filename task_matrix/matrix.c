#include "matrix.h"

void matrix_init(double matrix[MATRIX_SIZE][MATRIX_SIZE])
{
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
            matrix[i][j] = 499.0 * i * j;
    }
}

void matrix_multiply_seq(double A[MATRIX_SIZE][MATRIX_SIZE],
                         double B[MATRIX_SIZE][MATRIX_SIZE],
                         double C[MATRIX_SIZE][MATRIX_SIZE])
{
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            for (int k = 0; k < MATRIX_SIZE; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    }
}
