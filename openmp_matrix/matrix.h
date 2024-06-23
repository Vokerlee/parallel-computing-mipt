#ifndef MATRIX_PARALLEL_MATRIX_PARALLEL
#define MATRIX_PARALLEL_MATRIX_PARALLEL

#include <stdlib.h>

#ifndef MATRIX_SIZE
    #define MATRIX_SIZE 512
#endif

#ifndef N_THREADS
    #define N_THREADS 8
#endif

struct matrix;
typedef struct matrix matrix_t;

matrix_t *matrix_create(size_t size_x, size_t size_y);
void matrix_destroy(matrix_t *matrix);

const double *matrix_get_values(matrix_t *matrix);

void matrix_init(matrix_t *matrix);
int matrix_copy(const matrix_t *src, matrix_t *dst);

int matrix_multiply         (matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);
int matrix_multiply_opt1    (matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);
int matrix_multiply_opt2    (matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);
int matrix_multiply_opt3    (matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);
int matrix_multiply_opt4    (matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);
int matrix_multiply_strassen(matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);
int matrix_multiply_offload (matrix_t *matrix_A, matrix_t *matrix_B, matrix_t *matrix_C);

#endif
