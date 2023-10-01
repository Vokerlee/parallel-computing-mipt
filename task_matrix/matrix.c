#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

struct matrix
{
    double **values;

    size_t size_x;
    size_t size_y;
};

matrix_t *matrix_create(size_t size_x, size_t size_y)
{
    matrix_t *matrix = calloc(1, sizeof(matrix_t));
    if (matrix == NULL)
        return NULL;

    matrix->size_x = size_x;
    matrix->size_y = size_y;

    int matrix_destroy_flag = 0;

    matrix->values = calloc(size_y, sizeof(double));
    if (matrix->values == NULL)
    {
        matrix_destroy_flag = 1;
        goto destroy_matrix;
    }

    for (size_t i = 0; i < size_y; i++)
    {
        matrix->values[i] = aligned_alloc(32, size_x * sizeof(double));
        if (matrix->values[i] == NULL)
        {
            matrix_destroy_flag = 1;
            break;
        }
    }

    return matrix;

destroy_matrix:

    if (matrix_destroy_flag != 0)
        matrix_destroy(matrix);

    return NULL;
}

void matrix_destroy(matrix_t *matrix)
{
    if (matrix->values != NULL)
    {
        for (size_t i = 0; i < matrix->size_y; i++)
            free(matrix->values[i]);
    }

    free(matrix->values);
    free(matrix);
}

void matrix_init(matrix_t *matrix)
{
    for (size_t i = 0; i < matrix->size_y; i++)
    {
        for (size_t j = 0; j < matrix->size_x; j++)
            matrix->values[i][j] = 499.0 * i * j;
    }
}

void matrix_copy(const matrix_t *src, matrix_t *dst)
{
    dst->size_x = src->size_x;
    dst->size_y = src->size_y;

    for (size_t i = 0; i < src->size_y; i++)
    {
        for (size_t j = 0; j < src->size_x; j++)
            dst->values[i][j] = src->values[i][j];
    }
}

const double **matrix_get_values(matrix_t *matrix)
{
    return (const double **) matrix->values;
}

void matrix_multiply(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    #pragma omp parallel for
    for (size_t i = 0; i < matrix_C->size_y; i++)
    {
        for (size_t j = 0; j < matrix_C->size_x; j++)
        {
            for (size_t k = 0; k < matrix_A->size_x; k++)
                matrix_C->values[i][j] += matrix_A->values[i][k] * matrix_B->values[k][j];
        }
    }
}

void matrix_multiply_opt1(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    #pragma omp parallel for
    for (size_t i = 0; i < matrix_C->size_y; i++)
    {
        for (size_t k = 0; k < matrix_A->size_x; k++)
        {
            for (size_t j = 0; j < matrix_C->size_x; j++)
                matrix_C->values[i][j] += matrix_A->values[i][k] * matrix_B->values[k][j];
        }
    }
}

void matrix_multiply_opt2(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    const size_t block_size = 64;

    #pragma omp parallel for
    for (size_t bi = 0; bi < matrix_C->size_y; bi += block_size)
    {
        for (size_t bk = 0; bk < matrix_A->size_x; bk += block_size)
        {
            for (size_t bj = 0; bj < matrix_C->size_x; bj += block_size)
            {
                for (size_t i = bi; i < bi + block_size; i++)
                {
                    for (size_t k = bk; k < bk + block_size; k++)
                    {
                        for (size_t j = bj; j < bj + block_size; j++)
                            matrix_C->values[i][j] += matrix_A->values[i][k] * matrix_B->values[k][j];
                    }
                }
            }
        }
    }
}

void matrix_multiply_opt3(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    matrix_t *matrix_B_T = matrix_create(matrix_B->size_x, matrix_B->size_y);
    for (size_t i = 0; i < matrix_B_T->size_y; i++)
    {
        for (size_t j = 0; j < matrix_B_T->size_x; j++)
            matrix_B_T->values[j][i] = matrix_B->values[i][j];
    }

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_C->size_y; i++)
    {
        for (size_t j = 0; j < matrix_C->size_x; j++)
        {
            for (size_t k = 0; k < matrix_A->size_x; k++)
                matrix_C->values[i][j] += matrix_A->values[i][k] * matrix_B_T->values[j][k];
        }
    }

    matrix_destroy(matrix_B_T);
}

void matrix_multiply_opt4(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    const size_t block_size = 64;

    #pragma omp parallel for
    for (size_t bi = 0; bi < matrix_C->size_y; bi += block_size)
    {
        for (size_t bj = 0; bj < matrix_C->size_x; bj += block_size)
        {
            for (size_t bk = 0; bk < matrix_A->size_x; bk += block_size)
            {
                for (size_t i = bi; i < bi + block_size; i++)
                {
                    for (size_t j = bj; j < bj + block_size; j += 4)
                    {
                        __m256d c_vec = _mm256_load_pd(&(matrix_C->values[i][j]));

                        for (size_t k = bk; k < bk + block_size; k++)
                        {
                            __m256d a_vec = _mm256_set1_pd(matrix_A->values[i][k]);
                            __m256d b_vec = _mm256_load_pd(&(matrix_B->values[k][j]));
                            c_vec = _mm256_add_pd(c_vec, _mm256_mul_pd(a_vec, b_vec));
                        }

                        _mm256_store_pd(&(matrix_C->values[i][j]), c_vec);
                    }
                }
            }
        }
    }
}
