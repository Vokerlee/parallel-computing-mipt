#include "matrix.h"

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
        matrix->values[i] = calloc(size_x, sizeof(double));
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

const double **matrix_get_values(matrix_t *matrix)
{
    return (const double **) matrix->values;
}

void matrix_multiply(matrix_t *matrix_A,
                     matrix_t *matrix_B,
                     matrix_t *matrix_C)
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
