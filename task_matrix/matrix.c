#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

struct matrix
{
    double *values;

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

    matrix->values = aligned_alloc(32, size_x * size_y * sizeof(double));
    if (matrix->values == NULL)
    {
        matrix_destroy_flag = 1;
        goto destroy_matrix;
    }

    return matrix;

destroy_matrix:

    if (matrix_destroy_flag != 0)
        matrix_destroy(matrix);

    return NULL;
}

void matrix_destroy(matrix_t *matrix)
{
    free(matrix->values);
    free(matrix);
}

void matrix_init(matrix_t *matrix)
{
    size_t size_x = matrix->size_x;
    for (size_t i = 0; i < matrix->size_y; i++)
    {
        for (size_t j = 0; j < size_x; j++)
            matrix->values[i * size_x + j] = 499.0 * i * j;
    }
}

int matrix_add(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    if (matrix_A->size_x != matrix_B->size_x || matrix_A->size_y != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_B->size_y)
        return -1;

    size_t size_x = matrix_A->size_x;

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_A->size_y; i++)
    {
        for (size_t j = 0; j < size_x; j++)
            matrix_C->values[i * size_x + j] = matrix_A->values[i * size_x + j] + matrix_B->values[i * size_x + j];
    }

    return 0;
}

int matrix_sub(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    if (matrix_A->size_x != matrix_B->size_x || matrix_A->size_y != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_B->size_y)
        return -1;

    size_t size_x = matrix_A->size_x;

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_A->size_y; i++)
    {
        for (size_t j = 0; j < size_x; j++)
            matrix_C->values[i * size_x + j] = matrix_A->values[i * size_x + j] - matrix_B->values[i * size_x + j];
    }

    return 0;
}

int matrix_copy(const matrix_t *src, matrix_t *dst)
{
    if (dst->size_x != src->size_x || dst->size_y != src->size_y)
        return -1;

    size_t size_x = dst->size_x;

    #pragma omp parallel for
    for (size_t i = 0; i < src->size_y; i++)
    {
        for (size_t j = 0; j < size_x; j++)
            dst->values[i * size_x + j] = src->values[i * size_x + j];
    }

    return 0;
}

const double *matrix_get_values(matrix_t *matrix)
{
    return (const double *) matrix->values;
}

int matrix_multiply(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    if (matrix_A->size_x != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_A->size_y)
        return -1;

    size_t size_Cx = matrix_C->size_x;
    size_t size_Bx = matrix_B->size_x;

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_C->size_y; i++)
    {
        for (size_t j = 0; j < matrix_C->size_x; j++)
        {
            for (size_t k = 0; k < matrix_A->size_x; k++)
                matrix_C->values[i * size_Cx + j] += matrix_A->values[i * size_Cx + k] * matrix_B->values[k * size_Bx + j];
        }
    }

    return 0;
}

int matrix_multiply_opt1(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    if (matrix_A->size_x != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_A->size_y)
        return -1;

    size_t size_Cx = matrix_C->size_x;
    size_t size_Bx = matrix_B->size_x;

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_C->size_y; i++)
    {
        for (size_t k = 0; k < matrix_A->size_x; k++)
        {
            for (size_t j = 0; j < matrix_C->size_x; j++)
                matrix_C->values[i * size_Cx + j] += matrix_A->values[i * size_Cx + k] * matrix_B->values[k * size_Bx + j];
        }
    }

    return 0;
}

int matrix_multiply_opt2(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    const size_t block_size = 64;

    if (matrix_A->size_x != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_A->size_y)
        return -1;

    if (matrix_A->size_x % block_size != 0 || matrix_A->size_y % block_size != 0 ||
        matrix_B->size_x % block_size != 0 || matrix_B->size_y % block_size != 0 ||
        matrix_C->size_x % block_size != 0 || matrix_C->size_y % block_size != 0)
        return -1;

    size_t size_Cx = matrix_C->size_x;
    size_t size_Bx = matrix_B->size_x;

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
                            matrix_C->values[i * size_Cx + j] += matrix_A->values[i * size_Cx + k] * matrix_B->values[k * size_Bx + j];
                    }
                }
            }
        }
    }

    return 0;
}

int matrix_multiply_opt3(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    if (matrix_A->size_x != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_A->size_y)
        return -1;

    matrix_t *matrix_B_T = matrix_create(matrix_B->size_y, matrix_B->size_x);

    size_t size_Cx = matrix_C->size_x;
    size_t size_Bx = matrix_B->size_x;
    size_t size_By = matrix_B->size_y;

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_B_T->size_y; i++)
    {
        for (size_t j = 0; j < matrix_B_T->size_x; j++)
            matrix_B_T->values[j * size_By + i] = matrix_B->values[i * size_Bx + j];
    }

    #pragma omp parallel for
    for (size_t i = 0; i < matrix_C->size_y; i++)
    {
        for (size_t j = 0; j < matrix_C->size_x; j++)
        {
            for (size_t k = 0; k < matrix_A->size_x; k++)
                matrix_C->values[i * size_Cx + j] += matrix_A->values[i * size_Cx + k] * matrix_B_T->values[j * size_By + k];
        }
    }

    matrix_destroy(matrix_B_T);

    return 0;
}

int matrix_multiply_opt4(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    const size_t block_size = 64;

    if (matrix_A->size_x != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_A->size_y)
        return -1;

    if (matrix_A->size_x % block_size != 0 || matrix_A->size_y % block_size != 0 ||
        matrix_B->size_x % block_size != 0 || matrix_B->size_y % block_size != 0 ||
        matrix_C->size_x % block_size != 0 || matrix_C->size_y % block_size != 0)
        return -1;

    size_t size_Cx = matrix_C->size_x;
    size_t size_Bx = matrix_B->size_x;

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
                        __m256d c_vec = _mm256_load_pd(&(matrix_C->values[i * size_Cx + j]));

                        for (size_t k = bk; k < bk + block_size; k++)
                        {
                            __m256d a_vec = _mm256_set1_pd(matrix_A->values[i * size_Cx + k]);
                            __m256d b_vec = _mm256_load_pd(&(matrix_B->values[k * size_Bx + j]));
                            c_vec = _mm256_add_pd(c_vec, _mm256_mul_pd(a_vec, b_vec));
                        }

                        _mm256_store_pd(&(matrix_C->values[i * size_Cx + j]), c_vec);
                    }
                }
            }
        }
    }

    return 0;
}

int matrix_multiply_strassen(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C)
{
    const size_t block_size = 64;

    if (matrix_A->size_x != matrix_B->size_y ||
        matrix_C->size_x != matrix_B->size_x || matrix_C->size_y != matrix_A->size_y)
        return -1;

    if (matrix_A->size_x % block_size != 0 || matrix_A->size_y % block_size != 0 ||
        matrix_B->size_x % block_size != 0 || matrix_B->size_y % block_size != 0 ||
        matrix_C->size_x % block_size != 0 || matrix_C->size_y % block_size != 0)
        return -1;

    // if () {
    //     C[0][0] = A[0][0] * B[0][0];
    //     return;
    // }

    size_t Ax_size = matrix_A->size_x;
    size_t Bx_size = matrix_B->size_x;
    size_t Ay_size = matrix_A->size_y;

    size_t newAx_size = matrix_A->size_x / 2;
    size_t newAy_size = matrix_A->size_y / 2;
    size_t newBx_size = matrix_B->size_x / 2;
    size_t newBy_size = newAx_size;

    matrix_t *matrix_A11 = matrix_create(newAx_size, newAy_size);
    matrix_t *matrix_A12 = matrix_create(newAx_size, newAy_size);
    matrix_t *matrix_A21 = matrix_create(newAx_size, newAy_size);
    matrix_t *matrix_A22 = matrix_create(newAx_size, newAy_size);
    matrix_t *matrix_A_buffer = matrix_create(newAx_size, newAy_size);

    matrix_t *matrix_B11 = matrix_create(newBx_size, newBy_size);
    matrix_t *matrix_B12 = matrix_create(newBx_size, newBy_size);
    matrix_t *matrix_B21 = matrix_create(newBx_size, newBy_size);
    matrix_t *matrix_B22 = matrix_create(newBx_size, newBy_size);
    matrix_t *matrix_B_buffer = matrix_create(newBx_size, newBy_size);

    matrix_t *matrix_C11 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_C12 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_C21 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_C22 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_C1_buffer = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_C2_buffer = matrix_create(newBx_size, newAy_size);

    matrix_t *matrix_M1 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_M2 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_M3 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_M4 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_M5 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_M6 = matrix_create(newBx_size, newAy_size);
    matrix_t *matrix_M7 = matrix_create(newBx_size, newAy_size);

    #pragma omp parallel for
    for (int i = 0; i < newAy_size; i++)
    {
        for (int j = 0; j < newAx_size; j++)
        {
            matrix_A11->values[i * newAx_size + j] = matrix_A->values[i * Ax_size + j];
            matrix_A12->values[i * newAx_size + j] = matrix_A->values[i * Ax_size + j + newAx_size];
            matrix_A21->values[i * newAx_size + j] = matrix_A->values[(i + newAy_size) * Ax_size + j];
            matrix_A22->values[i * newAx_size + j] = matrix_A->values[(i + newAy_size) * Ax_size + j + newAx_size];
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < newBy_size; i++)
    {
        for (int j = 0; j < newBx_size; j++)
        {
            matrix_B11->values[i * newBx_size + j] = matrix_B->values[i * Bx_size + j];
            matrix_B12->values[i * newBx_size + j] = matrix_B->values[i * Bx_size + j + newBx_size];
            matrix_B21->values[i * newBx_size + j] = matrix_B->values[(i + newBy_size) * Bx_size + j];
            matrix_B22->values[i * newBx_size + j] = matrix_B->values[(i + newBy_size) * Bx_size + j + newBx_size];
        }
    }

    matrix_add(matrix_A11, matrix_A22, matrix_A_buffer);
    matrix_add(matrix_B11, matrix_B22, matrix_B_buffer);
    matrix_multiply_opt4(matrix_A_buffer, matrix_B_buffer, matrix_M1);

    matrix_add(matrix_A21, matrix_A22, matrix_A_buffer);
    matrix_multiply_opt4(matrix_A_buffer, matrix_B11, matrix_M2);

    matrix_sub(matrix_B12, matrix_B22, matrix_B_buffer);
    matrix_multiply_opt4(matrix_A11, matrix_B_buffer, matrix_M3);

    matrix_sub(matrix_B21, matrix_B11, matrix_B_buffer);
    matrix_multiply_opt4(matrix_A22, matrix_B_buffer, matrix_M4);

    matrix_sub(matrix_A11, matrix_A12, matrix_A_buffer);
    matrix_multiply_opt4(matrix_A_buffer, matrix_B22, matrix_M5);

    matrix_sub(matrix_A21, matrix_A11, matrix_A_buffer);
    matrix_add(matrix_B11, matrix_B12, matrix_B_buffer);
    matrix_multiply_opt4(matrix_A_buffer, matrix_B_buffer, matrix_M6);

    matrix_sub(matrix_A12, matrix_A22, matrix_A_buffer);
    matrix_add(matrix_B21, matrix_B22, matrix_B_buffer);
    matrix_multiply_opt4(matrix_A_buffer, matrix_B_buffer, matrix_M7);

    matrix_add(matrix_M1, matrix_M4, matrix_C1_buffer);
    matrix_add(matrix_C1_buffer, matrix_M7, matrix_C2_buffer);
    matrix_sub(matrix_C2_buffer, matrix_M5, matrix_C11);

    matrix_add(matrix_M1, matrix_M3, matrix_C1_buffer);
    matrix_add(matrix_C1_buffer, matrix_M6, matrix_C2_buffer);
    matrix_sub(matrix_C2_buffer, matrix_M2, matrix_C22);

    matrix_add(matrix_M1, matrix_M4, matrix_C21);
    matrix_add(matrix_M3, matrix_M5, matrix_C12);

    #pragma omp parallel for
    for (int i = 0; i < newAy_size; i++)
    {
        for (int j = 0; j < newAx_size; j++)
        {
            matrix_A11->values[i * newAx_size + j] = matrix_A->values[i * Ax_size + j];
            matrix_A12->values[i * newAx_size + j] = matrix_A->values[i * Ax_size + j + newAx_size];
            matrix_A21->values[i * newAx_size + j] = matrix_A->values[(i + newAy_size) * Ax_size + j];
            matrix_A22->values[i * newAx_size + j] = matrix_A->values[(i + newAy_size) * Ax_size + j + newAx_size];
        }
    }

    for (int i = 0; i < newAy_size; i++)
    {
        for (int j = 0; j < newBx_size; j++)
        {
            matrix_C->values[i * Bx_size + j] = matrix_C11->values[i * newBx_size + j];
            matrix_C->values[i * Bx_size + j + newBx_size] = matrix_C12->values[i * newBx_size + j];
            matrix_C->values[(i + newAy_size) * Bx_size + j] = matrix_C21->values[i * newBx_size + j];
            matrix_C->values[(i + newAy_size) * Bx_size + j + newBx_size] = matrix_C22->values[i * newBx_size + j];
        }
    }

    matrix_destroy(matrix_A11);
    matrix_destroy(matrix_A12);
    matrix_destroy(matrix_A21);
    matrix_destroy(matrix_A22);

    matrix_destroy(matrix_B11);
    matrix_destroy(matrix_B12);
    matrix_destroy(matrix_B21);
    matrix_destroy(matrix_B22);

    matrix_destroy(matrix_C11);
    matrix_destroy(matrix_C12);
    matrix_destroy(matrix_C21);
    matrix_destroy(matrix_C22);

    matrix_destroy(matrix_M1);
    matrix_destroy(matrix_M2);
    matrix_destroy(matrix_M3);
    matrix_destroy(matrix_M4);
    matrix_destroy(matrix_M5);
    matrix_destroy(matrix_M6);
    matrix_destroy(matrix_M7);

    matrix_destroy(matrix_A_buffer);
    matrix_destroy(matrix_B_buffer);
    matrix_destroy(matrix_C1_buffer);
    matrix_destroy(matrix_C2_buffer);

    return 0;
}
